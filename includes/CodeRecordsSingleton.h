#pragma once
#include <list>


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

    void printVariables();
    void printFunctions();

    void clear() {
        m_varDeclRecords.clear();
        m_funcDeclRecords.clear();
    }
private:
    void printDeclaration(const DeclRecord& declRecord);

    CodeRecordsSingleton() {}
    static CodeRecordsSingleton* m_pInstance;

    std::list<DeclRecord> m_varDeclRecords;
    std::list<DeclRecord> m_funcDeclRecords;
};