#include "../include/FlowNetwork.h"
#include "../include/DataTypes.h"
#include "../include/MaxFlow.h"
#include <algorithm>
#include <unordered_map>
#include <map>

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
    result.totalRequired = (int)originalData.submissions.size() * params_.minReviewsPerSubmission;

    // Usar std::map aqui garante que ao iterar no fim, os IDs aparecem por ordem
    std::map<int, int> reviewsReceived;
    for (const auto& [id, sub] : originalData.submissions)
        reviewsReceived[sub.id] = 0;

    // O VertexSet por ordem de inserção (Submissões primeiro) ajuda o determinismo
    for (auto* vertex : graph_.getVertexSet()) {
        if (vertex->getInfo().type != NodeType::SUBMISSION) continue;
        int subId = vertex->getInfo().id;

        for (auto* edge : vertex->getAdj()) {
            if (edge->getDest()->getInfo().type != NodeType::REVIEWER) continue;
            if (edge->getFlow() < 1.0) continue;

            int revId = edge->getDest()->getInfo().id;
            int matchedTopic = -1;
            switch(ctrl_.generateAssignments) {
                case AssignmentMode::SILENT:
                case AssignmentMode::PRIMARY:
                    matchedTopic = originalData.submissions.at(subId).primaryTopic;
                    break;
                case AssignmentMode::MIXED:
                    if (originalData.submissions.at(subId).primaryTopic == originalData.reviewers.at(revId).primaryExpertise) 
                        matchedTopic = originalData.submissions.at(subId).primaryTopic;
                    else 
                        matchedTopic = originalData.submissions.at(subId).secondaryTopic;
                    break;
                case AssignmentMode::FULL:
                    if (originalData.submissions.at(subId).primaryTopic == originalData.reviewers.at(revId).primaryExpertise || 
                        (originalData.reviewers.at(revId).secondaryExpertise != -1 && originalData.submissions.at(subId).primaryTopic == originalData.reviewers.at(revId).secondaryExpertise)) 
                        matchedTopic = originalData.submissions.at(subId).primaryTopic;
                    else 
                        matchedTopic = originalData.submissions.at(subId).secondaryTopic;
                    break;
            }

            result.assignments.push_back(Assignment{subId, revId, matchedTopic});
            reviewsReceived[subId]++;
            result.totalFlow++;
        }
    }

    // Como reviewsReceived é um std::map, as falhas são detetadas já ORDENADAS
    for (const auto& [subId, received] : reviewsReceived) {
        int needed = params_.minReviewsPerSubmission;
        if (received < needed) {
            int domain = originalData.submissions.at(subId).primaryTopic;
            result.missing.push_back(MissingReview{subId, domain, needed - received});
        }
    }

    // A ordenação já não é necessária porque iteremos as arestas do grafo 
    // com base em vértices e vizinhos inseridos em ordem devida pelo std::map

    return result;
}


/**
 * @brief Identifica revisores críticos (K=1).
 *
 * Corre um fluxo inicial para obter as atribuições base.
 * De seguida corta temporariamente a ligação de cada revisor que teve atribuições (um a um)
 * e tenta executar um novo max-flow. Se o total de flow diminuir e o assignment falhar,
 * então este revisor é critico.
 * 
 * @param data Os dados guardados do Parser (submissions, reviewers, parametros).
 * @return std::vector<int> Lista de IDs de revisores críticos.
 * @complexity O(R_usados * V * E^2)
 */
std::vector<int> FlowNetwork::riskAnalysis1(const ParseResult& data) {
    std::vector<int> criticalReviewers;
    const NodeInfo source = {NodeType::SOURCE, 0};
    const NodeInfo sink   = {NodeType::SINK,   0};

    // 1. Constroi a rede inteira UMA vez
    FlowNetwork trial(data);
    Graph<NodeInfo>& g = trial.getGraph();

    // 2. Correr a baseline para descobrir quem recebeu de facto atribuições
    edmondsKarp(&g, source, sink);
    AssignmentResult baseline = trial.extractResult(data);

    // Contar quantas submissões foram atribuidas a cada revisor na prestação baseline
    std::map<int, int> revLoad;
    for (const auto& [id, rev] : data.reviewers) revLoad[rev.id] = 0;
    for (const auto& a : baseline.assignments)    revLoad[a.reviewerId]++;

    // 3. Testar a falha de cada revisor iterativamente
    for (const auto& [revId, rev] : data.reviewers) {
        
        // Optimização gigante: se o revisor não contribuiu para a baseline, a sua ausência 
        // matematicamente não colapsará o Assignment. Nem precisamos testar!
        if (revLoad[revId] == 0) continue;

        NodeInfo revNode = {NodeType::REVIEWER, revId};

        // Sabotagem: Corta a saída deste revisor
        g.removeEdge(revNode, sink);

        // Testar se o full flow pode ser recuperado noutra configuração
        edmondsKarp(&g, source, sink);
        if (!trial.extractResult(data).isComplete()) {
            criticalReviewers.push_back(revId);
        }

        // Restaurar: Ligar o revisor novamente
        g.addEdge(revNode, sink, params_.maxReviewsPerReviewer);
    }

    return criticalReviewers;
}