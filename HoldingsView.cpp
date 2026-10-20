#include "HoldingsView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFrame>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>

HoldingsView::HoldingsView(Portfolio *portfolio, QWidget *parent)
    : QWidget(parent)
    , m_portfolio(portfolio)
    , m_currentFilter(0)
{
    setupUi();
    refreshData();
}

void HoldingsView::setPortfolio(Portfolio *portfolio) {
    m_portfolio = portfolio;
    refreshData();
}

void HoldingsView::setupUi() {
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
    contentLayout->setSpacing(18);

    // 1. Top Ribbon
    contentLayout->addWidget(createHeaderRibbon());

    // 2. Filter Bar
    contentLayout->addWidget(createFilterBar());

    // 3. Table Container Card
    contentLayout->addWidget(createTableContainer());

    // 4. Bottom 3 Summary Metric Cards
    contentLayout->addWidget(createBottomThreeCards());

    scrollArea->setWidget(contentWidget);
    rootLayout->addWidget(scrollArea);
}

QWidget* HoldingsView::createHeaderRibbon() {
    auto *banner = new QFrame(this);
    banner->setStyleSheet(
        "QFrame {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #DCE9FF;"
        "   border-radius: 14px;"
        "}"
    );

    auto *layout = new QHBoxLayout(banner);
    layout->setContentsMargins(22, 18, 22, 18);

    auto *textLayout = new QVBoxLayout();
    textLayout->setSpacing(4);

    auto *badgeLayout = new QHBoxLayout();
    badgeLayout->setSpacing(8);
    auto *dot = new QLabel("●", banner);
    dot->setStyleSheet("color: #00A389; font-size: 11px;");
    auto *subhead = new QLabel("AUDITED REAL-TIME • BOOK VALUE RECONCILIATION", banner);
    subhead->setStyleSheet("font-size: 11px; font-weight: 700; color: #006A61; letter-spacing: 0.8px;");
    badgeLayout->addWidget(dot);
    badgeLayout->addWidget(subhead);
    badgeLayout->addStretch();
    textLayout->addLayout(badgeLayout);

    auto *title = new QLabel("Current Portfolio Holdings", banner);
    title->setStyleSheet("font-size: 22px; font-weight: 700; color: #0B1C30;");
    textLayout->addWidget(title);

    auto *sub = new QLabel("Comprehensive inventory of your active financial positions with localized risk scoring.", banner);
    sub->setStyleSheet("font-size: 13px; color: #444651;");
    textLayout->addWidget(sub);

    layout->addLayout(textLayout, 1);

    // Right metrics & Add Asset button
    auto *mLayout = new QHBoxLayout();
    mLayout->setSpacing(14);

    auto *aumBox = new QFrame(banner);
    aumBox->setStyleSheet("background-color: #EFF4FF; border: 1px solid #DCE9FF; border-radius: 10px; padding: 6px 14px;");
    auto *aumV = new QVBoxLayout(aumBox);
    aumV->setSpacing(2);
    auto *aumTitle = new QLabel("AGGREGATED AUM", aumBox);
    aumTitle->setStyleSheet("font-size: 9px; font-weight: 700; color: #64748B; letter-spacing: 0.5px;");
    m_headerAumLabel = new QLabel("$485,250.00", aumBox);
    m_headerAumLabel->setStyleSheet("font-size: 16px; font-weight: 800; color: #0B1C30;");
    aumV->addWidget(aumTitle);
    aumV->addWidget(m_headerAumLabel);
    mLayout->addWidget(aumBox);

    auto *betaBox = new QFrame(banner);
    betaBox->setStyleSheet("background-color: #EFF4FF; border: 1px solid #DCE9FF; border-radius: 10px; padding: 6px 14px;");
    auto *betaV = new QVBoxLayout(betaBox);
    betaV->setSpacing(2);
    auto *betaTitle = new QLabel("PORTFOLIO BETA", betaBox);
    betaTitle->setStyleSheet("font-size: 9px; font-weight: 700; color: #64748B; letter-spacing: 0.5px;");
    m_headerBetaLabel = new QLabel("1.14 Active 📈", betaBox);
    m_headerBetaLabel->setStyleSheet("font-size: 16px; font-weight: 800; color: #00236F;");
    betaV->addWidget(betaTitle);
    betaV->addWidget(m_headerBetaLabel);
    mLayout->addWidget(betaBox);

    auto *addBtn = new QPushButton("➕  Add Asset", banner);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #00236F;"
        "   color: #FFFFFF;"
        "   border-radius: 8px;"
        "   padding: 10px 18px;"
        "   font-size: 13px;"
        "   font-weight: 700;"
        "}"
        "QPushButton:hover { background-color: #1E3A8A; }"
    );
    connect(addBtn, &QPushButton::clicked, this, [this]() {
        emit navigateRequested(1); // Go to Add Asset
    });
    mLayout->addWidget(addBtn);

    layout->addLayout(mLayout);

    return banner;
}

