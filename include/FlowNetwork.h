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
     * @brief Constrói a rede de fluxo bidirecional a partir dos dados limpos do Parser.
     * @param data As submissôes e revisores com respectivos constraints de controlo.
     */
    explicit FlowNetwork(const ParseResult& data);

    /**
     * @brief Referência ao grafo interno para injetar e analisar no edmondsKarp().
     * @return Graph<NodeInfo>&
     */
    Graph<NodeInfo>& getGraph();

    /**
     * @brief Extrai as atribuições das arestas com capacity saturated (flow=1).
     *
     * Deve ser invocado ESTRITAMENTE APÓS a execução de edmondsKarp().
     * Percorre iterativamente os caminhos sub→rev para apurar matches.
     * 
     * @param originalData O dataset usado para gerar relatórios de furos/missing reviews.
     * @return O Aglomerado AssignmentResult detalhando as estatísticas do fluxo
     * @complexity O(S × R) no pior caso
     */
    AssignmentResult extractResult(const ParseResult& originalData) const;

    /**
     * @brief Método analítico para testar Vulnerabilidades da Conferência se K Revisores faltarem (Implementado K=1).
     * 
     * @param data O ParseResult que baliza o baseline de fluxo disponível original.
     * @return vector the IDs de revisores identificados como Single Points of Failure.
     */
    std::vector<int> riskAnalysis1(const ParseResult& data);

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