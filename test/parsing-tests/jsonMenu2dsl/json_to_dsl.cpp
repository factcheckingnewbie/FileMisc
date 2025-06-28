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
    QString type = element["type"].toString();
    
    // Add indentation
    output += indentString(indent);
    
    // Write element type
    output += type;
    
    // Write name/title/text based on element type
    if (type == "Window" && element.contains("title")) {
        output += " \"" + element["title"].toString() + "\"";
    } else if (type == "Panel" && element.contains("subtype")) {
        output += " " + element["subtype"].toString();
    } else if ((type == "Button" || type == "Item") && element.contains("text")) {
        output += " \"" + element["text"].toString() + "\"";
    } else if (type == "FoldableMenu" && element.contains("title")) {
        output += " \"" + element["title"].toString() + "\"";
    }
    
    // Write properties
    writeProperties(output, element);
    
    output += "\n";
    
    // Write children
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
    QString type = element["type"].toString();
    QStringList propertyOrder = getPropertyOrder(type);
    
    // Write properties in order
    for (const QString& propName : propertyOrder) {
        if (element.contains(propName) && !shouldSkipProperty(propName, type)) {
            QJsonValue value = element[propName];
            
            output += " " + propName + ":";
            
            if (value.isString()) {
                output += value.toString();
            } else if (value.isBool()) {
                output += value.toBool() ? "true" : "false";
            } else if (value.isDouble()) {
                int intVal = static_cast<int>(value.toDouble());
                if (value.toDouble() == intVal) {
                    output += QString::number(intVal);
                } else {
                    output += QString::number(value.toDouble());
                }
            } else if (value.isArray() && propName == "sizes") {
                // Special handling for sizes array
                QJsonArray arr = value.toArray();
                QStringList sizes;
                for (const QJsonValue& v : arr) {
                    sizes << QString::number(v.toInt());
                }
                output += sizes.join(",");
            }
        }
    }
    
    // Write any remaining properties not in order
    for (auto it = element.begin(); it != element.end(); ++it) {
        if (!propertyOrder.contains(it.key()) && !shouldSkipProperty(it.key(), type)) {
            output += " " + it.key() + ":";
            
            if (it.value().isString()) {
                output += it.value().toString();
            } else if (it.value().isBool()) {
                output += it.value().toBool() ? "true" : "false";
            } else if (it.value().isDouble()) {
                int intVal = static_cast<int>(it.value().toDouble());
                if (it.value().toDouble() == intVal) {
                    output += QString::number(intVal);
                } else {
                    output += QString::number(it.value().toDouble());
                }
            }
        }
    }
}

QString JsonToDSL::indentString(int level) const {
    return QString(level * 4, ' ');
}

bool JsonToDSL::shouldSkipProperty(const QString& propName, const QString& elementType) const {
    // Skip internal properties
    if (propName == "type" || propName == "children") {
        return true;
    }
    
    // Skip properties that are part of the element declaration
    if (elementType == "Window" && propName == "title") {
        return true;
    }
    if (elementType == "Panel" && propName == "subtype") {
        return true;
    }
    if ((elementType == "Button" || elementType == "Item") && propName == "text") {
        return true;
    }
    if (elementType == "FoldableMenu" && propName == "title") {
        return true;
    }
    
    return false;
}

QStringList JsonToDSL::getPropertyOrder(const QString& elementType) const {
    // Define property order for consistent output
    if (elementType == "Window") {
        return {"width", "height"};
    } else if (elementType == "Panel") {
        return {"position", "orientation", "color", "sizes"};
    } else if (elementType == "Item") {
        return {"action", "shortcut", "icon", "checked"};
    } else if (elementType == "Button") {
        return {"action", "icon"};
    }
    return QStringList();
}
