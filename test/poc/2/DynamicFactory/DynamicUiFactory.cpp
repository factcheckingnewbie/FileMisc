#include "DynamicUiFactory.h"
#include <QFile>
#include <QDebug>

DynamicUiFactory::DynamicUiFactory(QObject *parent) : QObject(parent) {}

QWidget* DynamicUiFactory::createPanelFromYaml(const QString &yamlPath, QWidget *parent) {
    QWidget *panel = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(panel);

    YAML::Node config = YAML::LoadFile(yamlPath.toStdString());
    if (!config["buttons"]) {
        qWarning() << "YAML has no 'buttons' key";
        return panel;
    }
    for (const auto &btnNode : config["buttons"]) {
        QString text = QString::fromStdString(btnNode["text"].as<std::string>());
        QString color = btnNode["color"] ? QString::fromStdString(btnNode["color"].as<std::string>()) : "";
        QString command = QString::fromStdString(btnNode["command"].as<std::string>());
        QPushButton *btn = new QPushButton(text, panel);
        if (!color.isEmpty()) {
            btn->setStyleSheet(QString("background-color:%1;").arg(color));
        }
        QObject::connect(btn, &QPushButton::clicked, this, [this, command]() {
            runShellCommand(command);
        });
        layout->addWidget(btn);
    }
    panel->setLayout(layout);
    return panel;
}

void DynamicUiFactory::runShellCommand(const QString &command) {
    QProcess *proc = new QProcess(this);
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, proc, command](int exitCode, QProcess::ExitStatus) {
        emit commandExecuted(command, exitCode, QString::fromUtf8(proc->readAllStandardOutput()), QString::fromUtf8(proc->readAllStandardError()));
        proc->deleteLater();
    });
    proc->start("sh", QStringList() << "-c" << command);
}
