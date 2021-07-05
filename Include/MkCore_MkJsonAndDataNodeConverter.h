#pragma once


//------------------------------------------------------------------------------------------------//
// json -> data node
//
// jsoncpp±â¹Ý
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"
#include "json/json.h"


class MkPathName;
class MkHashStr;
class MkDataNode;

class MkJsonAndDataNodeConverter
{
public:

	// json -> data node
	bool Convert(const MkByteArray& source, MkDataNode& destination);
	bool Convert(const MkPathName& filePath, MkDataNode& destination);

	// data node -> json
	bool Convert(const MkDataNode& node, const MkPathName& filePath);

	void Clear(void);

	~MkJsonAndDataNodeConverter() { Clear(); }

protected:

	bool _ConvertObject(const Json::Value& rootValue, MkDataNode& rootNode);
	bool _ConvertValue(const MkHashStr& targetKey, const Json::Value& targetValue, MkDataNode& parentNode);

	bool _ConvertNode(const MkDataNode& parentNode, Json::Value& parentValue);

protected:

	MkStr m_TargetFilePath;
};

//------------------------------------------------------------------------------------------------//
