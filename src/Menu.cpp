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
    std::cout << "\033[2J\033[H"; // clear screen
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
//  Screen stubs — replace with real logic later
// ─────────────────────────────────────────────

void Menu::handleLoadFile() {
    printHeader();
    std::cout << "  Enter path to .csv file: ";
    std::string path;
    std::getline(std::cin, path);

    // TODO: call parser.load(path)
    showSuccess("File loaded: " + path);
    dataLoaded = true;
    loadedFile = path;
    waitEnter();
}

void Menu::handleShowData() {
    if (!dataLoaded) { showError("No data loaded. Please load a file first."); return; }
    printHeader();
    std::cout << "  === Submissions ===\n";
    std::cout << "  (none yet)\n";
    std::cout << "\n  === Reviewers ===\n";
    std::cout << "  (none yet)\n";
    std::cout << "\n  === Parameters ===\n";
    std::cout << "  (none yet)\n";
    waitEnter();
}

void Menu::handleGenerateAssignments() {
    if (!dataLoaded) { showError("No data loaded. Please load a file first."); return; }
    printHeader();
    std::cout << "  Building flow network...\n";
    // TODO: call flowNetwork.build() and MaxFlow::run()
    std::cout << "  Max flow value: (not implemented)\n";
    waitEnter();
}

void Menu::handleRiskAnalysis() {
    if (!dataLoaded) { showError("No data loaded. Please load a file first."); return; }
    printHeader();
    std::cout << "  Running risk analysis...\n";
    // TODO: call RiskAnalysis::run()
    std::cout << "  Result: (not implemented)\n";
    waitEnter();
}