QWidget* HoldingsView::createFilterBar() {
    auto *bar = new QWidget(this);
    auto *layout = new QHBoxLayout(bar);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    // Search bar
    m_searchEdit = new QLineEdit(bar);
    m_searchEdit->setPlaceholderText("🔍  Filter by asset name, ticker, or class...");
    m_searchEdit->setStyleSheet(
        "QLineEdit {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #DCE9FF;"
        "   border-radius: 8px;"
        "   padding: 8px 14px;"
        "   font-size: 13px;"
        "   color: #0B1C30;"
        "}"
        "QLineEdit:focus { border: 1px solid #00236F; }"
    );
    connect(m_searchEdit, &QLineEdit::textChanged, this, &HoldingsView::onSearchTextChanged);
    layout->addWidget(m_searchEdit, 2);

    // Category Filter Pills
    auto *pillsWidget = new QWidget(bar);
    auto *pillsLayout = new QHBoxLayout(pillsWidget);
    pillsLayout->setContentsMargins(0, 0, 0, 0);
    pillsLayout->setSpacing(6);

    m_filterButtonGroup = new QButtonGroup(this);
    m_filterButtonGroup->setExclusive(true);

    m_pillAll = new QPushButton("All (14)", pillsWidget);
    m_pillStocks = new QPushButton("Stocks (5)", pillsWidget);
    m_pillFds = new QPushButton("Fixed Deposits (6)", pillsWidget);
    m_pillMfs = new QPushButton("Mutual Funds (3)", pillsWidget);

    int id = 0;
    for (auto *p : {m_pillAll, m_pillStocks, m_pillFds, m_pillMfs}) {
        p->setCheckable(true);
        p->setCursor(Qt::PointingHandCursor);
        p->setStyleSheet(
            "QPushButton {"
            "   background-color: #FFFFFF;"
            "   color: #444651;"
            "   border: 1px solid #DCE9FF;"
            "   border-radius: 8px;"
            "   padding: 7px 14px;"
            "   font-size: 12px;"
            "   font-weight: 600;"
            "}"
            "QPushButton:hover { background-color: #EFF4FF; }"
            "QPushButton:checked {"
            "   background-color: #00236F;"
            "   color: #FFFFFF;"
            "   border-color: #00236F;"
            "}"
        );
        m_filterButtonGroup->addButton(p, id++);
        pillsLayout->addWidget(p);
    }
    m_pillAll->setChecked(true);
    connect(m_filterButtonGroup, &QButtonGroup::idClicked, this, &HoldingsView::onFilterPillClicked);

    layout->addWidget(pillsWidget, 3);

    return bar;
}

