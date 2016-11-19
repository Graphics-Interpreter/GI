#include <fstream>
#include <basicParser.h>
#include <lexers.h>
#include <AST.h>

using namespace parser;
using namespace ast;

std::shared_ptr<ExprAST> ExprAST::eval(Scope &) const {
    CLOG(DEBUG, "exception");
    throw std::logic_error("Expression cannot be evaluated.");
};

std::shared_ptr<ExprAST> ExprAST::apply(const std::vector<std::shared_ptr<ExprAST>> &, Scope &) {
    CLOG(DEBUG, "exception");
    throw std::logic_error("Expression cannot be applied.");
}

std::shared_ptr<ExprAST> ExprAST::toBool(Scope &s) const {
    return std::make_shared<BooleansAST>(true)->eval(s);
};

std::shared_ptr<ExprAST> LoadingFileAST::eval(Scope &s) const {
    std::ifstream fin{filename};
    std::string str{std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>()};
    lexers::Lexer lex{str};
    parseAllExpr(lex)->eval(s);
    return nullptr;
}

std::shared_ptr<ExprAST> IfStatementAST::eval(Scope &ss) const {
    if (condition->eval(ss)->toBool(ss)) {
        CLOG(DEBUG, "AST") << "Evaluate true clause.";
        return trueClause->eval(ss);
    } else {
        CLOG(DEBUG, "AST") << "Evaluate false clause.";
        return falseClause->eval(ss);
    }
}

std::shared_ptr<ExprAST> LessThanOperatorAST::eval(Scope &s) const {
    bool res = std::is_sorted(
            std::begin(actualArgs), std::end(actualArgs),
            [&](std::shared_ptr<ExprAST> p1, std::shared_ptr<ExprAST> p2) {
                //[&](decltype(actualArgs)::value_type p1, decltype(actualArgs)::value_type p2) {
                auto np1 = std::dynamic_pointer_cast<NumberAST>(p1->eval(s));
                auto np2 = std::dynamic_pointer_cast<NumberAST>(p2->eval(s));
                if (np1 && np2) {
                    return np1->getValue() < np2->getValue();
                } else {
                    CLOG(DEBUG, "exception");
                    throw std::logic_error(
                            "The operands in less than operator cannot be converted to number");
                }
            });
    if (res) return std::make_shared<NumberAST>(1);
    else return std::make_shared<NumberAST>(0);
}

std::shared_ptr<ExprAST> NumberAST::eval(Scope &) const {
    return std::make_shared<NumberAST>(getValue());
}

std::shared_ptr<ExprAST> NumberAST::toBool(Scope &s) const {
    return std::make_shared<BooleansAST>(getValue() != 0)->eval(s);
}

std::shared_ptr<ExprAST> IdentifierAST::eval(Scope &ss) const {
    CLOG(DEBUG, "AST") << "Evaluate identifier: " << getId();
    if (ss.count(getId())) {
        return ss[getId()];
    } else {
        CLOG(DEBUG, "exception");
        throw std::logic_error("Unbound identifier.");
    }
}

std::shared_ptr<ExprAST> LambdaAST::apply(const std::vector<std::shared_ptr<ExprAST>> &actualArgs,
                                          Scope &ss) {
    CLOG(DEBUG, "AST") << "Number of formal arguments is " << formalArgs.size();
    // Backup scope of lambda. If not, recursive calls will destroy scope by binding arguments.
    Scope tmp = context;
    for (size_t i = 0; i < actualArgs.size(); i++) {
        // Evaluate value from current scope and set them into scope of lambda
        tmp[formalArgs[i]] = actualArgs[i]->eval(ss);
    }
    return expression->eval(tmp);
}

std::shared_ptr<ExprAST> LambdaAST::eval(Scope &ss) const {
    context = ss;
    return std::make_shared<LambdaAST>(*this);
}


