#pragma once

bool IdentifySideEffects( CASTBase* pNode, CASTScope* pScope );
void Optimise( CASTBase* pNode, CASTScope* pScope, CASTBase* pParent, std::vector< std::pair< CASTExpression*, CASTExpression* > >& toReplace );
