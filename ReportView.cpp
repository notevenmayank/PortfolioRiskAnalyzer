#include "ReportView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QPainter>
#include <QMessageBox>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// Custom Semi-Circular Risk Gauge Widget
// ============================================================================
RiskGaugeWidget::RiskGaugeWidget(QWidget *parent)
    : QWidget(parent)
    , m_score(68)
{
    setFixedSize(240, 130);
    setStyleSheet("background: transparent;");
}

void RiskGaugeWidget::setScore(int score) {
    m_score = std::max(0, std::min(100, score));
    update();
}

void RiskGaugeWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();
    int cx = w / 2;
    int cy = h - 20;
    int radius = 80;

    QRectF arcRect(cx - radius, cy - radius, radius * 2, radius * 2);
    int penWidth = 14;

    // In Qt drawArc, angles are in 1/16th of a degree.
    // 0 deg is at 3 o'clock, 90 at 12 o'clock, 180 at 9 o'clock.
    // The top semi-circle spans 180° down to 0°.
    // 1. Green zone: 0% to 35% -> from 180° for length -35% * 180° = -63°
    QPen greenPen(QColor("#00A389"), penWidth, Qt::SolidLine, Qt::FlatCap);
    painter.setPen(greenPen);
    painter.drawArc(arcRect, static_cast<int>(180 * 16), static_cast<int>(-63 * 16));

    // 2. Amber zone: 35% to 65% -> from (180 - 63)° = 117° for length -30% * 180° = -54°
    QPen amberPen(QColor("#F59E0B"), penWidth, Qt::SolidLine, Qt::FlatCap);
    painter.setPen(amberPen);
    painter.drawArc(arcRect, static_cast<int>(117 * 16), static_cast<int>(-54 * 16));

    // 3. Red zone: 65% to 100% -> from (117 - 54)° = 63° for length -35% * 180° = -63°
    QPen redPen(QColor("#EF4444"), penWidth, Qt::SolidLine, Qt::FlatCap);
    painter.setPen(redPen);
    painter.drawArc(arcRect, static_cast<int>(63 * 16), static_cast<int>(-63 * 16));

    // Calculate needle angle
    // score 0 -> 180 deg (left), score 100 -> 0 deg (right)
    double needleAngleDeg = 180.0 - (m_score * 1.8);
    double needleRad = needleAngleDeg * M_PI / 180.0;

    int needleLength = radius - 10;
    double tipX = cx + needleLength * std::cos(needleRad);
    double tipY = cy - needleLength * std::sin(needleRad);

    // Draw needle
    QPen needlePen(QColor("#0B1C30"), 3, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(needlePen);
    painter.drawLine(QPointF(cx, cy), QPointF(tipX, tipY));

    // Draw center pivot hub
    painter.setBrush(QBrush(QColor("#0B1C30")));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(cx, cy), 7, 7);

    painter.setBrush(QBrush(QColor("#FFFFFF")));
    painter.drawEllipse(QPointF(cx, cy), 3, 3);
}

// ============================================================================
// ReportView Implementation
// ============================================================================
ReportView::ReportView(Portfolio *portfolio, QWidget *parent)
    : QWidget(parent)
    , m_portfolio(portfolio)
    , m_chart(nullptr)
    , m_chartView(nullptr)
    , m_pieSeries(nullptr)
{
    setupUi();
    refreshReport();
}

void ReportView::setPortfolio(Portfolio *portfolio) {
    m_portfolio = portfolio;
    refreshReport();
}

void ReportView::setupUi() {
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background-color: #F8F9FF; border: none; }");

    auto *contentWidget = new QWidget(scrollArea);
    contentWidget->setStyleSheet("background-color: #F8F9FF; font-family: 'Segoe UI', Inter, sans-serif;");
    auto *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(28, 24, 28, 24);
    contentLayout->setSpacing(20);

    // 1. Over-Concentration Alert Banner
    contentLayout->addWidget(createWarningBanner());

    // 2. Main Two-Column Analytics
    auto *mainCols = new QHBoxLayout();
    mainCols->setSpacing(20);
    mainCols->addWidget(createLeftColumn(), 7);  // 7/12 ratio
    mainCols->addWidget(createRightColumn(), 5); // 5/12 ratio
    contentLayout->addLayout(mainCols);

    scrollArea->setWidget(contentWidget);
    rootLayout->addWidget(scrollArea);
}