QWidget* HoldingsView::createTableContainer() {
    auto *card = new QFrame(this);
    card->setStyleSheet(
        "QFrame {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #DCE9FF;"
        "   border-radius: 14px;"
        "}"
    );

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(12);

    m_tableWidget = new QTableWidget(card);
    m_tableWidget->setColumnCount(7);
    m_tableWidget->setHorizontalHeaderLabels({
        "ASSET & TICKER", "ASSET CLASS", "UNITS / PRICE", "CURRENT VALUE",
        "PORTFOLIO SHARE", "RISK CLASSIFICATION", "ACTION"
    });

    m_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    m_tableWidget->setColumnWidth(4, 150);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);

    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setFrameShape(QFrame::NoFrame);
    m_tableWidget->setShowGrid(false);
    m_tableWidget->setAlternatingRowColors(false);
    m_tableWidget->setMinimumHeight(320);

    m_tableWidget->setStyleSheet(
        "QTableWidget {"
        "   background-color: #FFFFFF;"
        "   border: none;"
        "   gridline-color: transparent;"
        "   font-size: 13px;"
        "}"
        "QHeaderView::section {"
        "   background-color: #F8F9FF;"
        "   color: #64748B;"
        "   font-size: 10px;"
        "   font-weight: 700;"
        "   letter-spacing: 0.5px;"
        "   border: none;"
        "   border-bottom: 2px solid #EFF4FF;"
        "   padding: 10px 8px;"
        "}"
        "QTableWidget::item {"
        "   border-bottom: 1px solid #F1F5F9;"
        "   padding: 10px 8px;"
        "   color: #0B1C30;"
        "}"
        "QTableWidget::item:selected {"
        "   background-color: #EFF4FF;"
        "   color: #0B1C30;"
        "}"
    );

    layout->addWidget(m_tableWidget);

    // Summary footer row
    layout->addWidget(createBottomSummaryRow());

    // Table pagination / export line
    auto *exportLine = new QHBoxLayout();
    m_footerAssetCountLabel = new QLabel("Showing 1-14 of 14 positions", card);
    m_footerAssetCountLabel->setStyleSheet("font-size: 11px; color: #64748B;");
    exportLine->addWidget(m_footerAssetCountLabel);
    exportLine->addStretch();

    auto *csvBtn = new QPushButton("📥  Export CSV", card);
    csvBtn->setCursor(Qt::PointingHandCursor);
    csvBtn->setStyleSheet("background: #EFF4FF; color: #00236F; border: 1px solid #DCE9FF; border-radius: 6px; padding: 6px 12px; font-size: 11px; font-weight: 700;");
    connect(csvBtn, &QPushButton::clicked, this, &HoldingsView::onExportCsvClicked);
    exportLine->addWidget(csvBtn);

    layout->addLayout(exportLine);

    return card;
}

QWidget* HoldingsView::createBottomSummaryRow() {
    auto *f = new QFrame(this);
    f->setStyleSheet("background-color: #F8F9FF; border: 1px solid #DCE9FF; border-radius: 8px; padding: 10px 14px;");

    auto *l = new QHBoxLayout(f);
    l->setContentsMargins(8, 4, 8, 4);

    auto *check = new QLabel("✔ Total Portfolio Value", f);
    check->setStyleSheet("font-size: 13px; font-weight: 700; color: #0B1C30;");
    l->addWidget(check);
    l->addStretch();

    auto *badgeActive = new QLabel("14 HOLDINGS ACTIVE", f);
    badgeActive->setStyleSheet("background: #EFF4FF; color: #00236F; border-radius: 4px; padding: 2px 6px; font-size: 10px; font-weight: 700;");
    l->addWidget(badgeActive);

    auto *lblWeight = new QLabel("Weighted Aggregate:", f);
    lblWeight->setStyleSheet("font-size: 12px; color: #64748B;");
    l->addWidget(lblWeight);

    m_footerTotalValLabel = new QLabel("$485,250.00", f);
    m_footerTotalValLabel->setStyleSheet("font-size: 15px; font-weight: 800; color: #0B1C30;");
    l->addWidget(m_footerTotalValLabel);

    auto *lbl100 = new QLabel("100.0%", f);
    lbl100->setStyleSheet("background: #DCE1FF; color: #00236F; border-radius: 4px; padding: 2px 6px; font-size: 11px; font-weight: 700;");
    l->addWidget(lbl100);

    return f;
}

