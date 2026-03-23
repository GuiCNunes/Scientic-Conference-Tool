#ifndef MENU_H
#define MENU_H

#include <string>
#include "Parser.h"

/**
 * @brief Gere o menu interativo e a execução batch.
 */
class Menu {
public:
    Menu();
    void run();

private:
    bool        dataLoaded;
    std::string loadedFile;
    ParseResult data;

    // --- Helpers visuais ---
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

    // --- Output ---
    void writeOutput(const AssignmentResult& result, const std::string& filename);
};

#endif // MENU_H
