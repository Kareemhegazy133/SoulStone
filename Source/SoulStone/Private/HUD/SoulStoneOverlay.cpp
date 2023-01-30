// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SoulStoneOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USoulStoneOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void USoulStoneOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void USoulStoneOverlay::SetGold(int32 Gold)
{
	if (GoldText)
	{
		const FString String = FString::Printf(TEXT("%d"), Gold);
		const FText Text = FText::FromString(String);
		GoldText->SetText(Text);
	}
}

void USoulStoneOverlay::SetSouls(int32 Souls)
{
	if (SoulsText)
	{
		const FString String = FString::Printf(TEXT("%d"), Souls);
		const FText Text = FText::FromString(String);
		SoulsText->SetText(Text);
	}
}

void USoulStoneOverlay::SetLevel(int32 Level)
{
	if (LevelText)
	{
		const FString String = FString::Printf(TEXT("%d"), Level);
		const FText Text = FText::FromString(String);
		LevelText->SetText(Text);
	}
}
