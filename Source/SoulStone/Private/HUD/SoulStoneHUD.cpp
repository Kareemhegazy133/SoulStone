// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SoulStoneHUD.h"
#include "HUD/SoulStoneOverlay.h"

void ASoulStoneHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && SoulStoneOverlayClass)
		{
			SoulStoneOverlay = CreateWidget<USoulStoneOverlay>(Controller, SoulStoneOverlayClass);
			SoulStoneOverlay->AddToViewport();
		}
	}


}
