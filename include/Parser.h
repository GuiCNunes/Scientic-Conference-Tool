#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "DataTypes.h"

/**
 * @brief Estrutura que encapsula apenas os dados lidos do ficheiro.
 */
struct ParseResult {
    std::vector<Submission> submissions;
    std::vector<Reviewer>   reviewers;
    Parameters              params;
    Control                 ctrl;
    bool                    success = true;
};

class Parser {
public:
    /**
     * @brief Lê o ficheiro e devolve a estrutura populada.
     */
    ParseResult parse(const std::string& filename) const;

private:
    void parseSubmissionLine(const std::vector<std::string>& fields, ParseResult& result) const;
    void parseReviewerLine  (const std::vector<std::string>& fields, ParseResult& result) const;
    void parseParameterLine (const std::vector<std::string>& fields, ParseResult& result) const;
    void parseControlLine   (const std::vector<std::string>& fields, ParseResult& result) const;

    std::vector<std::string> splitCSV(const std::string& line) const;
    std::string trim(const std::string& s) const;
    int toInt(const std::string& s) const;
};

#endif // PARSER_H