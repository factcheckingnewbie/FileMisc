#include "json_to_dsl.h"
#include <QFile>
#include <QJsonParseError>

QString JsonToDSL::convert(const QJsonDocument& doc) {
    if (!doc.isObject()) {
        return QString();
    }
    
    QJsonObject root = doc.object();
    if (!root.contains("ui")) {
        return QString();
    }
    
    QString output;
    QJsonArray uiArray = root["ui"].toArray();
    
    for (const QJsonValue& value : uiArray) {
        if (value.isObject()) {
            writeElement(output, value.toObject(), 0);
        }
    }
    
    return output;
}

QString JsonToDSL::convertFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        return QString();
    }
    
    return convert(doc);
}

void JsonToDSL::writeElement(QString& output, const QJsonObject& element, int indent) {
    // Add indentation
    output += indentString(indent);
    
    // Write element type
    QString type = element["type"].toString();
    output += type;
    
    // Check for quoted string properties (title, text, name, label, etc.)
    QStringList quotedProps = {"title", "text", "name", "label", "caption", "placeholder", "description", "alias"};
    
    // First check if element has a non-property identifier (like Panel subtypes)
    if (type == "Panel" && element.contains("subtype")) {
        QString subtype = element["subtype"].toString();
        output += " " + subtype;
    } else if (type == "Command" && element.contains("name")) {
        // Command name is not quoted
        output += " " + element["name"].toString();
    } else if (type == "Step" && element.contains("name")) {
        // Step name is not quoted
        output += " " + element["name"].toString();
    } else {
        // Check for quoted properties
        for (const QString& prop : quotedProps) {
            if (element.contains(prop) && element[prop].isString()) {
                output += " \"" + element[prop].toString() + "\"";
                break;  // Only use first found
            }
        }
    }
    
    // Write all other properties
    writeProperties(output, element);
    
    output += "\n";
    
    // Write children recursively
    if (element.contains("children")) {
        QJsonArray children = element["children"].toArray();
        for (const QJsonValue& child : children) {
            if (child.isObject()) {
                writeElement(output, child.toObject(), indent + 1);
            }
        }
    }
}

void JsonToDSL::writeProperties(QString& output, const QJsonObject& element) {
    // Properties to skip (already handled or internal)
    QStringList skipProps = {"type", "children", "title", "text", "name", "label", "caption", 
                            "placeholder", "description", "alias"};
    
    // Skip subtype for Panel as it's already written
    if (element["type"].toString() == "Panel" && element.contains("subtype")) {
        skipProps.append("subtype");
    }
    
    // For Command and Step, name is not skipped as it's written without quotes
    if (element["type"].toString() == "Command" || element["type"].toString() == "Step") {
        skipProps.removeAll("name");
    }
    
    // Write all properties except skipped ones
    for (auto it = element.begin(); it != element.end(); ++it) {
        if (skipProps.contains(it.key())) {
            continue;
        }
        
        output += " " + it.key() + ":";
        
        QJsonValue value = it.value();
        if (value.isString()) {
            QString strVal = value.toString();
            // Check if value needs quoting (contains spaces or special chars)
            if (strVal.contains(' ') || strVal.contains('"') || strVal.contains('\'') || 
                strVal.contains('{') || strVal.contains('}')) {
                // Use single quotes if string contains double quotes
                if (strVal.contains('"')) {
                    output += "'" + strVal + "'";
                } else {
                    output += "'" + strVal + "'";
                }
            } else {
                output += strVal;
            }
        } else if (value.isBool()) {
            output += value.toBool() ? "true" : "false";
        } else if (value.isDouble()) {
            // Check if it's actually an integer
            double d = value.toDouble();
            int i = static_cast<int>(d);
            if (d == i) {
                output += QString::number(i);
            } else {
                output += QString::number(d);
            }
        } else if (value.isArray()) {
            // Convert array to comma-separated values
            QJsonArray arr = value.toArray();
            QStringList values;
            for (const QJsonValue& v : arr) {
                if (v.isDouble()) {
                    double d = v.toDouble();
                    int i = static_cast<int>(d);
                    values << QString::number(d == i ? i : d);
                } else if (v.isString()) {
                    values << v.toString();
                } else if (v.isBool()) {
                    values << (v.toBool() ? "true" : "false");
                }
            }
            output += values.join(",");
        }
    }
}

QString JsonToDSL::indentString(int level) const {
    return QString(level * 4, ' ');
}

bool JsonToDSL::shouldSkipProperty(const QString& propName, const QString& elementType) const {
    // Not used in generic version
    return false;
}

QStringList JsonToDSL::getPropertyOrder(const QString& elementType) const {
    // Not used in generic version
    return QStringList();
}
