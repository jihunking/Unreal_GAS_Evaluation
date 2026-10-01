// Copyright Epic Games, Inc. All Rights Reserved.


#include "Unreal_GASPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "AbilitySystemInterface.h"
#include "PlayerHUDWidget.h"
#include "Blueprint/UserWidget.h"
#include "Unreal_GAS.h"
#include "Widgets/Input/SVirtualJoystick.h"

void AUnreal_GASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 로컬 플레이어에게만 HUD를 생성해 각 플레이어 화면에 한 번만 표시한다.
	if (IsLocalPlayerController() && PlayerHUDWidgetClass)
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(this, PlayerHUDWidgetClass);

		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToPlayerScreen();

			// 현재 조종 중인 Pawn의 ASC를 HUD에 전달해 Health와 Mana 변경을 감지하게 한다.
			IAbilitySystemInterface* AbilitySystemInterface =
				Cast<IAbilitySystemInterface>(GetPawn());

			if (AbilitySystemInterface)
			{
				PlayerHUDWidget->InitializeWithAbilitySystem(
					AbilitySystemInterface->GetAbilitySystemComponent());
			}
		}
	}

	// only spawn touch controls on local player controllers
	if (IsLocalPlayerController() && ShouldUseTouchControls())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogUnreal_GAS, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void AUnreal_GASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool AUnreal_GASPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
