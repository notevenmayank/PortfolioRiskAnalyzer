#include "LoginDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGraphicsDropShadowEffect>
#include <QRegularExpression>
#include <QTimer>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("RiskWise • Portfolio Risk & Diversification Analyzer");
    setFixedSize(500, 680);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QString err;
    m_authManager.initialize(err);

    setupUi();
    applyStyles();
    setSignUpMode(false);
}

QString LoginDialog::getAuthenticatedUsername() const {
    return m_authenticatedUser;
}

void LoginDialog::setupUi() {
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(24, 24, 24, 24);
    rootLayout->setSpacing(0);

    // Main Card Surface
    auto *cardFrame = new QFrame(this);
    cardFrame->setObjectName("mainCard");

    auto *cardLayout = new QVBoxLayout(cardFrame);
    cardLayout->setContentsMargins(28, 24, 28, 24);
    cardLayout->setSpacing(12);

    // 1. Logo & Header
    auto *headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(4);
    headerLayout->setAlignment(Qt::AlignCenter);

    auto *logoContainer = new QFrame(cardFrame);
    logoContainer->setObjectName("logoIconBox");
    logoContainer->setFixedSize(48, 48);
    auto *logoLayout = new QVBoxLayout(logoContainer);
    logoLayout->setContentsMargins(0, 0, 0, 0);
    logoLayout->setAlignment(Qt::AlignCenter);

    auto *logoText = new QLabel("📈", logoContainer);
    logoText->setStyleSheet("font-size: 22px;");
    logoLayout->addWidget(logoText);
    headerLayout->addWidget(logoContainer, 0, Qt::AlignCenter);

    m_badgePill = new QLabel("● NEW ACCOUNT SETUP", cardFrame);
    m_badgePill->setObjectName("badgePill");
    m_badgePill->setAlignment(Qt::AlignCenter);
    m_badgePill->hide();
    headerLayout->addWidget(m_badgePill, 0, Qt::AlignCenter);

    m_titleLabel = new QLabel("Portfolio Risk & Diversification Analyzer", cardFrame);
    m_titleLabel->setObjectName("cardTitle");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setWordWrap(true);

    m_subtitleLabel = new QLabel("Intelligent asset allocation & risk modeling", cardFrame);
    m_subtitleLabel->setObjectName("cardSubtitle");
    m_subtitleLabel->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(m_titleLabel);
    headerLayout->addWidget(m_subtitleLabel);
    cardLayout->addLayout(headerLayout);

    // 2. Email / Username Field
    auto *emailLabel = new QLabel("INSTITUTIONAL EMAIL / ID", cardFrame);
    emailLabel->setObjectName("fieldLabel");
    cardLayout->addWidget(emailLabel);

    m_emailEdit = new QLineEdit(cardFrame);
    m_emailEdit->setObjectName("inputField");
    m_emailEdit->setPlaceholderText("e.g. analyst@investcorp.com");
    m_emailEdit->setText("analyst@investcorp.com");
    m_emailEdit->setMinimumHeight(40);
    cardLayout->addWidget(m_emailEdit);

    // 3. Password Field
    auto *pwdHeaderLayout = new QHBoxLayout();
    auto *pwdLabel = new QLabel("SECURE KEY / PASSWORD", cardFrame);
    pwdLabel->setObjectName("fieldLabel");
    pwdHeaderLayout->addWidget(pwdLabel);

    pwdHeaderLayout->addStretch();
    auto *forgotLink = new QLabel("<a href='#' style='color: #006a61; text-decoration: none; font-size: 11px; font-weight: 600;'>Forgot password?</a>", cardFrame);
    forgotLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    pwdHeaderLayout->addWidget(forgotLink);
    cardLayout->addLayout(pwdHeaderLayout);

    auto *pwdInputContainer = new QWidget(cardFrame);
    auto *pwdInputLayout = new QHBoxLayout(pwdInputContainer);
    pwdInputLayout->setContentsMargins(0, 0, 0, 0);
    pwdInputLayout->setSpacing(0);

    m_passwordEdit = new QLineEdit(pwdInputContainer);
    m_passwordEdit->setObjectName("inputField");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText("••••••••••••");
    m_passwordEdit->setText("InvestCorp2026!");
    m_passwordEdit->setMinimumHeight(40);

    m_pwdEyeBtn = new QPushButton("👁", pwdInputContainer);
    m_pwdEyeBtn->setObjectName("eyeBtn");
    m_pwdEyeBtn->setFixedSize(36, 40);
    m_pwdEyeBtn->setCursor(Qt::PointingHandCursor);

    pwdInputLayout->addWidget(m_passwordEdit);
    pwdInputLayout->addWidget(m_pwdEyeBtn);
    cardLayout->addWidget(pwdInputContainer);

    // 4. Real-time Strength Meter (Signup Mode)
    m_strengthMeterContainer = new QWidget(cardFrame);
    auto *strengthLayout = new QVBoxLayout(m_strengthMeterContainer);
    strengthLayout->setContentsMargins(0, 0, 0, 0);
    strengthLayout->setSpacing(4);

    auto *barsLayout = new QHBoxLayout();
    barsLayout->setSpacing(4);
    for (int i = 0; i < 4; ++i) {
        m_strengthBars[i] = new QFrame(m_strengthMeterContainer);
        m_strengthBars[i]->setFixedHeight(4);
        m_strengthBars[i]->setStyleSheet("background-color: #D3E4FE; border-radius: 2px;");
        barsLayout->addWidget(m_strengthBars[i]);
    }
    strengthLayout->addLayout(barsLayout);

    m_strengthLabel = new QLabel("Enter password", m_strengthMeterContainer);
    m_strengthLabel->setStyleSheet("font-size: 11px; color: #757682; font-weight: 500;");
    strengthLayout->addWidget(m_strengthLabel);

    m_strengthMeterContainer->hide();
    cardLayout->addWidget(m_strengthMeterContainer);

    // 5. Confirm Password (Signup Mode)
    m_confirmContainer = new QWidget(cardFrame);
    auto *confirmLayout = new QVBoxLayout(m_confirmContainer);
    confirmLayout->setContentsMargins(0, 0, 0, 0);
    confirmLayout->setSpacing(4);

    auto *confirmHeader = new QHBoxLayout();
    auto *confirmLabel = new QLabel("CONFIRM PASSWORD", m_confirmContainer);
    confirmLabel->setObjectName("fieldLabel");
    m_matchStatusLabel = new QLabel("", m_confirmContainer);
    m_matchStatusLabel->setStyleSheet("font-size: 11px; font-weight: 600;");

    confirmHeader->addWidget(confirmLabel);
    confirmHeader->addStretch();
    confirmHeader->addWidget(m_matchStatusLabel);
    confirmLayout->addLayout(confirmHeader);

    m_confirmPasswordEdit = new QLineEdit(m_confirmContainer);
    m_confirmPasswordEdit->setObjectName("inputField");
    m_confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    m_confirmPasswordEdit->setPlaceholderText("Re-enter password");
    m_confirmPasswordEdit->setMinimumHeight(40);
    confirmLayout->addWidget(m_confirmPasswordEdit);

    m_confirmContainer->hide();
    cardLayout->addWidget(m_confirmContainer);

    // 6. Checkboxes & Status
    m_optionsContainer = new QWidget(cardFrame);
    auto *optsLayout = new QHBoxLayout(m_optionsContainer);
    optsLayout->setContentsMargins(0, 0, 0, 0);

    m_rememberCheck = new QCheckBox("Remember analytical node", m_optionsContainer);
    m_rememberCheck->setChecked(true);
    m_rememberCheck->setStyleSheet("font-size: 12px; color: #444651;");

    auto *twoFaLabel = new QLabel("🛡️ 2FA Active", m_optionsContainer);
    twoFaLabel->setStyleSheet("font-size: 11px; color: #006a61; font-weight: 600;");

    optsLayout->addWidget(m_rememberCheck);
    optsLayout->addStretch();
    optsLayout->addWidget(twoFaLabel);
    cardLayout->addWidget(m_optionsContainer);

    m_termsCheck = new QCheckBox("I agree to the Terms of Service & Privacy Policy", cardFrame);
    m_termsCheck->setStyleSheet("font-size: 11px; color: #444651;");
    m_termsCheck->hide();
    cardLayout->addWidget(m_termsCheck);

    // Status label
    m_statusLabel = new QLabel(cardFrame);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);
    m_statusLabel->hide();
    cardLayout->addWidget(m_statusLabel);

    // 7. Primary Action Button
    m_submitBtn = new QPushButton("Login to Portfolio  →", cardFrame);
    m_submitBtn->setObjectName("primaryBtn");
    m_submitBtn->setMinimumHeight(44);
    m_submitBtn->setCursor(Qt::PointingHandCursor);
    cardLayout->addWidget(m_submitBtn);

    // 8. Financial Encryption Badges
    auto *badgesLayout = new QHBoxLayout();
    badgesLayout->setAlignment(Qt::AlignCenter);
    badgesLayout->setSpacing(8);

    auto *badge1 = new QLabel("🔒 256-Bit Financial Encryption", cardFrame);
    badge1->setObjectName("microBadge");
    auto *badge2 = new QLabel("✓ SOC2 Certified", cardFrame);
    badge2->setObjectName("microBadge");

    badgesLayout->addWidget(badge1);
    badgesLayout->addWidget(badge2);
    cardLayout->addLayout(badgesLayout);

    // 9. Switch Login / Signup Mode
    auto *switchLayout = new QHBoxLayout();
    switchLayout->setAlignment(Qt::AlignCenter);
    switchLayout->setSpacing(4);

    m_switchPromptLabel = new QLabel("Don't have an account?", cardFrame);
    m_switchPromptLabel->setStyleSheet("color: #444651; font-size: 12px;");

    m_switchModeBtn = new QPushButton("Create account", cardFrame);
    m_switchModeBtn->setObjectName("linkBtn");
    m_switchModeBtn->setCursor(Qt::PointingHandCursor);

    switchLayout->addWidget(m_switchPromptLabel);
    switchLayout->addWidget(m_switchModeBtn);
    cardLayout->addLayout(switchLayout);

    rootLayout->addWidget(cardFrame);

    // 10. Bottom Compliance Footer
    auto *footerLabel = new QLabel("Prime Institutional Risk Suite  •  Tier 4 Datacenter Redundancy  •  ISO/IEC 27001", this);
    footerLabel->setStyleSheet("color: #757682; font-size: 10px; font-weight: 500; margin-top: 8px;");
    footerLabel->setAlignment(Qt::AlignCenter);
    rootLayout->addWidget(footerLabel);

    // Connect signals & slots
    connect(m_pwdEyeBtn, &QPushButton::clicked, this, &LoginDialog::onTogglePasswordVisibility);
    connect(m_switchModeBtn, &QPushButton::clicked, this, &LoginDialog::onToggleModeClicked);
    connect(m_submitBtn, &QPushButton::clicked, this, &LoginDialog::onActionClicked);
    connect(m_passwordEdit, &QLineEdit::textChanged, this, &LoginDialog::onPasswordInputChanged);
    connect(m_confirmPasswordEdit, &QLineEdit::textChanged, this, &LoginDialog::onConfirmPasswordChanged);

    connect(m_emailEdit, &QLineEdit::returnPressed, this, &LoginDialog::onActionClicked);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onActionClicked);
    connect(m_confirmPasswordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onActionClicked);
}

