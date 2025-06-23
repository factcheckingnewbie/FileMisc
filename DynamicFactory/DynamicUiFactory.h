#pragma once

#include <QWidget>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>
#include <QString>
#include <QObject>
#include <yaml-cpp/yaml.h>

// Minimal dynamic UI factory for YAML-driven panels/buttons

class DynamicUiFactory : public QObject {
    Q_OBJECT
public:
    explicit DynamicUiFactory(QObject *parent = nullptr);

    // Loads YAML from file and creates a QWidget with buttons as defined
    QWidget* createPanelFromYaml(const QString &yamlPath, QWidget *parent = nullptr);

signals:
    void commandExecuted(const QString &command, int exitCode, const QString &stdoutText, const QString &stderrText);

private:
    void runShellCommand(const QString &command);
};
