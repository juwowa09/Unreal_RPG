// WonUngJu All Rights Reserved


#include "AbilitySystem/GEExecCalc/GEExecCalc_DamageTaken.h"
#include "AbilitySystem/WarriorAttributeSet.h"
#include "WarriorGamePlayTags.h"

#include "WarriorDebugHelper.h"

struct FWarriorDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)

	FWarriorDamageCapture()
	{
		// UWarriorAttributeSet 내부의 AttackPower, DefensePower 값을 캡쳐 source, target은 Attempt 할때 필요함
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, AttackPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DefensePower, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DamageTaken, Target, false);
	}
};

static const FWarriorDamageCapture& GetWarriorDamageCapture()
{
	static FWarriorDamageCapture WarriorDamageCapture;
	return WarriorDamageCapture;
}

UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
	/* Slow Way of doing capture */
	// FProperty* AttackPowerProperty = FindFieldChecked<FProperty>(
	// 	UWarriorAttributeSet::StaticClass(),
	// 	GET_MEMBER_NAME_CHECKED(UWarriorAttributeSet, AttackPower)
	// );
	// FGameplayEffectAttributeCaptureDefinition AttackPowerCaptureDefinition(
	// AttackPowerProperty,
	// 	EGameplayEffectAttributeCaptureSource::Source,
	// 	false
	// );
	//
	// RelevantAttributesToCapture.Add(AttackPowerCaptureDefinition);

	// Effect가 Ability 에 의해 생성됐을 당시의 값을 캡쳐
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DefensePowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DamageTakenDef);
}

void UGEExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// ExecutionParam = 모든 Context 정보를 다 담고있음 == 공격자, 피격자, EffectSpec, 캡처된 Attribute 등
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	// EffectSpec.GetContext().GetSourceObject();
	// EffectSpec.GetContext().GetAbility();
	// EffectSpec.GetContext().GetInstigator();
	// EffectSpec.GetContext().GetEffectCauser();

	// Aggregator(집계)된 Source, Target 의 태그들과, Soucre Level, Target Level이 필요하다.
	FAggregatorEvaluateParameters EvaluateParameters;
	// 현재 Effect 의 Source 오브젝트가 가지고있는 모든 Tags Aggregate
	EvaluateParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	// 현재 Effect 의 Target 오브젝트가 가지고있는 모든 Tags Aggregate
	EvaluateParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();
	
	float SourceAttackPower = 0.f;
	// 실제로 캡쳐된 공격력(이 시점에서 완전 snap)과 Soucre Target 태그, Level 을 바탕으로 수식을 SourceAttackPower에 계산해서 저장하는 함수
	// AttackPowerDef 에 정의한 Source, Target 여부에 따라 Params 에서 필요한 것들만 꺼내서 계산
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().AttackPowerDef, EvaluateParameters, SourceAttackPower);
	Debug::Print(TEXT("Source Attack Power"), SourceAttackPower);
	
	float BaseDamage = 0.f;
	int32 UsedLightAttackComboCount = 0;
	int32 UsedHeavyAttackComboCount = 0;

	// SetByCaller 에 저장해둔 값을 모두 가져와서 적용하는 부분
	for (const TPair<FGameplayTag,float>& TagMagnitude : EffectSpec.SetByCallerTagMagnitudes)
	{
		if (TagMagnitude.Key.MatchesTagExact(WarriorGamePlayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage = TagMagnitude.Value;
			Debug::Print(TEXT("BaseDamage"), BaseDamage);
		}
		if (TagMagnitude.Key.MatchesTagExact(WarriorGamePlayTags::Player_SetByCaller_AttackType_Light))
		{
			UsedLightAttackComboCount = TagMagnitude.Value;
			Debug::Print(TEXT("UsedLightAttackComboCount"), UsedLightAttackComboCount);
		}
		if (TagMagnitude.Key.MatchesTagExact(WarriorGamePlayTags::Player_SetByCaller_AttackType_Heavy))
		{
			UsedHeavyAttackComboCount = TagMagnitude.Value;
			Debug::Print(TEXT("UsedHeavyAttackComboCount"), UsedHeavyAttackComboCount);
		}
	}
	
	float TargetDefensePower = 0.f;
	// 실제로 캡쳐된 방어(이 시점에서 완전 snap)과 태그들을 바탕으로 수식을 TargetDefensePower에 계산해서 저장하는 함수
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().DefensePowerDef,EvaluateParameters,TargetDefensePower);
	Debug::Print(TEXT("TargetDefensePower"), TargetDefensePower);

	// 직접 데미지를 커스텀할 수 있음
	if (UsedLightAttackComboCount != 0)
	{
		// 콤보 횟수가 늘어날 수록 더욱 강해짐
		const float DamageIncreasePercentLight = (UsedLightAttackComboCount - 1) * 0.05 + 1.f;

		BaseDamage *= DamageIncreasePercentLight;
		Debug::Print(TEXT("Scaled Base Damage Light"), BaseDamage);
	}
	if (UsedHeavyAttackComboCount != 0)
	{
		const float DamageIncreasePercentHeavy = UsedHeavyAttackComboCount * 0.15f + 1.f;

		BaseDamage *= DamageIncreasePercentHeavy;
		Debug::Print(TEXT("Scaled Base Damage Heavy"), BaseDamage);
	}

	const float FinalDamageDone = BaseDamage * SourceAttackPower / TargetDefensePower;
	Debug::Print(TEXT("Final Damge Done"), FinalDamageDone);

	if (FinalDamageDone > 0.f)
	{
		// DamageTaken 캡쳐값을 덮어씌우고, Modifier 에 등록시켜 ASC가 적용하도록 만들어라.
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				GetWarriorDamageCapture().DamageTakenProperty,
				EGameplayModOp::Override,
				FinalDamageDone
			)
		);
	}

}
