#ifndef GI_AST_H
#define GI_AST_H

#include <exception>
#include <fstream>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <easylogging++.h>
#include <context.h>

namespace ast {
    using context::Scope;

    using pScope = std::shared_ptr<Scope>;

    using pExpr = std::shared_ptr<ExprAST>;

    class ExprAST {
    public:
        virtual pExpr eval(pScope &, const pExpr &) const;

        virtual pExpr apply(const std::vector<pExpr> &, pScope &);

        virtual std::string display() const;

        virtual ~ExprAST() {}
    };

    class AllExprAST : public ExprAST {
    public:
        AllExprAST(const std::vector<std::shared_ptr<ExprAST>> &v) : exprVec{v} {}

        virtual std::shared_ptr<ExprAST> eval(std::shared_ptr<Scope> &, const pExpr &) const override;

    private:
        std::vector<std::shared_ptr<ExprAST>> exprVec;
    };

    class BooleansFalseAST : public ExprAST {
    public:
        BooleansFalseAST() {}

        virtual std::string display() const override;
    };

    class BooleansTrueAST : public ExprAST {
    public:
        BooleansTrueAST() {}

        virtual std::string display() const override;
    };

    class NumberAST : public ExprAST {
    public:
        NumberAST(double n) : value{n} {}

        double getValue() const { return value; }

        virtual std::string display() const override;

    private:
        double value;
    };

    class IdentifierAST : public ExprAST {
    public:
        IdentifierAST(const std::string &tid) : id{tid} {}

        std::string getId() const { return id; }

        std::shared_ptr<ExprAST> eval(std::shared_ptr<Scope> &ss, const pExpr &) const override;

    private:
        std::string id;
    };

    class IfStatementAST : public ExprAST {
    public:
        IfStatementAST(std::shared_ptr<ExprAST> c, std::shared_ptr<ExprAST> t, std::shared_ptr<ExprAST> f) :
                condition{c}, trueClause{t}, falseClause{f} {}

        std::shared_ptr<ExprAST> eval(std::shared_ptr<Scope> &, const pExpr &) const override;

    private:
        std::shared_ptr<ExprAST> condition;
        std::shared_ptr<ExprAST> trueClause, falseClause;
    };

    class CondStatementAST : public ExprAST {
    public:
        CondStatementAST(const std::vector<std::shared_ptr<ExprAST>> &, const std::vector<std::shared_ptr<ExprAST>> &);

        std::shared_ptr<ExprAST> eval(std::shared_ptr<Scope> &, const pExpr &) const override;

    private:
        std::shared_ptr<ExprAST> ifStatement;
    };

    class LetStatementAST : public ExprAST {
    public:
        LetStatementAST(const std::vector<std::shared_ptr<ExprAST>> &id, const std::vector<std::shared_ptr<ExprAST>> &v,
                        const std::shared_ptr<ExprAST> &e) : identifier{id}, value{v}, expr{e} {}

        std::shared_ptr<ExprAST> eval(std::shared_ptr<Scope> &, const pExpr &) const override;

    private:
        std::vector<std::shared_ptr<ExprAST>> identifier, value;
        std::shared_ptr<ExprAST> expr;
    };

    class LoadingFileAST : public ExprAST {
    public:
        LoadingFileAST(const std::string &f) : filename{f} {}

        std::shared_ptr<ExprAST> eval(std::shared_ptr<Scope> &, const pExpr &) const override;

    private:
        std::string filename;
    };


    class PairAST : public ExprAST {
    public:
        PairAST(std::shared_ptr<ExprAST> f, std::shared_ptr<ExprAST> s) : data{f, s} {}

        std::shared_ptr<ExprAST> eval(std::shared_ptr<Scope> &s, const pExpr &) const override;

        virtual std::string display() const override;

        mutable std::pair<std::shared_ptr<ExprAST>, std::shared_ptr<ExprAST>> data;

    };

    class NilAST : public ExprAST {
    public:
        NilAST() {}

        virtual std::string display() const override;
    };


    class BindingAST : public ExprAST {
    public:
        BindingAST(const std::string &id) : identifier{id} {}

