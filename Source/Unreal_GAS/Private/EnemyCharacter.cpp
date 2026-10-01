// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "EnemyAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "EnemyHealthBarWidget.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	AbilitySystemComponent =CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	EnemyAttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("EnemyStats"));

	HealthBarWidgetComponent =CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	HealthBarWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	if (AbilitySystemComponent && EnemyAttributeSet)
	{
		HealthChangedDelegateHandle =AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UEnemyAttributeSet::GetHealthAttribute()).AddUObject(this,&AEnemyCharacter::HandleHealthChanged);

		MaxHealthChangedDelegateHandle =AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
				UEnemyAttributeSet::GetMaxHealthAttribute()).AddUObject(this,&AEnemyCharacter::HandleMaxHealthChanged);
	}

	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->InitWidget();
	}
	
UpdateHealthBar();

}

void AEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AbilitySystemComponent)
	{
		if (HealthChangedDelegateHandle.IsValid())
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEnemyAttributeSet::GetHealthAttribute()).Remove(HealthChangedDelegateHandle);
		}

		if (MaxHealthChangedDelegateHandle.IsValid())
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEnemyAttributeSet::GetMaxHealthAttribute()).Remove(MaxHealthChangedDelegateHandle);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void AEnemyCharacter::HandleHealthChanged(
	const FOnAttributeChangeData& Data)
{
	UpdateHealthBar();
}

void AEnemyCharacter::HandleMaxHealthChanged(
	const FOnAttributeChangeData& Data)
{
	UpdateHealthBar();
}

void AEnemyCharacter::UpdateHealthBar()
{
	if (!EnemyAttributeSet || !HealthBarWidgetComponent)
	{
		return;
	}

	UEnemyHealthBarWidget* HealthBarWidget = Cast<UEnemyHealthBarWidget>(HealthBarWidgetComponent->GetUserWidgetObject());

	if (!HealthBarWidget)
	{
		return;
	}

	const float MaxHealth = EnemyAttributeSet->GetMaxHealth();
	const float HealthPercent =MaxHealth > 0.0f ? EnemyAttributeSet->GetHealth() / MaxHealth : 0.0f;

	HealthBarWidget->SetHealthPercent(HealthPercent);
}

UAbilitySystemComponent* AEnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
