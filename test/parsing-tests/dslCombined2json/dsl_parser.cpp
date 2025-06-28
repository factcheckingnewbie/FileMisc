#include "dsl_parser.h"
#include <QRegularExpression>
#include <QDebug>

QJsonDocument DSLParser::parse(const QString& content) {
    clearError();
    
    if (content.trimmed().isEmpty()) {
        setError("Empty content", 1);
        return QJsonDocument();
    }
    
    // Tokenize
    std::vector<Token> tokens = tokenize(content);
    if (!success()) {
        return QJsonDocument();
    }
    
    // Parse
    ParseState state;
    state.tokens = std::move(tokens);
    
    QJsonArray elements;
    
    // Skip initial newlines
    while (check(state, Token::NEWLINE)) {
        advance(state);
    }
    
    while (!isAtEnd(state)) {
        // Check for indent token followed by identifier
        size_t savedPos = state.currentToken;
        
        // Skip newlines
        while (check(state, Token::NEWLINE)) {
            advance(state);
        }
        
        if (isAtEnd(state)) {
            break;
        }
        
        // We should have indent token, then identifier
        if (check(state, Token::INDENT)) {
            size_t afterIndent = state.currentToken + 1;
            if (afterIndent < state.tokens.size() && state.tokens[afterIndent].type == Token::IDENTIFIER) {
                QJsonObject element = parseElement(state);
                if (!success()) {
                    return QJsonDocument();
                }
                elements.append(element);
            } else {
                break;
            }
        } else {
            break;
        }
    }
    
    QJsonObject root;
    root["ui"] = elements;
    
    return QJsonDocument(root);
}

std::vector<DSLParser::Token> DSLParser::tokenize(const QString& content) {
    std::vector<Token> tokens;
    QStringList lines = content.split('\n');
    
    for (int lineNum = 0; lineNum < lines.size(); ++lineNum) {
        const QString& line = lines[lineNum];
        
        // Calculate indentation
        int indent = 0;
        int i = 0;
        while (i < line.length() && (line[i] == ' ' || line[i] == '\t')) {
            indent += (line[i] == '\t') ? 4 : 1;
            ++i;
        }
        
        // Skip empty lines
        if (i >= line.length()) {
            continue;
        }
        
        // Add indent/dedent tokens
        if (!tokens.empty() && tokens.back().type != Token::NEWLINE) {
            tokens.push_back({Token::NEWLINE, "\n", lineNum, 0});
        }
        
        // Process line content
        QString trimmed = line.mid(i);
        int column = i;
        
        // Add INDENT token with actual indent level
        tokens.push_back({Token::INDENT, QString::number(indent), lineNum + 1, 1});
        
        // Tokenize line content
        i = 0;
        while (i < trimmed.length()) {
            QChar ch = trimmed[i];
            
            // Skip whitespace
            if (ch.isSpace()) {
                ++i;
                ++column;
                continue;
            }
            
            // String literal
            if (ch == '"' || ch == '\'') {
                QChar quoteChar = ch;
                int start = i;
                ++i;
                while (i < trimmed.length() && trimmed[i] != quoteChar) {
                    if (trimmed[i] == '\\' && i + 1 < trimmed.length()) {
                        ++i; // Skip escaped character
                    }
                    ++i;
                }
                
                if (i >= trimmed.length()) {
                    setError("Unterminated string", lineNum + 1, column + 1);
                    return tokens;
                }
                
                QString value = trimmed.mid(start + 1, i - start - 1);
                tokens.push_back({Token::STRING, value, lineNum + 1, column + 1});
                ++i;
                column += i - start;
            }
            // Colon
            else if (ch == ':') {
                tokens.push_back({Token::COLON, ":", lineNum + 1, column + 1});
                ++i;
                ++column;
            }
            // Number
            else if (ch.isDigit() || (ch == '-' && i + 1 < trimmed.length() && trimmed[i + 1].isDigit())) {
                int start = i;
                if (ch == '-') ++i;
                while (i < trimmed.length() && (trimmed[i].isDigit() || trimmed[i] == '.')) {
                    ++i;
                }
                QString value = trimmed.mid(start, i - start);
                tokens.push_back({Token::NUMBER, value, lineNum + 1, column + 1});
                column += i - start;
            }
            // Identifier or keyword (including @references)
            else if (ch.isLetter() || ch == '_' || ch == '@') {
                int start = i;
                if (ch == '@') ++i;  // Include @ in identifier
                while (i < trimmed.length() && (trimmed[i].isLetterOrNumber() || trimmed[i] == '_' || trimmed[i] == '-' || trimmed[i] == '+')) {
                    ++i;
                }
                QString value = trimmed.mid(start, i - start);
                tokens.push_back({Token::IDENTIFIER, value, lineNum + 1, column + 1});
                column += i - start;
            }
            // Special handling for comma-separated values (e.g., sizes:300,500)
            else if (ch == ',') {
                // Continue with next iteration, comma is part of value parsing
                ++i;
                ++column;
            }
            else {
                setError(QString("Unexpected character '%1'").arg(ch), lineNum + 1, column + 1);
                return tokens;
            }
        }
    }
    
    if (!tokens.empty() && tokens.back().type != Token::NEWLINE) {
        tokens.push_back({Token::NEWLINE, "\n", lines.size(), 0});
    }
    
    tokens.push_back({Token::END_OF_FILE, "", lines.size() + 1, 0});
    
    return tokens;
}

