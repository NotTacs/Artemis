#pragma once
#include "CoreMinimal.h"

class AbilitySystemComponent
{
public:
	static void InternalServerTryActivateAbility(UAbilitySystemComponent* __this, FGameplayAbilitySpecHandle AbilityToActivate, bool InputPressed, const FPredictionKey& PredictionKey, const FGameplayEventData* TriggerEventData);
	static void LoadModule();
};