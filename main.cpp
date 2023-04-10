#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Frontend/FrontendActions.h"
#include "llvm/Support/CommandLine.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <iostream>

static llvm::cl::OptionCategory myOpts("myToolOptions");

class VarDeclPrinter : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) {
        if (const clang::VarDecl *pVarDecl = Result.Nodes.getNodeAs<clang::VarDecl>("varDecl")) {
            std::cout << "Found variable declaration:\n\t" << pVarDecl->getNameAsString() << "\n";
            std::cout << "\tin: " << pVarDecl->getLocation().printToString(pVarDecl->getASTContext().getSourceManager()) << "\n";
        }
    }
};

class FuncDeclPrinter : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) {
        if (const clang::FunctionDecl *pFuncDecl = Result.Nodes.getNodeAs<clang::FunctionDecl>("functionDecl")) {
            std::cout << "Found function declaration:\n\t" << pFuncDecl->getNameAsString() << "\n";
            std::cout << "\tin: " << pFuncDecl->getLocation().printToString(pFuncDecl->getASTContext().getSourceManager()) << "\n";
        }
    }
};


int main(int argc, const char **argv) { 
	auto expectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, myOpts);
	if (!expectedParser) {
          llvm::errs() << expectedParser.takeError();
          std::cout << "Something went wrong\n";
          return 1;
	}
    clang::tooling::CommonOptionsParser &optionsParser = expectedParser.get();
	clang::tooling::ClangTool tool(optionsParser.getCompilations(), optionsParser.getSourcePathList());

    std::cout << "---------------------------------------------------------------------------------\n";

    auto varMatcher = clang::ast_matchers::varDecl().bind("varDecl");
    VarDeclPrinter varCallbackPrinter;
    auto funcMatcher = clang::ast_matchers::functionDecl().bind("functionDecl");
    FuncDeclPrinter funcCallbackPrinter;
    clang::ast_matchers::MatchFinder finder;
    finder.addMatcher(varMatcher, &varCallbackPrinter);
    finder.addMatcher(funcMatcher, &funcCallbackPrinter);
    return tool.run(clang::tooling::newFrontendActionFactory(&finder).get());
}