        const std::string &getIdentifier() const {
            return identifier;
        }

    private:
        std::string identifier;
    };

    class ValueBindingAST : public BindingAST {
    public:
        ValueBindingAST(const std::string &id, std::shared_ptr<ExprAST> v)
                : BindingAST(id), value{v} {}

        std::shared_ptr<ExprAST> eval(std::shared_ptr<Scope> &ss, const pExpr &) const override;

    private:
        std::shared_ptr<ExprAST> value;
    };

    class LambdaAST : public ExprAST {
        friend class LambdaBindingAST;

    public:
        LambdaAST(const std::vector<std::string> &v,
                  std::vector<std::shared_ptr<ExprAST>> expr)
                : formalArgs{v}, expression{expr}, context{new Scope} {}

        LambdaAST(const LambdaAST &other) : formalArgs(other.formalArgs), expression(other.expression),
                                            context(new Scope(*other.context)) {}

        pExpr apply(const std::vector<pExpr> &actualArgs, pScope &) override;

        std::shared_ptr<ExprAST> eval(std::shared_ptr<Scope> &ss, const pExpr &) const override;

        virtual std::string display() const override;

    private:
        std::vector<std::string> formalArgs;
        std::vector<std::shared_ptr<ExprAST>> expression;
        mutable pScope context;
        mutable bool isSubRoutineEvaluated = false;
    };

    class BuiltinConsAST : public ExprAST {
    public:
        pExpr apply(const std::vector<pExpr> &actualArgs, pScope &) override;
    };

    class BuiltinCarAST : public ExprAST {
    public:
        pExpr apply(const std::vector<pExpr> &actualArgs, pScope &) override;
    };

    class BuiltinCdrAST : public ExprAST {
    public:
        pExpr apply(const std::vector<pExpr> &actualArgs, pScope &) override;
    };

    class BuiltinAddAST : public ExprAST {
    public:
        pExpr apply(const std::vector<pExpr> &actualArgs, pScope &) override;
    };

    class BuiltinMultiplyAST : public ExprAST {
    public:
        pExpr apply(const std::vector<pExpr> &actualArgs, pScope &) override;
    };

    class BuiltinListAST : public ExprAST {
    public:
        pExpr apply(const std::vector<pExpr> &actualArgs, pScope &) override;
    };

    class BuiltinNullAST : public ExprAST {
    public:
        pExpr apply(const std::vector<pExpr> &actualArgs, pScope &) override;
    };

    class BuiltinLessThanAST : public ExprAST {
    public:
        pExpr apply(const std::vector<pExpr> &actualArgs, pScope &) override;
    };

    class BuiltinOppositeAST : public ExprAST {
    public:
        pExpr apply(const std::vector<pExpr> &actualArgs, pScope &) override;
    };

    class BuiltinReciprocalAST : public ExprAST {
    public:
        pExpr apply(const std::vector<pExpr> &actualArgs, pScope &) override;
    };

    class LambdaBindingAST : public BindingAST {
    public:
        LambdaBindingAST(const std::string &id,
                         const std::vector<std::string> &v,
                         const std::vector<std::shared_ptr<ExprAST>> expr) :
                BindingAST(id), lambda{std::make_shared<LambdaAST>(v, expr)} {}

        std::shared_ptr<ExprAST> eval(std::shared_ptr<Scope> &ss, const pExpr &) const override;

    private:
        std::shared_ptr<LambdaAST> lambda;
    };

    class LambdaApplicationAST : public ExprAST {
    public:
        LambdaApplicationAST(const std::shared_ptr<ExprAST> &lam, const std::vector<std::shared_ptr<ExprAST>> &args)
                : lambdaOrIdentifier{lam}, actualArgs{args} {
        }

        std::shared_ptr<ExprAST> eval(std::shared_ptr<Scope> &ss, const pExpr &) const override;

    private:
        std::shared_ptr<ExprAST> lambdaOrIdentifier;
        std::vector<std::shared_ptr<ExprAST>> actualArgs;
    };

    class BuiltinDrawAST : public ExprAST {
    public:
        BuiltinDrawAST() {}
    };

}

#endif //GI_AST_H
