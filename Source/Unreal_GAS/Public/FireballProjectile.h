// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "FireballProjectile.generated.h"

class USphereComponent;

/**
 * AFireballProjectile은 월드 안을 날아가는 Fireball 발사체 Actor이다.
 * CollisionComponent가 벽이나 캐릭터와의 충돌을 검사하고, ProjectileMovementComponent가 앞으로 이동시킨다.
 */
UCLASS()
class UNREAL_GAS_API AFireballProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AFireballProjectile();

	// CollisionComponent는 Sphere 충돌체를 생성하고 충돌 반지름을 설정한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	// ProjectileMovementComponent는 Projectile의 이동을 담당한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComponent;

protected:
	virtual void BeginPlay() override;

};
