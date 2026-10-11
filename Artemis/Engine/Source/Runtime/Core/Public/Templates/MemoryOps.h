#pragma once
#include "CoreMinimal.h"
#include "IsTrivallyDestructible.h"
template <typename ElementType>
FORCEINLINE void DestructItem(ElementType* Element)
{
	if constexpr (!TIsTriviallyDestructible<ElementType>::Value)
	{
		// We need a typedef here because VC won't compile the destructor call below if ElementType itself has a member called ElementType
		typedef ElementType DestructItemsElementTypeTypedef;

		Element->DestructItemsElementTypeTypedef::~DestructItemsElementTypeTypedef();
	}
}