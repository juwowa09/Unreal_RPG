// WonUngJu All Rights Reserved


#include "DataAssets/Input/DataAsset_InputConfig.h"


UInputAction* UDataAsset_InputConfig::FindNativeInputActionByTag(const FGameplayTag& InInputTag) const
{
	for (const FWarriorInputActionConfig& InputActionConfig : NativeInputActions)
	{
		// Tag로 Action을 찾고, Action이 존재하면 Action을 return
		if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction)
		{
			return InputActionConfig.InputAction;
		}
	}
	
	return nullptr;
}