QWidget* ReportView::createWarningBanner() {
    m_alertBannerFrame = new QFrame(this);
    m_alertBannerFrame->setStyleSheet(
        "QFrame {"
        "   background-color: #FFFBEB;"
        "   border: 1px solid #FCD34D;"
        "   border-radius: 12px;"
        "}"
    );

    auto *layout = new QHBoxLayout(m_alertBannerFrame);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(16);

    auto *iconLabel = new QLabel("⚠️", m_alertBannerFrame);
    iconLabel->setStyleSheet("font-size: 26px; background: transparent;");
    layout->addWidget(iconLabel, 0, Qt::AlignTop);

    auto *textLayout = new QVBoxLayout();
    textLayout->setSpacing(4);

    auto *titleRow = new QHBoxLayout();
    titleRow->setSpacing(8);
    m_alertTitleLabel = new QLabel("Stock is 50% of your portfolio. Consider rebalancing.", m_alertBannerFrame);
    m_alertTitleLabel->setStyleSheet("font-size: 14px; font-weight: 700; color: #92400E;");

    auto *badge = new QLabel("ACTION RECOMMENDED", m_alertBannerFrame);
    badge->setStyleSheet("background-color: #FEF3C7; color: #B45309; border-radius: 4px; padding: 2px 6px; font-size: 9px; font-weight: 800;");

    titleRow->addWidget(m_alertTitleLabel);
    titleRow->addWidget(badge);
    titleRow->addStretch();
    textLayout->addLayout(titleRow);

    m_alertBodyLabel = new QLabel(
        "Your current equity exposure exceeds your target risk profile of 35-40%. "
        "Reallocating $48,500 into Fixed Deposits or Balanced Mutual Funds will restore your target risk score.",
        m_alertBannerFrame
    );
    m_alertBodyLabel->setStyleSheet("font-size: 12px; color: #78350F; line-height: 1.4;");
    m_alertBodyLabel->setWordWrap(true);
    textLayout->addWidget(m_alertBodyLabel);

    layout->addLayout(textLayout, 1);

    auto *simBtn = new QPushButton("🔄  Simulate Rebalancing", m_alertBannerFrame);
    simBtn->setCursor(Qt::PointingHandCursor);
    simBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #00236F;"
        "   color: #FFFFFF;"
        "   border-radius: 8px;"
        "   padding: 9px 16px;"
        "   font-size: 12px;"
        "   font-weight: 700;"
        "}"
        "QPushButton:hover { background-color: #1E3A8A; }"
    );
    connect(simBtn, &QPushButton::clicked, this, &ReportView::onSimulateRebalancingClicked);
    layout->addWidget(simBtn, 0, Qt::AlignVCenter);

    return m_alertBannerFrame;
}

