#include "dsl_parser.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <iomanip>

namespace DSLParser {

// Lexer implementation
Lexer::Lexer(const std::string& input) 
    : input(input), position(0), line(1), column(1) {
    indentStack.push_back(0);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    bool newLine = true;

    while (!isAtEnd()) {
        skipWhitespace();
        
        if (isAtEnd()) break;

        // Handle indentation at the beginning of a line
        if (newLine) {
            newLine = false;
            int spaces = 0;
            while (peek() == ' ' || peek() == '\t') {
                if (peek() == '\t') {
                    spaces += 4; // Treat tab as 4 spaces
                } else {
                    spaces++;
                }
                advance();
            }

            // Skip empty lines
            if (peek() == '\n' || peek() == '#') {
                if (peek() == '#') skipComment();
                continue;
            }

            int currentIndent = indentStack.back();
            if (spaces > currentIndent) {
                indentStack.push_back(spaces);
                tokens.push_back({TokenType::INDENT, "", line, column});
            } else if (spaces < currentIndent) {
                while (indentStack.size() > 1 && indentStack.back() > spaces) {
                    indentStack.pop_back();
                    tokens.push_back({TokenType::DEDENT, "", line, column});
                }
            }
        }

        char c = peek();

        // Skip comments
        if (c == '#') {
            skipComment();
            continue;
        }

        // Handle newlines
        if (c == '\n') {
            advance();
            tokens.push_back({TokenType::NEWLINE, "", line - 1, column});
            newLine = true;
            continue;
        }

        // Handle strings
        if (c == '"') {
            tokens.push_back(readString());
        }
        // Handle numbers
        else if (std::isdigit(c) || (c == '-' && std::isdigit(peek()))) {
            tokens.push_back(readNumber());
        }
        // Handle colons
        else if (c == ':') {
            advance();
            tokens.push_back({TokenType::COLON, ":", line, column - 1});
        }
        // Handle identifiers and keywords
        else if (std::isalpha(c) || c == '_' || c == '$' || c == '.') {
            tokens.push_back(readIdentifier());
        }
        else {
            // Skip any other characters
            advance();
        }
    }

    // Add remaining dedents
    while (indentStack.size() > 1) {
        indentStack.pop_back();
        tokens.push_back({TokenType::DEDENT, "", line, column});
    }

    tokens.push_back({TokenType::EOF_TOKEN, "", line, column});
    return tokens;
}

Token Lexer::readString() {
    int startLine = line;
    int startColumn = column;
    
    advance(); // Skip opening quote
    std::string value;
    
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\\') {
            advance();
            if (!isAtEnd()) {
                char escaped = advance();
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    default: value += escaped; break;
                }
            }
        } else {
            value += advance();
        }
    }
    
    if (peek() == '"') advance(); // Skip closing quote
    
    return {TokenType::STRING, value, startLine, startColumn};
}

Token Lexer::readNumber() {
    int startLine = line;
    int startColumn = column;
    std::string value;
    
    if (peek() == '-') {
        value += advance();
    }
    
    while (std::isdigit(peek())) {
        value += advance();
    }
    
    if (peek() == '.' && std::isdigit(input[position + 1])) {
        value += advance(); // Add decimal point
        while (std::isdigit(peek())) {
            value += advance();
        }
    }
    
    return {TokenType::NUMBER, value, startLine, startColumn};
}

Token Lexer::readIdentifier() {
    int startLine = line;
    int startColumn = column;
    std::string value;
    
    while (std::isalnum(peek()) || peek() == '_' || peek() == '-' || 
           peek() == '.' || peek() == '/' || peek() == '~' || 
           peek() == '*' || peek() == '$' || peek() == '+' ||
           peek() == ',' || peek() == '(' || peek() == ')') {
        value += advance();
    }
    
    return {TokenType::IDENTIFIER, value, startLine, startColumn};
}

void Lexer::skipWhitespace() {
    while (peek() == ' ' || peek() == '\t' || peek() == '\r') {
        advance();
    }
}