QWidget* HoldingsView::createBottomThreeCards() {
    auto *row = new QWidget(this);
    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(16);

    const QString cardStyle =
        "QFrame {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #DCE9FF;"
        "   border-radius: 12px;"
        "   padding: 14px;"
        "}";

    // Card 1
    auto *c1 = new QFrame(row);
    c1->setStyleSheet(cardStyle);
    auto *l1 = new QVBoxLayout(c1);
    l1->setSpacing(4);
    auto *t1 = new QLabel("🔴 HIGH RISK CONCENTRATION", c1);
    t1->setStyleSheet("font-size: 10px; font-weight: 700; color: #64748B; letter-spacing: 0.5px;");
    m_highRiskValLabel = new QLabel("$242,625.00 (50.0%)", c1);
    m_highRiskValLabel->setStyleSheet("font-size: 15px; font-weight: 800; color: #BA1A1A;");
    auto *s1 = new QLabel("Requires rebalancing within 14 trading days.", c1);
    s1->setStyleSheet("font-size: 11px; color: #64748B;");
    l1->addWidget(t1);
    l1->addWidget(m_highRiskValLabel);
    l1->addWidget(s1);
    layout->addWidget(c1);

    // Card 2
    auto *c2 = new QFrame(row);
    c2->setStyleSheet(cardStyle);
    auto *l2 = new QVBoxLayout(c2);
    l2->setSpacing(4);
    auto *t2 = new QLabel("🟢 CAPITAL PRESERVATION CUSHION", c2);
    t2->setStyleSheet("font-size: 10px; font-weight: 700; color: #64748B; letter-spacing: 0.5px;");
    m_capPreserveValLabel = new QLabel("$145,575.00 (30.0%)", c2);
    m_capPreserveValLabel->setStyleSheet("font-size: 15px; font-weight: 800; color: #006A61;");
    auto *s2 = new QLabel("Exceeds 25% minimum target buffer.", c2);
    s2->setStyleSheet("font-size: 11px; color: #64748B;");
    l2->addWidget(t2);
    l2->addWidget(m_capPreserveValLabel);
    l2->addWidget(s2);
    layout->addWidget(c2);

    // Card 3
    auto *c3 = new QFrame(row);
    c3->setStyleSheet(cardStyle);
    auto *l3 = new QVBoxLayout(c3);
    l3->setSpacing(4);
    auto *t3 = new QLabel("⚖️ DIVERSIFICATION EFFICIENCY", c3);
    t3->setStyleSheet("font-size: 10px; font-weight: 700; color: #64748B; letter-spacing: 0.5px;");
    m_divEfficiencyLabel = new QLabel("8.4 / 10 Score", c3);
    m_divEfficiencyLabel->setStyleSheet("font-size: 15px; font-weight: 800; color: #00236F;");
    auto *s3 = new QLabel("3 distinct asset subclasses with low cross-correlation.", c3);
    s3->setStyleSheet("font-size: 11px; color: #64748B;");
    l3->addWidget(t3);
    l3->addWidget(m_divEfficiencyLabel);
    l3->addWidget(s3);
    layout->addWidget(c3);

    return row;
}

void HoldingsView::onFilterPillClicked(int filterIndex) {
    m_currentFilter = filterIndex;
    refreshData();
}

void HoldingsView::onSearchTextChanged(const QString& query) {
    Q_UNUSED(query);
    refreshData();
}

