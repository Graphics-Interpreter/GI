#ifndef GI_LEXERS_H
#define GI_LEXERS_H

#include <string>
#include <stdexcept>
#include <sstream>

namespace lexers {
    class Lexer {
    public:
        enum TokenType {
            TokEOF = -1,
            TokNumber = -2,
            TokIdentifier = -3
        };

        Lexer(const std::string &exp) : expressionBuf{exp} { getNextTok(); }

        TokenType getTokType() const { return currentType; }

        TokenType getNextTok() {
            int type{expressionBuf.get()};
            expressionBuf.unget();

            if (type == EOF) {
                currentType = TokEOF;
            } else if (isdigit(type)) {
                expressionBuf >> token.number;
                currentType = TokNumber;
            } else if (isalpha(type)) {
                expressionBuf >> token.identifier;
                currentType = TokIdentifier;
            }
            return currentType;
        }

        double getNum() const {
            if (getTokType() == TokNumber)
                return token.number;
            else
                throw std::logic_error("Current Token type error.");
        }

        std::string getIdentifier() const {
            if (getTokType() == TokIdentifier)
                return token.identifier;
            else
                throw std::logic_error("Current Token type error.");
        }

    private:
        union Token {
            std::string identifier;
            double number;

            Token() {}

            ~Token() {}
        } token;

        std::stringstream expressionBuf;
        TokenType currentType = TokEOF;
    };
}
#endif //GI_LEXERS_H
