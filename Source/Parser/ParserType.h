#if !defined( SHADR_PARSER_TYPE_H )
#define SHADR_PARSER_TYPE_H

#include "Type.h"

SSemantic* ParseSemantic( SParseContext& rtContext, CType* pType );

CType* ParseTypeDefinition( SParseContext& rtContext, CASTAnnotationGroup* pAnnotation, CScope* pParentScope );

CType* ParseType( SParseContext& rtContext );

#endif //SHADR_PARSER_TYPE_H