QJsonObject DSLParser::parseElement(ParseState& state) {
    // Skip newlines but not indent tokens
    while (check(state, Token::NEWLINE)) {
        advance(state);
    }
    
    // Get indentation
    int indent = 0;
    if (check(state, Token::INDENT)) {
        indent = advance(state).value.toInt();
    }
    
    // Element type
    if (!check(state, Token::IDENTIFIER)) {
        setError("Expected element type", peek(state).line, peek(state).column);
        return QJsonObject();
    }
    
    QString elementType = advance(state).value;
    QString elementName;
    
    // Element name (optional, for quoted strings)
    if (check(state, Token::STRING)) {
        elementName = advance(state).value;
    }
    
    // Parse properties and create element
    QJsonObject element = parseProperties(state, elementType, elementName);
    element["type"] = elementType;
    
    if (!elementName.isEmpty()) {
        if (elementType == "Window") {
            element["title"] = elementName;
        } else if (elementType == "Button" || elementType == "Item") {
            element["text"] = elementName;
        } else if (elementType == "FoldableMenu") {
            element["title"] = elementName;
        } else if (elementType == "Panel" && !elementName.isEmpty()) {
            element["subtype"] = elementName;
        }
    }
    
    // Skip to next line
    while (!isAtEnd(state) && !check(state, Token::NEWLINE) && !check(state, Token::END_OF_FILE)) {
        advance(state);
    }
    
    // Parse children
    QJsonArray children = parseChildren(state, indent);
    if (!children.isEmpty()) {
        element["children"] = children;
    }
    
    return element;
}

QJsonObject DSLParser::parseProperties(ParseState& state, const QString& elementType, const QString& elementName) {
    QJsonObject properties;
    
    // Handle Panel subtypes
    if (elementType == "Panel" && check(state, Token::IDENTIFIER)) {
        Token next = peek(state);
        if (next.value != "position" && next.value != "orientation" && next.value != "color" && 
            next.value != "sizes" && next.value != "width" && next.value != "height") {
            properties["subtype"] = advance(state).value;
        }
    }
    
    // Parse property:value pairs
    while (!isAtEnd(state) && !check(state, Token::NEWLINE)) {
        if (!check(state, Token::IDENTIFIER)) {
            break;
        }
        
        Token propName = advance(state);
        
        if (!match(state, Token::COLON)) {
            // This might be a Panel subtype, not a property
            state.currentToken--; // Back up
            break;
        }
        
        // Parse property value
        if (check(state, Token::NUMBER)) {
            QString numStr = advance(state).value;
            bool isInt;
            int intVal = numStr.toInt(&isInt);
            if (isInt) {
                properties[propName.value] = intVal;
            } else {
                properties[propName.value] = numStr.toDouble();
            }
        }
        else if (check(state, Token::STRING)) {
            properties[propName.value] = advance(state).value;
        }
        else if (check(state, Token::IDENTIFIER)) {
            QString value = advance(state).value;
            
            // Handle special property names with colons (e.g., in:filelist, args:pattern,in:filelist)
            if (propName.value == "args" || propName.value == "in" || propName.value == "out") {
                // Collect full value including embedded colons
                QString fullValue = value;
                
                // Continue collecting until we hit a real property or newline
                while (!isAtEnd(state) && !check(state, Token::NEWLINE)) {
                    size_t savedPos = state.currentToken;
                    
                    // Check if next is a property (identifier:value pattern)
                    if (check(state, Token::IDENTIFIER)) {
                        Token nextId = peek(state);
                        advance(state);
                        if (check(state, Token::COLON)) {
                            // This is a new property, restore position and break
                            state.currentToken = savedPos;
                            break;
                        } else {
                            // Part of current value, add it
                            state.currentToken = savedPos;
                            if (match(state, Token::COLON)) {
                                fullValue += ":";
                            }
                            if (check(state, Token::IDENTIFIER) || check(state, Token::NUMBER)) {
                                fullValue += advance(state).value;
                            }
                        }
                    } else {
                        break;
                    }
                }
                
                // Handle comma-separated values with potential colons
                if (fullValue.contains(',') || fullValue.contains(':')) {
                    QStringList parts = fullValue.split(',');
                    if (parts.size() > 1) {
                        // Multiple values, store as array
                        QJsonArray arr;
                        for (const QString& part : parts) {
                            arr.append(part.trimmed());
                        }
                        properties[propName.value] = arr;
                    } else {
                        // Single value with potential colon
                        properties[propName.value] = fullValue;
                    }
                } else {
                    properties[propName.value] = fullValue;
                }
            }
            // Handle shell commands with quotes
            else if (propName.value == "shell") {
                // Expect a quoted string after shell:
                if (value.startsWith('\'') || value.startsWith('"')) {
                    QChar quoteChar = value[0];
                    QString shellCmd = value.mid(1);
                    
                    // Find closing quote
                    int endPos = shellCmd.indexOf(quoteChar);
                    if (endPos == -1) {
                        // Continue reading tokens until we find closing quote
                        while (!isAtEnd(state) && !check(state, Token::NEWLINE)) {
                            Token next = advance(state);
                            shellCmd += " " + next.value;
                            if (next.value.endsWith(quoteChar)) {
                                shellCmd = shellCmd.left(shellCmd.length() - 1);
                                break;
                            }
                        }
                    } else {
                        shellCmd = shellCmd.left(endPos);
                    }
                    properties[propName.value] = shellCmd;
                } else {
                    properties[propName.value] = value;
                }
            }
            // Handle comma-separated values (e.g., sizes:300,500)
            else if (value.contains(',')) {
                QStringList parts = value.split(',');
                QJsonArray arr;
                for (const QString& part : parts) {
                    QString trimmed = part.trimmed();
                    bool isNum;
                    int num = trimmed.toInt(&isNum);
                    if (isNum) {
                        arr.append(num);
                    } else {
                        arr.append(trimmed);
                    }
                }
                properties[propName.value] = arr;
            }
            else if (value == "true" || value == "false") {
                properties[propName.value] = (value == "true");
            } 
            else {
                // Check if value is a reference (starts with @)
                properties[propName.value] = value;
            }
        }
        else {
            setError("Expected property value", peek(state).line, peek(state).column);
            return properties;
        }
    }
    
    return properties;
}