void Lexer::skipComment() {
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return input[position];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char c = input[position++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

bool Lexer::isAtEnd() {
    return position >= input.length();
}

// Parser implementation
Parser::Parser(const std::vector<Token>& tokens) 
    : tokens(tokens), current(0), currentIndentLevel(0) {}

std::unique_ptr<ASTNode> Parser::parse() {
    auto root = std::make_unique<ElementNode>();
    root->type = "root";
    
    while (!isAtEnd()) {
        if (match(TokenType::NEWLINE) || match(TokenType::DEDENT)) {
            continue;
        }
        
        if (check(TokenType::IDENTIFIER) || check(TokenType::STRING)) {
            auto element = parseElement(0);
            if (element) {
                root->children.push_back(std::move(element));
            }
        } else {
            advance();
        }
    }
    
    return root;
}

std::unique_ptr<ElementNode> Parser::parseElement(int indentLevel) {
    auto element = std::make_unique<ElementNode>();
    
    // Get element type
    if (check(TokenType::IDENTIFIER)) {
        element->type = advance().value;
    } else {
        return nullptr;
    }
    
    // Get element name if it's a string
    if (check(TokenType::STRING)) {
        element->name = advance().value;
    }
    
    // Parse attributes
    parseAttributes(element.get());
    
    // Skip to next line
    while (match(TokenType::NEWLINE)) {}
    
    // Parse children if there's an indent
    if (match(TokenType::INDENT)) {
        while (!check(TokenType::DEDENT) && !isAtEnd()) {
            if (match(TokenType::NEWLINE)) {
                continue;
            }
            
            auto child = parseElement(indentLevel + 1);
            if (child) {
                element->children.push_back(std::move(child));
            }
        }
        match(TokenType::DEDENT);
    }
    
    return element;
}

void Parser::parseAttributes(ElementNode* element) {
    while (!check(TokenType::NEWLINE) && !check(TokenType::INDENT) && !isAtEnd()) {
        if (check(TokenType::IDENTIFIER)) {
            std::string key = advance().value;
            
            if (match(TokenType::COLON)) {
                std::string value;
                if (check(TokenType::STRING)) {
                    value = advance().value;
                } else if (check(TokenType::NUMBER)) {
                    value = advance().value;
                } else if (check(TokenType::IDENTIFIER)) {
                    value = advance().value;
                }
                element->attributes[key] = value;
            } else {
                // Standalone attribute (like "checked" or "readonly")
                element->attributes[key] = "true";
            }
        } else {
            advance();
        }
    }
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EOF_TOKEN;
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::NEWLINE) return;
        
        switch (peek().type) {
            case TokenType::IDENTIFIER:
                return;
            default:
                advance();
        }
    }
}

// ElementNode implementation
Json::Value ElementNode::toJson() const {
    Json::Value node;
    node["type"] = type;
    
    if (!name.empty()) {
        node["name"] = name;
    }
    
    if (!attributes.empty()) {
        Json::Value attrs;
        for (const auto& [key, value] : attributes) {
            // Try to parse as number
            try {
                if (value.find('.') != std::string::npos) {
                    attrs[key] = std::stod(value);
                } else {
                    attrs[key] = std::stoi(value);
                }
            } catch (...) {
                // Parse as boolean if it's "true" or "false"
                if (value == "true") {
                    attrs[key] = true;
                } else if (value == "false") {
                    attrs[key] = false;
                } else {
                    attrs[key] = value;
                }
            }
        }
        node["attributes"] = attrs;
    }
    
    if (!children.empty()) {
        Json::Value childArray(Json::arrayValue);
        for (const auto& child : children) {
            childArray.append(child->toJson());
        }
        node["children"] = childArray;
    }
    
    return node;
}

// DSLToJsonParser implementation
Json::Value DSLToJsonParser::parse(const std::string& dslContent) {
    Lexer lexer(dslContent);
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    return ast->toJson();
}

std::string DSLToJsonParser::parseToString(const std::string& dslContent) {
    Json::Value json = parse(dslContent);
    return formatJson(json);
}

std::string DSLToJsonParser::formatJson(const Json::Value& json) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ostringstream os;
    writer->write(json, &os);
    return os.str();
}

} // namespace DSLParser
