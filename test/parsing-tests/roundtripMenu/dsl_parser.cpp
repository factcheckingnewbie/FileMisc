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
            if (ch == '"') {
                int start = i;
                ++i;
                while (i < trimmed.length() && trimmed[i] != '"') {
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
            // Identifier or keyword
            else if (ch.isLetter() || ch == '_') {
                int start = i;
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
            
            // Handle comma-separated values (e.g., sizes:300,500)
            if (propName.value == "sizes") {
                QString fullValue = value;
                
                // Look for comma-separated numbers in the remaining tokens
                size_t peekPos = state.currentToken;
                while (peekPos < state.tokens.size()) {
                    if (state.tokens[peekPos].type == Token::IDENTIFIER || 
                        state.tokens[peekPos].type == Token::NUMBER) {
                        // Check if this token contains comma-separated values
                        QString tokenValue = state.tokens[peekPos].value;
                        if (tokenValue.contains(',') || fullValue.contains(',')) {
                            // Parse the full sizes value
                            QStringList parts = fullValue.split(',');
                            QJsonArray sizesArray;
                            for (const QString& part : parts) {
                                sizesArray.append(part.trimmed().toInt());
                            }
                            properties[propName.value] = sizesArray;
                            break;
                        }
                    }
                    
                    if (state.tokens[peekPos].type == Token::NEWLINE || 
                        state.tokens[peekPos].type == Token::COLON) {
                        break;
                    }
                    peekPos++;
                }
                
                // If no comma found, just use the single value
                if (!properties.contains(propName.value)) {
                    properties[propName.value] = value.toInt();
                }
            }
            else if (value == "true" || value == "false") {
                properties[propName.value] = (value == "true");
            } else {
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
