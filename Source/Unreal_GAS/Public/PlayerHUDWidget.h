// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UAbilitySystemComponent;
class UProgressBar;
class UTextBlock;

struct FOnAttributeChangeData;
/**
 * 
 */
UCLASS()
class UNREAL_GAS_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 표시할 플레이어의 ASC를 전달받고 Attribute 변경 이벤트를 등록한다.
    void InitializeWithAbilitySystem(UAbilitySystemComponent* InAbilitySystemComponent);

protected:
    // 위젯이 제거될 때 ASC에 등록한 Delegate를 해제한다.
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> HealthText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ManaText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthProgressBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> ManaProgressBar;

private:
    void HandleHealthChanged(const FOnAttributeChangeData& Data);
    void HandleManaChanged(const FOnAttributeChangeData& Data);

    void UpdateHealth();
    void UpdateMana();
    void UnbindFromAbilitySystem();

    // HUD가 ASC의 생명주기를 소유하지 않으므로 약한 포인터로 참조한다.
    TWeakObjectPtr<UAbilitySystemComponent> BoundAbilitySystemComponent;

    FDelegateHandle HealthChangedDelegateHandle;
    FDelegateHandle ManaChangedDelegateHandle;
};
