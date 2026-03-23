#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "DataTypes.h"

// ─────────────────────────────────────────────
//  ParseResult — dados lidos do ficheiro CSV
//
//  submissions e reviewers são hashmaps id→struct
//  para acesso O(1) por id em vez de O(n) com vector
// ─────────────────────────────────────────────

struct ParseResult {
    std::unordered_map<int, Submission> submissions;  // id → Submission
    std::unordered_map<int, Reviewer>   reviewers;    // id → Reviewer
    Parameters                          params;
    Control                             ctrl;
    bool                                success = true;
};

// ─────────────────────────────────────────────
//  Parser
// ─────────────────────────────────────────────

class Parser {
public:
    /**
     * @brief Lê e parseia um ficheiro .csv no formato do projeto.
     * @param filename Caminho para o ficheiro .csv
     * @return ParseResult com os dados lidos. success=false se houve erros críticos.
     * @complexity O(N) onde N é o número de linhas do ficheiro
     */
    ParseResult parse(const std::string& filename) const;

private:
    // --- Parsers por secção ---
    void parseSubmissionLine(const std::vector<std::string>& fields, ParseResult& result) const;
    void parseReviewerLine  (const std::vector<std::string>& fields, ParseResult& result) const;
    void parseParameterLine (const std::vector<std::string>& fields, ParseResult& result) const;
    void parseControlLine   (const std::vector<std::string>& fields, ParseResult& result) const;

    // --- Utilitários ---
    std::vector<std::string> splitCSV(const std::string& line) const;
    std::string trim(const std::string& s) const;
    int toInt(const std::string& s) const;
};

#endif // PARSER_H
