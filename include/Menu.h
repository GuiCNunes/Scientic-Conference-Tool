#ifndef SCIENTIFIC_CONFERENCE_ORGANIZATION_TOOL_P1_MENU_H
#define SCIENTIFIC_CONFERENCE_ORGANIZATION_TOOL_P1_MENU_H

#include <string>
#include "Parser.h"

/**
 * @brief Handles the interactive CLI menu and batch mode execution.
 */
class Menu {
public:
    Menu();
    void run();

private:
    bool dataLoaded;
    std::string loadedFile;
    ParseResult data;   // populated by handleLoadFile

    // --- Visual helpers ---
    void printHeader();
    int  showMainMenu();
    void showError(const std::string& msg);
    void showSuccess(const std::string& msg);
    void waitEnter();

    // --- Screens ---
    void handleLoadFile();
    void handleShowData();
    void handleGenerateAssignments();
    void handleRiskAnalysis();
};

#endif //SCIENTIFIC_CONFERENCE_ORGANIZATION_TOOL_P1_MENU_H