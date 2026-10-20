#ifndef ALLOCATIONCALCULATOR_H
#define ALLOCATIONCALCULATOR_H

#include "Portfolio.h"
#include <map>
#include <string>

using namespace std;

/**
 * @brief Computes asset allocation percentages and portfolio target variances.
 *
 * Core OOP Principles Demonstrated:
 * - Composition / Association: Operates on the Portfolio composition to perform risk
 *   and allocation analysis without violating structural boundaries.
 * - Encapsulation: Interacts strictly through Portfolio's public interface (getTotalValue,
 *   getAssets, getAmount, getType), respecting internal state protection.
 */
class AllocationCalculator {
public:
    map<string, double> calculateAllocation(const Portfolio& p) {
        map<string, double> categoryTotals;
        double totalValue = p.getTotalValue();

        if (totalValue <= 0.0) {
            return {};
        }

        for (const Asset* a : p.getAssets()) {
            categoryTotals[a->getType()] += a->getAmount();
        }

        map<string, double> allocationPercent;
        for (const auto& entry : categoryTotals) {
            allocationPercent[entry.first] = (entry.second / totalValue) * 100.0;
        }

        return allocationPercent;
    }

    map<string, double> getTargetAllocation() {
        return {
            {"Stock", 35.0},
            {"Fixed Deposit", 40.0},
            {"Mutual Fund", 25.0}
        };
    }

    map<string, double> calculateVariance(const Portfolio& p) {
        auto actual = calculateAllocation(p);
        auto target = getTargetAllocation();
        map<string, double> variance;

        for (const auto& t : target) {
            double act = actual[t.first];
            variance[t.first] = act - t.second; // e.g. 50 - 35 = +15.0%
        }
        return variance;
    }
};

#endif // ALLOCATIONCALCULATOR_H