std::shared_ptr<ExprAST> AddOperatorAST::eval(Scope &s) const {
    double num = 0;
    CLOG(DEBUG, "parser") << "Number of add operands are: " << actualArgs.size();
    for (auto element: actualArgs) {
        std::shared_ptr<ExprAST> res = element->eval(s);
        if (auto p = std::dynamic_pointer_cast<NumberAST>(res)) {
            CLOG(DEBUG, "AST") << "Add number: " << p->getValue();
            num += p->getValue();
        } else {
            CLOG(DEBUG, "exception");
            throw std::logic_error("The operands cannot be converted to number");
        }
    }
    return std::make_shared<NumberAST>(num);
}

std::shared_ptr<ExprAST> MinusOperatorAST::eval(Scope &s) const {
    double front = 0;
    if (auto p = std::dynamic_pointer_cast<NumberAST>(actualArgs.front()->eval(s))) {
        front = p->getValue();
    } else {
        CLOG(DEBUG, "exception");
        throw std::logic_error("The operands cannot be converted to number");
    }

    if (actualArgs.size() == 1) return std::make_shared<NumberAST>(-front);

    for (int i = 1; i < actualArgs.size(); i++) {
        auto res = actualArgs[i]->eval(s);
        if (auto p = std::dynamic_pointer_cast<NumberAST>(res)) {
            CLOG(DEBUG, "AST") << "Add number: " << p->getValue();
            front -= p->getValue();
        } else {
            CLOG(DEBUG, "exception");
            throw std::logic_error("The operands cannot be converted to number");
        }
    }
    return std::make_shared<NumberAST>(front);
}


std::shared_ptr<ExprAST> ValueBindingAST::eval(Scope &ss) const {
    ss[getIdentifier()] = value->eval(ss);
    return nullptr;
}


std::shared_ptr<ExprAST> LambdaBindingAST::eval(Scope &ss) const {
    // Set its identifier into its context to support recursion.
    ss[getIdentifier()] = lambda->upgradeScope()[getIdentifier()] = lambda;
    return nullptr;
}

std::shared_ptr<ExprAST> LambdaApplicationAST::eval(Scope &ss) const {
    if (std::dynamic_pointer_cast<LambdaAST>(lambdaOrIdentifier)) {
        return lambdaOrIdentifier->apply(actualArgs, ss);
    } else {
        return lambdaOrIdentifier->eval(ss)->apply(actualArgs, ss);
    }
}


std::shared_ptr<ExprAST> BooleansAST::eval(Scope &) const {
    if (booleans)
        return std::make_shared<BooleansAST>(*this);
    else
        return nullptr;
}

std::shared_ptr<ExprAST> AllExprAST::eval(Scope &s) const {
    std::shared_ptr<ExprAST> res;
    for (auto ptr : exprVec)
        res = ptr->eval(s);
    return res;
}

std::shared_ptr<ExprAST> PairAST::eval(Scope &s) const {
    return std::make_shared<PairAST>(data.first->eval(s), data.second->eval(s));
}

std::shared_ptr<ExprAST> BuiltinCarAST::eval(Scope &s) const {
    if (auto p = std::dynamic_pointer_cast<PairAST>(pair->eval(s))) {
        return p->data.first;
    } else {
        CLOG(DEBUG, "exception");
        throw std::logic_error("Cannot convert to pair");
    }
}

std::shared_ptr<ExprAST> BuiltinCdrAST::eval(Scope &s) const {
    if (auto p = std::dynamic_pointer_cast<PairAST>(pair->eval(s))) {
        return p->data.second;
    } else {
        CLOG(DEBUG, "exception");
        throw std::logic_error("Cannot convert to pair");
    }
}

std::shared_ptr<ExprAST> BuiltinNullAST::eval(Scope &s) const {
    if (auto p = std::dynamic_pointer_cast<NilAST>(pair->eval(s)))
        return std::make_shared<BooleansAST>(true);
    else
        return std::make_shared<BooleansAST>(false);
}

std::shared_ptr<ExprAST> NilAST::eval(Scope &s) const {
    return std::make_shared<NilAST>(*this);
}
