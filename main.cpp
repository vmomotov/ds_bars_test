#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Frontend/FrontendActions.h"
#include "llvm/Support/CommandLine.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <iostream>
#include "includes/CodeRecordsSingleton.h"

// рекурсивный Visitor
class FindDeclVisitor : public clang::RecursiveASTVisitor<FindDeclVisitor> {
public:
    explicit FindDeclVisitor(clang::ASTContext *pContext)
        : m_pContext(pContext) {}

// вызовется, когда найдет VarDecl
bool VisitVarDecl(clang::VarDecl *pDeclaration) {
    clang::FullSourceLoc fullLocation = m_pContext->getFullLoc(pDeclaration->getBeginLoc());
    if (fullLocation.isValid()) {
        CodeRecordsSingleton::getInstance()->addVarDecl(pDeclaration->getNameAsString(), fullLocation.getSpellingLineNumber(), fullLocation.getSpellingColumnNumber());
    }
    return true;
}

// вызовется, когда найдет FunctionDecl
bool VisitFunctionDecl(clang::FunctionDecl *pDeclaration) {
    clang::FullSourceLoc fullLocation = m_pContext->getFullLoc(pDeclaration->getBeginLoc());
    if (fullLocation.isValid()) {
        CodeRecordsSingleton::getInstance()->addFuncDecl(pDeclaration->getNameAsString(), fullLocation.getSpellingLineNumber(), fullLocation.getSpellingColumnNumber());
    }
    return true;
}

private:
    clang::ASTContext *m_pContext;
};

// наследуемся от ASTConsumer и используем наш FindDeclVisitor
class FindDeclConsumer : public clang::ASTConsumer {
public:
    explicit FindDeclConsumer(clang::ASTContext *pContext)
        : m_visitor(pContext) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        m_visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }
private:
    FindDeclVisitor m_visitor;
};

// наследуемся от ASTFrontendAction и используем наш FindDeclConsumer
class FindDeclAction : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        return std::make_unique<FindDeclConsumer>(&Compiler.getASTContext());
    }
};

CodeRecordsSingleton* CodeRecordsSingleton::m_pInstance = nullptr;

static llvm::cl::OptionCategory myOpts("myToolOptions");

int main(int argc, const char **argv) { 
	auto expectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, myOpts);
	if (!expectedParser) {
          llvm::errs() << expectedParser.takeError();
          std::cout << "Something went wrong\n";
          return 1;
	}
    clang::tooling::CommonOptionsParser &optionsParser = expectedParser.get();
    auto sourcePaths = optionsParser.getSourcePathList();
    // проходимся по всем файлам
    for (auto it = sourcePaths.begin(); it != sourcePaths.end(); ++it) {
        std::cout << "---------------------------------------------------------------------------------\n";
        std::cout << "File name: " << *it << "\n";
        CodeRecordsSingleton::getInstance()->clear();
        // создаём tool для данного файла и запускаем его
	    clang::tooling::ClangTool tool(optionsParser.getCompilations(), *it);
        tool.run(clang::tooling::newFrontendActionFactory<FindDeclAction>().get());

        CodeRecordsSingleton::getInstance()->printVariables();
        CodeRecordsSingleton::getInstance()->printFunctions();
    }
    return 0;
}