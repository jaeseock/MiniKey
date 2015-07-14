#pragma once


//------------------------------------------------------------------------------------------------//
// shader controller
// - object�� control interface
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkBaseTexture.h"
#include "MkPA_MkShaderEffectAssignPack.h"


class MkShaderController
{
public:

	// shader effect ����
	bool SetShaderEffect(const MkHashStr& name);

	// �������� shader effect name ��ȯ(������ MkHashStr::EMPTY ��ȯ)
	const MkHashStr& GetShaderEffectName(void) const;

	// ����
	void Clear(void);

	// ���� ��ȯ
	inline MkShaderEffectAssignPack& GetAssignPack(void) { return m_AssignPack; }
	inline const MkShaderEffectAssignPack& GetAssignPack(void) const { return m_AssignPack; }

	//------------------------------------------------------------------------------------------------//
	// shader texture
	//------------------------------------------------------------------------------------------------//

	inline void SetShaderTexture0(MkBaseTexture* tex) { m_ShaderTexture0 = tex; m_AssignPack.SetShaderTexture0(tex); }
	inline const MkBaseTexture* GetShaderTexture0(void) const { return m_ShaderTexture0; }

	inline void SetShaderTexture1(MkBaseTexture* tex) { m_ShaderTexture1 = tex; m_AssignPack.SetShaderTexture1(tex); }
	inline const MkBaseTexture* GetShaderTexture1(void) const { return m_ShaderTexture1; }

	inline void SetShaderTexture2(MkBaseTexture* tex) { m_ShaderTexture2 = tex; m_AssignPack.SetShaderTexture2(tex); }
	inline const MkBaseTexture* GetShaderTexture2(void) const { return m_ShaderTexture2; }

	//------------------------------------------------------------------------------------------------//
	// draw
	//------------------------------------------------------------------------------------------------//

	unsigned int BeginTechnique(void);
	void BeginPass(unsigned int index);
	void EndPass(void);
	void EndTechnique(void);

	MkShaderController();
	~MkShaderController() { Clear(); }

protected:

	MkShaderEffect* m_ShaderEffect;

	MkShaderEffectAssignPack m_AssignPack;

	MkBaseTexturePtr m_ShaderTexture0;
	MkBaseTexturePtr m_ShaderTexture1;
	MkBaseTexturePtr m_ShaderTexture2;
};
