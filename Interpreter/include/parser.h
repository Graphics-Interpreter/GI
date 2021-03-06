
#ifndef GI_PARSER_H
#define GI_PARSER_H

#include <lexers.h>
#include <AST.h>

namespace parser {
    using namespace ast;

    std::shared_ptr<ExprAST> parseExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseAllExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseRawExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseBracketExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseLoadingFileExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseTrueExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseFalseExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseNilExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseNumberExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseIdentifierExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseIfStatementExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseCondStatementExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseLetStatementExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseLambdaApplicationExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseFunctionApplicationExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseDefinitionExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseIdDefinitionExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseLambdaDefinitionExpr(lexers::Lexer &lex);

    std::shared_ptr<ExprAST> parseFunctionDefinitionExpr(lexers::Lexer &lex);
}
#endif //GI_PARSER_H