void LoginDialog::setSignUpMode(bool signUp) {
    m_isSignUpMode = signUp;

    m_badgePill->setVisible(signUp);
    m_strengthMeterContainer->setVisible(signUp);
    m_confirmContainer->setVisible(signUp);
    m_termsCheck->setVisible(signUp);
    m_optionsContainer->setVisible(!signUp);

    if (signUp) {
        m_titleLabel->setText("Portfolio Risk & Diversification Analyzer");
        m_subtitleLabel->setText("Institutional-grade stress modeling & risk allocation platform");
        m_submitBtn->setText("Sign Up & Start Analyzing  →");
        m_switchPromptLabel->setText("Already have an account?");
        m_switchModeBtn->setText("Log in");
    } else {
        m_titleLabel->setText("Portfolio Risk & Diversification Analyzer");
        m_subtitleLabel->setText("Intelligent asset allocation & risk modeling");
        m_submitBtn->setText("Login to Portfolio  →");
        m_switchPromptLabel->setText("Don't have an account?");
        m_switchModeBtn->setText("Create account");
    }

    m_statusLabel->hide();
}

void LoginDialog::onToggleModeClicked() {
    setSignUpMode(!m_isSignUpMode);
}

void LoginDialog::onTogglePasswordVisibility() {
    m_pwdVisible = !m_pwdVisible;
    m_passwordEdit->setEchoMode(m_pwdVisible ? QLineEdit::Normal : QLineEdit::Password);
    m_pwdEyeBtn->setText(m_pwdVisible ? "🙈" : "👁");
}

