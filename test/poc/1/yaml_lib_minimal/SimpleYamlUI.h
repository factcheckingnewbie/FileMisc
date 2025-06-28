#ifndef SIMPLEYAMLUI_H
#define SIMPLEYAMLUI_H

#include <QWidget>
#include <QString>
#include <QList>
#include <QPushButton>
#include <QMenu>
#include <QMenuBar>
class SimpleYamlUI : public QObject {
    Q_OBJECT
    
public:
    explicit SimpleYamlUI(QObject *parent = nullptr);
    ~SimpleYamlUI();
    
    // Load UI from YAML file
    bool loadFile(const QString &filename);
    
    // Load UI from YAML string
    bool loadString(const QString &yamlContent);
    
    // Get created widgets
    QList<QPushButton*> buttons() const { return m_buttons; }
    QList<QMenu*> menus() const { return m_menus; }
    
    // Add widgets to a layout/menubar
    void addButtonsTo(QLayout *layout);
    void addMenusTo(QMenuBar *menuBar);
    
signals:
    void buttonClicked(const QString &id);
    void menuTriggered(const QString &id);
    void error(const QString &message);
    
private:
    QList<QPushButton*> m_buttons;
    QList<QMenu*> m_menus;
    void clearAll();
};

#endif // SIMPLEYAMLUI_H
