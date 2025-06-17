#include "PanelWrapper.h"
#include <QDebug>

PanelWrapper::PanelWrapper(const QString &panelId, QWidget *parent)
    : QWidget(parent)
    , m_filePanel(new FilePanel(this))
    , m_goToTreeButton(new QPushButton("Go To Tree", this))
    , m_panelId(panelId.isEmpty() ? QUuid::createUuid().toString() : panelId)
{
    // Set up layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    
    // Configure button
    m_goToTreeButton->setMaximumHeight(30);
    
    // FUTURE CommandMaster: Add command bar here
    // m_commandBar = new QLineEdit(this);
    // m_commandBar->setPlaceholderText("Type command or alias...");
    // layout->addWidget(m_commandBar);
    
    // FUTURE CommandMaster: Add context selector
    // m_contextSelector = new QComboBox(this);
    // m_contextSelector->addItems({"plain", "sudo", "docker"});
    // layout->addWidget(m_contextSelector);
    
    // Add widgets
    layout->addWidget(m_goToTreeButton);
    layout->addWidget(m_filePanel);
    
    // Connect button
    connect(m_goToTreeButton, &QPushButton::clicked, this, &PanelWrapper::onGoToTreeClicked);
    
    // FUTURE CommandMaster: Connect command bar
    // connect(m_commandBar, &QLineEdit::returnPressed, this, [this]() {
    //     QString command = m_commandBar->text();
    //     emit commandTyped(command, m_panelId);
    //     
    //     // Build action request
    //     QVariantMap params;
    //     params["command"] = command;
    //     params["currentPath"] = currentUrl().toString();
    //     emit actionRequested("command:execute", params, m_panelId);
    // });
    
    // FUTURE CommandMaster: Context changes
    // connect(m_contextSelector, &QComboBox::currentTextChanged, this, [this](const QString &context) {
    //     emit contextChangeRequested(context, m_panelId);
    // });
    
    qDebug() << "[DEBUG] PanelWrapper created with ID:" << m_panelId;
}

void PanelWrapper::setDirectory(const QUrl &url)
{
    m_filePanel->setDirectory(url);
    
    // FUTURE Audit: Log directory changes
    // QVariantMap details;
    // details["previousUrl"] = m_filePanel->currentUrl().toString();
    // details["newUrl"] = url.toString();
    // emit auditableActionPerformed("directory:changed", details, m_panelId);
}

QUrl PanelWrapper::currentUrl() const
{
    return m_filePanel->currentUrl();
}

void PanelWrapper::onGoToTreeClicked()
{
    QUrl currentUrl = m_filePanel->currentUrl();
    qDebug() << "[DEBUG] 'Go To Tree' clicked from panel:" << m_panelId;
    qDebug() << "  Current URL:" << currentUrl.toString();
    
    // Emit signal with panel identification
    emit goToTreeRequested(currentUrl, m_panelId);
    
    // FUTURE ActionMotor: This will become a generic action request
    // QVariantMap params;
    // params["targetUrl"] = currentUrl.toString();
    // params["source"] = "panel";
    // emit actionRequested("navigation:syncTreeToPanel", params, m_panelId);
    
    // FUTURE Audit: Log this action
    // QVariantMap auditDetails;
    // auditDetails["action"] = "goToTree";
    // auditDetails["url"] = currentUrl.toString();
    // emit auditableActionPerformed("navigation:goToTree", auditDetails, m_panelId);
}

// FUTURE CommandMaster: Command context management
// void PanelWrapper::setCommandContext(const QString &context)
// {
//     m_commandContext = context;
//     emit contextChangeRequested(context, m_panelId);
//     
//     // Update UI to reflect context
//     if (context == "sudo") {
//         m_commandBar->setStyleSheet("QLineEdit { background-color: #ffcccc; }");
//     } else if (context == "docker") {
//         m_commandBar->setStyleSheet("QLineEdit { background-color: #ccccff; }");
//     } else {
//         m_commandBar->setStyleSheet("");
//     }
// }

// FUTURE ActionMotor: Register available actions
// void PanelWrapper::registerAction(const QString &actionId, const QVariantMap &metadata)
// {
//     m_registeredActions[actionId] = metadata;
//     qDebug() << "[DEBUG] Action registered:" << actionId << "for panel:" << m_panelId;
// }
