#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "DataTypes.h"

using namespace std;

/**
 * @brief Estrutura que encapsula apenas os dados lidos do ficheiro.
 */
struct ParseResult {
    vector<Submission> submissions;
    vector<Reviewer>   reviewers;
    Parameters         params;
    Control            ctrl;
    bool               success = true;
};

class Parser {
public:
    /**
     * @brief Lê o ficheiro e devolve a estrutura populada.
     */
    ParseResult parse(const string& filename) const;

private:
    void parseSubmissionLine(const vector<string>& fields, ParseResult& result) const;
    void parseReviewerLine  (const vector<string>& fields, ParseResult& result) const;
    void parseParameterLine (const vector<string>& fields, ParseResult& result) const;
    void parseControlLine   (const vector<string>& fields, ParseResult& result) const;

    vector<string> splitCSV(const string& line) const;
    string trim(const string& s) const;
    int toInt(const string& s) const;
};

#endif // PARSER_H