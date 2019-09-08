#include <stdio.h>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

int main( int argc, char *argv[] )
{
	if( argc < 2 )
		llvm::errs() << "Expected an argument - IR file name\n";

    llvm::LLVMContext *Context;
	llvm::SMDiagnostic err;
    std::unique_ptr<llvm::Module> module = llvm::parseIRFile(argv[1], err, *Context);
	if(!module)
	{
		err.print( argv[0], llvm::errs() );
		return 1;
	}
	return 0;
}
