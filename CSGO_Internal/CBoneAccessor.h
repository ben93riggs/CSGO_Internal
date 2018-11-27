#pragma once

class matrix3x4_t;
class C_BaseAnimating;

class CBoneAccessor
{
public:
	matrix3x4_t *GetBoneArrayForWrite()
	{
		return m_pBones;
	}

	void SetBoneArrayForWrite(matrix3x4_t *bone_array)
	{
		m_pBones = bone_array;
	}

	const C_BaseAnimating *m_pAnimating;

	matrix3x4_t *m_pBones;
	int m_ReadableBones;		// Which bones can be read.
	int m_WritableBones;		// Which bones can be written.
};
