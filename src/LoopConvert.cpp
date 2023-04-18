/**
 * Author: James Pangia
 * 
 * Description:
 *      taken from https://intel.github.io/llvm-docs/clang/LibASTMatchersTutorial.html
 * 
 * Purpose:
 *      build a working example of using Clang source-to-source transformation
 * 
*/

//clang includes
#include "clang/Frontend/FrontendActions.h" //for clang::SyntaxOnlyAction
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

//llvm includes
#include "llvm/Support/CommandLine.h" //for llvm::cl::extrahelp

using namespace clang::tooling;
using namespace llvm;

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
    return Tool.run(newFrontendActionFactory<clang::SyntaxOnlyAction>().get());
}