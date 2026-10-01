// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHealthBarWidget.h"
#include "Components/ProgressBar.h"


void UEnemyHealthBarWidget::SetHealthPercent(float HealthPercent)
{
    if (HealthProgressBar)
    {
        HealthProgressBar->SetPercent(FMath::Clamp(HealthPercent, 0.0f, 1.0f));
    }
}