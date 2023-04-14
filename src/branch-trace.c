/**
 * Author: James Pangia
 * Institution: NCSU
 * 
 * root github: https://github.com/jpangia/program-behavior-profiling
 * 
 * Description:
 *  Program to identify a branch trace for input C programs
 *  
 *  based off the sample code at the bottom of: https://intel.github.io/llvm-docs/clang/RAVFrontendAction.html
 *  
 * usage: execute a sample run with (from repo root directory):
 *    `./build/sample-frontend-action "namespace n { namespace m { class C {}; } }"`
*/

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;

class FindBranchVisitor
  : public RecursiveASTVisitor<FindBranchVisitor> {
public:
  explicit FindBranchVisitor(ASTContext *Context)
    : Context(Context) {}
  //TODO:
  //implement bool Visit[nodetype](nodetype*) function for each translation unit of interest
  // catch the following nodes:
  // - if (IfStmt)
  // - else (ElseStmt)
  // - while (WhileStmt)
  // - do-while (DoStmt)
  // - for (ForStmt)
  // - switch (SwitchStmt)
  // x case (SwitchCase, but this doesn't branch)
  // - break (BreakStmt)
  // - continue (ContinueStmt)
  // - goto (GotoStmt)
  // - indirect goto? (IndirectGotoStmt)
  // - function call (CallExpr)
  // - return (ReturnStmt)
  bool VisitCXXRecordDecl(CXXRecordDecl *Declaration) 
  {
      Declaration->dump();
      FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
      printf("hasManager:%d\n", FullLocation.hasManager());
      if (Declaration->getQualifiedNameAsString() == "n::m::C") 
      {
        // FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
        if (FullLocation.isValid())
          llvm::outs() << "Found declaration at "
                      << FullLocation.getSpellingLineNumber() << ":"
                      << FullLocation.getSpellingColumnNumber() << "\n";
      }

    return true;
  }

private:
  ASTContext *Context;
};

class FindNamedClassConsumer : public clang::ASTConsumer {
public:
  explicit FindNamedClassConsumer(ASTContext *Context)
    : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  FindBranchVisitor Visitor;
};

class FindNamedClassAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::make_unique<FindNamedClassConsumer>(&Compiler.getASTContext());
  }
};

int main(int argc, char **argv) {
  if (argc > 1) {
    clang::tooling::runToolOnCode(std::make_unique<FindNamedClassAction>(), argv[1]);
  }
}