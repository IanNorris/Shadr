#include "LLVM.h"
#include "Utility/Error.h"

void InitialiseLLVM( void )
{
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();
}

CModule::CModule( const char* pszModule )
: m_rtContext( llvm::getGlobalContext() )
, m_tIRBuilder( m_rtContext )
, m_pModule( nullptr )
, m_pExecutionEngine( nullptr )
{
	std::unique_ptr<llvm::Module> pModule( std::make_unique<llvm::Module>( pszModule, m_rtContext ) );
	m_pModule = pModule.get();

#ifdef _WIN32
	/*
	* MCJIT works on Windows, but currently only through ELF object format.
	*/
	std::string targetTriple = llvm::sys::getProcessTriple();
	targetTriple.append("-elf");
	m_pModule->setTargetTriple(targetTriple);
#endif

	// Create the JIT.  This takes ownership of the module.
	std::string tError;
	
	m_pExecutionEngine = llvm::EngineBuilder( std::move(pModule) )
		.setErrorStr(&tError)
		.setMCJITMemoryManager( std::make_unique<llvm::SectionMemoryManager>() )
		.create();

	Assert( m_pExecutionEngine, "Creation of execution engine failed with error: %s", tError.c_str() )

	m_pFunctionPassManager = std::make_unique<llvm::FunctionPassManager>( m_pModule );

	// Set up the optimizer pipeline.  Start with registering info about how the
	// target lays out data structures.
	m_pModule->setDataLayout( m_pExecutionEngine->getDataLayout() );

	m_pFunctionPassManager->add( new llvm::DataLayoutPass() );

	// Provide basic AliasAnalysis support for GVN.
	m_pFunctionPassManager->add( llvm::createBasicAliasAnalysisPass() );

	// Do simple "peephole" optimizations and bit-twiddling optimizations.
	m_pFunctionPassManager->add( llvm::createInstructionCombiningPass() );

	// Re-associate expressions.
	m_pFunctionPassManager->add( llvm::createReassociatePass() );

	// Eliminate Common SubExpressions.
	m_pFunctionPassManager->add( llvm::createGVNPass() );

	// Simplify the control flow graph (deleting unreachable blocks, etc).
	m_pFunctionPassManager->add( llvm::createCFGSimplificationPass() );

	m_pFunctionPassManager->doInitialization();
}