void LoginDialog::onPasswordInputChanged(const QString& text) {
    if (m_isSignUpMode) {
        updatePasswordStrength(text);
        onConfirmPasswordChanged(m_confirmPasswordEdit->text());
    }
}

void LoginDialog::onConfirmPasswordChanged(const QString& text) {
    if (!m_isSignUpMode || text.isEmpty()) {
        m_matchStatusLabel->setText("");
        return;
    }

    if (text == m_passwordEdit->text()) {
        m_matchStatusLabel->setText("✓ Passwords match");
        m_matchStatusLabel->setStyleSheet("color: #006a61; font-size: 11px; font-weight: 600;");
    } else {
        m_matchStatusLabel->setText("✕ Passwords do not match");
        m_matchStatusLabel->setStyleSheet("color: #ba1a1a; font-size: 11px; font-weight: 600;");
    }
}

void LoginDialog::updatePasswordStrength(const QString& pass) {
    int score = 0;
    if (pass.length() >= 8) score++;
    if (pass.contains(QRegularExpression("[0-9]"))) score++;
    if (pass.contains(QRegularExpression("[A-Z]")) && pass.contains(QRegularExpression("[a-z]"))) score++;
    if (pass.contains(QRegularExpression("[^A-Za-z0-9]"))) score++;

    for (int i = 0; i < 4; ++i) {
        m_strengthBars[i]->setStyleSheet("background-color: #D3E4FE; border-radius: 2px;");
    }

    if (pass.isEmpty()) {
        m_strengthLabel->setText("Enter password");
        m_strengthLabel->setStyleSheet("color: #757682; font-size: 11px;");
    } else if (score <= 1) {
        m_strengthBars[0]->setStyleSheet("background-color: #BA1A1A; border-radius: 2px;");
        m_strengthLabel->setText("Weak Security (Minimum 8 characters)");
        m_strengthLabel->setStyleSheet("color: #BA1A1A; font-size: 11px; font-weight: 600;");
    } else if (score <= 3) {
        for (int i = 0; i <= score - 1; ++i) {
            m_strengthBars[i]->setStyleSheet("background-color: #86F2E4; border-radius: 2px;");
        }
        m_strengthLabel->setText("Moderate Strength");
        m_strengthLabel->setStyleSheet("color: #006A61; font-size: 11px; font-weight: 600;");
    } else {
        for (int i = 0; i < 4; ++i) {
            m_strengthBars[i]->setStyleSheet("background-color: #006A61; border-radius: 2px;");
        }
        m_strengthLabel->setText("Strong Protection (Institutional Grade)");
        m_strengthLabel->setStyleSheet("color: #006A61; font-size: 11px; font-weight: 600;");
    }
}

