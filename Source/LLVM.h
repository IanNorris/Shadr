#if !defined( SHADR_LLVM_H )
#define SHADR_LLVM_H

//Ugh... LLVM is not warning clean
#pragma warning(push, 0)  

#include "llvm/Analysis/Passes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/Host.h"
#include "llvm/Transforms/Scalar.h"

#pragma warning(pop)

class CModule
{
public:

	CModule( const char* pszModule );

private:

	llvm::LLVMContext&								m_rtContext;
	llvm::Module*									m_pModule;
	llvm::ExecutionEngine*							m_pExecutionEngine;
	std::unique_ptr<llvm::FunctionPassManager>		m_pFunctionPassManager;
};

void InitialiseLLVM( void );

#endif //SHADR_LLVM_H