QWidget* ReportView::createLeftColumn() {
    auto *col = new QWidget(this);
    auto *layout = new QVBoxLayout(col);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(18);

    const QString cardStyle =
        "QFrame {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #DCE9FF;"
        "   border-radius: 14px;"
        "}";

    // 1. Asset Category Distribution Card
    auto *distCard = new QFrame(col);
    distCard->setStyleSheet(cardStyle);
    auto *distLay = new QVBoxLayout(distCard);
    distLay->setContentsMargins(22, 18, 22, 18);
    distLay->setSpacing(14);

    auto *h1 = new QHBoxLayout();
    auto *t1 = new QLabel("Asset Category Distribution", distCard);
    t1->setStyleSheet("font-size: 16px; font-weight: 700; color: #0B1C30;");
    auto *pillTot = new QLabel("Total: $485,250", distCard);
    pillTot->setStyleSheet("background-color: #EFF4FF; color: #00236F; border-radius: 6px; padding: 4px 10px; font-size: 11px; font-weight: 700;");
    h1->addWidget(t1);
    h1->addStretch();
    h1->addWidget(pillTot);
    distLay->addLayout(h1);

    auto *chartRow = new QHBoxLayout();
    chartRow->setSpacing(16);

    m_pieSeries = new QPieSeries(this);
    m_pieSeries->setHoleSize(0.60);

    m_chart = new QChart();
    m_chart->addSeries(m_pieSeries);
    m_chart->legend()->hide();
    m_chart->setMargins(QMargins(0, 0, 0, 0));
    m_chart->setBackgroundBrush(Qt::transparent);

    m_chartView = new QChartView(m_chart, distCard);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setFixedSize(200, 200);
    m_chartView->setStyleSheet("background: transparent; border: none;");

    m_chartCenterText = new QLabel("ACTIVE CLASSES\n3 Core\nBalanced Weight", m_chartView);
    m_chartCenterText->setAlignment(Qt::AlignCenter);
    m_chartCenterText->setStyleSheet("font-size: 11px; font-weight: 700; color: #0B1C30; background: transparent; line-height: 1.2;");
    m_chartCenterText->setGeometry(30, 65, 140, 70);

    chartRow->addWidget(m_chartView, 0, Qt::AlignCenter);

    // Legend items on right
    auto *legLay = new QVBoxLayout();
    legLay->setSpacing(10);

    auto createLegendItem = [](const QString& col, const QString& title, const QString& desc, QLabel* &valLbl, QWidget* p) -> QWidget* {
        auto *w = new QWidget(p);
        auto *hl = new QHBoxLayout(w);
        hl->setContentsMargins(0, 0, 0, 0);

        auto *dot = new QLabel("■", w);
        dot->setStyleSheet(QString("color: %1; font-size: 14px;").arg(col));
        hl->addWidget(dot);

        auto *vl = new QVBoxLayout();
        vl->setSpacing(2);
        auto *tl = new QLabel(title, w);
        tl->setStyleSheet("font-size: 12px; font-weight: 700; color: #0B1C30;");
        auto *dl = new QLabel(desc, w);
        dl->setStyleSheet("font-size: 10px; color: #64748B;");
        vl->addWidget(tl);
        vl->addWidget(dl);
        hl->addLayout(vl, 1);

        valLbl = new QLabel("$0.00", w);
        valLbl->setStyleSheet("font-size: 12px; font-weight: 700; color: #0B1C30;");
        hl->addWidget(valLbl);

        return w;
    };

    legLay->addWidget(createLegendItem("#00236F", "Stocks (Equities) 50.0%", "Growth & Volatility", m_stockLegendVal, distCard));
    legLay->addWidget(createLegendItem("#006A61", "Fixed Deposits 30.0%", "Guaranteed Returns", m_fdLegendVal, distCard));
    legLay->addWidget(createLegendItem("#384055", "Mutual Funds 20.0%", "Pooled Strategy", m_mfLegendVal, distCard));

    chartRow->addLayout(legLay, 1);
    distLay->addLayout(chartRow);
    layout->addWidget(distCard);

    // 2. Variance: Actual vs. Target Allocation Card
    auto *varCard = new QFrame(col);
    varCard->setStyleSheet(cardStyle);
    auto *varLay = new QVBoxLayout(varCard);
    varLay->setContentsMargins(22, 18, 22, 18);
    varLay->setSpacing(12);

    auto *varH = new QHBoxLayout();
    auto *varTitle = new QLabel("Variance: Actual vs. Target Allocation", varCard);
    varTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: #0B1C30;");
    auto *benchTag = new QLabel("BENCHMARK: MODERATE-GROWTH", varCard);
    benchTag->setStyleSheet("font-size: 10px; font-weight: 700; color: #64748B; letter-spacing: 0.5px;");
    varH->addWidget(varTitle);
    varH->addStretch();
    varH->addWidget(benchTag);
    varLay->addLayout(varH);

    auto createVarRow = [](const QString& name, const QString& badgeText, const QString& badgeColor,
                           const QString& ratioText, QProgressBar* &barRef, QLabel* &badgeRef, QLabel* &ratioRef, QWidget* parent) -> QVBoxLayout* {
        auto *v = new QVBoxLayout();
        v->setSpacing(4);

        auto *h = new QHBoxLayout();
        auto *nl = new QLabel(name, parent);
        nl->setStyleSheet("font-size: 12px; font-weight: 600; color: #0B1C30;");

        badgeRef = new QLabel(badgeText, parent);
        badgeRef->setStyleSheet(QString("background-color: %1; border-radius: 4px; padding: 2px 6px; font-size: 10px; font-weight: 700;").arg(badgeColor));

        ratioRef = new QLabel(ratioText, parent);
        ratioRef->setStyleSheet("font-size: 11px; color: #64748B;");

        h->addWidget(nl);
        h->addWidget(badgeRef);
        h->addStretch();
        h->addWidget(ratioRef);
        v->addLayout(h);

        barRef = new QProgressBar(parent);
        barRef->setFixedHeight(8);
        barRef->setTextVisible(false);
        barRef->setStyleSheet(
            "QProgressBar { background-color: #E2E8F0; border-radius: 4px; border: none; }"
            "QProgressBar::chunk { background-color: #00236F; border-radius: 4px; }"
        );
        v->addWidget(barRef);

        return v;
    };

    varLay->addLayout(createVarRow("Stocks", "+15.0% Overweight", "#FFDAD6; color: #BA1A1A", "Actual 50% / Target 35%", m_stockVarBar, m_stockVarBadge, m_stockVarRatio, varCard));
    varLay->addLayout(createVarRow("Fixed Deposits", "-10.0% Underweight", "#FFF0D4; color: #B45309", "Actual 30% / Target 40%", m_fdVarBar, m_fdVarBadge, m_fdVarRatio, varCard));
    varLay->addLayout(createVarRow("Mutual Funds", "-5.0% Underweight", "#EFF4FF; color: #222A3E", "Actual 20% / Target 25%", m_mfVarBar, m_mfVarBadge, m_mfVarRatio, varCard));
    layout->addWidget(varCard);

    // 3. Historical Allocation Trajectory Card
    auto *histCard = new QFrame(col);
    histCard->setStyleSheet(cardStyle);
    auto *histLay = new QVBoxLayout(histCard);
    histLay->setContentsMargins(22, 18, 22, 18);
    histLay->setSpacing(12);

    auto *histH = new QHBoxLayout();
    auto *histTitle = new QLabel("📈  Historical Allocation Trajectory", histCard);
    histTitle->setStyleSheet("font-size: 15px; font-weight: 700; color: #0B1C30;");
    auto *histSub = new QLabel("Trailing 5 Quarters", histCard);
    histSub->setStyleSheet("font-size: 11px; color: #64748B;");
    histH->addWidget(histTitle);
    histH->addStretch();
    histH->addWidget(histSub);
    histLay->addLayout(histH);

    // Visual quarterly columns
    auto *barsH = new QHBoxLayout();
    barsH->setSpacing(14);
    for (const QString& q : {"Q1 '23", "Q2 '23", "Q3 '23", "Q4 '23", "Q1 '24", "Now"}) {
        auto *qv = new QVBoxLayout();
        qv->setSpacing(4);
        qv->setAlignment(Qt::AlignCenter);

        auto *stackFrame = new QFrame(histCard);
        stackFrame->setFixedSize(38, 70);
        stackFrame->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #00236F, stop:0.5 #006A61, stop:1 #384055); border-radius: 4px;");

        auto *qlbl = new QLabel(q, histCard);
        qlbl->setStyleSheet("font-size: 10px; color: #64748B; font-weight: 600;");

        qv->addWidget(stackFrame);
        qv->addWidget(qlbl);
        barsH->addLayout(qv);
    }
    histLay->addLayout(barsH);
    layout->addWidget(histCard);

    return col;
}

