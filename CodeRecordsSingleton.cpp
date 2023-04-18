#include <iostream>
#include "includes/CodeRecordsSingleton.h"

void CodeRecordsSingleton::printVariables() {
    std::cout << "Variables declarations count: " << m_varDeclRecords.size() << " \n";
    for(auto it = m_varDeclRecords.begin(); it != m_varDeclRecords.end(); ++it) {
        std::cout << "\tVariable - ";
        printDeclaration(*it);
    }
}

void CodeRecordsSingleton::printFunctions() {
    std::cout << "Functions declarations count: " << m_funcDeclRecords.size() << " \n";
    for(auto it = m_funcDeclRecords.begin(); it != m_funcDeclRecords.end(); ++it) {
        std::cout << "\tFunction - ";
        printDeclaration(*it);
    }
}

void CodeRecordsSingleton::printDeclaration(const DeclRecord& declRecord) {
    std::cout << declRecord.name  << " at " << declRecord.lineNumber << ":" << declRecord.columnNumber << "\n";
}