#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include "Portfolio.h"
#include "AllocationCalculator.h"
#include "RiskScorer.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <string>

using namespace std;

class ReportGenerator {
public:
    void generateReport(const Portfolio& p) {
        AllocationCalculator calc;
        RiskScorer scorer;

        map<string, double> allocation = calc.calculateAllocation(p);
        double riskScore = scorer.calculateOverallRisk(p);
        string riskCategory = scorer.getRiskCategory(riskScore);

        cout << "\n==================================================\n";
        cout << "        PORTFOLIO ALLOCATION BREAKDOWN\n";
        cout << "==================================================\n";

        for (auto& entry : allocation) {
            cout << left << setw(15) << entry.first << ": ";
            int bars = (int)(entry.second / 5); // 1 bar per 5%
            for (int i = 0; i < bars; i++) cout << "#";
            cout << " " << fixed << setprecision(1) << entry.second << "%\n";
        }

        cout << "--------------------------------------------------\n";
        cout << "Weighted Portfolio Risk Score : " << fixed << setprecision(2)
             << riskScore << " / 1.0\n";
        cout << "Risk Category                 : " << riskCategory << "\n";
        cout << "==================================================\n";

        checkConcentration(allocation);
    }

    void checkConcentration(const map<string, double>& allocation) {
        double threshold = 40.0;
        bool flagged = false;

        for (const auto& entry : allocation) {
            if (entry.second > threshold) {
                cout << "\n[ALERT] '" << entry.first << "' is " << fixed
                     << setprecision(1) << entry.second << "% of your portfolio.\n";
                cout << "   This exceeds the recommended threshold of "
                     << threshold << "%.\n";
                cout << "   Suggestion: Consider rebalancing.\n";
                flagged = true;
            }
        }

        if (!flagged) {
            cout << "\n[OK] No category is over-concentrated.\n";
        }
    }

    void generateSummaryReport(const Portfolio& p, const string& filepath) {
        ofstream out(filepath);
        if (!out) return;

        AllocationCalculator calc;
        RiskScorer scorer;

        auto allocation = calc.calculateAllocation(p);
        int risk100 = scorer.calculateRiskScore100(p);
        string riskCategory = scorer.getRiskCategory100(risk100);

        out << "==========================================================\n";
        out << "   RISKWISE INSTITUTIONAL PORTFOLIO ALLOCATION REPORT\n";
        out << "==========================================================\n";
        out << "Total Portfolio Value: $" << fixed << setprecision(2) << p.getTotalValue() << "\n";
        out << "Active Holdings Count: " << p.getAssetCount() << "\n";
        out << "Overall Risk Score   : " << risk100 << " / 100 (" << riskCategory << ")\n";
        out << "Portfolio Beta       : " << setprecision(2) << scorer.calculatePortfolioBeta(p) << "\n";
        out << "Diversification Index: " << setprecision(1) << scorer.calculateDiversificationIndex(p) << " / 10.0\n\n";

        out << "----------------------------------------------------------\n";
        out << "ASSET CLASS ALLOCATION BREAKDOWN:\n";
        out << "----------------------------------------------------------\n";
        for (const auto& entry : allocation) {
            out << left << setw(18) << entry.first << ": " << fixed << setprecision(1) << entry.second << "%\n";
        }

        out << "\n----------------------------------------------------------\n";
        out << "HOLDINGS INVENTORY:\n";
        out << "----------------------------------------------------------\n";
        for (const auto* a : p.getAssets()) {
            out << left << setw(28) << a->getName() << " | "
                << setw(16) << a->getType() << " | "
                << "$" << setw(12) << fixed << setprecision(2) << a->getAmount() << " | "
                << "Risk: " << setprecision(2) << a->calculateRisk() << "\n";
        }
        out << "==========================================================\n";
        out.close();
    }
};

#endif // REPORTGENERATOR_H