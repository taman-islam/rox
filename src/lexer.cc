#include "lexer.h"
#include <unordered_map>
#include <iostream>

namespace rox {

const std::unordered_map<std::string, TokenType>& Lexer::getKeywords() {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"and", TokenType::AND},
        {"else", TokenType::ELSE},
        {"false", TokenType::FALSE},
        {"function", TokenType::FUNCTION},
        {"if", TokenType::IF},
        {"const", TokenType::CONST},
        {"none", TokenType::NONE},
        {"or", TokenType::OR},
        {"print", TokenType::PRINT},
        {"read_line", TokenType::READ_LINE},
        {"return", TokenType::RETURN},
        {"true", TokenType::TRUE},
        {"repeat", TokenType::REPEAT},
        {"range", TokenType::RANGE},
        {"not", TokenType::NOT},
        {"break", TokenType::BREAK},
        {"continue", TokenType::CONTINUE},
        {"num32", TokenType::TYPE_NUM32},
        {"num", TokenType::TYPE_NUM},
        {"float", TokenType::TYPE_FLOAT},
        {"bool", TokenType::TYPE_BOOL},
        {"char", TokenType::TYPE_CHAR},
        {"list", TokenType::TYPE_LIST},
        {"dictionary", TokenType::TYPE_DICT},
        {"string", TokenType::TYPE_STRING},
        {"rox_result", TokenType::TYPE_ROX_RESULT},
    };
    return keywords;
}

const std::unordered_set<std::string>& Lexer::getBuiltins() {
    static const std::unordered_set<std::string> builtins = {
        // Core Functions
        "isOk", "getValue", "getError", "ok", "error",
        // Constants not in keywords
        "pi", "e",
        // Math Functions (num32)
        "num32_abs", "num32_min", "num32_max", "num32_pow",
        // Math Functions (num)
        "num_abs", "num_min", "num_max", "num_pow",
        // Math Functions (float)
        "float_abs", "float_min", "float_max", "float_pow", "float_sqrt",
        "float_sin", "float_cos", "float_tan", "float_log", "float_exp", "float_floor", "float_ceil",
        // Collection Helpers
        "rox_at", "rox_set", "rox_remove", "rox_has", "rox_keys", "rox_div", "rox_mod", "rox_get",
        // Special
        "main"
    };
    return builtins;
}

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-': addToken(TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(TokenType::STAR); break;
        case '%': addToken(TokenType::PERCENT); break;

        case '=':
            addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
            break;
        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
                addToken(TokenType::COMMENT);
            } else {
                addToken(TokenType::SLASH);
            }
            break;

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;

        case '\n':
            line++;
            break;

        case '"': string(); break;
        case '\'': character(); break;

        default:
            if (isdigit(c)) {
                number();
            } else if (isalpha(c)) {
                identifier();
            } else {
                std::cerr << "Unexpected character at line " << line << ": " << c << std::endl;
            }
            break;
    }
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);

    if (text.rfind("roxv26_", 0) == 0) { // Check if starts with "roxv26_"
        std::cerr << "Error: Identifier '" << text << "' cannot start with reserved prefix 'roxv26_'." << std::endl;
        exit(1);
    }

    TokenType type = TokenType::IDENTIFIER;
    const auto& keywords = getKeywords();
    auto search = keywords.find(text);
    if (search != keywords.end()) {
        type = search->second;
    }
    addToken(type);
}

void Lexer::number() {
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        // Consume the "."
        advance();

        while (isdigit(peek())) advance();

        addToken(TokenType::NUMBER_FLOAT); // Float literal
        return;
    }

    // Look for n32 suffix
    if (peek() == 'n') {
        if (peekNext() == '3' && source.size() > current + 2 && source[current+2] == '2') {
             advance(); // n
             advance(); // 3
             advance(); // 2
             addToken(TokenType::NUMBER_INT); // num32
             return;
        }
    }

    addToken(TokenType::NUMBER_INT); // num64 (default int)
    return;
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        std::cerr << "Unterminated string at line " << line << std::endl;
        return;
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING);
}

void Lexer::character() {
    if (peek() == '\\') {
        advance(); // escape
        advance(); // char
    } else {
        advance();
    }

    if (peek() == '\'') {
        advance();
        addToken(TokenType::CHAR_LITERAL);
    } else {
         std::cerr << "Unterminated char literal at line " << line << std::endl;
    }
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

char Lexer::advance() {
    return source[current++];
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;

    current++;
    return true;
}

void Lexer::addToken(TokenType type) {
    addToken(type, "");
}

void Lexer::addToken(TokenType type, std::string /*literal*/) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line});
}

} // namespace rox
