#include "Menu.h"
#include <iostream>
#include <limits>

// ─────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────

Menu::Menu() : dataLoaded(false), loadedFile("") {}

// ─────────────────────────────────────────────
//  Public entry point
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
//  Visual helpers
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
    for (const auto& s : data.submissions) {
        std::cout << "  [" << s.id << "] " << s.title
                  << " | Topic: " << s.primaryTopic;
        if (s.secondaryTopic != -1)
            std::cout << "/" << s.secondaryTopic;
        std::cout << "\n";
    }

    std::cout << "\n  === Reviewers (" << data.reviewers.size() << ") ===\n";
    for (const auto& r : data.reviewers) {
        std::cout << "  [" << r.id << "] " << r.name
                  << " | Expertise: " << r.primaryExpertise;
        if (r.secondaryExpertise != -1)
            std::cout << "/" << r.secondaryExpertise;
        std::cout << "\n";
    }

    std::cout << "\n  === Parameters ===\n";
    std::cout << "  MinReviewsPerSubmission : " << data.params.minReviewsPerSubmission << "\n";
    std::cout << "  MaxReviewsPerReviewer   : " << data.params.maxReviewsPerReviewer   << "\n";

    std::cout << "\n  === Control ===\n";
    std::cout << "  GenerateAssignments : " << data.ctrl.generateAssignments << "\n";
    std::cout << "  RiskAnalysis        : " << data.ctrl.riskAnalysis        << "\n";
    std::cout << "  OutputFileName      : " << data.ctrl.outputFilename      << "\n";

    waitEnter();
}

void Menu::handleGenerateAssignments() {
    if (!dataLoaded) { showError("No data loaded. Please load a file first."); return; }
    printHeader();
    std::cout << "  Building flow network...\n";
    // TODO: wire FlowNetwork + MaxFlow
    std::cout << "  Max flow value: (not implemented)\n";
    waitEnter();
}

void Menu::handleRiskAnalysis() {
    if (!dataLoaded) { showError("No data loaded. Please load a file first."); return; }
    printHeader();
    std::cout << "  Running risk analysis (K=" << data.ctrl.riskAnalysis << ")...\n";
    // TODO: wire RiskAnalysis
    std::cout << "  Result: (not implemented)\n";
    waitEnter();
}