void LoginDialog::onActionClicked() {
    QString user = m_emailEdit->text().trimmed();
    QString pass = m_passwordEdit->text();
    QString err;

    if (user.isEmpty() || pass.isEmpty()) {
        m_statusLabel->setText("Please enter both email/username and password.");
        m_statusLabel->setStyleSheet("color: #BA1A1A; font-weight: 600; font-size: 12px;");
        m_statusLabel->show();
        return;
    }

    if (m_isSignUpMode) {
        if (pass != m_confirmPasswordEdit->text()) {
            m_statusLabel->setText("Passwords do not match.");
            m_statusLabel->setStyleSheet("color: #BA1A1A; font-weight: 600; font-size: 12px;");
            m_statusLabel->show();
            return;
        }

        if (m_authManager.registerUser(user, pass, err)) {
            m_statusLabel->setText("Account verified! You can now log in.");
            m_statusLabel->setStyleSheet("color: #006A61; font-weight: 600; font-size: 12px;");
            m_statusLabel->show();
            setSignUpMode(false);
        } else {
            m_statusLabel->setText(err);
            m_statusLabel->setStyleSheet("color: #BA1A1A; font-weight: 600; font-size: 12px;");
            m_statusLabel->show();
        }
    } else {
        // First check or auto-register standard sample user if first run
        if (!m_authManager.authenticateUser(user, pass, err)) {
            // Auto register default user for instant test if database is fresh
            QString regErr;
            if (user == "analyst@investcorp.com" && m_authManager.registerUser(user, pass, regErr)) {
                m_authenticatedUser = user;
                accept();
                return;
            }

            m_statusLabel->setText(err);
            m_statusLabel->setStyleSheet("color: #BA1A1A; font-weight: 600; font-size: 12px;");
            m_statusLabel->show();
        } else {
            m_authenticatedUser = user;
            accept();
        }
    }
}

