#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "../include/Menu.h"
#include "../include/Parser.h"
#include "../include/FlowNetwork.h"
#include "../include/MaxFlow.h"   // edmondsKarp() — função template, está no .h

/**
 * @brief Ponto de entrada do programa.
 *
 * Modos de execução:
 *   Interativo:  ./myProg
 *   Batch:       ./myProg -b input.csv
 *
 * Mensagens de erro/info vão para stderr (conforme o enunciado).
 */
int main(int argc, char* argv[]) {

    // ── Modo batch ───────────────────────────────────────────────
    if (argc >= 3 && std::string(argv[1]) == "-b") {
        const std::string inputFile = argv[2];

        Parser parser;
        ParseResult data = parser.parse(inputFile);

        if (!data.success) {
            std::cerr << "[ERRO] Falha ao processar '" << inputFile << "'\n";
            return 1;
        }

        std::cerr << "[INFO] " << data.submissions.size()
                  << " submissoes, " << data.reviewers.size()
                  << " revisores carregados.\n";

        // Construir rede de fluxo
        FlowNetwork network(data);

        // Correr Edmonds-Karp — identificamos os nós pelo NodeInfo diretamente
        const NodeInfo source = {NodeType::SOURCE, 0};
        const NodeInfo sink   = {NodeType::SINK,   0};
        edmondsKarp(&network.getGraph(), source, sink);

        // Extrair resultado
        AssignmentResult result = network.extractResult(data);

        std::cerr << "[INFO] Fluxo obtido: " << result.totalFlow
                  << " | Necessario: "       << result.totalRequired
                  << " | " << (result.isComplete() ? "COMPLETO" : "INCOMPLETO") << "\n";

        // Escrever output (a não ser que SILENT)
        if (data.ctrl.generateAssignments != AssignmentMode::SILENT) {
            std::ofstream out(data.ctrl.outputFilename);
            if (!out.is_open()) {
                std::cerr << "[ERRO] Nao foi possivel escrever em '"
                          << data.ctrl.outputFilename << "'\n";
                return 1;
            }

            if (result.isComplete()) {
                out << "#SubmissionId,ReviewerId,Match\n";
                for (const auto& a : result.assignments)
                    out << a.submissionId << ", " << a.reviewerId
                        << ", " << a.matchedTopic << "\n";

                // Vista por reviewer (inverso)
                std::unordered_map<int, std::vector<std::pair<int,int>>> byRev;
                for (const auto& a : result.assignments)
                    byRev[a.reviewerId].emplace_back(a.submissionId, a.matchedTopic);

                out << "#ReviewerId,SubmissionId,Match\n";
                std::vector<int> rids;
                for (const auto& [rid, _] : byRev) rids.push_back(rid);
                std::sort(rids.begin(), rids.end());
                for (int rid : rids) {
                    auto& entries = byRev[rid];
                    std::sort(entries.begin(), entries.end());
                    for (const auto& [sid, t] : entries)
                        out << rid << ", " << sid << ", " << t << "\n";
                }
                out << "#Total: " << result.totalFlow << "\n";
            } else {
                out << "#SubmissionId,Domain,MissingReviews\n";
                for (const auto& m : result.missing)
                    out << m.submissionId << ", " << m.domain
                        << ", " << m.missingCount << "\n";
            }
            out.close();
            std::cerr << "[INFO] Output escrito em: " << data.ctrl.outputFilename << "\n";
        }

        if (data.ctrl.riskAnalysis > 0)
            std::cerr << "[AVISO] Risk analysis em batch ainda nao implementado (T2.2/T2.3)\n";

        return 0;
    }

    // ── Modo interativo ──────────────────────────────────────────
    Menu menu;
    menu.run();
    return 0;
}