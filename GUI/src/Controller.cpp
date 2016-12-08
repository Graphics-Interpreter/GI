#include <stack>
#include <string>
#include <Controller.h>
#include <GUIbuiltinFunc.h>

using namespace std;
using namespace sf;
using namespace con;
using namespace context;
using namespace parser;

void Controller::appendChar(char c) {
    if (!toType.count(c))
        return normalCharProcess(c);
    switch (toType[c]) {
        case BackSpace:
            return backSpaceProcess();
        case LineFeed:
            return lineFeedProcess();
        default:;
    }
}

void Controller::lineFeedProcess() {
    currentText.formatString.lineFeedProcess();
    adjustText();
}

void Controller::backSpaceProcess() {
    currentText.formatString.backSpaceProcess();
}

void Controller::normalCharProcess(char c) {
    currentText.formatString.normalCharProcess(c);
}

void Controller::moveScreen(float delta) {
    currentText.offsetY += delta;
    for_each(begin(history), end(history), [delta](Text &text) { text.offsetY += delta; });
}

void Controller::clearScreen() {
    history.clear();
    currentText.clearStr();
    scope.clear();
}

void Controller::execute() {
    history.push_back(currentText);
    history.push_back(evaluation());

    currentText.offsetY = history.back().offsetY + history.back().getHeight();
    currentText.clearStr();
    pushString(currentText.formatString, "]=> ");

    adjustText();
}

con::Text Controller::evaluation() {
    Text resultText;
    resultText.color = sf::Color::Blue;
    resultText.offsetY = currentText.offsetY + currentText.getHeight();
    pushString(resultText.formatString, ";Value: ");

    try {
        lexers::Lexer lex(currentText.formatString.toString().substr(4));
        if (auto ptr = parseAllExpr(lex)->eval(scope)) {
            pushString(resultText.formatString, ptr->display());
        } else {
            pushString(resultText.formatString, "\'()");
        }
    } catch (std::logic_error &e) {
        pushString(resultText.formatString, e.what());
    }

    return resultText;
}

void Controller::drawToWindows() {
    drawingBoard.clear(sf::Color::White);
    for (auto va: shapes) drawingBoard.draw(va);

    textWindow.clear(sf::Color::White);
    textWindow.draw(currentText);
    for (auto text: history) textWindow.draw(text);
}

void Controller::appendShape(const sf::VertexArray &va) {
    shapes.push_back(va);
}

void con::Text::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    sf::Text text{formatString.toString(), font, fontSize};
    text.setFillColor(color);
    text.move(0, offsetY);
    target.draw(text);
}

float con::Text::getHeight() const {
    return sf::Text{formatString.toString(), font, fontSize}.getLocalBounds().height;
}

void con::Text::clearStr() {
    formatString.clearStr();
}

con::Controller::Controller(sf::RenderTarget &text, sf::RenderTarget &board)
        : textWindow{text}, drawingBoard{board} {
    lexers::Lexer lex("(load \"Base.scm\")");
    parser::parseAllExpr(lex)->eval(scope);
    scope.addBuiltinFunc("draw", std::make_shared<ast::BuiltinDrawAST>(*this));
    scope.addBuiltinFunc("line", std::make_shared<ast::BuiltinLineAST>());
    pushString(currentText.formatString, "]=> ");
}

void Controller::adjustText() {
    if (history.empty()) return;
    auto newline = currentText.fontSize;
    auto lastLine = std::max(currentText.offsetY + currentText.getHeight() + newline,
                             history.back().offsetY + history.back().getHeight() + newline);

    if (textWindow.getSize().y < lastLine) {
        auto delta = lastLine - textWindow.getSize().y;
        for_each(begin(history), end(history), [delta](Text &text) { text.offsetY -= delta; });
        currentText.offsetY -= delta;
    }
}
