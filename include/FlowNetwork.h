#ifndef FLOWNETWORK_H
#define FLOWNETWORK_H

#include "Graph.h"
#include "DataTypes.h"   // NodeType, NodeInfo, Parameters, Control, AssignmentResult
#include "Parser.h"      // ParseResult

/**
 * @brief Constrói e mantém a rede de fluxo para o problema de atribuição.
 *
 * Estrutura da rede (modo PRIMARY):
 *
 *   SOURCE ──[cap=minReviews]──► SUBMISSION_X
 *   SUBMISSION_X ──[cap=1, se match]──► REVIEWER_Y
 *   REVIEWER_Y ──[cap=maxReviews]──► SINK
 *
 * Após construção:
 *   1. Chamar edmondsKarp(&network.getGraph(), source, sink)
 *   2. Chamar network.extractResult(data) para obter as atribuições
 *
 * @complexity Construção O(S × R) onde S=submissions, R=reviewers
 */
class FlowNetwork {
public:
    /**
     * @brief Constrói a rede de fluxo a partir dos dados do parser.
     */
    explicit FlowNetwork(const ParseResult& data);

    /**
     * @brief Referência ao grafo para passar ao edmondsKarp().
     */
    Graph<NodeInfo>& getGraph();

    /**
     * @brief Extrai as atribuições das arestas com flow=1.
     *
     * Deve ser chamado APÓS edmondsKarp().
     * @param originalData Dados originais para calcular reviews em falta
     * @complexity O(S × R) no pior caso
     */
    AssignmentResult extractResult(const ParseResult& originalData) const;

private:
    Graph<NodeInfo> graph_;
    Parameters      params_;
    Control         ctrl_;

    /**
     * @brief Constrói vértices e arestas da rede.
     * @complexity O(S × R)
     */
    void buildGraph(const ParseResult& data);

    /**
     * @brief Verifica se um par (submission, reviewer) deve ser ligado.
     *
     * Depende de ctrl_.generateAssignments:
     *   PRIMARY/SILENT: sub.primaryTopic == rev.primaryExpertise
     *   MIXED:          primary ou secondary do paper vs primary do revisor
     *   FULL:           qualquer combinação de primary/secondary de ambos
     */
    bool hasMatch(const Submission& sub, const Reviewer& rev) const;
};

#endif // FLOWNETWORK_H