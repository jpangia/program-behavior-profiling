/**
 * Author: James Pangia
 *
 * Description:
 *      example material taken from the following:
 *          https://intel.github.io/llvm-docs/clang/LibASTMatchersTutorial.html
 *          https://github.com/eliben/llvm-clang-samples/blob/master/src_clang/rewritersample.cpp
 *
 * Purpose:
 *      [TODO]
 *
 * usage:
 *      $path/branch-track <path to input> --
 * sample usage:
 *      ./bin/branch-track ../../program-behavior-profiling/sample-code/simpleFor/simpleFor.c  --
 *  IMPORTANT: must have the -- as the final argument, otherwise the program attempts to process the standard include headers.
 *              Either the program fails because it doesn't have root permissions, or it actually edits those files
 *              Either outcome would be bad. Best to not run this as root.
 * 
*/

//clang includes
#include "clang/AST/DeclBase.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h" //for clang::SyntaxOnlyAction
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

//llvm includes
#include "llvm/Support/CommandLine.h" //for llvm::cl::extrahelp
#include "llvm/TargetParser/Host.h"

//regular includes
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>

using namespace clang;
using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;

//global Rewriter
Rewriter glob_Rewriter;

//global dictionary file to record branch information
std::ofstream* glob_dictFile;

// global to hold the name of the input program;
std::string glob_inputProgName;

class BranchTracker: public MatchFinder::MatchCallback
{
public:
    int branchCounter = 0;    

    /**
     * references glob_Rewriter and glob_dictFile
     * @param ctxt the ASTContext of the statement; used to get location info
     * @param lpStmt the statement to process
     * @returns false if a non-loop statement was passed, true otherwise
    */
    bool processLoopBranch(clang::ASTContext *const ctxt, const Stmt* lpStmt)
    {
        //to hold the body or the while statement if lpStmt is a DoStmt
        const Stmt* body;
        SourceLocation whileLoc;
        bool isDo = false;
        if(isa<DoStmt>(lpStmt))
        {
            isDo = true;
            whileLoc = dyn_cast<DoStmt>(lpStmt)->getWhileLoc();
            body = dyn_cast<DoStmt>(lpStmt)->getBody();
        }
        else if(isa<ForStmt>(lpStmt))
        {
            body = dyn_cast<ForStmt>(lpStmt)->getBody();
        }
        else if(isa<WhileStmt>(lpStmt))
        {
            body = dyn_cast<WhileStmt>(lpStmt)->getBody();
        }
        else
        {
            return false;
        }
        
        clang::FullSourceLoc lpJmpLoc = ctxt->getFullLoc(lpStmt->getBeginLoc());
        if(isDo)
        {
            //set the jump source to be the while statement
            lpJmpLoc = ctxt->getFullLoc(whileLoc);
        }

        //insert loop body instrumentation print
        std::string branchPrint = "\nfprintf(stderr, \"branch " + std::to_string(++branchCounter) + "\\n\");";
        glob_Rewriter.InsertTextAfterToken(body->getBeginLoc(), branchPrint);
        //write branch information
        clang::FullSourceLoc bodyStart = ctxt->getFullLoc(body->getBeginLoc());
        std::string branchInfo = "branch " + std::to_string(branchCounter) + 
                                    " " + glob_inputProgName + ", " + 
                                    std::to_string(lpJmpLoc.getLineNumber()) + ", " +
                                    std::to_string(bodyStart.getLineNumber());
        *glob_dictFile <<  branchInfo << std::endl;


        //insert loop exit instrumentation print
        branchPrint = "\nfprintf(stderr, \"branch " + std::to_string(++branchCounter) + "\\n\");";
        SourceLocation insertPoint = lpStmt->getEndLoc();
        clang::FullSourceLoc lpEnd = ctxt->getFullLoc(body->getEndLoc());
        //account for do-whiles
        if(isDo)
        {
            //modify branch instrumentation line (no need to increment branchCounter this time)
            branchPrint = ";\nfprintf(stderr, \"branch " + std::to_string(branchCounter) + "\\n\")";
            //replace lpEnd with whileEnd
            lpEnd = ctxt->getFullLoc(whileLoc);
        }
        glob_Rewriter.InsertTextAfterToken(insertPoint, branchPrint);
        
        //write branch information
        branchInfo = "branch " + std::to_string(branchCounter) + 
                                    " " + glob_inputProgName + ", " + 
                                    std::to_string(lpJmpLoc.getLineNumber()) + ", " +
                                    std::to_string(lpEnd.getLineNumber());
        *glob_dictFile <<  branchInfo << std::endl;

        return true;
    }

