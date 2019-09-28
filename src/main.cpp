#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <iostream>

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
    llvm::InitializeNativeTarget();
    LLVMInitializeAllTargets();

    // Prepare compilation arguments
    std::vector<const char *> args;
    args.push_back("--target=x86_64"); // check Triple.h for more target asm.
    args.push_back("-xc"); // Code is in c language
    args.push_back("-"); // Read code from stdin

    std::shared_ptr<clang::CompilerInvocation> CI =
        clang::createInvocationFromCommandLine(llvm::makeArrayRef(args) , NULL);

    // Create CompilerInstance
    clang::CompilerInstance Clang;
    Clang.setInvocation(CI);

    // Initialize CompilerInstace
    Clang.createDiagnostics();

    // Create and execute action
    clang::CodeGenAction *compilerAction; 
    compilerAction = new clang::EmitObjAction();
    Clang.ExecuteAction(*compilerAction);

    // Get compiled object (be carefull with buffer size)
    close(codeInPipe[0]);
    char objBuffer[2048];
    read(codeOutPipe[0], objBuffer, sizeof(objBuffer));
    std::cout << objBuffer << std::endl;
	return 0;
}
