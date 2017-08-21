#pragma once

#include <map>
#include <mbgl/style/expression/expression.hpp>

namespace mbgl {
namespace style {
namespace expression {

class Let : public Expression {
public:
    using Bindings = std::map<std::string, std::shared_ptr<Expression>>;
    
    Let(Bindings bindings_, std::unique_ptr<Expression> result_) :
        Expression(result_->getType()),
        bindings(std::move(bindings_)),
        result(std::move(result_))
    {}
    
    EvaluationResult evaluate(const EvaluationParameters& params) const override;
    void accept(std::function<void(const Expression*)>) const override;

private:
    Bindings bindings;
    std::unique_ptr<Expression> result;
};

class Var : public Expression {
public:
    Var(std::string name_, std::shared_ptr<Expression> value_) :
        Expression(value_->getType()),
        name(std::move(name_)),
        value(value_)
    {}
    
    EvaluationResult evaluate(const EvaluationParameters& params) const override;
    void accept(std::function<void(const Expression*)>) const override;

private:
    std::string name;
    std::shared_ptr<Expression> value;
};

} // namespace expression
} // namespace style
} // namespace mbgl