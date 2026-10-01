// Copyright Epic Games, Inc. All Rights Reserved.

#include "Unreal_GASCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Unreal_GAS.h"

#include "AbilitySystemComponent.h"
#include "PlayerAttributeSet.h"

AUnreal_GASCharacter::AUnreal_GASCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));	// 지정한 타입의 기본 서브오브젝트를 생성한다.
	PlayerAttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("Stat"));		// 지정한 타입의 기본 서브오브젝트를 생성한다.


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AUnreal_GASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUnreal_GASCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AUnreal_GASCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUnreal_GASCharacter::Look);
		
		if (FireballAction)
		{
			EnhancedInputComponent->BindAction(FireballAction, ETriggerEvent::Started, this, &AUnreal_GASCharacter::ActivateFireball);
		}
	}
	else
	{
		UE_LOG(LogUnreal_GAS, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AUnreal_GASCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AUnreal_GASCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AUnreal_GASCharacter::ActivateFireball()
{
	if (AbilitySystemComponent && FireballAbilityClass)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(FireballAbilityClass);
	}
}

void AUnreal_GASCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AUnreal_GASCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AUnreal_GASCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AUnreal_GASCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

UAbilitySystemComponent* AUnreal_GASCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;	// 캐릭터가 소유한 ASC 포인터를 반환한다.
}

void AUnreal_GASCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ASC 포인터가 유효한지 검사한다.
	if (AbilitySystemComponent != nullptr)	
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);	// 첫 번째 인자를 OwnerActor, 두 번째 인자를 AvatarActor로 ASC에 등록한다.

		// 서버 권한이 있고, FireballAbilityClass가 유효한지 검사한다.
		if (HasAuthority() && FireballAbilityClass)
		{
			FGameplayAbilitySpec AbilitySpec(FireballAbilityClass, 1);		// FGameplayAbilitySpec을 생성하고
			AbilitySystemComponent->GiveAbility(AbilitySpec);				// ASC에 어빌리티를 부여한다.
		}
	}
}
