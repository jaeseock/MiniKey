#ifndef __MINIKEY_CORE_MKDATANODELOADINGTARGET_H__
#define __MINIKEY_CORE_MKDATANODELOADINGTARGET_H__

//------------------------------------------------------------------------------------------------//
// MkDataNode를 background loading하기 위한 인터페이스
//
// ex>
//	MkLoadingTargetPtr ptr = new MkDataNodeLoadingTarget;
//	MK_BG_LOADER.RegisterLoadingTarget(ptr, L"datanode_00.txt", MkStr::EMPTY);
//	...
//	void Update(void)
//	{
//		...
//		if (ptr->GetDataState() == MkBaseLoadingTarget::eComplete)
//		{
//			MkDataNodeLoadingTarget* target = dynamic_cast<MkDataNodeLoadingTarget*>(ptr.GetPtr());
//			target->GetDataNode().SaveToText(L"test00.txt");
//		}
//		else if (ptr->GetDataState() == MkBaseLoadingTarget::eError)
//		...
//		ptr = NULL;
//	}
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBaseLoadingTarget.h"
#include "MkCore_MkDataNode.h"

//------------------------------------------------------------------------------------------------//

class MkDataNodeLoadingTarget : public MkBaseLoadingTarget
{
public:

	virtual bool CustomSetUp(const MkByteArray& byteArray, const MkPathName& filePath, const MkStr& argument)
	{
		if (!MkBaseLoadingTarget::CustomSetUp(byteArray, filePath, argument))
			return false;

		return m_DataNode.Load(byteArray);
	}

	inline const MkDataNode& GetDataNode(void) const { return m_DataNode; }

	MkDataNodeLoadingTarget() : MkBaseLoadingTarget() {}
	virtual ~MkDataNodeLoadingTarget() { m_DataNode.Clear(); }

protected:

	MkDataNode m_DataNode;
};

//------------------------------------------------------------------------------------------------//

#endif
