// Fill out your copyright notice in the Description page of Project Settings.


#include "FireballProjectile.h"

AFireballProjectile::AFireballProjectile()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));		// 지정한 타입의 기본 서브오브젝트를 생성한다.
	CollisionComponent->InitSphereRadius(20.0f);													// InitSphereRadius()는 Sphere 충돌 반지름을 설정한다.


	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);							// 충돌 활성화 방식을 설정한다.
	CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);				// 이 충돌체가 속할 Object Channel을 설정한다.
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);			// 모든 충돌 채널에 대한 반응을 한번에 설정한다.
	
	// 특정 충돌 채널에 대한 반응을 설정한다.
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);	
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);			

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));	// 지정한 타입의 기본 서브오브젝트를 생성한다.
	ProjectileMovementComponent->UpdatedComponent = CollisionComponent;															// 위치를 갱신할 대상 컴포넌트를 지정한다.
	
	ProjectileMovementComponent->InitialSpeed = 1000.0f;				// 발사체의 초기 속력을 설정한다.
	ProjectileMovementComponent->MaxSpeed = 1000.0f;					// 발사체의 최대 속력을 설정한다.
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;			// 발사체에 적용할 중력 비율을 설정한다.
	ProjectileMovementComponent->bRotationFollowsVelocity = true;		// Actor의 회전이 현재 이동 방향을 따르도록 설정한다.
	ProjectileMovementComponent->bShouldBounce = false;					// Block 충돌 후 튀어 오를지 여부를 설정한다.
	
	InitialLifeSpan = 5.0f;												// Actor가 자동으로 파괴될 때까지의 시간을 설정한다.

	SetRootComponent(CollisionComponent);

	PrimaryActorTick.bCanEverTick = false;
}

void AFireballProjectile::BeginPlay()
{
	Super::BeginPlay();

	AActor* OwnerActor = GetOwner();			// 이 Actor의 Owner로 지정된 Actor를 반환한다.
	if (OwnerActor != nullptr)					// Owner 포인터가 유효한지 검사한다.
	{
		CollisionComponent->IgnoreActorWhenMoving(OwnerActor, true);	// 이동 충돌 검사에서 지정한 Actor를 무시할지 설정한다.
	}
}

