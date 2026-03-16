#ifndef SCIENTIFIC_CONFERENCE_ORGANIZATION_TOOL_P1_MENU_H
#define SCIENTIFIC_CONFERENCE_ORGANIZATION_TOOL_P1_MENU_H

#include <string>

/**
 * @brief Handles the interactive CLI menu and batch mode execution.
 *
 * Visual layer only — logic is injected later via handler methods.
 */
class Menu {
public:
    Menu();
    void run();

private:
    bool dataLoaded;
    std::string loadedFile;

    // --- Visual helpers ---
    void printHeader();
    int  showMainMenu();
    void showError(const std::string& msg);
    void showSuccess(const std::string& msg);
    void waitEnter();

    // --- Screen stubs (to be implemented with real logic later) ---
    void handleLoadFile();
    void handleShowData();
    void handleGenerateAssignments();
    void handleRiskAnalysis();
};

#endif //SCIENTIFIC_CONFERENCE_ORGANIZATION_TOOL_P1_MENU_H