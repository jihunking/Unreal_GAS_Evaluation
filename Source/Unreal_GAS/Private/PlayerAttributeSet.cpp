// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttributeSet.h"
#include "GameplayEffectExtension.h"


UPlayerAttributeSet::UPlayerAttributeSet()
{
	// 초기값 설정
	InitMaxHealth(100.0f);	
	InitMaxMana(100.0f);
	
	InitHealth(100.0f);
	InitMana(100.0f);
}

// PreAttributeChange : Attribute 값이 실제로 변경되기 직전에 호출
// NewValue는 적용 예정 값이므로, 여기서 수정하면 제한된 값이 실제 Attribute에 적용된다.
void UPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Health는 0보다 작아지거나 MaxHealth보다 커지지 않도록 제한한다.
	if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	// MaxHealth는 Health의 유효 범위를 유지할 수 있도록 최소 1로 제한한다.
	else if(Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}

	// Mana는 0보다 작아지거나 MaxMana보다 커지지 않도록 제한한다.
	else if(Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}

	// MaxMana는 Mana의 유효 범위를 유지할 수 있도록 최소 1로 제한한다.
	else if(Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

// Instant 또는 Periodic GameplayEffect가 Attribute의 Base Value를 변경한 직후 호출된다.
// GameplayEffect 적용 결과를 다시 유효 범위로 보정하여 잘못된 Base Value가 남지 않게 한다.
void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 이번 GameplayEffect가 Health를 변경했다면 실제 Health 값을 정상 범위로 확정한다.
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	
	// 이번 GameplayEffect가 Mana를 변경했다면 실제 Mana 값을 정상 범위로 확정한다.
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	}
}
