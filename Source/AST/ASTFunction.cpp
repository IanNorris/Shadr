#include "CompilationUnit.h"
#include "AST.h"
#include "Utility/Error.h"
#include <minmax.h>

bool CanCoerceType( const CType& from, const CType& to, bool bAllowWarnings );

bool CASTExpressionFunctionCall::FindMatchingFunction( CScope* pScope )
{
	std::vector<CASTPrototype*> apPrototypes;
	std::vector<CASTPrototype*> apPrototypesTemp;
	pScope->FindPrototypes( m_tName, apPrototypes );
	
	if( apPrototypes.empty() )
	{
		Error_Compiler( 
			EError_Error, 
			GetParserPosition().pszFilename, 
			GetParserPosition().uCurrentRow, 
			GetParserPosition().uCurrentCol, 
			"Function \"%s\" is not declared.", 
			GetFunctionName().c_str()
		);

		return false;
	}

	size_t uParamCount = m_apParameters.size();

	for( auto& protoIter = apPrototypes.begin(); protoIter != apPrototypes.end(); )
	{
		if( (*protoIter)->CanFitParameterCount( uParamCount ) )
		{
			++protoIter;
		}
		else
		{
			protoIter = apPrototypes.erase( protoIter );
		}
	}

	if( apPrototypes.empty() )
	{
		Error_Compiler( 
			EError_Error, 
			GetParserPosition().pszFilename, 
			GetParserPosition().uCurrentRow, 
			GetParserPosition().uCurrentCol, 
			"No function \"%s\" is defined that takes %u parameters.", 
			GetFunctionName().c_str(),
			uParamCount
		);

		return false;
	}

	//Try for an exact match
	if( apPrototypes.size() > 1 )
	{
		//It's entirely possible that we could eliminate all possibilities here, so we need to be able to backtrack
		apPrototypesTemp = apPrototypes;

		for( auto& protoIter = apPrototypes.begin(); protoIter != apPrototypes.end(); )
		{
			auto& prototype = (*protoIter);
			auto& prototypeParams = prototype->GetParameters();
			int paramCount = prototypeParams.size();
			int callParamCount = m_apParameters.size();

			bool passedTest = true;

			int commonParamCount = min( paramCount, callParamCount );
			for( int paramIndex = 0; paramIndex < commonParamCount; paramIndex++ )
			{
				auto& callParam = m_apParameters[ paramIndex ];
				auto& callParamType = m_apParameters[ paramIndex ]->GetType();

				auto& prototypeParam = prototypeParams[ paramIndex ];
				auto& prototypeParamType = prototypeParam->GetType();

				if( callParamType.CompareTo( prototypeParamType ) )
				{

				}
				else
				{
					protoIter = apPrototypes.erase( protoIter );
					passedTest = false;
					break;
				}
			}

			if( passedTest )
			{
				++protoIter;
			}
		}

		if( apPrototypes.empty() )
		{
			apPrototypes = apPrototypesTemp;
		}
	}
	
	for( int attempt = 0; attempt < 2; attempt++ )
	{
		apPrototypesTemp = apPrototypes;

		//First try a strict coercion, then a weak coercion
		for( auto& protoIter = apPrototypes.begin(); protoIter != apPrototypes.end(); )
		{
			auto& prototype = (*protoIter);
			auto& prototypeParams = prototype->GetParameters();
			int paramCount = prototypeParams.size();
			int callParamCount = m_apParameters.size();

			bool passedTest = true;

			int commonParamCount = min( paramCount, callParamCount );
			for( int paramIndex = 0; paramIndex < commonParamCount; paramIndex++ )
			{
				auto& callParam = m_apParameters[ paramIndex ];
				auto& callParamType = m_apParameters[ paramIndex ]->GetType();

				auto& prototypeParam = prototypeParams[ paramIndex ];
				auto& prototypeParamType = prototypeParam->GetType();

				if( CanCoerceType( callParamType, prototypeParamType, attempt == 0 ) )
				{

				}
				else
				{
					protoIter = apPrototypes.erase( protoIter );
					passedTest = false;
					break;
				}
			}

			if( passedTest )
			{
				++protoIter;
			}
		}

		if( apPrototypes.empty() )
		{
			apPrototypes = apPrototypesTemp;
		}
		else
		{
			break;
		}
	}

	if( apPrototypes.empty() )
	{
		Error_Compiler( 
			EError_Error, 
			GetParserPosition().pszFilename, 
			GetParserPosition().uCurrentRow, 
			GetParserPosition().uCurrentCol, 
			"Function \"%s\" is defined but no conversion exists based on the values passed in.", 
			GetFunctionName().c_str(),
			uParamCount
		);

		return false;
	}
	else
	{
		m_pPrototype = apPrototypes[0];

		if( m_pPrototype->IsIntrinsic() )
		{
			m_bIntrinsic = true;
		}

		return true;
	}
}

