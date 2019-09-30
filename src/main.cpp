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

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetSelect.h>

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
    constexpr auto testCode = "int main() { return 2+2*2; }";

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

    std::vector<const char *> args;
    // clang -cc1 mode
    args.push_back("-O0");
    args.push_back("--target=x86_64"); // check Triple.h for more target asm.
    args.push_back("-xc"); // Code is in c language
    args.push_back("-"); // Read code from stdin

    std::shared_ptr<clang::CompilerInvocation> CI =
        clang::createInvocationFromCommandLine(llvm::makeArrayRef(args) , NULL);
    // Create CompilerInstance
    clang::CompilerInstance Clang;
    Clang.setInvocation(std::move(CI));
    // Initialize CompilerInstace
    Clang.createDiagnostics();
	if (!Clang.hasDiagnostics()) {
        llvm::errs() << "createDiagnostics failed.\n";
        return 1;
    }

    //TODO: Infer the builtin include path if unspecified

	// Create and execute the frontend to generate an LLVM bitcode module.
	std::unique_ptr<clang::CodeGenAction> Act(new clang::EmitLLVMOnlyAction());
	if (!Clang.ExecuteAction(*Act)) {
	    return 1;
    }

    llvm::InitializeAllTargets();
    llvm::InitializeAllAsmPrinters();

    std::unique_ptr<llvm::LLVMContext> Ctx(Act->takeLLVMContext());
    std::unique_ptr<llvm::Module> theModule = Act->takeModule();

    llvm::errs() << *theModule << "\n";
#if 0
    // Get compiled object (be carefull with buffer size)
    close(codeInPipe[PIPE_READ]);
    char objBuffer[4096];
    ssize_t read_cnt = read(codeOutPipe[PIPE_READ], objBuffer, sizeof(objBuffer));
    llvm::errs() << "Read: " << read_cnt << " bytes\n";
    llvm::errs() << objBuffer << "\n";
#endif
	return 0;
}
