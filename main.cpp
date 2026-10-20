#include "Portfolio.h"
#include "AllocationCalculator.h"
#include "RiskScorer.h"
#include "ReportGenerator.h"
#include "FileManager.h"
#include <iostream>
#include <limits>
using namespace std;

void showMenu() {
    cout << "\n==================================================\n";
    cout << "   PORTFOLIO RISK & DIVERSIFICATION ANALYZER\n";
    cout << "==================================================\n";
    cout << "1. Add New Asset\n";
    cout << "2. View All Holdings\n";
    cout << "3. Calculate Allocation & Risk Report\n";
    cout << "4. Save Portfolio to File\n";
    cout << "5. Load Portfolio from File\n";
    cout << "6. Exit\n";
    cout << "--------------------------------------------------\n";
    cout << "Enter choice: ";
}

void addAsset(Portfolio& p) {
    int type;
    cout << "\nSelect Asset Type:\n";
    cout << "1. Fixed Deposit\n2. Stock\n3. Mutual Fund\n";
    cout << "Enter choice: ";
    cin >> type;

    string name;
    double amount;

    if (type == 1) {
        double rate;
        int tenure;
        cout << "Enter FD Name: ";
        cin >> name;
        cout << "Enter Invested Amount: ";
        cin >> amount;
        cout << "Enter Interest Rate: ";
        cin >> rate;
        cout << "Enter Tenure (years): ";
        cin >> tenure;
        p.addAsset(new FixedDeposit(name, amount, rate, tenure));
    }
    else if (type == 2) {
        double vol;
        cout << "Enter Stock Name: ";
        cin >> name;
        cout << "Enter Invested Amount: ";
        cin >> amount;
        cout << "Enter Volatility Index (e.g. 1.2): ";
        cin >> vol;
        p.addAsset(new Stock(name, amount, vol));
    }
    else if (type == 3) {
        double eqRatio;
        cout << "Enter Mutual Fund Name: ";
        cin >> name;
        cout << "Enter Invested Amount: ";
        cin >> amount;
        cout << "Enter Equity Ratio (0.0 to 1.0): ";
        cin >> eqRatio;
        p.addAsset(new MutualFund(name, amount, eqRatio));
    }
    else {
        cout << "Invalid choice.\n";
        return;
    }

    cout << "Asset added successfully.\n";
}

void viewHoldings(Portfolio& p) {
    cout << "\n---------------------------------------------\n";
    cout << left << setw(15) << "Name" << setw(15) << "Type"
         << setw(10) << "Amount" << "Risk\n";
    cout << "---------------------------------------------\n";

    for (Asset* a : p.getAssets()) {
        cout << left << setw(15) << a->getName()
             << setw(15) << a->getType()
             << setw(10) << a->getAmount()
             << fixed << setprecision(2) << a->calculateRisk() << "\n";
    }

    cout << "---------------------------------------------\n";
    cout << "Total Portfolio Value: " << p.getTotalValue() << "\n";
}

int main() {
    Portfolio portfolio;
    ReportGenerator reportGen;
    FileManager fileManager;
    int choice;

    do {
        showMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                addAsset(portfolio);
                break;
            case 2:
                viewHoldings(portfolio);
                break;
            case 3:
                reportGen.generateReport(portfolio);
                break;
            case 4: {
                string filename;
                cout << "Enter filename to save: ";
                cin >> filename;
                fileManager.saveToFile(portfolio, filename);
                break;
            }
            case 5: {
                string filename;
                cout << "Enter filename to load: ";
                cin >> filename;
                fileManager.loadFromFile(portfolio, filename);
                break;
            }
            case 6:
                cout << "Exiting... Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 6);

    return 0;
}