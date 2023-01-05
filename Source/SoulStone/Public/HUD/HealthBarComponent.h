// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class SOULSTONE_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	void SetHealthPercent(float Percent);

private:
	// Makes HealthBarWidget start off as nullptr
	UPROPERTY()
	class UHealthBar* HealthBarWidget;
};
