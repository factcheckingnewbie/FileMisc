#include "dsl_generator.h"
#include <QJsonValue>
#include <QDebug>

QString DSLGenerator::generate(const QJsonDocument& doc) {
    if (!doc.isObject()) {
        return QString();
    }
    
    QJsonObject root = doc.object();
    if (!root.contains("ui")) {
        return QString();
    }
    
    QString output;
    QJsonArray uiArray = root["ui"].toArray();
    
    bool firstElement = true;
    for (const QJsonValue& value : uiArray) {
        if (!firstElement) {
            output += "\n";
        }
        firstElement = false;
        
        QJsonObject element = value.toObject();
        generateElement(element, output, 0);
    }
    
    return output;
}

QString DSLGenerator::generateFromString(const QString& jsonString) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        return QString();
    }
    
    return generate(doc);
}

void DSLGenerator::generateElement(const QJsonObject& element, QString& output, int indent) {
    QString type = element["type"].toString();
    QString indentStr = getIndentString(indent);
    
    output += indentStr + type;
    
    // Add name/title/text based on element type
    if (type == "Window" && element.contains("title")) {
        output += " \"" + element["title"].toString() + "\"";
    } else if (type == "FoldableMenu" && element.contains("title")) {
        output += " \"" + element["title"].toString() + "\"";
    } else if ((type == "Button" || type == "Item") && element.contains("text")) {
        output += " \"" + element["text"].toString() + "\"";
    } else if (type == "Panel" && element.contains("subtype")) {
        output += " " + element["subtype"].toString();
    }
    
    // Add properties
    QString props = generateProperties(element);
    if (!props.isEmpty()) {
        output += " " + props;
    }
    
    output += "\n";
    
    // Generate children
    if (element.contains("children")) {
        QJsonArray children = element["children"].toArray();
        for (const QJsonValue& childValue : children) {
            QJsonObject child = childValue.toObject();
            generateElement(child, output, indent + 1);
        }
    }
}

QString DSLGenerator::generateProperties(const QJsonObject& element) {
    QString result;
    QString type = element["type"].toString();
    
    // Get property order for this element type
    QStringList propertyOrder = getPropertyOrder(type);
    
    // First, add properties in preferred order
    for (const QString& prop : propertyOrder) {
        if (element.contains(prop)) {
            // Skip special properties that are already handled
            if (prop == "type" || prop == "children" || 
                (prop == "title" && (type == "Window" || type == "FoldableMenu")) ||
                (prop == "text" && (type == "Button" || type == "Item")) ||
                (prop == "subtype" && type == "Panel")) {
                continue;
            }
            
            if (!result.isEmpty()) {
                result += " ";
            }
            
            QJsonValue value = element[prop];
            if (value.isString()) {
                result += prop + ":" + value.toString();
            } else if (value.isDouble()) {
                int intVal = value.toInt();
                double doubleVal = value.toDouble();
                if (intVal == doubleVal) {
                    result += prop + ":" + QString::number(intVal);
                } else {
                    result += prop + ":" + QString::number(doubleVal);
                }
            } else if (value.isBool()) {
                result += prop + ":" + (value.toBool() ? "true" : "false");
            } else if (value.isArray() && prop == "sizes") {
                QJsonArray arr = value.toArray();
                QStringList sizes;
                for (const QJsonValue& v : arr) {
                    sizes << QString::number(v.toInt());
                }
                result += prop + ":" + sizes.join(",");
            }
        }
    }
    
    // Then add any remaining properties not in the order list
    for (auto it = element.begin(); it != element.end(); ++it) {
        QString key = it.key();
        if (!propertyOrder.contains(key) && 
            key != "type" && key != "children" &&
            !(key == "title" && (type == "Window" || type == "FoldableMenu")) &&
            !(key == "text" && (type == "Button" || type == "Item")) &&
            !(key == "subtype" && type == "Panel")) {
            
            if (!result.isEmpty()) {
                result += " ";
            }
            
            QJsonValue value = it.value();
            if (value.isString()) {
                result += key + ":" + value.toString();
            } else if (value.isDouble()) {
                int intVal = value.toInt();
                double doubleVal = value.toDouble();
                if (intVal == doubleVal) {
                    result += key + ":" + QString::number(intVal);
                } else {
                    result += key + ":" + QString::number(doubleVal);
                }
            } else if (value.isBool()) {
                result += key + ":" + (value.toBool() ? "true" : "false");
            }
        }
    }
    
    return result;
}

QString DSLGenerator::getIndentString(int level) const {
    return QString(level * 4, ' ');
}

QStringList DSLGenerator::getPropertyOrder(const QString& elementType) const {
    // Define preferred property order for each element type
    if (elementType == "Window") {
        return {"width", "height"};
    } else if (elementType == "Panel") {
        return {"position", "orientation", "color", "sizes"};
    } else if (elementType == "Item") {
        return {"action", "shortcut", "icon", "checked"};
    } else if (elementType == "Button") {
        return {"action", "icon"};
    }
    
    // Default order for common properties
    return {"action", "position", "orientation", "color", "width", "height", "icon", "shortcut", "checked"};
}
