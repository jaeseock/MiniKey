
#include "MkCore_MkStr.h"
#include "MkCore_MkXORer.h"


//------------------------------------------------------------------------------------------------//

void MkXORer::Convert(MkByteArray& targetArray, const MkByteArray& key)
{
	unsigned int targetSize = targetArray.GetSize();
	unsigned int keySize = key.GetSize();
	if ((targetSize > 0) && (keySize > 0))
	{
		unsigned int keyIndex = 0;
		for (unsigned int i=0; i<targetSize; ++i)
		{
			 targetArray[i] ^= key[keyIndex];

			++keyIndex;
			if (keyIndex >= keySize)
			{
				keyIndex = 0;
			}
		}
	}
}

void MkXORer::Convert(MkByteArray& targetArray, const MkStr& key)
{
	if (!key.Empty())
	{
		// wchar_t array -> unsigned char array. key ���� m_Str�� ������ NULL���� ���Ե��� ����
		MkByteArray keyArray(MkMemoryBlockDescriptor<unsigned char>(reinterpret_cast<const unsigned char*>(key.GetPtr()), key.GetSize() * 2));
		Convert(targetArray, keyArray);
	}
}

//------------------------------------------------------------------------------------------------//