    virtual void run(const MatchFinder::MatchResult& Result) override
    {   
        // catch break
        if(const clang::BreakStmt* BS = Result.Nodes.getNodeAs<clang::BreakStmt>("breakStmt"))
        {
            //note getParents expects an object not a pointer
            auto parents = Result.Context->getParents(*BS);
            const Stmt* ST; //to hold the parent statement

            //to hold the branch's source and target locations
            FullSourceLoc bsLoc;
            FullSourceLoc bsTrgtLoc;

            do 
            {
                if(parents.empty())
                {
                    llvm::errs() << "no parent";
                    return;
                }

                //get parent
                ST = parents[0].get<Stmt>();

                //update parent set for next iteration
                parents = Result.Context->getParents(*ST);
                
                //check for loop or switch statement
                if(isa<DoStmt>(ST))
                {
                    //recast the statement as a DoStmt
                    //note: dyn_cast template should only be passed the 
                    //      target type; no need to pass that it is a pointer
                    //      that is automagically determined
                    auto DoS = dyn_cast<DoStmt>(ST);

                    //get break location
                    bsLoc = Result.Context->getFullLoc(BS->getBeginLoc());
                    //get break target location
                    bsTrgtLoc = Result.Context->getFullLoc(DoS->getWhileLoc());
                    break;
                }
                else if(isa<ForStmt>(ST))
                {
                    //recast the statement as a ForStmt
                    auto ForS = dyn_cast<ForStmt>(ST);

                    //get break location
                    bsLoc = Result.Context->getFullLoc(BS->getBeginLoc());
                    //get break target location
                    bsTrgtLoc = Result.Context->getFullLoc(ForS->getBody()->getEndLoc());
                    break;
                }
                else if(isa<WhileStmt>(ST))
                {
                    //recast the statement as a WhileStmt
                    auto WhileS = dyn_cast<WhileStmt>(ST);

                    //get break location
                    bsLoc = Result.Context->getFullLoc(BS->getBeginLoc());
                    //get break target location
                    bsTrgtLoc = Result.Context->getFullLoc(WhileS->getBody()->getEndLoc());
                    break;
                }
                else if(isa<SwitchStmt>(ST))
                {
                    //recast the statement as a SwitchStmt
                    auto SwitchS = dyn_cast<SwitchStmt>(ST);
                    //get break location
                    bsLoc = Result.Context->getFullLoc(BS->getBeginLoc());
                    //get break target location
                    bsTrgtLoc = Result.Context->getFullLoc(SwitchS->getBody()->getEndLoc());
                    break;
                }
            } while(true);

            //insert instrumentation print
            std::string branchPrint = "fprintf(stderr, \"branch " + std::to_string(++branchCounter) + "\\n\");\n";
            glob_Rewriter.InsertTextBefore(bsLoc, branchPrint);

            //write branch information
            std::string branchInfo = "branch " + std::to_string(branchCounter) + 
                                     " " + glob_inputProgName + ", " + 
                                     std::to_string(bsLoc.getLineNumber()) + ", " +
                                     std::to_string(bsTrgtLoc.getLineNumber());
            *glob_dictFile <<  branchInfo << std::endl;

        }
        // catch continue
        if(const clang::ContinueStmt* CS = Result.Nodes.getNodeAs<clang::ContinueStmt>("continueStmt"))
        {
            //we need to traverse up to the node's parents until we find a loop for the `continue`
            //first, get the node's parents
            //note getParents expects an object not a pointer
            auto parents = Result.Context->getParents(*CS);
            const Stmt* ST; //to hold the parent statement

            //to hold the branch's source and target locations
            FullSourceLoc csLoc;
            FullSourceLoc csTrgtLoc;

            //find the loop that the continue targets
            do 
            {
                if(parents.empty())
                {
                    llvm::errs() << "no parent";
                    return;
                }

                ST = parents[0].get<Stmt>();
                
                //update parent set for next iteration
                parents = Result.Context->getParents(*ST);

                //check for a loop statement
                if(isa<DoStmt>(ST))
                {
                    //recast the statement as a DoStmt
                    //note: dyn_cast template should only be passed the 
                    //      target type; no need to pass that it is a pointer
                    //      that is automagically determined
                    auto DoS = dyn_cast<DoStmt>(ST);

                    //get continue location
                    csLoc = Result.Context->getFullLoc(CS->getBeginLoc());
                    //get continue target location
                    csTrgtLoc = Result.Context->getFullLoc(DoS->getWhileLoc());
                    break;
                }
                else if(isa<ForStmt>(ST))
                {
                    //recast the statement as a ForStmt
                    auto ForS = dyn_cast<ForStmt>(ST);

                    //get continue location
                    csLoc = Result.Context->getFullLoc(CS->getBeginLoc());
                    //get continue target location
                    csTrgtLoc = Result.Context->getFullLoc(ForS->getForLoc());
                    break;
                }
                else if(isa<WhileStmt>(ST))
                {
                    //recast the statement as a WhileStmt
                    auto WhileS = dyn_cast<WhileStmt>(ST);

                    //get continue location
                    csLoc = Result.Context->getFullLoc(CS->getBeginLoc());
                    //get continue target location
                    csTrgtLoc = Result.Context->getFullLoc(WhileS->getWhileLoc());
                    break;
                }
            } while(true);

            //insert instrumentation print
            std::string branchPrint = "fprintf(stderr, \"branch " + std::to_string(++branchCounter) + "\\n\");\n";
            glob_Rewriter.InsertTextBefore(csLoc, branchPrint);

            //write branch information
            std::string branchInfo = "branch " + std::to_string(branchCounter) + 
                                     " " + glob_inputProgName + ", " + 
                                     std::to_string(csLoc.getLineNumber()) + ", " +
                                     std::to_string(csTrgtLoc.getLineNumber());
            *glob_dictFile <<  branchInfo << std::endl;
        }
        // catch do-while
        if(const clang::DoStmt* DS = Result.Nodes.getNodeAs<clang::DoStmt>("doStmt"))
        {
            processLoopBranch(Result.Context, DS);
        }
        
        // catch for
        if(const clang::ForStmt* FS = Result.Nodes.getNodeAs<clang::ForStmt>("forLoop"))
        {
            processLoopBranch(Result.Context, FS);
        }

        // catch function call
        if(const clang::CallExpr* CE = Result.Nodes.getNodeAs<clang::CallExpr>("callExpr"))
        {   
            llvm::StringRef funcName = CE->getDirectCallee()->getName();
            std::string branchPrint = ";\n" \
                                      "fprintf(stderr, \"function %p\\n\", &" + std::string(funcName.data()) + ")";

            glob_Rewriter.InsertTextAfterToken(CE->getEndLoc(), branchPrint);
        }

        // catch goto
        if(const clang::GotoStmt* GS = Result.Nodes.getNodeAs<clang::GotoStmt>("gotoStmt"))
        {
            fprintf(stderr, "currently unimplemented\n");
        }

        // catch if
        if(const clang::IfStmt* IS = Result.Nodes.getNodeAs<clang::IfStmt>("ifStmt"))
        {
            //insert branch statement into if
            std::string branchPrint = "\nfprintf(stderr, \"branch " + std::to_string(++branchCounter) + "\\n\");\n";
            glob_Rewriter.InsertTextAfterToken(IS->getThen()->getBeginLoc(), branchPrint);

            //write to dictionary
            FullSourceLoc ifLoc = Result.Context->getFullLoc(IS->getBeginLoc());
            FullSourceLoc thenLoc = Result.Context->getFullLoc(IS->getThen()->getBeginLoc());
            std::string branchInfo = "branch " + std::to_string(branchCounter) + 
                                     " " + glob_inputProgName + ", " + 
                                     std::to_string(ifLoc.getLineNumber()) + ", " +
                                     std::to_string(thenLoc.getLineNumber());
            *glob_dictFile <<  branchInfo << std::endl;
            //optionally insert branch statement into else
            if(auto ES = IS->getElse())
            {
                //insert branch statement into else
                std::string branchPrint = "\nfprintf(stderr, \"branch " + std::to_string(++branchCounter) + "\\n\");\n";
                glob_Rewriter.InsertTextAfterToken(ES->getBeginLoc(), branchPrint);

                //write to dictionary
                FullSourceLoc ifLoc = Result.Context->getFullLoc(IS->getBeginLoc());
                FullSourceLoc elseLoc = Result.Context->getFullLoc(ES->getBeginLoc());
                std::string branchInfo = "branch " + std::to_string(branchCounter) + 
                                        " " + glob_inputProgName + ", " + 
                                        std::to_string(ifLoc.getLineNumber()) + ", " +
                                        std::to_string(elseLoc.getLineNumber());
                *glob_dictFile <<  branchInfo << std::endl;
            }
        }

        // catch switch
        // note: the branch entries are stuctured to reflect high-level logic, not the underlying jumps
        // so the jump source will be the switch, and the jump destination will be the case
        if(const clang::SwitchStmt* SS = Result.Nodes.getNodeAs<clang::SwitchStmt>("switchStmt"))
        {
            const SwitchCase* nextCase = SS->getSwitchCaseList();
            FullSourceLoc SSLoc = Result.Context->getFullLoc(SS->getBeginLoc());

            //iterate through all cases
            while(nextCase != nullptr)
            {
                //insert instrumentation
                std::string branchPrint = "\nfprintf(stderr, \"branch " + std::to_string(++branchCounter) + "\\n\");";
                glob_Rewriter.InsertTextAfterToken(nextCase->getColonLoc(), branchPrint);

                //write source and destination to dictionary
                FullSourceLoc caseLoc = Result.Context->getFullLoc(nextCase->getBeginLoc());
                std::string branchInfo = "branch " + std::to_string(branchCounter) + 
                                     " " + glob_inputProgName + ", " + 
                                     std::to_string(SSLoc.getLineNumber()) + ", " +
                                     std::to_string(caseLoc.getLineNumber());
                *glob_dictFile <<  branchInfo << std::endl;

                //advance the pointer
                nextCase = nextCase->getNextSwitchCase();
            }
        }

        // catch while
        if(const clang::WhileStmt* WS = Result.Nodes.getNodeAs<clang::WhileStmt>("whileLoop"))
        {
            processLoopBranch(Result.Context, WS);
        }
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
    std::ofstream outputFile;
    std::string outFileName;
    std::string dictFileName = "";
    // check if an output file is passed after the --
    // if so, create the output stream
    if(argc >=4)
    {
        outFileName = argv[3];
        outputFile = std::ofstream(outFileName, std::ofstream::out);
        if(!outputFile.good())
        {
            llvm::errs() << outFileName << " was not good!";
            return 1;
        }

        if(argc >= 5)
        {
            dictFileName = argv[4];
        }
    }

    //create the optionsparser for the ClangTool
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
    if(!ExpectedParser)
    {
        //quit because couldn't get the options
        llvm::errs() << "usage: branch-track <path to input C source file> -- "
                     << "[path to file to write instrumented C file to]";
        return 1;
    }

    CommonOptionsParser& OptionsParser = ExpectedParser.get();
    ClangTool Tool(OptionsParser.getCompilations(),
                    OptionsParser.getSourcePathList());

    glob_inputProgName = OptionsParser.getSourcePathList()[0];
    
    //create a parallel rewriter and the prerequisite infrastructure:
    // the ClangTool should traverse the input source file, while the 
    // Rewriter attached to the same file (but probably not the same object)
    // edits the file;
    // don't forget to add "--" after the file name so clang doesn't try to traverse 
    //  standard library included .h files
    // (I don't understand clang well enough to know why this works, but it does)

    // CompilerInstance will hold the instance of the Clang compiler for us,
    // managing the various objects needed to run the compiler.
    clang::CompilerInstance TheCompInst;
    TheCompInst.createDiagnostics();

    clang::LangOptions &lo = TheCompInst.getLangOpts();
    lo.CPlusPlus = 1;

    // Initialize target info with the default triple for our platform.
    auto TO = std::make_shared<clang::TargetOptions>();
    TO->Triple = llvm::sys::getDefaultTargetTriple();
    TargetInfo *TI =
        TargetInfo::CreateTargetInfo(TheCompInst.getDiagnostics(), TO);
    TheCompInst.setTarget(TI);

    TheCompInst.createFileManager();
    FileManager &FileMgr = TheCompInst.getFileManager();
    TheCompInst.createSourceManager(FileMgr);
    SourceManager &SourceMgr = TheCompInst.getSourceManager();
    TheCompInst.createPreprocessor(TU_Module);
    TheCompInst.createASTContext();

    // A Rewriter helps us manage the code rewriting task.
    glob_Rewriter.setSourceMgr(SourceMgr, TheCompInst.getLangOpts());

    // Set the main file handled by the source manager to the input file.
    llvm::ErrorOr<const FileEntry*> FileIn = FileMgr.getFile(argv[1]);
    SourceMgr.setMainFileID(
        SourceMgr.createFileID(FileIn.get(), SourceLocation(), SrcMgr::C_User));
    TheCompInst.getDiagnosticClient().BeginSourceFile(
        TheCompInst.getLangOpts(), &TheCompInst.getPreprocessor());

    //create the matchers
    StatementMatcher IfMatcher = ifStmt().bind("ifStmt");
    StatementMatcher WhileMatcher = whileStmt().bind("whileLoop");
    StatementMatcher DoMatcher = doStmt().bind("doStmt");
    StatementMatcher ForMatcher = forStmt().bind("forLoop");
    StatementMatcher SwitchMatcher = switchStmt().bind("switchStmt");
    StatementMatcher BreakMatcher = breakStmt().bind("breakStmt");
    StatementMatcher ContinueMatcher = continueStmt().bind("continueStmt");
    StatementMatcher GotoMatcher = gotoStmt().bind("gotoStmt");
    StatementMatcher CallExpr = callExpr().bind("callExpr");
    StatementMatcher ReturnMatcher = returnStmt().bind("returnStmt");

    BranchTracker Tracker;
    MatchFinder Finder;
    //add the matchers to the finder
    Finder.addMatcher(IfMatcher, &Tracker);
    Finder.addMatcher(WhileMatcher, &Tracker);
    Finder.addMatcher(DoMatcher, &Tracker);
    Finder.addMatcher(ForMatcher, &Tracker);
    Finder.addMatcher(SwitchMatcher, &Tracker);
    Finder.addMatcher(BreakMatcher, &Tracker);
    Finder.addMatcher(ContinueMatcher, &Tracker);
    Finder.addMatcher(GotoMatcher, &Tracker);
    Finder.addMatcher(CallExpr, &Tracker);
    Finder.addMatcher(ReturnMatcher, &Tracker);
    // Finder.addMatcher(translationUnitDecl().bind("translationUnitDecl"), &Tracker);

    //create the dictionary file, using a passed file path, if specified
    if(dictFileName.length() > 0)
    {
        glob_dictFile = new std::ofstream(dictFileName, std::ofstream::out);
    }
    else
    {
        glob_dictFile = new std::ofstream("dictionary.txt", std::ofstream::out);
    }
    
    //run the tool
    Tool.run(newFrontendActionFactory(&Finder).get());

    // At this point the rewriter's buffer should be full with the rewritten
    // file contents.
    const RewriteBuffer *RewriteBuf =
        glob_Rewriter.getRewriteBufferFor(SourceMgr.getMainFileID());
    if(outputFile.is_open())
    {
        llvm::outs() << "wrote to " << outFileName;
        outputFile << std::string(RewriteBuf->begin(), RewriteBuf->end());
    }
    else
    {
        llvm::outs() << std::string(RewriteBuf->begin(), RewriteBuf->end());
    }

    glob_dictFile->close();
    delete glob_dictFile;
    return 0;
}
