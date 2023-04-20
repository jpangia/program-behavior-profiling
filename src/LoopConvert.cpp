/**
 * Author: James Pangia
 *
 * Description:
 *      taken from https://intel.github.io/llvm-docs/clang/LibASTMatchersTutorial.html
 *
 * Purpose:
 *      build a working example of using Clang source-to-source transformation
 *
 * usage:
 *      $path/LoopConvert [input c file] --
 *  note: must have the -- at the end, otherwise you get a "no compilation database" error
*/

//clang includes
#include "clang/Frontend/FrontendActions.h" //for clang::SyntaxOnlyAction
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Transformer/RewriteRule.h"
#include "clang/Tooling/Transformer/Stencil.h"

//llvm includes
#include "llvm/Support/CommandLine.h" //for llvm::cl::extrahelp

//regular includes
#include <stdio.h>

using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;
using namespace clang::transformer;

const clang::TranslationUnitDecl* TU;

class LoopPrinter: public MatchFinder::MatchCallback
{
public:

    virtual void run(const MatchFinder::MatchResult& Result)
    {
        printf("printer oink\n");
        if(const clang::ForStmt* FS = Result.Nodes.getNodeAs<clang::ForStmt>("forLoop"))
        {
            const clang::ASTContext* ctxt = Result.Context;
            // FS->dump(llvm::outs(), ctxt);
            makeRule(forStmt(), noopEdit(node("forLoop")), cat("found a for loop"));
            FS->dumpPretty(*ctxt);
        }

        // if(const clang::TranslationUnitDecl* tinyTU = Result.Nodes.getNodeAs<clang::TranslationUnitDecl>("program"))
        // {
        //     printf("tinyTU oink\n");
        //     TU = tinyTU;
        //     TU->dump();
        // }

        // TU = Result.Context->getTranslationUnitDecl();
    }
};

//add option category to option parser
static llvm::cl::OptionCategory MyToolCategory("my-tool options");

//default help message
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

//additional help
static cl:: extrahelp MoreHelp("\nMore help!\n");

int main(int argc, const char** argv)
{
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
    if(!ExpectedParser)
    {
        //quit because couldn't get the options
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }

    CommonOptionsParser& OptionsParser = ExpectedParser.get();
    ClangTool Tool(OptionsParser.getCompilations(),
                    OptionsParser.getSourcePathList());


    StatementMatcher LoopMatcher =
    forStmt(hasLoopInit(declStmt(hasSingleDecl(varDecl(hasInitializer(integerLiteral(equals(0)))))))).bind("forLoop");

    // StatementMatcher ProgMatcher = ;

    LoopPrinter Printer;
    MatchFinder Finder;
    Finder.addMatcher(LoopMatcher, &Printer);
    Finder.addMatcher(translationUnitDecl().bind("program"), &Printer);

    printf("oink\n");
    Tool.run(newFrontendActionFactory(&Finder).get());
    return 0;
}
