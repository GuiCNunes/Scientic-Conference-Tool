#ifndef DA_TP_CLASSES_GRAPH
#define DA_TP_CLASSES_GRAPH

// Original code by Gonçalo Leão
// Updated by DA 2024/2025 Team

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>

template <class T>
class Edge;

#define INF std::numeric_limits<double>::max()

/************************* Vertex  **************************/

template <class T>
class Vertex {
public:
    Vertex(T in);
    bool operator<(Vertex<T> & vertex) const;

    T getInfo() const;
    std::vector<Edge<T> *> getAdj() const;
    bool isVisited() const;
    bool isProcessing() const;
    unsigned int getIndegree() const;
    double getDist() const;
    Edge<T> *getPath() const;
    std::vector<Edge<T> *> getIncoming() const;

    void setInfo(T info);
    void setVisited(bool visited);
    void setProcessing(bool processing);
    void setIndegree(unsigned int indegree);
    void setDist(double dist);
    void setPath(Edge<T> *path);

    int getLow() const;
    void setLow(int value);
    int getNum() const;
    void setNum(int value);

    Edge<T> * addEdge(Vertex<T> *dest, double w);
    bool removeEdge(T in);
    void removeOutgoingEdges();

protected:
    T info;
    std::vector<Edge<T> *> adj;

    bool visited    = false;
    bool processing = false;
    int  low = -1, num = -1;
    unsigned int indegree = 0;
    double dist = 0;
    Edge<T> *path = nullptr;

    std::vector<Edge<T> *> incoming;

    int queueIndex = 0;

    void deleteEdge(Edge<T> *edge);
};

/********************** Edge  ****************************/

template <class T>
class Edge {
public:
    Edge(Vertex<T> *orig, Vertex<T> *dest, double w);

    Vertex<T> * getDest() const;
    double getWeight() const;
    bool isSelected() const;
    Vertex<T> * getOrig() const;
    Edge<T> *getReverse() const;
    double getFlow() const;

    void setSelected(bool selected);
    void setReverse(Edge<T> *reverse);
    void setFlow(double flow);

protected:
    Vertex<T> *orig;      // origem (declarado antes de dest — ordem do construtor)
    Vertex<T> *dest;
    double     weight;

    bool       selected = false;
    Edge<T>   *reverse  = nullptr;
    double     flow     = 0;
};

/********************** Graph  ****************************/

template <class T>
class Graph {
public:
    ~Graph();

    Vertex<T> *findVertex(const T &in) const;

    bool addVertex(const T &in);
    bool removeVertex(const T &in);

    bool addEdge(const T &sourc, const T &dest, double w);
    bool removeEdge(const T &source, const T &dest);
    bool addBidirectionalEdge(const T &sourc, const T &dest, double w);

    int getNumVertex() const;
    std::vector<Vertex<T> *> getVertexSet() const;

protected:
    std::vector<Vertex<T> *> vertexSet;

    double **distMatrix = nullptr;
    int    **pathMatrix = nullptr;

    int findVertexIdx(const T &in) const;
};

void deleteMatrix(int    **m, int n);
void deleteMatrix(double **m, int n);

/************************* Vertex  **************************/

template <class T>
Vertex<T>::Vertex(T in) : info(in) {}

template <class T>
Edge<T> * Vertex<T>::addEdge(Vertex<T> *d, double w) {
    auto newEdge = new Edge<T>(this, d, w);
    adj.push_back(newEdge);
    d->incoming.push_back(newEdge);
    return newEdge;
}

template <class T>
bool Vertex<T>::removeEdge(T in) {
    bool removedEdge = false;
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge<T> *edge = *it;
        Vertex<T> *dest = edge->getDest();
        if (dest->getInfo() == in) {
            it = adj.erase(it);
            deleteEdge(edge);
            removedEdge = true;
        } else {
            it++;
        }
    }
    return removedEdge;
}

template <class T>
void Vertex<T>::removeOutgoingEdges() {
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge<T> *edge = *it;
        it = adj.erase(it);
        deleteEdge(edge);
    }
}

template <class T>
bool Vertex<T>::operator<(Vertex<T> & vertex) const {
    return this->dist < vertex.dist;
}

template <class T> T                       Vertex<T>::getInfo()       const { return this->info; }
template <class T> std::vector<Edge<T>*>   Vertex<T>::getAdj()        const { return this->adj; }
template <class T> bool                    Vertex<T>::isVisited()      const { return this->visited; }
template <class T> bool                    Vertex<T>::isProcessing()   const { return this->processing; }
template <class T> unsigned int            Vertex<T>::getIndegree()    const { return this->indegree; }
template <class T> double                  Vertex<T>::getDist()        const { return this->dist; }
template <class T> Edge<T>*                Vertex<T>::getPath()        const { return this->path; }
template <class T> std::vector<Edge<T> *>  Vertex<T>::getIncoming()    const { return this->incoming; }
template <class T> int                     Vertex<T>::getLow()         const { return this->low; }
template <class T> int                     Vertex<T>::getNum()         const { return this->num; }

