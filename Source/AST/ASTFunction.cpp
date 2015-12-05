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
			"Function \"%s\" is not defined.", 
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

		return true;
	}

	
}