#include "../include/Menu.h"
#include "../include/FlowNetwork.h"
#include "../include/MaxFlow.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <unordered_map>
#include <map>

// ─────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────

Menu::Menu() : dataLoaded(false), loadedFile("") {}

// ─────────────────────────────────────────────
//  Entrada pública
// ─────────────────────────────────────────────

void Menu::run() {
    while (true) {
        printHeader();
        int choice = showMainMenu();
        switch (choice) {
            case 1: handleLoadFile();            break;
            case 2: handleShowData();            break;
            case 3: handleGenerateAssignments(); break;
            case 4: handleRiskAnalysis();        break;
            case 0:
                std::cout << "\n  Goodbye!\n\n";
                return;
            default:
                showError("Invalid option. Please try again.");
        }
    }
}

// ─────────────────────────────────────────────
//  Helpers visuais
// ─────────────────────────────────────────────

void Menu::printHeader() {
    std::cout << "\033[2J\033[H";
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║   Conference Review Assignment Tool      ║\n";
    if (!loadedFile.empty()) {
        std::string display = loadedFile.size() > 32
            ? "..." + loadedFile.substr(loadedFile.size() - 29)
            : loadedFile;
        std::cout << "║   File: " << display
                  << std::string(33 - display.size(), ' ') << "║\n";
    }
    std::cout << "╚══════════════════════════════════════════╝\n";
}

int Menu::showMainMenu() {
    std::cout << "\n";
    std::cout << "  [1] Load input file\n";
    std::cout << "  [2] Show loaded data\n";
    std::cout << "  [3] Generate assignments\n";
    std::cout << "  [4] Run risk analysis\n";
    std::cout << "  [0] Exit\n";
    std::cout << "\n  > ";

    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}

void Menu::showError(const std::string& msg) {
    std::cout << "\n  \033[31m[!] " << msg << "\033[0m\n";
    waitEnter();
}

void Menu::showSuccess(const std::string& msg) {
    std::cout << "\n  \033[32m[✓] " << msg << "\033[0m\n";
}

void Menu::waitEnter() {
    std::cout << "\n  Press Enter to continue...";
    std::cin.get();
}

// ─────────────────────────────────────────────
//  Screens
// ─────────────────────────────────────────────

void Menu::handleLoadFile() {
    printHeader();
    std::cout << "  Enter path to .csv file: ";
    std::string path;
    std::getline(std::cin, path);

    if (path.size() < 4 || path.substr(path.size() - 4) != ".csv") {
        showError("File must have a .csv extension.");
        return;
    }

    Parser parser;
    ParseResult result = parser.parse(path);

    if (!result.success) {
        showError("Failed to parse file. Check format and try again.");
        return;
    }

    data       = result;
    dataLoaded = true;
    loadedFile = path;

    showSuccess("Loaded " + std::to_string(data.submissions.size()) +
                " submissions and " + std::to_string(data.reviewers.size()) + " reviewers.");
    waitEnter();
}

void Menu::handleShowData() {
    if (!dataLoaded) { showError("No data loaded. Please load a file first."); return; }

    printHeader();

    std::cout << "  === Submissions (" << data.submissions.size() << ") ===\n";
    // Iterar diretamente sobre o std::map (já vem ordenado pelo ID!)
    for (const auto& [id, s] : data.submissions) {
        std::cout << "  [" << s.id << "] " << s.title
                  << " | Topic: " << s.primaryTopic;
        if (s.secondaryTopic != -1) std::cout << "/" << s.secondaryTopic;
        std::cout << "\n";
    }

    std::cout << "\n  === Reviewers (" << data.reviewers.size() << ") ===\n";
    // Iterar diretamente sobre o std::map (já vem ordenado pelo ID!)
    for (const auto& [id, r] : data.reviewers) {
        std::cout << "  [" << r.id << "] " << r.name
                  << " | Expertise: " << r.primaryExpertise;
        if (r.secondaryExpertise != -1) std::cout << "/" << r.secondaryExpertise;
        std::cout << "\n";
    }

    std::cout << "\n  === Parameters ===\n";
    std::cout << "  MinReviewsPerSubmission : " << data.params.minReviewsPerSubmission << "\n";
    std::cout << "  MaxReviewsPerReviewer   : " << data.params.maxReviewsPerReviewer   << "\n";

    std::cout << "\n  === Control ===\n";
    std::cout << "  GenerateAssignments : " << static_cast<int>(data.ctrl.generateAssignments) << "\n";
    std::cout << "  RiskAnalysis        : " << data.ctrl.riskAnalysis        << "\n";
    std::cout << "  OutputFileName      : " << data.ctrl.outputFilename      << "\n";

    waitEnter();
}