template <class T> void Vertex<T>::setInfo(T in)                      { this->info       = in; }
template <class T> void Vertex<T>::setVisited(bool visited)            { this->visited    = visited; }
template <class T> void Vertex<T>::setProcessing(bool processing)      { this->processing = processing; }
template <class T> void Vertex<T>::setIndegree(unsigned int indegree)  { this->indegree   = indegree; }
template <class T> void Vertex<T>::setDist(double dist)                { this->dist       = dist; }
template <class T> void Vertex<T>::setPath(Edge<T> *path)              { this->path       = path; }
template <class T> void Vertex<T>::setLow(int value)                   { this->low        = value; }
template <class T> void Vertex<T>::setNum(int value)                   { this->num        = value; }

template <class T>
void Vertex<T>::deleteEdge(Edge<T> *edge) {
    Vertex<T> *dest = edge->getDest();
    auto it = dest->incoming.begin();
    while (it != dest->incoming.end()) {
        if ((*it)->getOrig()->getInfo() == info) {
            it = dest->incoming.erase(it);
        } else {
            it++;
        }
    }
    delete edge;
}

/********************** Edge  ****************************/

template <class T>
Edge<T>::Edge(Vertex<T> *orig, Vertex<T> *dest, double w)
    : orig(orig), dest(dest), weight(w) {}

template <class T> Vertex<T>*  Edge<T>::getDest()    const { return this->dest; }
template <class T> double      Edge<T>::getWeight()  const { return this->weight; }
template <class T> Vertex<T>*  Edge<T>::getOrig()    const { return this->orig; }
template <class T> Edge<T>*    Edge<T>::getReverse() const { return this->reverse; }
template <class T> bool        Edge<T>::isSelected() const { return this->selected; }
template <class T> double      Edge<T>::getFlow()    const { return this->flow; }

template <class T> void Edge<T>::setSelected(bool selected)      { this->selected = selected; }
template <class T> void Edge<T>::setReverse(Edge<T> *reverse)    { this->reverse  = reverse; }
template <class T> void Edge<T>::setFlow(double flow)            { this->flow     = flow; }

/********************** Graph  ****************************/

template <class T>
int Graph<T>::getNumVertex() const { return vertexSet.size(); }

template <class T>
std::vector<Vertex<T> *> Graph<T>::getVertexSet() const { return vertexSet; }

template <class T>
Vertex<T> * Graph<T>::findVertex(const T &in) const {
    for (auto v : vertexSet)
        if (v->getInfo() == in)
            return v;
    return nullptr;
}

template <class T>
int Graph<T>::findVertexIdx(const T &in) const {
    for (unsigned i = 0; i < vertexSet.size(); i++)
        if (vertexSet[i]->getInfo() == in)
            return i;
    return -1;
}

template <class T>
bool Graph<T>::addVertex(const T &in) {
    if (findVertex(in) != nullptr) return false;
    vertexSet.push_back(new Vertex<T>(in));
    return true;
}

template <class T>
bool Graph<T>::removeVertex(const T &in) {
    for (auto it = vertexSet.begin(); it != vertexSet.end(); it++) {
        if ((*it)->getInfo() == in) {
            auto v = *it;
            v->removeOutgoingEdges();
            for (auto u : vertexSet) u->removeEdge(v->getInfo());
            vertexSet.erase(it);
            delete v;
            return true;
        }
    }
    return false;
}

template <class T>
bool Graph<T>::addEdge(const T &sourc, const T &dest, double w) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr) return false;
    v1->addEdge(v2, w);
    return true;
}

template <class T>
bool Graph<T>::removeEdge(const T &sourc, const T &dest) {
    Vertex<T> *srcVertex = findVertex(sourc);
    if (srcVertex == nullptr) return false;
    return srcVertex->removeEdge(dest);
}

template <class T>
bool Graph<T>::addBidirectionalEdge(const T &sourc, const T &dest, double w) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr) return false;
    auto e1 = v1->addEdge(v2, w);
    auto e2 = v2->addEdge(v1, w);
    e1->setReverse(e2);
    e2->setReverse(e1);
    return true;
}

template <class T>
Graph<T>::~Graph() {
    deleteMatrix(distMatrix, vertexSet.size());
    deleteMatrix(pathMatrix, vertexSet.size());
}

inline void deleteMatrix(int **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++) if (m[i] != nullptr) delete [] m[i];
        delete [] m;
    }
}

inline void deleteMatrix(double **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++) if (m[i] != nullptr) delete [] m[i];
        delete [] m;
    }
}

#endif /* DA_TP_CLASSES_GRAPH */