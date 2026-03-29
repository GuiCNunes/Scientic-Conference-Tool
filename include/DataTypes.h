#ifndef DATATYPES_H
#define DATATYPES_H

#include <string>
#include <vector>
#include <functional>

// ─────────────────────────────────────────────
//  Entidades do domínio
// ─────────────────────────────────────────────

/**
 * @brief Representa um paper submetido à conferência.
 */
struct Submission {
    int         id             = -1;   ///< Identificador único do paper
    std::string title;                 ///< Título do paper
    std::string authors;               ///< Lista de autores
    std::string email;                 ///< Email de contacto
    int         primaryTopic   = -1;   ///< Domínio principal de investigação
    int         secondaryTopic = -1;   ///< Domínio secundário (-1 se não existir)
};

/**
 * @brief Representa um revisor disponível para avaliar papers.
 */
struct Reviewer {
    int         id                 = -1;   ///< Identificador único do revisor
    std::string name;                      ///< Nome do revisor
    std::string email;                     ///< Email de contacto
    int         primaryExpertise   = -1;   ///< Domínio de expertise principal
    int         secondaryExpertise = -1;   ///< Domínio secundário (-1 se não existir)
};

// ─────────────────────────────────────────────
//  Parâmetros lidos de #Parameters
// ─────────────────────────────────────────────

/**
 * @brief Regula as exigências numéricas do assignment.
 */
struct Parameters {
    int minReviewsPerSubmission    = 1; ///< Numero mínimo de avaliações exigidas por paper
    int maxReviewsPerReviewer      = 1; ///< Limite de papers que cada revisor pode avaliar
    int primaryReviewerExpertise   = 1; ///< Peso do domínio primário do revisor
    int secondaryReviewerExpertise = 0; ///< Peso do domínio secundário do revisor
    int primarySubmissionDomain    = 1; ///< Peso do domínio primário da submissão
    int secondarySubmissionDomain  = 0; ///< Peso do domínio secundário da submissão
};

// ─────────────────────────────────────────────
//  Controlo lido de #Control
// ─────────────────────────────────────────────

/**
 * @brief Define os Modos de emparelhamento possíveis aplicados às regras de compatibilidade.
 */
enum class AssignmentMode {
    SILENT  = 0,  ///< Corre matching primário mas não exporta resultados para o terminal/ficheiro
    PRIMARY = 1,  ///< Exige full match entre primary topic (paper) e primary expertise (revisor)
    MIXED   = 2,  ///< Permite cruzar secondary topic do paper com primary do revisor
    FULL    = 3   ///< Permite matching total cobrindo qualquer combinação primary/secondary
};

/**
 * @brief Variáveis de controlo do fluxo e análise de restrições.
 */
struct Control {
    AssignmentMode generateAssignments = AssignmentMode::PRIMARY; ///< Regra de match aplicada
    int            riskAnalysis        = 0;                       ///< Grau/Fator multiplicador do risk analysis
    std::string    outputFilename      = "output.csv";            ///< Destino dos logs de matching
};

// ─────────────────────────────────────────────
//  NodeInfo — "etiqueta" de cada nó na rede de fluxo
//
//  Definido AQUI e apenas aqui.
//  FlowNetwork.h inclui DataTypes.h e usa esta definição.
// ─────────────────────────────────────────────

/**
 * @brief Enumerador interno que distingue a classe da qual um nó do grafo pertence.
 */
enum class NodeType {
    SOURCE,      ///< Nó de Injeção
    SINK,        ///< Nó Terminal de Absorção
    SUBMISSION,  ///< Nó representante de um Paper/Submission
    REVIEWER     ///< Nó representante de um Revisor
};

/**
 * @brief Agrupa as características representativas de cada vertex da Rede de Fluxo (Graph).
 */
struct NodeInfo {
    NodeType type; ///< Classe do nó
    int      id;   ///< Identificador. 0 para SOURCE/SINK, ID do CSV para sub/rev

    /**
     * @brief Compara este objecto a outro no mapeamento.
     */
    bool operator==(const NodeInfo& other) const {
        return type == other.type && id == other.id;
    }
};

// ─────────────────────────────────────────────
//  Resultado do assignment
// ─────────────────────────────────────────────

/**
 * @brief Representante minimalista e logavel de que um Paper encontra o seu Revisor.
 */
struct Assignment {
    int submissionId; ///< ID do paper a avaliar
    int reviewerId;   ///< ID do revisor responsável
    int matchedTopic; ///< O tópico que originou a ligação
};

/**
 * @brief Identifica uma falha em satisfazer o constraint Mínimo de reviews.
 */
struct MissingReview {
    int submissionId; ///< Id do paper prejudicado
    int domain;       ///< Domínio no qual foi tentado o match
    int missingCount; ///< Diff das reviews necessárias face às avaliações realmente arranjadas
};

/**
 * @brief Coleção gerada pelo extractResult com sumário estatístico do Max-Flow.
 */
struct AssignmentResult {
    std::vector<Assignment>    assignments;   ///< Array com ligações concretizadas 
    std::vector<MissingReview> missing;       ///< Furos registados (Submissões com faltas)
    int  totalFlow     = 0;                   ///< Escoamento conseguido total da SOURCE à SINK
    int  totalRequired = 0;                   ///< Restrição total da exigência do grafo

    /**
     * @brief Verifica se não houve constrição na atribuição (Flow == Demanda)
     */
    bool isComplete() const { return totalFlow >= totalRequired; }
};

#endif // DATATYPES_H