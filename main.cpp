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


struct DeclRecord {
    std::string name;
    unsigned int lineNumber;
    unsigned int columnNumber;

    DeclRecord(std::string _name, unsigned int _lineNumber, unsigned int _columnNumber)
        : name(_name)
        , lineNumber(_lineNumber)
        , columnNumber(_columnNumber) {}
};

class CodeRecordsSingleton {
public:
    static CodeRecordsSingleton* getInstance() {
        if (m_pInstance == nullptr) {
            m_pInstance = new CodeRecordsSingleton();
        }
        return m_pInstance;
    }
    void addVarDecl(std::string name, unsigned int lineNumber, unsigned int columnNumber) {
        m_varDeclRecords.push_back( DeclRecord(name, lineNumber, columnNumber) );
    }

    void addFuncDecl(std::string name, unsigned int lineNumber, unsigned int columnNumber) {
        m_funcDeclRecords.push_back( DeclRecord(name, lineNumber, columnNumber) );
    }

    void printVariables() {
        std::cout << "Variables declarations count: " << m_varDeclRecords.size() << " \n";
        for(auto it = m_varDeclRecords.begin(); it != m_varDeclRecords.end(); ++it) {
            std::cout << "\tVariable - ";
            printDeclaration(*it);
        }
    }

    void printFunctions() {
        std::cout << "Functions declarations count: " << m_funcDeclRecords.size() << " \n";
        for(auto it = m_funcDeclRecords.begin(); it != m_funcDeclRecords.end(); ++it) {
            std::cout << "\tFunction - ";
            printDeclaration(*it);
        }
    }

    void clear() {
        m_varDeclRecords.clear();
        m_funcDeclRecords.clear();
    }
private:
    void printDeclaration(const DeclRecord& declRecord) {
        std::cout << declRecord.name  << " at " << declRecord.lineNumber << ":" << declRecord.columnNumber << "\n";
    }

    CodeRecordsSingleton() {}
    static CodeRecordsSingleton* m_pInstance;

    std::list<DeclRecord> m_varDeclRecords;
    std::list<DeclRecord> m_funcDeclRecords;
};

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