#ifndef DSL_GENERATOR_H
#define DSL_GENERATOR_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class DSLGenerator {
public:
    DSLGenerator() = default;
    ~DSLGenerator() = default;
    
    // Generate DSL from JSON document
    QString generate(const QJsonDocument& doc);
    
    // Generate DSL from JSON string
    QString generateFromString(const QString& jsonString);

private:
    void generateElement(const QJsonObject& element, QString& output, int indent);
    QString generateProperties(const QJsonObject& element);
    QString getIndentString(int level) const;
    
    // Property ordering for consistent output
    QStringList getPropertyOrder(const QString& elementType) const;
};

#endif // DSL_GENERATOR_H
