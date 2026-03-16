//
// Created by user on 16-03-2026.
//
#include <iostream>
#include "../include/Parser.h"
#include "Parser.cpp"
using namespace std;

int main() {
    Parser parser;
    ParseResult result = parser.parse("/home/user/DA/Scientific-Conference-Organization-Tool-P1/dataset1.csv");

    if (!result.success) {
        cout << "Falha ao carregar o ficheiro dataset1.csv" << endl;
        return 1;
    }

    cout << "--- RESULTADOS DO PARSER ---" << endl;
    cout << "Submissoes lidas: " << result.submissions.size() << endl;
    cout << "Revisores lidos: " << result.reviewers.size() << endl;
    cout << "\n--- PARAMETROS ---" << endl;
    cout << "MinReviewsPerSubmission: " << result.params.minReviewsPerSubmission << endl;
    cout << "MaxReviewsPerReviewer: " << result.params.maxReviewsPerReviewer << endl;
    cout << "\n--- CONTROLO ---" << endl;
    cout << "GenerateAssignments: " << result.ctrl.generateAssignments << endl;
    cout << "RiskAnalysis: " << result.ctrl.riskAnalysis << endl;
    cout << "OutputFileName: " << result.ctrl.outputFilename << endl;

    if (!result.submissions.empty()) {
        cout << "\nExemplo da primeira submissao:" << endl;
        cout << "ID: " << result.submissions[0].id << ", Titulo: " << result.submissions[0].title << endl;
    }
    if (!result.reviewers.empty()) {
        cout << "Exemplo do primeiro revisor:" << endl;
        cout << "ID: " << result.reviewers[0].id << ", Nome: " << result.reviewers[0].name << endl;
    }

    return 0;
}