void LoginDialog::applyStyles() {
    setStyleSheet(
        "QDialog {"
        "   background-color: #F8F9FF;"
        "}"
        "#mainCard {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #E5EEFF;"
        "   border-radius: 16px;"
        "}"
        "#logoIconBox {"
        "   background-color: #EFF4FF;"
        "   border-radius: 12px;"
        "}"
        "#badgePill {"
        "   background-color: #EFF4FF;"
        "   color: #006A61;"
        "   font-size: 10px;"
        "   font-weight: 700;"
        "   padding: 3px 10px;"
        "   border-radius: 12px;"
        "}"
        "#cardTitle {"
        "   color: #0B1C30;"
        "   font-size: 18px;"
        "   font-weight: 700;"
        "   letter-spacing: -0.5px;"
        "}"
        "#cardSubtitle {"
        "   color: #444651;"
        "   font-size: 12px;"
        "}"
        "#fieldLabel {"
        "   color: #0B1C30;"
        "   font-size: 11px;"
        "   font-weight: 700;"
        "   letter-spacing: 0.5px;"
        "}"
        "#inputField {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #C5C5D3;"
        "   border-radius: 8px;"
        "   padding: 8px 12px;"
        "   color: #0B1C30;"
        "   font-size: 13px;"
        "}"
        "#inputField:focus {"
        "   border: 2px solid #00236F;"
        "}"
        "#eyeBtn {"
        "   background-color: transparent;"
        "   border: none;"
        "   font-size: 16px;"
        "   margin-left: -36px;"
        "}"
        "#primaryBtn {"
        "   background-color: #00236F;"
        "   color: #FFFFFF;"
        "   font-size: 13px;"
        "   font-weight: 600;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 10px;"
        "}"
        "#primaryBtn:hover {"
        "   background-color: #1E3A8A;"
        "}"
        "#primaryBtn:pressed {"
        "   background-color: #00164E;"
        "}"
        "#microBadge {"
        "   background-color: #EFF4FF;"
        "   color: #444651;"
        "   font-size: 10px;"
        "   font-weight: 500;"
        "   padding: 4px 8px;"
        "   border-radius: 12px;"
        "}"
        "#linkBtn {"
        "   background: transparent;"
        "   border: none;"
        "   color: #006A61;"
        "   font-weight: 600;"
        "   font-size: 12px;"
        "}"
        "#linkBtn:hover {"
        "   text-decoration: underline;"
        "}"
    );
}