void HoldingsView::refreshData() {
    if (!m_portfolio) return;

    double totalVal = m_portfolio->getTotalValue();
    double beta = m_riskScorer.calculatePortfolioBeta(*m_portfolio);
    int riskScore = m_riskScorer.calculateRiskScore100(*m_portfolio);

    m_headerAumLabel->setText(QString("$%L1").arg(totalVal, 0, 'f', 2));
    m_headerBetaLabel->setText(QString("%1 Active 📈").arg(beta, 0, 'f', 2));
    m_footerTotalValLabel->setText(QString("$%L1").arg(totalVal, 0, 'f', 2));

    m_pillAll->setText(QString("All (%1)").arg(m_portfolio->getAssetCount()));
    m_pillStocks->setText(QString("Stocks (%1)").arg(m_portfolio->getStockCount()));
    m_pillFds->setText(QString("Fixed Deposits (%1)").arg(m_portfolio->getFdCount()));
    m_pillMfs->setText(QString("Mutual Funds (%1)").arg(m_portfolio->getMfCount()));

    // Bottom cards
    double stockTot = m_portfolio->getCategoryTotal("Stock");
    double fdTot = m_portfolio->getCategoryTotal("Fixed Deposit");
    double stockPct = (totalVal > 0) ? (stockTot / totalVal) * 100.0 : 0.0;
    double fdPct = (totalVal > 0) ? (fdTot / totalVal) * 100.0 : 0.0;

    m_highRiskValLabel->setText(QString("$%L1 (%2%)").arg(stockTot, 0, 'f', 2).arg(stockPct, 0, 'f', 1));
    m_capPreserveValLabel->setText(QString("$%L1 (%2%)").arg(fdTot, 0, 'f', 2).arg(fdPct, 0, 'f', 1));
    m_divEfficiencyLabel->setText(QString("%1 / 10 Score").arg(m_riskScorer.calculateDiversificationIndex(*m_portfolio), 0, 'f', 1));

    // Populate Table
    m_tableWidget->setRowCount(0);
    QString searchQuery = m_searchEdit->text().trimmed().toLower();

    const auto& assets = m_portfolio->getAssets();
    int visibleCount = 0;

    for (size_t i = 0; i < assets.size(); ++i) {
        Asset *a = assets[i];
        if (!a) continue;

        QString type = QString::fromStdString(a->getType());
        QString name = QString::fromStdString(a->getName());
        QString ticker = QString::fromStdString(a->getTicker());

        // Category filter check
        if (m_currentFilter == 1 && type != "Stock") continue;
        if (m_currentFilter == 2 && type != "Fixed Deposit") continue;
        if (m_currentFilter == 3 && type != "Mutual Fund") continue;

        // Search query check
        if (!searchQuery.isEmpty()) {
            if (!name.toLower().contains(searchQuery) &&
                !ticker.toLower().contains(searchQuery) &&
                !type.toLower().contains(searchQuery)) {
                continue;
            }
        }

        int row = m_tableWidget->rowCount();
        m_tableWidget->insertRow(row);
        m_tableWidget->setRowHeight(row, 56);
        visibleCount++;

        // 0. ASSET & TICKER
        auto *nameWidget = new QWidget();
        auto *nameLay = new QVBoxLayout(nameWidget);
        nameLay->setContentsMargins(6, 4, 6, 4);
        nameLay->setSpacing(1);
        auto *nl = new QLabel(name, nameWidget);
        nl->setStyleSheet("font-size: 13px; font-weight: 700; color: #0B1C30;");
        auto *tl = new QLabel(ticker, nameWidget);
        tl->setStyleSheet("font-size: 11px; color: #64748B;");
        nameLay->addWidget(nl);
        nameLay->addWidget(tl);
        m_tableWidget->setCellWidget(row, 0, nameWidget);

        // 1. ASSET CLASS BADGE
        auto *badgeWidget = new QWidget();
        auto *badgeLay = new QHBoxLayout(badgeWidget);
        badgeLay->setContentsMargins(4, 4, 4, 4);
        badgeLay->setAlignment(Qt::AlignCenter);
        auto *badge = new QLabel(badgeWidget);
        if (type == "Stock") {
            badge->setText("● STOCK");
            badge->setStyleSheet("background-color: #DCE1FF; color: #00236F; border-radius: 4px; padding: 3px 8px; font-size: 10px; font-weight: 700;");
        } else if (type == "Fixed Deposit") {
            badge->setText("● FIXED DEPOSIT");
            badge->setStyleSheet("background-color: #D0F8EC; color: #006A61; border-radius: 4px; padding: 3px 8px; font-size: 10px; font-weight: 700;");
        } else {
            badge->setText("● MUTUAL FUND");
            badge->setStyleSheet("background-color: #EFF4FF; color: #222A3E; border-radius: 4px; padding: 3px 8px; font-size: 10px; font-weight: 700;");
        }
        badgeLay->addWidget(badge);
        m_tableWidget->setCellWidget(row, 1, badgeWidget);

        // 2. UNITS / PRICE
        QString unitPriceStr;
        if (auto *s = dynamic_cast<Stock*>(a)) {
            unitPriceStr = QString("%1 sh @ $%2").arg(s->getUnits(), 0, 'f', 2).arg(s->getPrice(), 0, 'f', 2);
        } else if (auto *fd = dynamic_cast<FixedDeposit*>(a)) {
            unitPriceStr = QString("1 Cert (%1% APY)").arg(fd->getInterestRate(), 0, 'f', 2);
        } else {
            unitPriceStr = QString("%1 units").arg(a->getAmount() / 150.0, 0, 'f', 2);
        }
        auto *unitItem = new QTableWidgetItem(unitPriceStr);
        unitItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        unitItem->setForeground(QBrush(QColor("#444651")));
        m_tableWidget->setItem(row, 2, unitItem);

        // 3. CURRENT VALUE
        auto *valItem = new QTableWidgetItem(QString("$%L1").arg(a->getAmount(), 0, 'f', 2));
        valItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
        valItem->setForeground(QBrush(QColor("#0B1C30")));
        valItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
        m_tableWidget->setItem(row, 3, valItem);

        // 4. PORTFOLIO SHARE (Bar + %)
        double sharePct = (totalVal > 0) ? (a->getAmount() / totalVal) * 100.0 : 0.0;
        auto *shareWidget = new QWidget();
        auto *shareLay = new QHBoxLayout(shareWidget);
        shareLay->setContentsMargins(6, 4, 6, 4);
        shareLay->setSpacing(6);

        auto *bar = new QProgressBar(shareWidget);
        bar->setFixedSize(60, 6);
        bar->setRange(0, 100);
        bar->setValue(static_cast<int>(sharePct));
        bar->setTextVisible(false);
        bar->setStyleSheet("QProgressBar { background: #E2E8F0; border-radius: 3px; border: none; } QProgressBar::chunk { background: #00236F; border-radius: 3px; }");

        auto *pctLbl = new QLabel(QString("%1%").arg(sharePct, 0, 'f', 1), shareWidget);
        pctLbl->setStyleSheet("font-size: 11px; font-weight: 700; color: #444651;");

        shareLay->addWidget(bar);
        shareLay->addWidget(pctLbl);
        m_tableWidget->setCellWidget(row, 4, shareWidget);

        // 5. RISK CLASSIFICATION BADGE
        double riskVal = a->calculateRisk();
        auto *riskWidget = new QWidget();
        auto *riskLay = new QHBoxLayout(riskWidget);
        riskLay->setContentsMargins(4, 4, 4, 4);
        riskLay->setAlignment(Qt::AlignCenter);

        auto *rBadge = new QLabel(riskWidget);
        if (riskVal > 0.65) {
            rBadge->setText(QString("High Risk (%1)").arg(riskVal, 0, 'f', 2));
            rBadge->setStyleSheet("background-color: #FFDAD6; color: #BA1A1A; border-radius: 4px; padding: 3px 8px; font-size: 10px; font-weight: 700;");
        } else if (riskVal >= 0.35) {
            rBadge->setText(QString("Moderate (%1)").arg(riskVal, 0, 'f', 2));
            rBadge->setStyleSheet("background-color: #FFF0D4; color: #B45309; border-radius: 4px; padding: 3px 8px; font-size: 10px; font-weight: 700;");
        } else {
            rBadge->setText(QString("Low Risk (%1)").arg(riskVal, 0, 'f', 2));
            rBadge->setStyleSheet("background-color: #D0F8EC; color: #006A61; border-radius: 4px; padding: 3px 8px; font-size: 10px; font-weight: 700;");
        }
        riskLay->addWidget(rBadge);
        m_tableWidget->setCellWidget(row, 5, riskWidget);

        // 6. ACTION (Delete button)
        auto *delBtn = new QPushButton("🗑️", m_tableWidget);
        delBtn->setCursor(Qt::PointingHandCursor);
        delBtn->setFixedSize(30, 30);
        delBtn->setStyleSheet("QPushButton { background: transparent; border: 1px solid #DCE9FF; border-radius: 6px; font-size: 12px; } QPushButton:hover { background: #FFDAD6; border-color: #BA1A1A; }");

        int modelIdx = static_cast<int>(i);
        connect(delBtn, &QPushButton::clicked, this, [this, modelIdx]() {
            onDeleteAssetRow(modelIdx);
        });
        m_tableWidget->setCellWidget(row, 6, delBtn);
    }

    m_footerAssetCountLabel->setText(QString("Showing %1 of %2 positions").arg(visibleCount).arg(m_portfolio->getAssetCount()));
}

void HoldingsView::onDeleteAssetRow(int modelIndex) {
    if (!m_portfolio) return;

    auto reply = QMessageBox::question(
        this,
        "Remove Asset",
        "Are you sure you want to remove this position from your portfolio?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        m_portfolio->removeAsset(static_cast<size_t>(modelIndex));
        refreshData();
        emit assetRemoved();
    }
}

void HoldingsView::onExportCsvClicked() {
    if (!m_portfolio) return;

    QString filename = QFileDialog::getSaveFileName(
        this,
        "Export Holdings to CSV",
        "portfolio_holdings.csv",
        "CSV Files (*.csv);;All Files (*)"
    );

    if (filename.isEmpty()) return;

    m_fileManager.exportToCsv(*m_portfolio, filename.toStdString());

    QMessageBox::information(
        this,
        "CSV Export Completed",
        QString("Successfully exported %1 holdings to:\n%2").arg(m_portfolio->getAssetCount()).arg(filename)
    );
}
