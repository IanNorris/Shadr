#include "LLVM.h"
#include "AST.h"
#include "Error.h"

llvm::Value* CASTPrototype::GenerateCode( CModule* pModule )
{
	std::vector<llvm::Type*> tArguments;

	llvm::FunctionType* pFunctionType = llvm::FunctionType::get( llvm::Type::getDoubleTy( llvm::getGlobalContext() ), tArguments, false );

	llvm::Function* pFunction = llvm::Function::Create( pFunctionType, llvm::Function::ExternalLinkage, m_tName, pModule->GetModule() );

	//TODO: Make sure it isn't already defined

	return pFunction;
}

llvm::Value* CASTFunction::GenerateCode( CModule* pModule )
{
	llvm::Function* pFunction = (llvm::Function*)m_pProrotype->GenerateCode( pModule );
	Assert( pFunction, "Function prototype generation failed" );

	// Create a new basic block to start insertion into.
	llvm::BasicBlock *BB = llvm::BasicBlock::Create( llvm::getGlobalContext(), "entry", pFunction );
	pModule->GetIRBuilder().SetInsertPoint(BB);

	llvm::Value* pReturnValue = m_pBody->GenerateCode( pModule );

	Assert( pReturnValue, "Function body is invalid" );

	// Finish off the function.
	pModule->GetIRBuilder().CreateRet( pReturnValue );

	// Validate the generated code, checking for consistency.
	llvm::verifyFunction( *pFunction );

	// Optimize the function.
	pModule->GetFPM()->run( *pFunction );

	return pFunction;
}