QWidget* ReportView::createRightColumn() {
    auto *col = new QWidget(this);
    auto *layout = new QVBoxLayout(col);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(18);

    const QString cardStyle =
        "QFrame {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #DCE9FF;"
        "   border-radius: 14px;"
        "}";

    // 1. Comprehensive Risk Meter & Scoring Card
    auto *gaugeCard = new QFrame(col);
    gaugeCard->setStyleSheet(cardStyle);
    auto *gLay = new QVBoxLayout(gaugeCard);
    gLay->setContentsMargins(20, 18, 20, 18);
    gLay->setSpacing(8);

    auto *gH = new QHBoxLayout();
    auto *gTitle = new QLabel("QUANT ASSESSMENT\nComprehensive Risk Meter & Scoring", gaugeCard);
    gTitle->setStyleSheet("font-size: 14px; font-weight: 700; color: #0B1C30;");
    m_gaugeZoneBadge = new QLabel("ZONE: ELEVATED", gaugeCard);
    m_gaugeZoneBadge->setStyleSheet("background-color: #FFDAD6; color: #BA1A1A; border-radius: 4px; padding: 3px 8px; font-size: 9px; font-weight: 800;");
    gH->addWidget(gTitle);
    gH->addStretch();
    gH->addWidget(m_gaugeZoneBadge);
    gLay->addLayout(gH);

    m_gaugeWidget = new RiskGaugeWidget(gaugeCard);
    gLay->addWidget(m_gaugeWidget, 0, Qt::AlignCenter);

    m_gaugeScoreText = new QLabel("<b style='font-size: 26px; color: #0B1C30;'>68</b> <span style='color: #64748B;'>/ 100</span><br><span style='font-size: 11px; color: #B45309; font-weight:600;'>High-Moderate Boundary</span>", gaugeCard);
    m_gaugeScoreText->setAlignment(Qt::AlignCenter);
    gLay->addWidget(m_gaugeScoreText);

    // Zone reference legend
    auto *zoneLay = new QHBoxLayout();
    zoneLay->setSpacing(6);
    auto createZoneBox = [](const QString& name, const QString& rng, const QString& bg, const QString& fg, QWidget* p) -> QFrame* {
        auto *f = new QFrame(p);
        f->setStyleSheet(QString("background-color: %1; border-radius: 6px; padding: 4px 8px;").arg(bg));
        auto *v = new QVBoxLayout(f);
        v->setContentsMargins(2, 2, 2, 2);
        v->setSpacing(1);
        auto *n = new QLabel(name, f);
        n->setStyleSheet(QString("font-size: 9px; font-weight: 800; color: %1; text-align: center;").arg(fg));
        n->setAlignment(Qt::AlignCenter);
        auto *r = new QLabel(rng, f);
        r->setStyleSheet(QString("font-size: 9px; color: %1; text-align: center;").arg(fg));
        r->setAlignment(Qt::AlignCenter);
        v->addWidget(n);
        v->addWidget(r);
        return f;
    };
    zoneLay->addWidget(createZoneBox("LOW RISK", "0 - 35", "#D0F8EC", "#006A61", gaugeCard));
    zoneLay->addWidget(createZoneBox("MODERATE", "36 - 65", "#FFF0D4", "#B45309", gaugeCard));
    zoneLay->addWidget(createZoneBox("HIGH RISK", "66 - 100", "#FFDAD6", "#BA1A1A", gaugeCard));
    gLay->addLayout(zoneLay);

    layout->addWidget(gaugeCard);

    // 2. Risk Drivers Contribution Card
    auto *driversCard = new QFrame(col);
    driversCard->setStyleSheet(cardStyle);
    auto *dLay = new QVBoxLayout(driversCard);
    dLay->setContentsMargins(20, 18, 20, 18);
    dLay->setSpacing(10);

    auto *dH = new QHBoxLayout();
    auto *dTitle = new QLabel("Risk Drivers Contribution", driversCard);
    dTitle->setStyleSheet("font-size: 14px; font-weight: 700; color: #0B1C30;");
    auto *dSub = new QLabel("Factor Attribution", driversCard);
    dSub->setStyleSheet("font-size: 10px; color: #64748B;");
    dH->addWidget(dTitle);
    dH->addStretch();
    dH->addWidget(dSub);
    dLay->addLayout(dH);

    auto createDriverItem = [](const QString& name, const QString& pct, const QString& desc, const QString& color, QProgressBar* &barRef, QWidget* parent) -> QVBoxLayout* {
        auto *v = new QVBoxLayout();
        v->setSpacing(3);

        auto *h = new QHBoxLayout();
        auto *nl = new QLabel(name, parent);
        nl->setStyleSheet("font-size: 11px; font-weight: 600; color: #0B1C30;");
        auto *pl = new QLabel(pct, parent);
        pl->setStyleSheet(QString("font-size: 11px; font-weight: 800; color: %1;").arg(color));
        h->addWidget(nl);
        h->addStretch();
        h->addWidget(pl);
        v->addLayout(h);

        barRef = new QProgressBar(parent);
        barRef->setFixedHeight(6);
        barRef->setTextVisible(false);
        barRef->setStyleSheet(QString("QProgressBar { background: #E2E8F0; border-radius: 3px; border: none; } QProgressBar::chunk { background: %1; border-radius: 3px; }").arg(color));
        v->addWidget(barRef);

        auto *dl = new QLabel(desc, parent);
        dl->setStyleSheet("font-size: 10px; color: #64748B;");
        v->addWidget(dl);

        return v;
    };

    dLay->addLayout(createDriverItem("Equity Volatility Impact", "64%", "Dominant factor driven by mega-cap technology exposure.", "#BA1A1A", m_driverEquityBar, driversCard));
    dLay->addLayout(createDriverItem("Duration & Interest Rate Sensitivity", "18%", "Fixed Deposits lock-in periods cushion short-term yield shifts.", "#006A61", m_driverDurationBar, driversCard));
    dLay->addLayout(createDriverItem("Liquidity & Concentration Drag", "18%", "Diversified across open-ended liquid mutual funds.", "#00236F", m_driverLiquidityBar, driversCard));

    layout->addWidget(driversCard);

    // 3. Step-by-Step Diversification Roadmap Card
    auto *roadCard = new QFrame(col);
    roadCard->setStyleSheet(cardStyle);
    auto *rLay = new QVBoxLayout(roadCard);
    rLay->setContentsMargins(20, 18, 20, 18);
    rLay->setSpacing(12);

    auto *rH = new QHBoxLayout();
    auto *rTitle = new QLabel("Step-by-Step Diversification Roadmap", roadCard);
    rTitle->setStyleSheet("font-size: 14px; font-weight: 700; color: #0B1C30;");
    rH->addWidget(rTitle);
    rLay->addLayout(rH);

    auto *rSub = new QLabel("Tailored sequence to restore Target Risk Score ≤ 42", roadCard);
    rSub->setStyleSheet("font-size: 11px; color: #006A61; font-weight: 600;");
    rLay->addWidget(rSub);

    auto createStep = [](int num, const QString& title, const QString& body, QWidget* parent) -> QHBoxLayout* {
        auto *hl = new QHBoxLayout();
        hl->setSpacing(10);
        auto *badge = new QLabel(QString::number(num), parent);
        badge->setFixedSize(22, 22);
        badge->setAlignment(Qt::AlignCenter);
        badge->setStyleSheet("background-color: #00236F; color: #FFFFFF; border-radius: 11px; font-size: 11px; font-weight: 700;");
        hl->addWidget(badge, 0, Qt::AlignTop);

        auto *vl = new QVBoxLayout();
        vl->setSpacing(2);
        auto *tl = new QLabel(title, parent);
        tl->setStyleSheet("font-size: 12px; font-weight: 700; color: #0B1C30;");
        auto *bl = new QLabel(body, parent);
        bl->setStyleSheet("font-size: 11px; color: #64748B;");
        bl->setWordWrap(true);
        vl->addWidget(tl);
        vl->addWidget(bl);
        hl->addLayout(vl, 1);
        return hl;
    };

    rLay->addLayout(createStep(1, "Trim Equity Over-exposure", "Trim $24,250 from overweighted cyclical tech positions to capture capital gains without triggering high tax brackets.", roadCard));
    rLay->addLayout(createStep(2, "Augment Fixed Deposits Anchor", "Inject $28,500 into high-yield 18-month Fixed Deposits offering a guaranteed 5.4% APR floor.", roadCard));
    rLay->addLayout(createStep(3, "Reinvest in Balanced Hybrid Funds", "Allocate remaining surplus into multi-asset mutual funds for continuous automated rebalancing.", roadCard));

    auto *applyBtn = new QPushButton("Apply Recommendations", roadCard);
    applyBtn->setCursor(Qt::PointingHandCursor);
    applyBtn->setStyleSheet("background-color: #006A61; color: #FFFFFF; border-radius: 8px; padding: 10px; font-size: 12px; font-weight: 700;");
    connect(applyBtn, &QPushButton::clicked, this, &ReportView::onSimulateRebalancingClicked);
    rLay->addWidget(applyBtn);

    layout->addWidget(roadCard);

    return col;
}

