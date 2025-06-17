#pragma once

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QUrl>
#include <QUuid>
#include <QDir>  // FIXED: Added missing include
#include "FilePanel.h"

class PanelWrapper : public QWidget
{
    Q_OBJECT
public:
    explicit PanelWrapper(const QString &panelId = "", QWidget *parent = nullptr);
    ~PanelWrapper() override = default;  // FIXED: Added destructor
    
    // FIXED: Removed direct FilePanel access to maintain encapsulation
    // FilePanel* filePanel() const { return m_filePanel; }
    
    // Panel identification for CommandMaster
    QString panelId() const { return m_panelId; }
    
    // Convenience methods to forward to FilePanel
    void setDirectory(const QUrl &url);
    QUrl currentUrl() const;
    
    // FIXED: Added method to check if panel is active
    bool isActive() const { return m_isActive; }
    void setActive(bool active);
    
    // FUTURE CommandMaster: Set command execution context
    // This will allow panels to have different execution contexts (plain/sudo/docker/ssh/etc)
    // void setCommandContext(const QString &context);
    
    // FUTURE CommandMaster: Enable/disable command mode
    // void setCommandModeEnabled(bool enabled);
    
    // FUTURE ActionMotor: Register available actions for this panel
    // void registerAction(const QString &actionId, const QVariantMap &metadata);

signals:
    // Current navigation - includes panelId for future action routing
    void goToTreeRequested(const QUrl &url, const QString &panelId);
    
    // FIXED: Added signal for panel activation
    void panelActivated(PanelWrapper *panel);
    
    // FUTURE CommandMaster: Generic action request signal
    // This will be the main interface to ActionMotor
    // ActionMotor will receive these and decide what to do based on:
    // - Action type (navigation, file operation, custom command, etc)
    // - Current context (which panel, what permissions, etc)
    // - YAML configuration
    // void actionRequested(const QString &actionId, const QVariantMap &parameters, const QString &panelId);
    
    // FUTURE CommandMaster: Command/alias typed in command bar
    // void commandTyped(const QString &command, const QString &panelId);
    
    // FUTURE CommandMaster: Context change request
    // void contextChangeRequested(const QString &context, const QString &panelId);
    
    // FUTURE ActionMotor: Action completion notification
    // void actionCompleted(const QString &actionId, bool success, const QString &panelId);
    
    // FUTURE Audit: All actions should emit this for logging
    // void auditableActionPerformed(const QString &action, const QVariantMap &details, const QString &panelId);

protected:
    // FIXED: Added proper event handling
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onGoToTreeClicked();

private:
    FilePanel *m_filePanel;
    QPushButton *m_goToTreeButton;
    QString m_panelId;
    QUrl m_currentUrl;  // Track current URL since FilePanel doesn't expose it
    bool m_isActive;    // FIXED: Track active state
    
    // FUTURE CommandMaster: Command execution context
    // QString m_commandContext; // plain/sudo/docker/ssh/etc
    
    // FUTURE CommandMaster: Command bar widget
    // QLineEdit *m_commandBar;
    
    // FUTURE CommandMaster: Context selector
    // QComboBox *m_contextSelector;
    
    // FUTURE ActionMotor: Map of registered actions
    // QMap<QString, QVariantMap> m_registeredActions;
};

// FUTURE ActionMotor: Action request structure
// struct ActionRequest {
//     QString actionId;
//     QString panelId;
//     QString context;
//     QVariantMap parameters;
//     QDateTime timestamp;
//     QString user;
// };
