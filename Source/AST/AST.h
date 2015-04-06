#if !defined( SHADR_AST_H )
#define SHADR_AST_H

#include "Tokens.h"
#include "Type.h"
#include "Utility/Error.h"

class CASTBase
{
public:

	CASTBase()
	{}

	virtual ~CASTBase() {}

	//virtual llvm::Value* GenerateCode( CModule* pModule ) = 0;
	
};

class CASTProgram : public CASTBase
{
public:

	CASTProgram()
	: CASTBase()
	{}

	void AddElement( CASTBase* pElement ) { m_apElements.push_back( pElement ); }

private:

	std::vector< CASTBase* > m_apElements;
};



class CASTDefinition : public CASTBase
{
public:

	CASTDefinition()
	: CASTBase()
	{}
};

class CASTVariableDefinition : public CASTDefinition
{
public:

	CASTVariableDefinition( const CType& rtType, const std::string& rtName )
	: CASTDefinition()
	, m_tType( rtType )
	, m_tName( rtName )
	{}

	const CType& GetType() const { return m_tType; }
	CType& GetType() { return m_tType; }

	const std::string& GetName() const { return m_tName; }

	/*llvm::Value* GenerateCode( CModule* pModule )
	{
		Assert( 0, "Attempting to call GenerateCode on an unsupported AST node (CASTVariableDefinition)." );
		return NULL;
	}*/

private:

	CType		m_tType;
	std::string m_tName;
};

#include "ASTExpression.h"
#include "ASTStatement.h"
#include "ASTNumeric.h"
#include "ASTFunction.h"

#endif //SHADR_AST_H