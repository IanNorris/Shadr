#if !defined( SHADR_AST_H )
#define SHADR_AST_H

#include "Tokens.h"
#include "Type.h"
#include "Utility/Error.h"

class CASTBase
{
public:

	CASTBase( const CType& rtType )
	: m_tType( rtType )
	{}

	virtual ~CASTBase() {}

	virtual llvm::Value* GenerateCode( CModule* pModule ) = 0;
	const CType& GetType() const { return m_tType; }
	CType& GetType() { return m_tType; }

private:

	CType		m_tType;
};

class CASTDefinition : public CASTBase
{
public:

	CASTDefinition( const CType& rtType )
	: CASTBase( rtType )
	{}
};

class CASTVariableDefinition : public CASTDefinition
{
public:

	CASTVariableDefinition( const CType& rtType, const std::string& rtName )
	: CASTDefinition( rtType )
	, m_tName( rtName )
	{}

	const std::string& GetName() const { return m_tName; }

	llvm::Value* GenerateCode( CModule* pModule )
	{
		Assert( 0, "Attempting to call GenerateCode on an unsupported AST node (CASTVariableDefinition)." );
		return NULL;
	}

private:

	std::string m_tName;
};

class CASTBlock : public CASTBase
{
public:

	CASTBlock()
	: CASTBase( CType::GetVoidType() )
	{}

	void Add( CASTBase* pChild )
	{
		m_apChildren.push_back( pChild );
	}

	llvm::Value* GenerateCode( CModule* pModule );

private:

	std::vector< CASTBase* > m_apChildren;
};

#include "ASTExpression.h"
#include "ASTNumeric.h"
#include "ASTFunction.h"

#endif //SHADR_AST_H