void ReportView::refreshReport() {
    if (!m_portfolio) return;

    double totalVal = m_portfolio->getTotalValue();
    auto alloc = m_calculator.calculateAllocation(*m_portfolio);
    int riskScore100 = m_riskScorer.calculateRiskScore100(*m_portfolio);

    // 1. Update Warning Banner
    bool overConcentrated = false;
    for (const auto& pair : alloc) {
        if (pair.second > 40.0) {
            overConcentrated = true;
            m_alertTitleLabel->setText(QString("%1 is %2% of your portfolio. Consider rebalancing.")
                .arg(QString::fromStdString(pair.first))
                .arg(pair.second, 0, 'f', 1));
            m_alertBodyLabel->setText(QString(
                "Your current %1 exposure exceeds your target risk profile of 35-40%%. "
                "Reallocating capital into Fixed Deposits or Balanced Mutual Funds will restore your target risk score."
            ).arg(QString::fromStdString(pair.first)));
            break;
        }
    }

    if (!overConcentrated) {
        m_alertBannerFrame->setStyleSheet("background-color: #D0F8EC; border: 1px solid #A7F3D0; border-radius: 12px;");
        m_alertTitleLabel->setText("Portfolio is Well-Diversified across all asset classes.");
        m_alertTitleLabel->setStyleSheet("font-size: 14px; font-weight: 700; color: #006A61;");
        m_alertBodyLabel->setText("All asset allocations remain under the 40.0% safety boundary. No rebalancing actions are required at this time.");
    } else {
        m_alertBannerFrame->setStyleSheet("background-color: #FFFBEB; border: 1px solid #FCD34D; border-radius: 12px;");
        m_alertTitleLabel->setStyleSheet("font-size: 14px; font-weight: 700; color: #92400E;");
    }

    // 2. Update Donut Chart
    m_pieSeries->clear();
    double stockVal = m_portfolio->getCategoryTotal("Stock");
    double fdVal = m_portfolio->getCategoryTotal("Fixed Deposit");
    double mfVal = m_portfolio->getCategoryTotal("Mutual Fund");

    if (totalVal > 0) {
        if (stockVal > 0) {
            auto *s1 = m_pieSeries->append("Stocks", stockVal);
            s1->setColor(QColor("#00236F"));
            s1->setBorderColor(QColor("#FFFFFF"));
            s1->setBorderWidth(2);
        }
        if (fdVal > 0) {
            auto *s2 = m_pieSeries->append("Fixed Deposits", fdVal);
            s2->setColor(QColor("#006A61"));
            s2->setBorderColor(QColor("#FFFFFF"));
            s2->setBorderWidth(2);
        }
        if (mfVal > 0) {
            auto *s3 = m_pieSeries->append("Mutual Funds", mfVal);
            s3->setColor(QColor("#384055"));
            s3->setBorderColor(QColor("#FFFFFF"));
            s3->setBorderWidth(2);
        }
    }

    m_stockLegendVal->setText(QString("$%L1").arg(stockVal, 0, 'f', 2));
    m_fdLegendVal->setText(QString("$%L1").arg(fdVal, 0, 'f', 2));
    m_mfLegendVal->setText(QString("$%L1").arg(mfVal, 0, 'f', 2));

    // 3. Update Variance Bars
    double stockPct = alloc["Stock"];
    double fdPct = alloc["Fixed Deposit"];
    double mfPct = alloc["Mutual Fund"];

    m_stockVarBar->setValue(static_cast<int>(stockPct));
    m_fdVarBar->setValue(static_cast<int>(fdPct));
    m_mfVarBar->setValue(static_cast<int>(mfPct));

    double stockVar = stockPct - 35.0;
    double fdVar = fdPct - 40.0;
    double mfVar = mfPct - 25.0;

    m_stockVarBadge->setText(QString("%1%1% Overweight").arg(stockVar >= 0 ? "+" : "").arg(stockVar, 0, 'f', 1));
    m_fdVarBadge->setText(QString("%1%1% Underweight").arg(fdVar >= 0 ? "+" : "").arg(fdVar, 0, 'f', 1));
    m_mfVarBadge->setText(QString("%1%1% Underweight").arg(mfVar >= 0 ? "+" : "").arg(mfVar, 0, 'f', 1));

    // 4. Update Gauge
    m_gaugeWidget->setScore(riskScore100);
    m_gaugeScoreText->setText(QString(
        "<b style='font-size: 26px; color: #0B1C30;'>%1</b> <span style='color: #64748B;'>/ 100</span><br>"
        "<span style='font-size: 11px; color: %2; font-weight:600;'>%3</span>"
    ).arg(riskScore100)
     .arg(riskScore100 > 65 ? "#BA1A1A" : (riskScore100 > 35 ? "#B45309" : "#006A61"))
     .arg(riskScore100 > 65 ? "Elevated Risk Boundary" : (riskScore100 > 35 ? "High-Moderate Boundary" : "Conservative Zone")));

    if (riskScore100 > 65) {
        m_gaugeZoneBadge->setText("ZONE: ELEVATED");
        m_gaugeZoneBadge->setStyleSheet("background-color: #FFDAD6; color: #BA1A1A; border-radius: 4px; padding: 3px 8px; font-size: 9px; font-weight: 800;");
    } else if (riskScore100 > 35) {
        m_gaugeZoneBadge->setText("ZONE: MODERATE");
        m_gaugeZoneBadge->setStyleSheet("background-color: #FFF0D4; color: #B45309; border-radius: 4px; padding: 3px 8px; font-size: 9px; font-weight: 800;");
    } else {
        m_gaugeZoneBadge->setText("ZONE: OPTIMAL");
        m_gaugeZoneBadge->setStyleSheet("background-color: #D0F8EC; color: #006A61; border-radius: 4px; padding: 3px 8px; font-size: 9px; font-weight: 800;");
    }

    // 5. Update Risk Drivers bars
    m_driverEquityBar->setValue(64);
    m_driverDurationBar->setValue(18);
    m_driverLiquidityBar->setValue(18);
}

void ReportView::onSimulateRebalancingClicked() {
    QMessageBox::information(
        this,
        "Rebalancing Simulation",
        "Automated Rebalance Algorithm:\n\n"
        "1. Trim $48,500 from High-Beta Equities (MSFT, NVDA)\n"
        "2. Allocate $28,500 to 5-Year High Yield Fixed Deposits\n"
        "3. Allocate $20,000 to Balanced Index Mutual Funds\n\n"
        "Projected Result:\n"
        "• Equity Weight: Reduced from 50.0% to 40.0%\n"
        "• Portfolio Risk Score: Drops from 68 to 44 / 100 (Optimal Zone)"
    );
}
