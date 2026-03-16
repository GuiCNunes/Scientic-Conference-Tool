//
// Created by user on 16-03-2026.
//
#include <iostream>
#include "../include/Parser.h"

using namespace std;

// Converte AssignmentMode para string legível
static string assignmentModeToString(AssignmentMode mode) {
    switch (mode) {
        case AssignmentMode::REGULAR:  return "0 (SILENT)";
        case AssignmentMode::PRIMARY: return "1 (PRIMARY)";
        case AssignmentMode::MIXED:   return "2 (MIXED)";
        case AssignmentMode::FULL:    return "3 (FULL)";
        default:                      return "desconhecido";
    }
}

int main() {
    Parser parser;
    ParseResult result = parser.parse("/home/user/DA/Scientific-Conference-Organization-Tool-P1/dataset1.csv");

    if (!result.success) {
        cout << "Falha ao carregar o ficheiro dataset1.csv" << endl;
        return 1;
    }

    cout << "--- RESULTADOS DO PARSER ---" << endl;
    cout << "Submissoes lidas: " << result.submissions.size() << endl;
    cout << "Revisores lidos:  " << result.reviewers.size()   << endl;

    cout << "\n--- PARAMETROS ---" << endl;
    cout << "MinReviewsPerSubmission:    " << result.params.minReviewsPerSubmission    << endl;
    cout << "MaxReviewsPerReviewer:      " << result.params.maxReviewsPerReviewer      << endl;
    cout << "PrimaryReviewerExpertise:   " << result.params.primaryReviewerExpertise   << endl;
    cout << "SecondaryReviewerExpertise: " << result.params.secondaryReviewerExpertise << endl;
    cout << "PrimarySubmissionDomain:    " << result.params.primarySubmissionDomain    << endl;
    cout << "SecondarySubmissionDomain:  " << result.params.secondarySubmissionDomain  << endl;

    cout << "\n--- CONTROLO ---" << endl;
    cout << "GenerateAssignments: " << assignmentModeToString(result.ctrl.generateAssignments) << endl;
    cout << "RiskAnalysis:        " << result.ctrl.riskAnalysis   << endl;
    cout << "OutputFileName:      " << result.ctrl.outputFilename << endl;

    cout << "\n--- SUBMISSOES ---" << endl;
    for (const auto& s : result.submissions) {
        cout << "  [" << s.id << "] " << s.title
             << " | Autores: " << s.authors
             << " | Primary: " << s.primaryTopic;
        if (s.secondaryTopic != -1)
            cout << " | Secondary: " << s.secondaryTopic;
        cout << endl;
    }

    cout << "\n--- REVISORES ---" << endl;
    for (const auto& r : result.reviewers) {
        cout << "  [" << r.id << "] " << r.name
             << " | Primary: " << r.primaryExpertise;
        if (r.secondaryExpertise != -1)
            cout << " | Secondary: " << r.secondaryExpertise;
        cout << endl;
    }

    return 0;
}
