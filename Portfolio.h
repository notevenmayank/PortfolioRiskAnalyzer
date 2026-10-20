#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include "Asset.h"
#include <vector>
#include <string>

using namespace std;

/**
 * @brief Container class managing dynamic polymorphic Asset pointers.
 *
 * Core OOP Principles Demonstrated:
 * - Encapsulation: Private member data 'holdings' is strictly hidden from outside access.
 *   Access and state modifications occur solely through controlled public methods
 *   (e.g., addAsset, removeAsset, clear, getTotalValue, getCategoryTotal).
 * - Composition: Portfolio composes and manages a collection of heap-allocated Asset objects,
 *   taking responsibility for their allocation, lifecycle, and destruction (~Portfolio, clear).
 * - Const Correctness: Query and accessor methods are marked const to protect object state.
 */
class Portfolio {
private:
    vector<Asset*> holdings;

public:
    Portfolio();
    ~Portfolio();

    void addAsset(Asset* a);
    bool removeAsset(size_t index);
    void clear();

    vector<Asset*> getAssets() const;
    double getTotalValue() const;

    size_t getAssetCount() const;
    int getStockCount() const;
    int getFdCount() const;
    int getMfCount() const;

    double getCategoryTotal(const string& type) const;

    // Seeds default institutional test portfolio matching fintech mockup ($485,250 AUM)
    void populateSampleData();
};

#endif // PORTFOLIO_H