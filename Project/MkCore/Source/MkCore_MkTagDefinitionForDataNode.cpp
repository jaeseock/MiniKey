
#include "MkCore_MkTagDefinitionForDataNode.h"

//------------------------------------------------------------------------------------------------//

#define MKDEF_NODE_BEGIN_INDEX 100
#define MKDEF_TEMPLATE_BEGIN_INDEX 101
#define MkDEF_PUSHING_TEMPLATE_INDEX 102
#define MKDEF_BLOCK_END_INDEX 103

//------------------------------------------------------------------------------------------------//

const MkStr MkTagDefinitionForDataNode::TagForNode(L"Node");
const int MkTagDefinitionForDataNode::IndexMarkForNodeBegin(MKDEF_NODE_BEGIN_INDEX);

const MkStr MkTagDefinitionForDataNode::TagForTemplate(L"Template");
const int MkTagDefinitionForDataNode::IndexMarkForTemplateBegin(MKDEF_TEMPLATE_BEGIN_INDEX);

const MkStr MkTagDefinitionForDataNode::TagForPushingTemplate(L"PushTemplate");
const int MkTagDefinitionForDataNode::IndexMarkForPushingTemplate(MkDEF_PUSHING_TEMPLATE_INDEX);

const int MkTagDefinitionForDataNode::IndexMarkForNodeBlockEnd(MKDEF_BLOCK_END_INDEX);

const MkStr MkTagDefinitionForDataNode::TagForDQM(L"__#DQM");

const MkStr MkTagDefinitionForDataNode::TagForBlockBegin(L"{");
const MkStr MkTagDefinitionForDataNode::TagForBlockEnd(L"}");
const MkStr MkTagDefinitionForDataNode::TagForApplyTemplate(L":");
const MkStr MkTagDefinitionForDataNode::TagForUnitAssign(L"=");
const MkStr MkTagDefinitionForDataNode::TagForUnitEnd(L";");
const MkStr MkTagDefinitionForDataNode::TagForArrayDivider(L"/");

const unsigned char __BDN_TAG[4] = { '#', 'B', 'D', 'N' };
const MkByteArray MkTagDefinitionForDataNode::TagForBinaryDataNode(static_cast<const unsigned char*>(__BDN_TAG), 4);

//------------------------------------------------------------------------------------------------//
