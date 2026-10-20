#ifndef RISKSCORER_H
#define RISKSCORER_H

#include "Portfolio.h"
#include <string>
#include <cmath>
#include <map>

using namespace std;

/**
 * @brief Algorithmic Risk Assessment & Diversification Scoring engine.
 */
class RiskScorer {
public:
    double calculateOverallRisk(const Portfolio& p) {
        double totalValue = p.getTotalValue();
        if (totalValue <= 0.0) return 0.0;

        double weightedRisk = 0.0;
        for (const Asset* a : p.getAssets()) {
            double weight = a->getAmount() / totalValue;
            // Calls virtual function calculateRisk() - Polymorphism!
            weightedRisk += weight * a->calculateRisk();
        }
        return weightedRisk;
    }

    int calculateRiskScore100(const Portfolio& p) {
        double risk = calculateOverallRisk(p);
        // Scales 0.0-1.0 to 0-100 score (e.g. 0.68 -> 68)
        int score = static_cast<int>(std::round(risk * 100.0));
        return std::max(0, std::min(100, score));
    }

    double calculatePortfolioBeta(const Portfolio& p) {
        double totalValue = p.getTotalValue();
        if (totalValue <= 0.0) return 1.0;

        double weightedBeta = 0.0;
        for (const Asset* a : p.getAssets()) {
            double weight = a->getAmount() / totalValue;
            if (const Stock* s = dynamic_cast<const Stock*>(a)) {
                weightedBeta += weight * s->getVolatilityIndex();
            } else if (a->getType() == "Fixed Deposit") {
                weightedBeta += weight * 0.05; // FDs have near-zero beta
            } else {
                weightedBeta += weight * 0.85; // Mutual fund baseline beta
            }
        }
        return weightedBeta;
    }

    double calculateDiversificationIndex(const Portfolio& p) {
        double totalValue = p.getTotalValue();
        if (totalValue <= 0.0) return 0.0;

        // Herfindahl-Hirschman based concentration score inverted to 10-point scale
        double hhi = 0.0;
        map<string, double> categorySums;
        for (const Asset* a : p.getAssets()) {
            categorySums[a->getType()] += a->getAmount();
        }

        for (const auto& entry : categorySums) {
            double share = entry.second / totalValue;
            hhi += share * share;
        }

        // Perfect split across 3 classes = 1/3 each -> HHI = 0.333
        // Concentrated in 1 class -> HHI = 1.0
        double score = (1.0 - hhi) / (1.0 - 0.333) * 10.0;
        if (score > 10.0) score = 10.0;
        if (score < 1.0) score = 1.0;
        return std::round(score * 10.0) / 10.0; // Round to 1 decimal, e.g. 7.2
    }

    string getRiskCategory(double score) {
        if (score < 0.35) return "Low Risk";
        else if (score < 0.65) return "Moderate Risk";
        else return "High Risk";
    }

    string getRiskCategory100(int score100) {
        if (score100 <= 35) return "Low Risk";
        else if (score100 <= 65) return "Moderate Risk";
        else return "High Risk";
    }
};

#endif // RISKSCORER_H