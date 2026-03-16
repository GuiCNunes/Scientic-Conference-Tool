#include "../include/Parser.h"
#include <fstream>
#include <iostream>

using namespace std;

ParseResult Parser::parse(const string& filename) const {
    ParseResult res;

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o ficheiro '" << filename << "'!" << endl;
        res.success = false;
        return res;
    }

    string line;
    string currentSection = "NONE";

    while (getline(file, line)) {
        // retirar espaços do inicio e fim da linha
        string trimmedRaw = trim(line);
        if (trimmedRaw == "#Submissions") { currentSection = "SUBMISSIONS"; continue; }
        if (trimmedRaw == "#Reviewers")   { currentSection = "REVIEWERS";   continue; }
        if (trimmedRaw == "#Parameters")  { currentSection = "PARAMETERS";  continue; }
        if (trimmedRaw == "#Control")     { currentSection = "CONTROL";     continue; }

        if (!trimmedRaw.empty() && trimmedRaw[0] == '#') continue;

        // tirar comments do excel (# fora de aspas)
        bool inQ = false;
        size_t hashPos = string::npos;
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == '"') inQ = !inQ;
            else if (line[i] == '#' && !inQ) { hashPos = i; break; }
        }

        //linha vai de 0 até ao final dos campos (sem incluir comentários)
        if (hashPos != string::npos)
            line = line.substr(0, hashPos);

        line = trim(line);
        if (line.empty()) continue;

        // mete cada valor(field) dessa linha do excel num vetor
        vector<string> fields = splitCSV(line);

        // chama para a secção atual uma função que atribui os fields a uma struct
        if      (currentSection == "SUBMISSIONS") parseSubmissionLine(fields, res);
        else if (currentSection == "REVIEWERS")   parseReviewerLine(fields, res);
        else if (currentSection == "PARAMETERS")  parseParameterLine(fields, res);
        else if (currentSection == "CONTROL")     parseControlLine(fields, res);
        else {
            cerr << "Aviso: Dados ignorados pois nao estao dentro de nenhuma seccao: " << line << endl;
        }
    }

    file.close();
    return res;
}


void Parser::parseSubmissionLine(const vector<string>& fields, ParseResult& result) const {
    if (fields.size() < 5) {
        cerr << "Erro: Submissao com campos insuficientes. Linha ignorada." << endl;
        return;
    }

    Submission s;
    s.id             = toInt(fields[0]);
    s.title          = fields[1];
    s.authors        = fields[2];
    s.email          = fields[3];
    s.primaryTopic   = toInt(fields[4]);
    //se houver uma sexta coluna (tópico secundário) e ela não estiver vazia guardamos o field
    s.secondaryTopic = (fields.size() >= 6 && !fields[5].empty()) ? toInt(fields[5]) : -1;

    // ver ID duplicado
    for (const auto& existing : result.submissions) {
        if (existing.id == s.id) {
            cerr << "Erro: ID de submissao duplicado (" << s.id << "). Linha ignorada." << endl;
            result.success = false;
            return;
        }
    }

    result.submissions.push_back(s);
}

void Parser::parseReviewerLine(const vector<string>& fields, ParseResult& result) const {
    if (fields.size() < 4) {
        cerr << "Erro: Revisor com campos insuficientes. Linha ignorada." << endl;
        return;
    }

    Reviewer r;
    r.id                 = toInt(fields[0]);
    r.name               = fields[1];
    r.email              = fields[2];
    r.primaryExpertise   = toInt(fields[3]);
    //se houver uma quinta coluna (expertise secundária) e ela não estiver vazia guardamos o field
    r.secondaryExpertise = (fields.size() >= 5 && !fields[4].empty()) ? toInt(fields[4]) : -1;

    // ver ID duplicado
    for (const auto& existing : result.reviewers) {
        if (existing.id == r.id) {
            cerr << "Erro: ID de revisor duplicado (" << r.id << "). Linha ignorada." << endl;
            result.success = false;
            return;
        }
    }

    result.reviewers.push_back(r);
}

void Parser::parseParameterLine(const vector<string>& fields, ParseResult& result) const {
    if (fields.size() < 2) {
        cerr << "Erro: Parametro incompleto. Linha ignorada." << endl;
        return;
    }

    string key   = trim(fields[0]);
    string value = trim(fields[1]);
    Parameters& p = result.params;

    if      (key == "MinReviewsPerSubmission")    p.minReviewsPerSubmission    = toInt(value);
    else if (key == "MaxReviewsPerReviewer")      p.maxReviewsPerReviewer      = toInt(value);
    else if (key == "PrimaryReviewerExpertise")   p.primaryReviewerExpertise   = toInt(value);
    else if (key == "SecondaryReviewerExpertise") p.secondaryReviewerExpertise = toInt(value);
    else if (key == "PrimarySubmissionDomain")    p.primarySubmissionDomain    = toInt(value);
    else if (key == "SecondarySubmissionDomain")  p.secondarySubmissionDomain  = toInt(value);
    else cerr << "Aviso: Parametro desconhecido ignorado -> " << key << endl;
}

void Parser::parseControlLine(const vector<string>& fields, ParseResult& result) const {
    if (fields.size() < 2) {
        cerr << "Erro: Comando de controlo incompleto. Linha ignorada." << endl;
        return;
    }

    string key   = trim(fields[0]);
    string value = trim(fields[1]);
    Control& c = result.ctrl;

    if (key == "GenerateAssignments") {
        int v = toInt(value);
        if (v < 0 || v > 3) {
            cerr << "Erro: Valor invalido para GenerateAssignments (" << v << "). Deve ser 0, 1, 2 ou 3." << endl;
            result.success = false;
        } else {
            c.generateAssignments = static_cast<AssignmentMode>(v);
        }
    }
    else if (key == "RiskAnalysis") {
        int v = toInt(value);
        if (v < 0) {
            cerr << "Erro: Valor invalido para RiskAnalysis (" << v << "). Deve ser >= 0." << endl;
            result.success = false;
        } else {
            c.riskAnalysis = v;
        }
    }
    else if (key == "OutputFileName") c.outputFilename = value;
    else cerr << "Aviso: Comando de controlo desconhecido ignorado -> " << key << endl;
}


vector<string> Parser::splitCSV(const string& line) const {
    vector<string> fields;
    string field;
    bool inQuotes = false;

    //
    for (char c : line) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(trim(field));
            field.clear();
        } else {
            field += c;
        }
    }

    string last = trim(field);
    fields.push_back(last);

    return fields;
}

string Parser::trim(const string& s) const {
    // apenas whitespace — as aspas são tratadas no splitCSV
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    string result = s.substr(start, end - start + 1);
    // remover aspas externas se o campo estiver completamente entre aspas
    if (result.size() >= 2 && result.front() == '"' && result.back() == '"')
        result = result.substr(1, result.size() - 2);
    return result;
}

int Parser::toInt(const string& s) const {
    try {
        return stoi(s);
    } catch (...) {
        cerr << "Erro: Falha ao converter '" << s << "' para numero. Assumido o valor 0." << endl;
        return 0;
    }
}