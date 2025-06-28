#ifndef JSON_TO_DSL_H
#define JSON_TO_DSL_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class JsonToDSL {
public:
    JsonToDSL() = default;
    ~JsonToDSL() = default;
    
    // Convert JSON document to DSL string
    QString convert(const QJsonDocument& doc);
    
    // Convert JSON file to DSL string
    QString convertFile(const QString& filePath);
    
private:
    void writeElement(QString& output, const QJsonObject& element, int indent);
    void writeProperties(QString& output, const QJsonObject& element);
    QString indentString(int level) const;
    
    // Property order for consistent output
    bool shouldSkipProperty(const QString& propName, const QString& elementType) const;
    QStringList getPropertyOrder(const QString& elementType) const;
};

#endif // JSON_TO_DSL_H
