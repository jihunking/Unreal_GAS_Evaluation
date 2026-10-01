// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"
#include "AbilitySystemComponent.h"
#include "PlayerAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerHUDWidget::InitializeWithAbilitySystem(
	UAbilitySystemComponent* InAbilitySystemComponent)
{
	// 이미 같은 ASC와 연결되어 있다면 Delegate를 중복 등록하지 않고 표시값만 갱신한다.
	if (BoundAbilitySystemComponent.Get() == InAbilitySystemComponent)
	{
		UpdateHealth();
		UpdateMana();
		return;
	}

	// 다른 ASC와 연결되어 있을 수 있으므로 기존 Delegate를 먼저 해제한다.
	UnbindFromAbilitySystem();
	BoundAbilitySystemComponent = InAbilitySystemComponent;

	if (!InAbilitySystemComponent)
	{
		return;
	}

	// Health가 변경될 때 HandleHealthChanged()가 호출되도록 등록한다.
	HealthChangedDelegateHandle =
		InAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetHealthAttribute()).AddUObject(this, &UPlayerHUDWidget::HandleHealthChanged);

	// Mana가 변경될 때 HandleManaChanged()가 호출되도록 등록한다.
	ManaChangedDelegateHandle =
		InAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetManaAttribute()).AddUObject(this, &UPlayerHUDWidget::HandleManaChanged);

	// Delegate가 호출되기 전에도 초기 Health와 Mana가 표시되도록 즉시 갱신한다.
	UpdateHealth();
	UpdateMana();
}

void UPlayerHUDWidget::NativeDestruct()
{
	// 이 위젯을 더 이상 사용하지 않으면 ASC에 등록한 Delegate를 해제한다.
	UnbindFromAbilitySystem();
	Super::NativeDestruct();
}

void UPlayerHUDWidget::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	UpdateHealth();
}

void UPlayerHUDWidget::HandleManaChanged(const FOnAttributeChangeData& Data)
{
	UpdateMana();
}

void UPlayerHUDWidget::UpdateHealth()
{
	UAbilitySystemComponent* AbilitySystemComponent = BoundAbilitySystemComponent.Get();

	if (!AbilitySystemComponent)
	{
		return;
	}

	const float Health = AbilitySystemComponent->GetNumericAttribute(UPlayerAttributeSet::GetHealthAttribute());
	const float MaxHealth = AbilitySystemComponent->GetNumericAttribute(UPlayerAttributeSet::GetMaxHealthAttribute());

	if (HealthText)
	{
		HealthText->SetText(FText::Format(NSLOCTEXT("PlayerHUDWidget", "HealthFormat", "{0} / {1}"),
			FText::AsNumber(FMath::RoundToInt(Health)),
			FText::AsNumber(FMath::RoundToInt(MaxHealth))));
	}

	if (HealthProgressBar)
	{
		const float HealthPercent = MaxHealth > 0.0f ? FMath::Clamp(Health / MaxHealth, 0.0f, 1.0f) : 0.0f;
		HealthProgressBar->SetPercent(HealthPercent);
	}
}

void UPlayerHUDWidget::UpdateMana()
{
	UAbilitySystemComponent* AbilitySystemComponent = BoundAbilitySystemComponent.Get();

	if (!AbilitySystemComponent)
	{
		return;
	}

	const float Mana = AbilitySystemComponent->GetNumericAttribute(UPlayerAttributeSet::GetManaAttribute());
	const float MaxMana = AbilitySystemComponent->GetNumericAttribute(UPlayerAttributeSet::GetMaxManaAttribute());

	if (ManaText)
	{
		ManaText->SetText(FText::Format(NSLOCTEXT("PlayerHUDWidget", "ManaFormat", "{0} / {1}"),
			FText::AsNumber(FMath::RoundToInt(Mana)),
			FText::AsNumber(FMath::RoundToInt(MaxMana))));
	}

	if (ManaProgressBar)
	{
		const float ManaPercent = MaxMana > 0.0f? FMath::Clamp(Mana / MaxMana, 0.0f, 1.0f) : 0.0f;
		ManaProgressBar->SetPercent(ManaPercent);
	}
}

void UPlayerHUDWidget::UnbindFromAbilitySystem()
{
	UAbilitySystemComponent* AbilitySystemComponent =BoundAbilitySystemComponent.Get();

	if (AbilitySystemComponent)
	{
		if (HealthChangedDelegateHandle.IsValid())
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetHealthAttribute()).Remove(HealthChangedDelegateHandle);
		}

		if (ManaChangedDelegateHandle.IsValid())
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetManaAttribute()).Remove(ManaChangedDelegateHandle);
		}
	}

	HealthChangedDelegateHandle.Reset();
	ManaChangedDelegateHandle.Reset();
	BoundAbilitySystemComponent.Reset();
}

