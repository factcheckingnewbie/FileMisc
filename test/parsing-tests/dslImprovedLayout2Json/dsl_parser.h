#ifndef DSL_PARSER_H
#define DSL_PARSER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <json/json.h>

namespace DSLParser {

// Token types for lexical analysis
enum class TokenType {
    STRING,
    NUMBER,
    IDENTIFIER,
    COLON,
    NEWLINE,
    INDENT,
    DEDENT,
    EOF_TOKEN,
    COMMENT
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

// Lexer for tokenizing the DSL
class Lexer {
public:
    explicit Lexer(const std::string& input);
    std::vector<Token> tokenize();

private:
    std::string input;
    size_t position;
    int line;
    int column;
    std::vector<int> indentStack;

    Token readString();
    Token readNumber();
    Token readIdentifier();
    void skipWhitespace();
    void skipComment();
    char peek();
    char advance();
    bool isAtEnd();
};

// AST Node types
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual Json::Value toJson() const = 0;
};

class ElementNode : public ASTNode {
public:
    std::string type;
    std::string name;
    std::unordered_map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<ASTNode>> children;

    Json::Value toJson() const override;
};

// Parser for building the AST
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ASTNode> parse();

private:
    std::vector<Token> tokens;
    size_t current;
    int currentIndentLevel;

    std::unique_ptr<ElementNode> parseElement(int indentLevel);
    void parseAttributes(ElementNode* element);
    bool match(TokenType type);
    bool check(TokenType type);
    Token advance();
    Token peek();
    Token previous();
    bool isAtEnd();
    void synchronize();
};

// Main parser class
class DSLToJsonParser {
public:
    Json::Value parse(const std::string& dslContent);
    std::string parseToString(const std::string& dslContent);

private:
    std::string formatJson(const Json::Value& json);
};

} // namespace DSLParser

#endif // DSL_PARSER_H
