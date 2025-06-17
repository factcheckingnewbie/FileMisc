#include "PanelWrapper.h"
#include <QDebug>
#include <QMouseEvent>

PanelWrapper::PanelWrapper(const QString &panelId, QWidget *parent)
    : QWidget(parent)
    , m_filePanel(new FilePanel(this))
    , m_goToTreeButton(new QPushButton("Go To Tree", this))
    , m_panelId(panelId.isEmpty() ? QUuid::createUuid().toString() : panelId)
    , m_currentUrl(QUrl::fromLocalFile(QDir::homePath()))
    , m_followsTreePanel(false)  // Default: panels don't follow tree
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
    
    // Set initial visual state
    updateVisualState();
    
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
    if (!url.isValid()) {
        qDebug() << "[WARNING] Invalid URL passed to setDirectory:" << url.toString();
        return;
    }
    
    m_currentUrl = url;
    m_filePanel->setDirectory(url);
    
    qDebug() << "[DEBUG] Panel" << m_panelId << "directory set to:" << url.toString();
    
    // FUTURE Audit: Log directory changes
    // QVariantMap details;
    // details["previousUrl"] = m_filePanel->currentUrl().toString();
    // details["newUrl"] = url.toString();
    // emit auditableActionPerformed("directory:changed", details, m_panelId);
}

QUrl PanelWrapper::currentUrl() const
{
    return m_currentUrl;
}

void PanelWrapper::setFollowsTreePanel(bool follows)
{
    if (m_followsTreePanel == follows) return;
    
    m_followsTreePanel = follows;
    updateVisualState();
    
    qDebug() << "[DEBUG] Panel" << m_panelId 
             << (follows ? "NOW FOLLOWS" : "NO LONGER FOLLOWS") 
             << "TreePanel selections";
    
    emit followsTreePanelToggled(follows, m_panelId);
}

void PanelWrapper::updateVisualState()
{
    if (m_followsTreePanel) {
        // Green border when following tree
        setStyleSheet("PanelWrapper { border: 2px solid #27ae60; }");
        m_goToTreeButton->setText("Go To Tree [FOLLOWING]");
    } else {
        // No border when not following
        setStyleSheet("");
        m_goToTreeButton->setText("Go To Tree");
    }
}

void PanelWrapper::onTreePanelDirectorySelected(const QUrl &url)
{
    // Only update if this panel is set to follow tree
    if (m_followsTreePanel) {
        qDebug() << "[DEBUG] Panel" << m_panelId << "following TreePanel to:" << url.toString();
        setDirectory(url);
    }
}

void PanelWrapper::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Toggle follows tree state on click
        setFollowsTreePanel(!m_followsTreePanel);
    }
    QWidget::mousePressEvent(event);
}

void PanelWrapper::onGoToTreeClicked()
{
    qDebug() << "[DEBUG] 'Go To Tree' clicked from panel:" << m_panelId;
    qDebug() << "  Current URL:" << m_currentUrl.toString();
    
    // Emit signal with panel identification
    emit goToTreeRequested(m_currentUrl, m_panelId);
    
    // FUTURE ActionMotor: This will become a generic action request
    // QVariantMap params;
    // params["targetUrl"] = m_currentUrl.toString();
    // params["source"] = "panel";
    // emit actionRequested("navigation:syncTreeToPanel", params, m_panelId);
    
    // FUTURE Audit: Log this action
    // QVariantMap auditDetails;
    // auditDetails["action"] = "goToTree";
    // auditDetails["url"] = m_currentUrl.toString();
    // emit auditableActionPerformed("navigation:goToTree", auditDetails, m_panelId);
}
