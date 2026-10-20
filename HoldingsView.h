#ifndef HOLDINGSVIEW_H
#define HOLDINGSVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>
#include <QProgressBar>

#include "Portfolio.h"
#include "RiskScorer.h"
#include "FileManager.h"

/**
 * @brief The HoldingsView class implements Screen 5 (Current Portfolio Holdings).
 *
 * Features:
 * - Top audit ribbon with Aggregated AUM ($485,250.00) and Portfolio Beta (1.14 Active).
 * - Real-time search filter and category filter pills: All (14), Stocks (5), Fixed Deposits (6), Mutual Funds (3).
 * - High-density QTableWidget with:
 *   - Asset & Ticker
 *   - Asset Class Badges
 *   - Units / Price
 *   - Current Value
 *   - Portfolio Share (with visual bar + %)
 *   - Risk Classification Badges
 *   - Row Delete Action
 * - Total Portfolio Value summary footer bar.
 * - CSV Export capability via FileManager.
 * - Bottom 3 analytical cards: High Risk Concentration, Capital Preservation, Diversification Efficiency.
 */
class HoldingsView : public QWidget {
    Q_OBJECT

public:
    explicit HoldingsView(Portfolio *portfolio, QWidget *parent = nullptr);
    ~HoldingsView() override = default;

    void setPortfolio(Portfolio *portfolio);

public slots:
    void refreshData();
    void onSearchTextChanged(const QString& query);
    void onFilterPillClicked(int filterIndex);
    void onExportCsvClicked();
    void onDeleteAssetRow(int modelIndex);

signals:
    void navigateRequested(int viewIndex);
    void assetRemoved();

private:
    void setupUi();
    QWidget* createHeaderRibbon();
    QWidget* createFilterBar();
    QWidget* createTableContainer();
    QWidget* createBottomSummaryRow();
    QWidget* createBottomThreeCards();

    Portfolio *m_portfolio;
    RiskScorer m_riskScorer;
    FileManager m_fileManager;

    // Header labels
    QLabel *m_headerAumLabel;
    QLabel *m_headerBetaLabel;

    // Filter controls
    QLineEdit *m_searchEdit;
    QButtonGroup *m_filterButtonGroup;
    QPushButton *m_pillAll;
    QPushButton *m_pillStocks;
    QPushButton *m_pillFds;
    QPushButton *m_pillMfs;
    int m_currentFilter; // 0: All, 1: Stocks, 2: FD, 3: MF

    // Table
    QTableWidget *m_tableWidget;
    QLabel *m_footerTotalValLabel;
    QLabel *m_footerAssetCountLabel;

    // Bottom 3 Cards
    QLabel *m_highRiskValLabel;
    QLabel *m_capPreserveValLabel;
    QLabel *m_divEfficiencyLabel;
};

#endif // HOLDINGSVIEW_H
