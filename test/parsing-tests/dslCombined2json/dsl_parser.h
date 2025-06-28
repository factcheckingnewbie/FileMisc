#ifndef DSL_PARSER_H
#define DSL_PARSER_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <vector>
#include <optional>

class DSLParser {
public:
    struct ParseError {
        QString message;
        int line;
        int column;
    };

    DSLParser() = default;
    ~DSLParser() = default;
    
    // Parse DSL content to JSON document
    QJsonDocument parse(const QString& content);
    
    // Get last parse error (if any)
    std::optional<ParseError> lastError() const { return m_lastError; }
    
    // Check if last parse succeeded
    bool success() const { return !m_lastError.has_value(); }

private:
    struct Token {
        enum Type {
            IDENTIFIER,
            STRING,
            NUMBER,
            COLON,
            NEWLINE,
            INDENT,
            DEDENT,
            END_OF_FILE
        };
        
        Type type;
        QString value;
        int line;
        int column;
    };
    
    struct ParseState {
        std::vector<Token> tokens;
        size_t currentToken = 0;
        std::vector<int> indentStack = {0};
    };
    
    std::optional<ParseError> m_lastError;
    
    // Lexical analysis
    std::vector<Token> tokenize(const QString& content);
    void addIndentTokens(std::vector<Token>& tokens, int currentIndent, int newIndent, int line);
    
    // Parsing
    QJsonObject parseElement(ParseState& state);
    QJsonObject parseProperties(ParseState& state, const QString& elementType, const QString& elementName);
    QJsonArray parseChildren(ParseState& state, int parentIndent);
    
    // Token utilities
    bool isAtEnd(const ParseState& state) const;
    Token peek(const ParseState& state) const;
    Token advance(ParseState& state);
    bool match(ParseState& state, Token::Type type);
    bool check(const ParseState& state, Token::Type type) const;
    
    // Error handling
    void setError(const QString& message, int line, int column = 0);
    void clearError();
};

#endif // DSL_PARSER_H
