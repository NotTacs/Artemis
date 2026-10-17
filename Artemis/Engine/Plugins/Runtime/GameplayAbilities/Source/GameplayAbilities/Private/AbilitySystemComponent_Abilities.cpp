#include "pch.h"
#include "../Public/AbilitySystemComponent.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

void AbilitySystemComponent::InternalServerTryActivateAbility(UAbilitySystemComponent* __this, FGameplayAbilitySpecHandle Handle, bool InputPressed, const FPredictionKey& PredictionKey, const FGameplayEventData* TriggerEventData)
{
	FGameplayAbilitySpec* Spec = __this->FindAbilitySpecFromHandle(Handle);
	if (!Spec)
	{
		// Can potentially happen in race conditions where client tries to activate ability that is removed server side before it is received.
		//ABILITY_LOG(Display, TEXT("InternalServerTryActivateAbility. Rejecting ClientActivation of ability with invalid SpecHandle!"));
		__this->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
		return;
	}

	const UGameplayAbility* AbilityToActivate = Spec->Ability;

	if (!AbilityToActivate)
	{
		//ABILITY_LOG(Error, TEXT("InternalServerTryActiveAbility. Rejecting ClientActivation of unconfigured spec ability!"));
		__this->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
		return;
	}

	// Ignore a client trying to activate an ability requiring server execution
	if (AbilityToActivate->NetSecurityPolicy == EGameplayAbilityNetSecurityPolicy::ServerOnlyExecution ||
		AbilityToActivate->NetSecurityPolicy == EGameplayAbilityNetSecurityPolicy::ServerOnly)
	{
		//ABILITY_LOG(Display, TEXT("InternalServerTryActiveAbility. Rejecting ClientActivation of %s due to security policy violation."), *GetNameSafe(AbilityToActivate));
		__this->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
		return;
	}

	// Consume any pending target info, to clear out cancels from old executions
	//ConsumeAllReplicatedData(Handle, PredictionKey);

	//FScopedPredictionWindow ScopedPredictionWindow(this, PredictionKey);

	//ensure(AbilityActorInfo.IsValid());

	UGameplayAbility* InstancedAbility = nullptr;
	Spec->InputPressed = true;

	// Attempt to activate the ability (server side) and tell the client if it succeeded or failed.
	if (__this->InternalTryActivateAbility(Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
	{
		// TryActivateAbility handles notifying the client of success
	}
	else
	{
		//ABILITY_LOG(Display, TEXT("InternalServerTryActivateAbility. Rejecting ClientActivation of %s. InternalTryActivateAbility failed: %s"), *GetNameSafe(Spec->Ability), *InternalTryActivateAbilityFailureTags.ToStringSimple());
		__this->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
		Spec->InputPressed = false;

		//MarkAbilitySpecDirty(*Spec);
	}
}
void AbilitySystemComponent::LoadModule()
{
	FMemoryLibrary::VFTHook(UFortAbilitySystemComponentAthena::GetDefaultObj()->VTable, 0x108, InternalServerTryActivateAbility);
}