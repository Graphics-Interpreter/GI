#ifndef GI_LEXERS_H
#define GI_LEXERS_H

#include <string>
#include <stdexcept>
#include <algorithm>
#include <sstream>

namespace lexers {
    class Lexer {
    public:
        enum TokenType {
            TokEOF = -1,
            TokNumber = -2,
            TokIdentifier = -3,
            TokOpenBrace = -4,
            TokCloseBrace = -5,
            TokDefine = -6,
            TokLet = -7,
        };

        Lexer(const std::string &exp) {
            std::string tmp;
            std::for_each(std::begin(exp), std::end(exp), [&tmp](const char c) {
                if (c == ')' || c == '(') {
                    tmp.push_back(' ');
                    tmp.push_back(c);
                    tmp.push_back(' ');
                } else {
                    tmp.push_back(c);
                }
            });
            expressionBuf << tmp;
            getNextTok();
        }

        TokenType getTokType() const { return currentType; }

        TokenType getNextTok() {
            int type{expressionBuf.get()};
            expressionBuf.unget();

            if (type == EOF) {
                currentType = TokEOF;
            } else if (isspace(type)) {
                expressionBuf.get();
                return getNextTok();
            } else if (isdigit(type)) {
                expressionBuf >> number;
                currentType = TokNumber;
            } else if (isalpha(type)) {
                expressionBuf >> identifier;
                if (identifier == "define")
                    currentType = TokDefine;
                else if (identifier == "let")
                    currentType = TokLet;
                else
                    currentType = TokIdentifier;
            } else if (type == '(') {
                expressionBuf.get();
                currentType = TokOpenBrace;
            } else if (type == ')') {
                expressionBuf.get();
                currentType = TokCloseBrace;
            }
            return currentType;
        }

        double getNum() const {
            if (getTokType() == TokNumber)
                return number;
            else
                throw std::logic_error("Current Token type error.");
        }

        std::string getIdentifier() const {
            if (getTokType() == TokIdentifier)
                return identifier;
            else
                throw std::logic_error("Current Token type error.");
        }

    private:
        std::string identifier;
        double number;

        std::stringstream expressionBuf;
        TokenType currentType = TokEOF;
    };
}
#endif //GI_LEXERS_H
