#ifndef ROX_FORMATTER_H
#define ROX_FORMATTER_H

#include "token.h"
#include <vector>
#include <string>

namespace rox {

class Formatter {
public:
    Formatter(const std::vector<Token>& tokens);
    std::string format();

private:
    std::vector<Token> tokens;
    int indentLevel = 0;
};

} // namespace rox

#endif // ROX_FORMATTER_H