QJsonArray DSLParser::parseChildren(ParseState& state, int parentIndent) {
    QJsonArray children;
    
    // Skip current line's remaining tokens and newline
    while (!isAtEnd(state) && !check(state, Token::NEWLINE)) {
        advance(state);
    }
    if (check(state, Token::NEWLINE)) {
        advance(state);
    }
    
    // Parse children with greater indentation
    while (!isAtEnd(state)) {
        size_t savedPos = state.currentToken;
        
        // Skip newlines
        while (check(state, Token::NEWLINE)) {
            advance(state);
        }
        
        if (isAtEnd(state)) {
            break;
        }
        
        // Check indentation
        if (check(state, Token::INDENT)) {
            int childIndent = peek(state).value.toInt();
            
            if (childIndent <= parentIndent) {
                // End of children, restore position
                state.currentToken = savedPos;
                break;
            }
            
            // Check if this is a direct child (not grandchild)
            if (childIndent > parentIndent) {
                // Parse only direct children at the first child indent level
                if (children.isEmpty() || childIndent == children[0].toObject()["_indent"].toInt()) {
                    QJsonObject child = parseElement(state);
                    if (!success()) {
                        return children;
                    }
                    child["_indent"] = childIndent;
                    children.append(child);
                } else if (childIndent < children[0].toObject()["_indent"].toInt()) {
                    // Lower indent than first child, stop
                    state.currentToken = savedPos;
                    break;
                }
                // Skip grandchildren (they'll be parsed by their parent)
            }
        } else {
            // No indent token, stop
            state.currentToken = savedPos;
            break;
        }
    }
    
    // Clean up indent markers
    for (int i = 0; i < children.size(); ++i) {
        QJsonObject child = children[i].toObject();
        child.remove("_indent");
        children[i] = child;
    }
    
    return children;
}

bool DSLParser::isAtEnd(const ParseState& state) const {
    return state.currentToken >= state.tokens.size() || 
           state.tokens[state.currentToken].type == Token::END_OF_FILE;
}

DSLParser::Token DSLParser::peek(const ParseState& state) const {
    if (isAtEnd(state)) {
        return {Token::END_OF_FILE, "", 0, 0};
    }
    return state.tokens[state.currentToken];
}

DSLParser::Token DSLParser::advance(ParseState& state) {
    if (!isAtEnd(state)) {
        return state.tokens[state.currentToken++];
    }
    return {Token::END_OF_FILE, "", 0, 0};
}

bool DSLParser::match(ParseState& state, Token::Type type) {
    if (check(state, type)) {
        advance(state);
        return true;
    }
    return false;
}

bool DSLParser::check(const ParseState& state, Token::Type type) const {
    if (isAtEnd(state)) {
        return false;
    }
    return state.tokens[state.currentToken].type == type;
}

void DSLParser::setError(const QString& message, int line, int column) {
    m_lastError = ParseError{message, line, column};
}

void DSLParser::clearError() {
    m_lastError.reset();
}
