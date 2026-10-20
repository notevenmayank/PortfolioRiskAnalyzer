#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QFrame>
#include "AuthManager.h"

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog() override = default;

    QString getAuthenticatedUsername() const;

private slots:
    void onToggleModeClicked();
    void onTogglePasswordVisibility();
    void onActionClicked();
    void onPasswordInputChanged(const QString& text);
    void onConfirmPasswordChanged(const QString& text);

private:
    void setupUi();
    void setSignUpMode(bool signUp);
    void applyStyles();
    void updatePasswordStrength(const QString& pass);

    AuthManager m_authManager;
    QString m_authenticatedUser;
    bool m_isSignUpMode{false};

    // Header widgets
    QLabel *m_badgePill;
    QLabel *m_titleLabel;
    QLabel *m_subtitleLabel;

    // Inputs
    QLineEdit *m_emailEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_pwdEyeBtn;
    bool m_pwdVisible{false};

    // Sign up specific widgets
    QWidget *m_strengthMeterContainer;
    QFrame *m_strengthBars[4];
    QLabel *m_strengthLabel;

    QWidget *m_confirmContainer;
    QLineEdit *m_confirmPasswordEdit;
    QLabel *m_matchStatusLabel;

    QCheckBox *m_rememberCheck;
    QCheckBox *m_termsCheck;
    QWidget *m_optionsContainer;

    QLabel *m_statusLabel;
    QPushButton *m_submitBtn;
    QPushButton *m_switchModeBtn;
    QLabel *m_switchPromptLabel;
};

#endif // LOGINDIALOG_H