bool CASTExpressionFunctionCall::FindMatchingFunctionBody( CScope* pScope )
{
	std::vector<CASTFunction*> apFunctions;
	pScope->FindFunctions( m_tName, apFunctions );
	
	if( apFunctions.empty() )
	{
		//Intrinsics do not have a body normally, unless it has been overriden
		if( !m_bIntrinsic )
		{
			Error_Compiler( 
				EError_Error, 
				GetParserPosition().pszFilename, 
				GetParserPosition().uCurrentRow, 
				GetParserPosition().uCurrentCol, 
				"Function \"%s\" is not defined.", 
				GetFunctionName().c_str()
			);
		}

		return false;
	}

	size_t uParamCount = m_apParameters.size();

	for( auto& protoIter = apFunctions.begin(); protoIter != apFunctions.end(); )
	{
		if( (*protoIter)->GetPrototype()->CanFitParameterCount( uParamCount ) )
		{
			++protoIter;
		}
		else
		{
			protoIter = apFunctions.erase( protoIter );
		}
	}

	if( apFunctions.empty() )
	{
		//Intrinsics do not have a body normally, unless it has been overriden
		if( !m_bIntrinsic )
		{
			Error_Compiler( 
				EError_Error, 
				GetParserPosition().pszFilename, 
				GetParserPosition().uCurrentRow, 
				GetParserPosition().uCurrentCol, 
				"No function \"%s\" is defined that takes %u parameters.", 
				GetFunctionName().c_str(),
				uParamCount
			);
		}

		return false;
	}

	for( auto& protoIter = apFunctions.begin(); protoIter != apFunctions.end(); )
	{
		auto& prototype = (*protoIter);
		auto& prototypeParams = prototype->GetPrototype()->GetParameters();
		int paramCount = prototypeParams.size();
		auto& callParameters = m_pPrototype->GetParameters();
		int callParamCount = callParameters.size();

		bool passedTest = true;

		if( !prototype->GetPrototype()->GetReturnType().CompareTo( m_pPrototype->GetReturnType() ) )
		{
			protoIter = apFunctions.erase( protoIter );
			passedTest = false;
		}
		else
		{
			int commonParamCount = min( paramCount, callParamCount );
			for( int paramIndex = 0; paramIndex < commonParamCount; paramIndex++ )
			{
				auto& callParam = callParameters[ paramIndex ];
				auto& callParamType = callParameters[ paramIndex ]->GetType();

				auto& prototypeParam = prototypeParams[ paramIndex ];
				auto& prototypeParamType = prototypeParam->GetType();

				if( callParamType.CompareTo( prototypeParamType ) )
				{

				}
				else
				{
					protoIter = apFunctions.erase( protoIter );
					passedTest = false;
					break;
				}
			}
		}

		if( passedTest )
		{
			++protoIter;
		}
	}

	if( apFunctions.size() == 1 )
	{
		m_pFunctionBody = apFunctions[0];
		return true;
	}
	else if( apFunctions.size() > 1 )
	{
		Error_Compiler( 
			EError_Error, 
			GetParserPosition().pszFilename, 
			GetParserPosition().uCurrentRow, 
			GetParserPosition().uCurrentCol, 
			"Function \"%s\" is multiply defined.", 
			GetFunctionName().c_str()
		);

		m_pFunctionBody = apFunctions[0];
		return true;
	}

	//Intrinsics do not have a body normally, unless it has been overriden
	if( !m_bIntrinsic )
	{
		Error_Compiler( 
				EError_Error, 
				GetParserPosition().pszFilename, 
				GetParserPosition().uCurrentRow, 
				GetParserPosition().uCurrentCol, 
				"Function \"%s\" is declared but not defined.", 
				GetFunctionName().c_str()
			);
	}

	return false;
}