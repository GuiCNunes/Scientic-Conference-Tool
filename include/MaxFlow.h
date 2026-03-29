#ifndef MAXFLOW_H
#define MAXFLOW_H

#include "Graph.h"
#include <queue>
#include <algorithm>

/**
 * @brief Implementação de Edmonds-Karp (Max-Flow com BFS).
 *
 * IMPORTANTE: por ser código template, toda a implementação fica neste .h.
 * Não existe MaxFlow.cpp — o compilador precisa de ver a implementação
 * completa para cada tipo T instanciado.
 *
 * Grafo residual:
 *   Não construímos um grafo separado. Usamos as arestas existentes:
 *   - Forward  (v->getAdj()):      residual = weight - flow
 *   - Backward (v->getIncoming()): residual = flow  (fluxo que podemos devolver)
 *
 * @complexity O(V × E²)
 */

// ─────────────────────────────────────────────
//  testAndVisit
// ─────────────────────────────────────────────

/**
 * @brief Tenta adicionar 'w' à fila do BFS se não foi visitado e há capacidade residual.
 * 
 * Marca o nó adjacente como visitado, define o caminho reverso na aresta
 * e injeta a referência no queue.
 * 
 * @param q Fila do BFS atual
 * @param e Aresta pelo qual o nó w foi alcançado
 * @param w O nó destino a considerar
 * @param residual Flow em excesso ou capacidade que permite passar a aresta
 */
template <class T>
void testAndVisit(std::queue<Vertex<T>*>& q, Edge<T>* e, Vertex<T>* w, double residual) {
    if (!w->isVisited() && residual > 0) {
        w->setVisited(true);
        w->setPath(e);   // guarda a aresta para reconstruir o caminho
        q.push(w);
    }
}

// ─────────────────────────────────────────────
//  findAugmentingPath — BFS
// ─────────────────────────────────────────────

/**
 * @brief BFS no grafo residual de s até t.
 *
 * Para cada vértice visitado, setPath() guarda a aresta pela qual chegámos.
 * Assim podemos reconstruir o caminho completo a partir do SINK usando O(V).
 *
 * @param g Referência do Grafo
 * @param s Nó fonte de injeção
 * @param t Nó terminal ou SINK
 * 
 * @return true se encontrou caminho de s até t
 * @complexity O(V + E)
 */
template <class T>
bool findAugmentingPath(Graph<T>* g, Vertex<T>* s, Vertex<T>* t) {
    for (auto* v : g->getVertexSet())
        v->setVisited(false);

    s->setVisited(true);
    std::queue<Vertex<T>*> q;
    q.push(s);

    while (!q.empty()) {
        auto* v = q.front();
        q.pop();

        // Arestas forward: residual = weight - flow
        for (auto* e : v->getAdj())
            testAndVisit(q, e, e->getDest(), e->getWeight() - e->getFlow());

        // Arestas backward (via incoming): residual = flow
        for (auto* e : v->getIncoming())
            testAndVisit(q, e, e->getOrig(), e->getFlow());

        if (t->isVisited()) return true;
    }
    return false;
}

// ─────────────────────────────────────────────
//  findMinResidualAlongPath — bottleneck
// ─────────────────────────────────────────────

/**
 * @brief Percorre o caminho de t até s e encontra o mínimo residual (bottleneck).
 * 
 * Usando o rastro de `getPath()` percorre transversalmente o percurso de SINK até
 * SOURCE registando qual delas permitiu o escoamento bloqueável minoritário.
 * 
 * @param s Nó Origem (SOURCE)
 * @param t Nó Destino (SINK)
 * @return A flutuação máxima do fluxo sem estourar as capacidades limitadas.
 * @complexity O(V)
 */
template <class T>
double findMinResidualAlongPath(Vertex<T>* s, Vertex<T>* t) {
    double f = INF;
    auto* cur = t;

    while (cur != s) {
        auto* e = cur->getPath();
        if (e->getDest() == cur) {
            // aresta forward
            f   = std::min(f, e->getWeight() - e->getFlow());
            cur = e->getOrig();
        } else {
            // aresta backward
            f   = std::min(f, e->getFlow());
            cur = e->getDest();
        }
    }
    return f;
}

// ─────────────────────────────────────────────
//  augmentFlowAlongPath — aplica fluxo
// ─────────────────────────────────────────────

/**
 * @brief Aplica o fluxo f ao caminho de t até s.
 *
 * Ajusta gradualmente as orientações e o estofo dos flow para preparar The Residual Graph:
 * - Forward:  flow += f  (enviar fluxo de S -> T)
 * - Backward: flow -= f  (devolver fluxo — "desfazer" decisão de Flow anterior)
 *
 * @param s Nó Origem (SOURCE)
 * @param t Nó Destino (SINK)
 * @param f Incremento calculado do bottleneck Flow a somar aos caminhos
 * @complexity O(V)
 */
template <class T>
void augmentFlowAlongPath(Vertex<T>* s, Vertex<T>* t, double f) {
    auto* cur = t;

    while (cur != s) {
        auto* e = cur->getPath();
        if (e->getDest() == cur) {
            e->setFlow(e->getFlow() + f);
            cur = e->getOrig();
        } else {
            e->setFlow(e->getFlow() - f);
            cur = e->getDest();
        }
    }
}

// ─────────────────────────────────────────────
//  edmondsKarp — algoritmo principal
// ─────────────────────────────────────────────

/**
 * @brief Corre Edmonds-Karp no grafo g de sourceInfo até sinkInfo.
 *
 * Após esta função, cada aresta tem o seu campo flow com o valor final.
 * Usa BFS para garantir complexidade O(V × E²) — melhor que Ford-Fulkerson genérico.
 *
 * @param g          Ponteiro para o grafo
 * @param sourceInfo NodeInfo do nó de origem  ex: {NodeType::SOURCE, 0}
 * @param sinkInfo   NodeInfo do nó de destino ex: {NodeType::SINK, 0}
 * @complexity O(V × E²)
 */
template <class T>
void edmondsKarp(Graph<T>* g, const T& sourceInfo, const T& sinkInfo) {
    auto* s = g->findVertex(sourceInfo);
    auto* t = g->findVertex(sinkInfo);

    if (s == nullptr || t == nullptr) return;

    // Reset ao fluxo
    for (auto* v : g->getVertexSet())
        for (auto* e : v->getAdj())
            e->setFlow(0);

    // Enquanto houver caminho aumentante: augmentar
    while (findAugmentingPath(g, s, t)) {
        double bottleneck = findMinResidualAlongPath(s, t);
        augmentFlowAlongPath(s, t, bottleneck);
    }
}

#endif // MAXFLOW_H