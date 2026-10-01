// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FireballGameplayAbility.generated.h"

class AFireballProjectile;

/**
 * UFireballGameplayAbility는 Fireball 사용 조건을 검사하고 캐릭터 앞에 발사체를 생성하는 어빌리티이다.
 * GAS의 Cost를 통해 Mana를 소모하고, Cooldown을 통해 재사용 대기 시간을 적용한다.
 */
UCLASS()
class UNREAL_GAS_API UFireballGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFireballGameplayAbility();

protected:
	// 생성할 발사체 클래스를 저장하며 AFireballProjectile 파생 클래스만 지정할 수 있다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fireball")
	TSubclassOf<AFireballProjectile> ProjectileClass;	

	// GAS가 어빌리티를 활성화할 때 호출하는 함수다.
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;	

};
