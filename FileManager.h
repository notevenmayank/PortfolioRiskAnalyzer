#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "Portfolio.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

class FileManager {
public:
    bool saveToFile(const Portfolio& p, const string& filename) {
        ofstream outFile(filename);
        if (!outFile) {
            return false;
        }

        for (const Asset* a : p.getAssets()) {
            if (const Stock* s = dynamic_cast<const Stock*>(a)) {
                outFile << "Stock," << s->getName() << "," << s->getAmount() << ","
                        << s->getVolatilityIndex() << "," << s->getTicker() << ","
                        << s->getSector() << "," << s->getPrice() << "," << s->getDate() << "\n";
            } else if (const FixedDeposit* fd = dynamic_cast<const FixedDeposit*>(a)) {
                outFile << "Fixed Deposit," << fd->getName() << "," << fd->getAmount() << ","
                        << fd->getInterestRate() << "," << fd->getTenureYears() << ","
                        << fd->getTicker() << "," << fd->getDate() << "\n";
            } else if (const MutualFund* mf = dynamic_cast<const MutualFund*>(a)) {
                outFile << "Mutual Fund," << mf->getName() << "," << mf->getAmount() << ","
                        << mf->getEquityRatio() << "," << mf->getExpenseRatio() << ","
                        << mf->getTicker() << "," << mf->getDate() << "\n";
            }
        }
        outFile.close();
        return true;
    }

    bool loadFromFile(Portfolio& p, const string& filename) {
        ifstream inFile(filename);
        if (!inFile) {
            return false;
        }

        p.clear();
        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string type, name, amtStr;

            getline(ss, type, ',');
            getline(ss, name, ',');
            getline(ss, amtStr, ',');

            double amount = stod(amtStr);

            if (type == "Stock") {
                string volStr, ticker, sector, priceStr, date;
                getline(ss, volStr, ',');
                getline(ss, ticker, ',');
                getline(ss, sector, ',');
                getline(ss, priceStr, ',');
                getline(ss, date, ',');
                double vol = volStr.empty() ? 1.2 : stod(volStr);
                double price = priceStr.empty() ? 250.0 : stod(priceStr);
                p.addAsset(new Stock(name, amount, vol, ticker, sector.empty() ? "Equities" : sector, price, date));
            } else if (type == "Fixed Deposit") {
                string rateStr, tenureStr, ticker, date;
                getline(ss, rateStr, ',');
                getline(ss, tenureStr, ',');
                getline(ss, ticker, ',');
                getline(ss, date, ',');
                double rate = rateStr.empty() ? 5.5 : stod(rateStr);
                int tenure = tenureStr.empty() ? 3 : stoi(tenureStr);
                p.addAsset(new FixedDeposit(name, amount, rate, tenure, ticker, date));
            } else if (type == "Mutual Fund") {
                string eqStr, expStr, ticker, date;
                getline(ss, eqStr, ',');
                getline(ss, expStr, ',');
                getline(ss, ticker, ',');
                getline(ss, date, ',');
                double eqRatio = eqStr.empty() ? 0.6 : stod(eqStr);
                double expRatio = expStr.empty() ? 0.75 : stod(expStr);
                p.addAsset(new MutualFund(name, amount, eqRatio, expRatio, ticker, date));
            }
        }
        inFile.close();
        return true;
    }

    bool exportToCsv(const Portfolio& p, const string& filename) {
        ofstream outFile(filename);
        if (!outFile) return false;

        outFile << "Asset Name,Ticker,Category,Amount ($),Risk Factor,Date\n";
        for (Asset* a : p.getAssets()) {
            outFile << "\"" << a->getName() << "\","
                    << "\"" << a->getTicker() << "\","
                    << "\"" << a->getType() << "\","
                    << fixed << setprecision(2) << a->getAmount() << ","
                    << a->calculateRisk() << ","
                    << "\"" << a->getDate() << "\"\n";
        }
        outFile.close();
        return true;
    }
};

#endif // FILEMANAGER_H