#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <vector>
/*
#include "llvm/IR/LLVMContext.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
*/

#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm-c/Target.h>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Basic/TargetInfo.h>

#define PIPE_WRITE  1
#define PIPE_READ   0

int main( int argc, char *argv[] )
{

    // Sample input
    constexpr auto testCodeFileName = "test.cpp";
    constexpr auto testCode = "int test() { return 2+2; }";

    // Send code through a pipe to stdin
    int codeInPipe[2];
    pipe2(codeInPipe, O_NONBLOCK);
    write(codeInPipe[PIPE_WRITE], (void *) testCode, strlen(testCode));
    close(codeInPipe[PIPE_WRITE]); // close the pipe to send an EOF
    dup2(codeInPipe[PIPE_READ], STDIN_FILENO);

    // Prepare reception of code through stdout
    int codeOutPipe[2];
    pipe(codeOutPipe);
    dup2(codeOutPipe[PIPE_WRITE], STDOUT_FILENO);

    // Initialized Targets
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    LLVMInitializeAllTargets();

    // Prepare compilation arguments
    std::vector<const char *> args;
    args.push_back("--target=x86_64"); // check Triple.h for more target asm.
    args.push_back("-x c"); // Code is in c language
    args.push_back("-"); // Read code from stdin

    std::unique_ptr<clang::CompilerInvocation> CI =
        clang::createInvocationFromCommandLine(llvm::makeArrayRef(args) , NULL);
	return 0;
}
