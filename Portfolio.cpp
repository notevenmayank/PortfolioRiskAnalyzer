/**
 * @file Portfolio.cpp
 * @brief Implementation of Portfolio container methods.
 *
 * Core OOP Principles Demonstrated:
 * - Encapsulation: Internal vector state 'holdings' managed through controlled methods.
 * - Composition: Lifetime management (memory cleanup through delete in clear() and ~Portfolio()).
 */

#include "Portfolio.h"

Portfolio::Portfolio() {
}

Portfolio::~Portfolio() {
    clear();
}

void Portfolio::clear() {
    for (Asset* a : holdings) {
        delete a;
    }
    holdings.clear();
}

void Portfolio::addAsset(Asset* a) {
    if (a) {
        holdings.push_back(a);
    }
}

bool Portfolio::removeAsset(size_t index) {
    if (index < holdings.size()) {
        delete holdings[index];
        holdings.erase(holdings.begin() + index);
        return true;
    }
    return false;
}

vector<Asset*> Portfolio::getAssets() const {
    return holdings;
}

double Portfolio::getTotalValue() const {
    double total = 0.0;
    for (const Asset* a : holdings) {
        total += a->getAmount();
    }
    return total;
}

size_t Portfolio::getAssetCount() const {
    return holdings.size();
}

int Portfolio::getStockCount() const {
    int count = 0;
    for (const Asset* a : holdings) {
        if (a->getType() == "Stock") count++;
    }
    return count;
}

int Portfolio::getFdCount() const {
    int count = 0;
    for (const Asset* a : holdings) {
        if (a->getType() == "Fixed Deposit") count++;
    }
    return count;
}

int Portfolio::getMfCount() const {
    int count = 0;
    for (const Asset* a : holdings) {
        if (a->getType() == "Mutual Fund") count++;
    }
    return count;
}

double Portfolio::getCategoryTotal(const string& type) const {
    double sum = 0.0;
    for (const Asset* a : holdings) {
        if (a->getType() == type) {
            sum += a->getAmount();
        }
    }
    return sum;
}

void Portfolio::populateSampleData() {
    clear();

    // 1. Stocks (₹24,26,250 total = 50.0%)
    addAsset(new Stock("Reliance Industries Ltd", 850000.00, 1.12, "RELIANCE", "Energy & Conglomerate", 2950.00));
    addAsset(new Stock("Tata Consultancy Services", 920000.00, 1.05, "TCS", "Information Technology", 3820.00));
    addAsset(new Stock("Apple Inc. (Marketstack EOD)", 656250.00, 1.15, "AAPL", "Information Technology", 28812.00));

    // 2. Fixed Deposits (₹14,55,750 total = 30.0%)
    addAsset(new FixedDeposit("SBI 3-Yr High Yield Term Deposit", 600000.00, 7.10, 3, "FD-SBI3Y"));
    addAsset(new FixedDeposit("HDFC Bank 5-Yr Senior Deposit", 855750.00, 7.25, 5, "FD-HDFC5Y"));

    // 3. Mutual Funds (₹9,70,500 total = 20.0%)
    addAsset(new MutualFund("Parag Parikh Flexi Cap Fund", 520500.00, 0.85, 0.65, "PPFAS-DIR"));
    addAsset(new MutualFund("HDFC Top 100 Index Fund Direct", 450000.00, 1.00, 0.20, "HDFC100-DIR"));
}