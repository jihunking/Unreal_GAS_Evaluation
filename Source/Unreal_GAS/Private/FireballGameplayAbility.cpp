// Fill out your copyright notice in the Description page of Project Settings.


#include "FireballGameplayAbility.h"
#include "FireballProjectile.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

UFireballGameplayAbility::UFireballGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;		// Actor마다 독립된 어빌리티 인스턴스를 생성하도록 설정한다.
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;		// 어빌리티의 실행 본체를 서버에서만 처리하도록 설정한다.
}

void UFireballGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

	// 어빌리티 실행 주체 정보가 유효한지 검사한다.
	if (!ActorInfo)	
	{
		return;
	}
	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor.Get());		// AvatarActor를 APawn으로 변환하며, 변환할 수 없으면 nullptr을 반환한다.
	
	// 발사 주체와 생성할 클래스가 모두 유효한지 검사한다.
	if (!Pawn || !ProjectileClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);	// 어빌리티 종료를 복제하고 취소된 실행으로 처리한다.
		return;
	}

	UWorld* World = Pawn->GetWorld();	// Pawn이 속한 World를 반환하며 Actor 생성에 사용한다.
	
	if(!World)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);	// World가 없어 Actor를 생성할 수 없으므로 취소 종료한다.
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))	// 비용·쿨타임을 검사하고 적용하며 실패하면 false를 반환한다.
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FVector ForwardVector = Pawn->GetActorForwardVector();													// Actor가 바라보는 앞쪽 방향을 단위 벡터로 반환한다.
	FVector SpawnLocation = Pawn->GetActorLocation() + ForwardVector * 100.0f + FVector::UpVector * 50.0f;	// Actor 위치에 오프셋을 더해 스폰 위치를 계산한다.
	FRotator SpawnRotation = ForwardVector.Rotation();														// 방향 벡터를 Actor 회전값으로 변환한다.

	FActorSpawnParameters SpawnParams;	// Actor 생성에 사용할 추가 설정을 보관하는 구조체다.
	SpawnParams.Owner = Pawn;			// 생성된 Actor의 Owner를 지정한다.
	SpawnParams.Instigator = Pawn;		// 데미지를 발생시킨 Pawn을 지정한다.

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;								// 생성 위치의 충돌 처리 방식을 설정한다.
	AFireballProjectile* SpawnedProjectile = World->SpawnActor<AFireballProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);	// 지정한 클래스·위치·회전·설정으로 Actor를 생성한다.

	// SpawnActor()가 nullptr을 반환했는지 검사한다.
	if (!SpawnedProjectile)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);			// 종료 상태를 복제하고 취소된 어빌리티로 처리한다.
		return;
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);				// 종료 상태를 복제하고 정상 종료된 어빌리티로 처리한다.
}

