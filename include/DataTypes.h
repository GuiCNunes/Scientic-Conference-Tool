#ifndef DATATYPES_H
#define DATATYPES_H

#include <string>
#include <vector>
#include <functional>

// ─────────────────────────────────────────────
//  Entidades do domínio
// ─────────────────────────────────────────────

struct Submission {
    int         id             = -1;
    std::string title;
    std::string authors;
    std::string email;
    int         primaryTopic   = -1;
    int         secondaryTopic = -1;  // -1 = não tem
};

struct Reviewer {
    int         id                 = -1;
    std::string name;
    std::string email;
    int         primaryExpertise   = -1;
    int         secondaryExpertise = -1;  // -1 = não tem
};

// ─────────────────────────────────────────────
//  Parâmetros lidos de #Parameters
// ─────────────────────────────────────────────

struct Parameters {
    int minReviewsPerSubmission    = 1;
    int maxReviewsPerReviewer      = 1;
    int primaryReviewerExpertise   = 1;
    int secondaryReviewerExpertise = 0;
    int primarySubmissionDomain    = 1;
    int secondarySubmissionDomain  = 0;
};

// ─────────────────────────────────────────────
//  Controlo lido de #Control
// ─────────────────────────────────────────────

enum class AssignmentMode {
    SILENT  = 0,  // corre mas não reporta
    PRIMARY = 1,  // só primary topics de ambos  (T2.1)
    MIXED   = 2,  // secondary dos papers + primary dos revisores
    FULL    = 3   // primary + secondary de ambos
};

struct Control {
    AssignmentMode generateAssignments = AssignmentMode::PRIMARY;
    int            riskAnalysis        = 0;
    std::string    outputFilename      = "output.csv";
};

// ─────────────────────────────────────────────
//  NodeInfo — "etiqueta" de cada nó na rede de fluxo
//
//  Definido AQUI e apenas aqui.
//  FlowNetwork.h inclui DataTypes.h e usa esta definição.
// ─────────────────────────────────────────────

enum class NodeType {
    SOURCE,      // nó artificial de origem (único)
    SINK,        // nó artificial de destino (único)
    SUBMISSION,  // um paper   (id = id do CSV)
    REVIEWER     // um revisor (id = id do CSV)
};

struct NodeInfo {
    NodeType type;
    int      id;   // 0 para SOURCE/SINK; id real do CSV para os restantes

    bool operator==(const NodeInfo& other) const {
        return type == other.type && id == other.id;
    }
};

// // Hash para NodeInfo — necessário para findVertex() funcionar no Graph<NodeInfo>
// namespace std {
//     template <>
//     struct hash<NodeInfo> {
//         size_t operator()(const NodeInfo& n) const {
//             return hash<int>()(static_cast<int>(n.type)) ^ (hash<int>()(n.id) << 1);
//         }
//     };
// }

// ─────────────────────────────────────────────
//  Resultado do assignment
// ─────────────────────────────────────────────

struct Assignment {
    int submissionId;
    int reviewerId;
    int matchedTopic;
};

struct MissingReview {
    int submissionId;
    int domain;
    int missingCount;
};

struct AssignmentResult {
    std::vector<Assignment>    assignments;
    std::vector<MissingReview> missing;
    int  totalFlow     = 0;
    int  totalRequired = 0;
    bool isComplete() const { return totalFlow >= totalRequired; }
};

#endif // DATATYPES_H