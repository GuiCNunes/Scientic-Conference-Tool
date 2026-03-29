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

    /** @brief Renderiza a área gráfica superior com informação de carregamento */
    void printHeader();
    /** @brief Desdobra as opções principais do User Menu na prompt */
    int  showMainMenu();
    /** @brief Escreve texto de Erro a vermelho no CLI */
    void showError(const std::string& msg);
    /** @brief Eloga em verde o sucesso com checkmark verde no CLI */
    void showSuccess(const std::string& msg);
    /** @brief Pausa a aplicação garantindo Input Buffer contínuo para evitar pass-through */
    void waitEnter();

    // --- Screens ---
    /** @brief Menu de Seleção de Input dataset CSV */
    void handleLoadFile();
    /** @brief Pista 2: Visualiza sumário dos dados do parse em terminal */
    void handleShowData();
    /** @brief Constrói a Rede de Fluxos Principal baseada num assignment e despeja em `output.csv` */
    void handleGenerateAssignments();
    /** @brief Desmonta e Testa a criticidade de cada Revisor um por um usando o Max Flow */
    void handleRiskAnalysis();

    // --- Output ---
    /** @brief Formata o AssignmentResult em formato compatível com as regras exigidas e exporta `filename` */
    void writeOutput(const AssignmentResult& result, const std::string& filename);
};

#endif // MENU_H