void Menu::handleGenerateAssignments() {
    if (!dataLoaded) { showError("No data loaded. Please load a file first."); return; }

    printHeader();
    std::cout << "  Building flow network...\n";

    // 1. Construir rede de fluxo com NodeInfo
    FlowNetwork network(data);

    std::cout << "  Running Edmonds-Karp...\n";

    // 2. Correr Edmonds-Karp — identifica nós pelo NodeInfo diretamente
    const NodeInfo source = {NodeType::SOURCE, 0};
    const NodeInfo sink   = {NodeType::SINK,   0};
    edmondsKarp(&network.getGraph(), source, sink);

    // 3. Extrair resultado das arestas com flow=1
    AssignmentResult result = network.extractResult(data);

    // 4. Mostrar no terminal
    std::cout << "\n  --- Resultado ---\n";
    std::cout << "  Fluxo obtido:     " << result.totalFlow     << "\n";
    std::cout << "  Total necessario: " << result.totalRequired << "\n";
    std::cout << "  Estado: "
              << (result.isComplete()
                  ? "\033[32mCOMPLETO\033[0m"
                  : "\033[31mINCOMPLETO\033[0m")
              << "\n";

    if (!result.assignments.empty()) {
        std::cout << "\n  #SubmissionId, ReviewerId, Topic\n";
        for (const auto& a : result.assignments)
            std::cout << "  " << a.submissionId
                      << ", " << a.reviewerId
                      << ", " << a.matchedTopic << "\n";
    }

    if (!result.missing.empty()) {
        std::cout << "\n  #SubmissionId, Domain, MissingReviews\n";
        for (const auto& m : result.missing)
            std::cout << "  " << m.submissionId
                      << ", " << m.domain
                      << ", " << m.missingCount << "\n";
    }

    // 5. Escrever ficheiro de output (a não ser que SILENT)
    if (data.ctrl.generateAssignments != AssignmentMode::SILENT) {
        writeOutput(result, data.ctrl.outputFilename);
        showSuccess("Output escrito em: " + data.ctrl.outputFilename);
    }

    waitEnter();
}

void Menu::handleRiskAnalysis() {
    if (!dataLoaded) { showError("No data loaded. Please load a file first."); return; }

    int K = data.ctrl.riskAnalysis;
    if (K == 0) {
        showError("RiskAnalysis = 0: nenhuma analise de risco pedida.");
        return;
    }

    printHeader();
    std::cout << "  Running risk analysis (K=" << K << ")...\n";
    // T2.3 (K>1)
    if (K == 1) {
        FlowNetwork network(data);
        std::vector<int> critical = network.riskAnalysis1(data);

        std::cout << "\n  #Risk Analysis: 1\n";
        if (critical.empty()) {
            std::cout << "  (no critical reviewer — assignment survives any single absence)\n";
        } else {
            std::cout << "  ";
            for (size_t i = 0; i < critical.size(); ++i) {
                std::cout << critical[i] << (i + 1 == critical.size() ? "" : ",");
            }
            std::cout << "\n";
        }

        if (data.ctrl.generateAssignments != AssignmentMode::SILENT) {
            std::ofstream out(data.ctrl.outputFilename, std::ios::app);
            out << "#Risk Analysis: 1\n";
            for (size_t i = 0; i < critical.size(); ++i) {
                out << critical[i] << (i + 1 == critical.size() ? "" : ",");
            }
            out << "\n";
        }
    } else {
        std::cout << "  K>1 não implementado\n";
    }

    waitEnter();
}

// ─────────────────────────────────────────────
//  writeOutput — formato do enunciado
// ─────────────────────────────────────────────

void Menu::writeOutput(const AssignmentResult& result, const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Erro: Nao foi possivel escrever em '" << filename << "'\n";
        return;
    }

    // Secção 1: por submission (Já vem ordenado do extractResult)
    out << "#SubmissionId,ReviewerId,Match\n";
    for (const auto& a : result.assignments) {
        out << a.submissionId << ", " << a.reviewerId << ", " << a.matchedTopic << "\n";
    }

    // Secção 2: por reviewer (inverso)
    // Mudámos para std::map -> As chaves (ReviewerId) ficam logo ordenadas!
    std::map<int, std::vector<std::pair<int,int>>> byReviewer;
    for (const auto& a : result.assignments) {
        // Como o 'result.assignments' vem ordenado por SubmissionId,
        // os push_back vão entrar naturalmente ordenados.
        byReviewer[a.reviewerId].push_back({a.submissionId, a.matchedTopic});
    }

    out << "#ReviewerId,SubmissionId,Match\n";
    for (const auto& [rid, entries] : byReviewer) {
        for (const auto& entry : entries) {
            out << rid << ", " << entry.first << ", " << entry.second << "\n";
        }
    }

    // Secção 3: Total
    out << "#Total: " << result.totalFlow << "\n";

    // Secção 4: Falhas (Imprime apenas se houver revisões em falta)
    if (!result.missing.empty()) {
        out << "#SubmissionId,Domain,MissingReviews\n";
        // Já vem ordenado do extractResult
        for (const auto& m : result.missing) {
            out << m.submissionId << ", " << m.domain << ", " << m.missingCount << "\n";
        }
    }
}