// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SoulStoneHUD.generated.h"

class USoulStoneOverlay;

UCLASS()
class SOULSTONE_API ASoulStoneHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = SoulStone)
	TSubclassOf<USoulStoneOverlay> SoulStoneOverlayClass;

	UPROPERTY()
	USoulStoneOverlay* SoulStoneOverlay;

public:
	FORCEINLINE USoulStoneOverlay* GetSoulStoneOverlay() const { return SoulStoneOverlay; }
};
