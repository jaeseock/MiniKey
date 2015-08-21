
#include "MkCore_MkCheck.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkTextNode.h"
#include "MkPA_MkDrawTextNodeStep.h"


//------------------------------------------------------------------------------------------------//

void MkDrawTextNodeStep::SetUp(const MkTextNode* textNode)
{
	m_TextNode = textNode;
	if (m_TextNode != NULL)
	{
		_SetAlphaKeyArguments(m_TextNode->GetWholePixelSize(), MKDEF_PA_TEXTNODE_COLOR_KEY);
	}

	if (m_TargetTexture != NULL)
	{
		MK_CHECK(m_TargetTexture.GetReferenceCounter() == 1, L"MkDrawTextNodeStep의 texture에 참조가 남아있어 파괴 후 재생성 불가")
			return;

		m_TargetTexture = NULL; // 파괴 후 재생성
	}
	
	_CreateTexture();

	m_GenerateTexture = true;
}

void MkDrawTextNodeStep::Clear(void)
{
	m_TextNode = NULL;
	MkDrawToStaticTextureStep::Clear();
}

MkDrawTextNodeStep::MkDrawTextNodeStep() : MkDrawToStaticTextureStep()
{
	m_TextNode = NULL;
}

bool MkDrawTextNodeStep::_DrawContents(void)
{
	bool ok = (m_TextNode != NULL);
	if (ok)
	{
		m_TextNode->__Draw();
	}
	return ok;
}

//------------------------------------------------------------------------------------------------//
