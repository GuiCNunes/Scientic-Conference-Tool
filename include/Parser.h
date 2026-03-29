#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <map>
#include "DataTypes.h"

// ─────────────────────────────────────────────
//  ParseResult — dados lidos do ficheiro CSV
//
//  submissions e reviewers são hashmaps id→struct
//  para acesso O(1) por id em vez de O(n) com vector
// ─────────────────────────────────────────────

struct ParseResult {
    std::map<int, Submission> submissions;  // id → Submission
    std::map<int, Reviewer>   reviewers;    // id → Reviewer
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
    /** @brief Processa um input da área #Submissions e adiciona ao map */
    void parseSubmissionLine(const std::vector<std::string>& fields, ParseResult& result) const;
    /** @brief Processa um input correspondente a um Reviewer e insere-o */
    void parseReviewerLine  (const std::vector<std::string>& fields, ParseResult& result) const;
    /** @brief Transfere chaves/valores da secção Params para as structs adequadas */
    void parseParameterLine (const std::vector<std::string>& fields, ParseResult& result) const;
    /** @brief Regula a Enum class associada ao Control a partir do input de ficheiro */
    void parseControlLine   (const std::vector<std::string>& fields, ParseResult& result) const;

    // --- Utilitários ---
    /** @brief Desdobra uma de múltiplas linhas baseada num delineador por vírgula CSV ignoring quotes internas */
    std::vector<std::string> splitCSV(const std::string& line) const;
    /** @brief Corta white-spaces, tabs, e newlines nas extremidades da string fornecida */
    std::string trim(const std::string& s) const;
    /** @brief Safely converte std::string em inteiros com try/catch */
    int toInt(const std::string& s) const;
};

#endif // PARSER_H
