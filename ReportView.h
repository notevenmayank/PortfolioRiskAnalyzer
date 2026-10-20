#ifndef REPORTVIEW_H
#define REPORTVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QProgressBar>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>

#include "Portfolio.h"
#include "AllocationCalculator.h"
#include "RiskScorer.h"
#include "ReportGenerator.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QT_CHARTS_USE_NAMESPACE
#endif

/**
 * @brief Custom QPainter widget rendering the 180° semi-circular risk gauge with needle.
 */
class RiskGaugeWidget : public QWidget {
    Q_OBJECT
public:
    explicit RiskGaugeWidget(QWidget *parent = nullptr);
    void setScore(int score);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_score; // 0 to 100
};

/**
 * @brief The ReportView class implements Screen 6 (Allocation Report & Quant Risk Assessment).
 *
 * Features:
 * - Over-concentration warning banner (> 40% equity alert with rebalance simulator).
 * - Left Analytical Column:
 *   - Donut chart + breakdown legend cards
 *   - Variance: Actual vs. Target Allocation comparison progress bars
 *   - Historical Allocation Trajectory stacked bars
 * - Right Quant Assessment Column:
 *   - Custom semi-circular arc Risk Gauge widget with needle (68/100)
 *   - Risk Drivers factor contribution bars (Equity volatility 64%, Duration 18%, Liquidity 18%)
 *   - Step-by-Step Diversification Roadmap cards with actionable guidance.
 */
class ReportView : public QWidget {
    Q_OBJECT

public:
    explicit ReportView(Portfolio *portfolio, QWidget *parent = nullptr);
    ~ReportView() override = default;

    void setPortfolio(Portfolio *portfolio);

public slots:
    void refreshReport();
    void onSimulateRebalancingClicked();

signals:
    void navigateRequested(int viewIndex);

private:
    void setupUi();
    QWidget* createWarningBanner();
    QWidget* createLeftColumn();
    QWidget* createRightColumn();

    Portfolio *m_portfolio;
    AllocationCalculator m_calculator;
    RiskScorer m_riskScorer;
    ReportGenerator m_reportGen;

    // Warning Banner
    QFrame *m_alertBannerFrame;
    QLabel *m_alertTitleLabel;
    QLabel *m_alertBodyLabel;

    // Left Column: Donut & Variance
    QChart *m_chart;
    QChartView *m_chartView;
    QPieSeries *m_pieSeries;
    QLabel *m_chartCenterText;

    QLabel *m_stockLegendVal;
    QLabel *m_fdLegendVal;
    QLabel *m_mfLegendVal;

    // Variance bars
    QProgressBar *m_stockVarBar;
    QLabel *m_stockVarBadge;
    QLabel *m_stockVarRatio;

    QProgressBar *m_fdVarBar;
    QLabel *m_fdVarBadge;
    QLabel *m_fdVarRatio;

    QProgressBar *m_mfVarBar;
    QLabel *m_mfVarBadge;
    QLabel *m_mfVarRatio;

    // Right Column: Risk Gauge & Factor Drivers
    RiskGaugeWidget *m_gaugeWidget;
    QLabel *m_gaugeScoreText;
    QLabel *m_gaugeZoneBadge;

    QProgressBar *m_driverEquityBar;
    QProgressBar *m_driverDurationBar;
    QProgressBar *m_driverLiquidityBar;
};

#endif // REPORTVIEW_H
