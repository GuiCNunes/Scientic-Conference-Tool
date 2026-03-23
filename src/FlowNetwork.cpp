#include "../include/FlowNetwork.h"
#include "../include/DataTypes.h"
#include <algorithm>
#include <unordered_map>

// ─────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────

FlowNetwork::FlowNetwork(const ParseResult& data)
    : params_(data.params), ctrl_(data.ctrl) {
    buildGraph(data);
}

Graph<NodeInfo>& FlowNetwork::getGraph() {
    return graph_;
}

// ─────────────────────────────────────────────
//  buildGraph — O(S × R)
// ─────────────────────────────────────────────

/**
 * @brief Constrói a rede de fluxo passo a passo:
 *
 *  1. SOURCE e SINK
 *  2. Para cada submission: nó + SOURCE → SUB  (cap = minReviews)
 *  3. Para cada reviewer:   nó + REV → SINK    (cap = maxReviews)
 *  4. Para cada par (sub, rev): se há match → SUB → REV (cap = 1)
 */
void FlowNetwork::buildGraph(const ParseResult& data) {
    const NodeInfo sourceNode = {NodeType::SOURCE, 0};
    const NodeInfo sinkNode   = {NodeType::SINK,   0};

    graph_.addVertex(sourceNode);
    graph_.addVertex(sinkNode);

    // Submissions: SOURCE → SUB
    for (const auto& [id, sub] : data.submissions) {
        NodeInfo subNode = {NodeType::SUBMISSION, sub.id};
        graph_.addVertex(subNode);

        // cap = minReviewsPerSubmission
        // "Este paper precisa de N reviews"
        graph_.addEdge(sourceNode, subNode, params_.minReviewsPerSubmission);
    }

    // Reviewers: REV → SINK
    for (const auto& [id, rev] : data.reviewers) {
        NodeInfo revNode = {NodeType::REVIEWER, rev.id};
        graph_.addVertex(revNode);

        // cap = maxReviewsPerReviewer
        // "Este revisor pode fazer no máximo M reviews no total"
        graph_.addEdge(revNode, sinkNode, params_.maxReviewsPerReviewer);
    }

    // Ligações SUB → REV onde há match de domínio
    for (const auto& [subId, sub] : data.submissions) {
        for (const auto& [revId, rev] : data.reviewers) {

            if (!hasMatch(sub, rev)) continue;

            NodeInfo subNode = {NodeType::SUBMISSION, sub.id};
            NodeInfo revNode = {NodeType::REVIEWER,   rev.id};

            // cap = 1
            // "Um revisor só pode ser atribuído UMA vez ao mesmo paper"
            graph_.addEdge(subNode, revNode, 1);
        }
    }
}

// ─────────────────────────────────────────────
//  hasMatch — condição de ligação entre paper e revisor
// ─────────────────────────────────────────────

/**
 * @brief Verifica se sub e rev devem ser ligados na rede.
 *
 * Modos conforme o enunciado:
 *   SILENT/PRIMARY (0/1): só primary de ambos
 *   MIXED          (2):   primary OU secondary do paper, primary do revisor
 *   FULL           (3):   qualquer combinação de primary/secondary de ambos
 */
bool FlowNetwork::hasMatch(const Submission& sub, const Reviewer& rev) const {
    switch (ctrl_.generateAssignments) {

        case AssignmentMode::SILENT:   // igual a PRIMARY — corre mas não reporta
        case AssignmentMode::PRIMARY:
            // T2.1: só primary topics de ambos
            return sub.primaryTopic == rev.primaryExpertise;

        case AssignmentMode::MIXED:
            // Primary OU secondary do paper vs primary do revisor
            if (sub.primaryTopic == rev.primaryExpertise)   return true;
            if (sub.secondaryTopic != -1 &&
                sub.secondaryTopic == rev.primaryExpertise) return true;
            return false;

        case AssignmentMode::FULL:
            // Qualquer combinação de primary/secondary de ambos
            if (sub.primaryTopic == rev.primaryExpertise)                          return true;
            if (sub.primaryTopic != -1 && sub.primaryTopic   == rev.secondaryExpertise) return true;
            if (sub.secondaryTopic != -1 && sub.secondaryTopic == rev.primaryExpertise) return true;
            if (sub.secondaryTopic != -1 && rev.secondaryExpertise != -1 &&
                sub.secondaryTopic == rev.secondaryExpertise)                      return true;
            return false;
    }
    return false;
}

// ─────────────────────────────────────────────
//  extractResult — lê o flow após edmondsKarp()
// ─────────────────────────────────────────────

/**
 * @brief Percorre as arestas SUBMISSION→REVIEWER com flow=1.
 *
 * Uma aresta SUB→REV com flow=1 = revisor foi atribuído a este paper.
 * Conta reviews por submission e detecta as incompletas.
 *
 * @complexity O(S × R) no pior caso
 */
AssignmentResult FlowNetwork::extractResult(const ParseResult& originalData) const {
    AssignmentResult result;
    result.totalRequired = (int)originalData.submissions.size()
                           * params_.minReviewsPerSubmission;

    // Contar reviews recebidas por cada submission
    std::unordered_map<int, int> reviewsReceived;
    for (const auto& [id, sub] : originalData.submissions)
        reviewsReceived[sub.id] = 0;

    // Percorrer todos os vértices de submission
    for (auto* vertex : graph_.getVertexSet()) {
        if (vertex->getInfo().type != NodeType::SUBMISSION) continue;

        int subId = vertex->getInfo().id;

        for (auto* edge : vertex->getAdj()) {
            // Só arestas para REVIEWER com flow activo
            if (edge->getDest()->getInfo().type != NodeType::REVIEWER) continue;
            if (edge->getFlow() < 1.0) continue;

            int revId = edge->getDest()->getInfo().id;

            // Descobrir tópico que fez o match
            int matchedTopic = originalData.submissions.at(subId).primaryTopic;

            result.assignments.push_back({subId, revId, matchedTopic});
            reviewsReceived[subId]++;
            result.totalFlow++;
        }
    }

    // Detectar submissions incompletas
    for (const auto& [subId, received] : reviewsReceived) {
        int needed = params_.minReviewsPerSubmission;
        if (received < needed) {
            int domain  = originalData.submissions.at(subId).primaryTopic;
            int missing = needed - received;
            result.missing.push_back({subId, domain, missing});
        }
    }

    // Ordenar para output consistente (como o enunciado pede)
    std::sort(result.assignments.begin(), result.assignments.end(),
        [](const Assignment& a, const Assignment& b) {
            return a.submissionId != b.submissionId
                 ? a.submissionId < b.submissionId
                 : a.reviewerId   < b.reviewerId;
        });
    std::sort(result.missing.begin(), result.missing.end(),
        [](const MissingReview& a, const MissingReview& b) {
            return a.submissionId < b.submissionId;
        });

    return result;
}