#ifndef ASSET_H
#define ASSET_H

#include <string>
using namespace std;

class Asset {
protected:
    string name;
    double amount;

public:
    Asset(string n, double amt) : name(n), amount(amt) {}

    virtual double calculateRisk() = 0;   // pure virtual -> makes Asset abstract

    string getName() { return name; }
    double getAmount() { return amount; }
    virtual string getType() = 0;         // also pure virtual

    virtual ~Asset() {}                   // virtual destructor -- important
};

#endif
// ---------- Stock ----------
class Stock : public Asset {
private:
    double volatilityIndex; // like a beta value, e.g. 1.2

public:
    Stock(string n, double amt, double vol)
        : Asset(n, amt), volatilityIndex(vol) {}

    double calculateRisk() override {
        return volatilityIndex * 0.8; // higher beta -> higher risk
    }

    string getType() override {
        return "Stock";
    }
};

// ---------- Fixed Deposit ----------
class FixedDeposit : public Asset {
private:
    double interestRate;
    int tenureYears;

public:
    FixedDeposit(string n, double amt, double rate, int tenure)
        : Asset(n, amt), interestRate(rate), tenureYears(tenure) {}

    double calculateRisk() override {
        return 0.05; // FDs are near risk-free
    }

    string getType() override {
        return "Fixed Deposit";
    }
};

// ---------- Mutual Fund ----------
class MutualFund : public Asset {
private:
    double equityRatio; // e.g. 0.7 means 70% equity, 30% debt

public:
    MutualFund(string n, double amt, double eqRatio)
        : Asset(n, amt), equityRatio(eqRatio) {}

    double calculateRisk() override {
        // equity is riskier than debt, so weight accordingly
        return (equityRatio * 0.7) + ((1 - equityRatio) * 0.1);
    }

    string getType() override {
        return "Mutual Fund";
    }
};