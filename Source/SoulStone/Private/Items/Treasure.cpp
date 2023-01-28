// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Characters/SoulStoneCharacter.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASoulStoneCharacter* SoulStoneCharacter = Cast<ASoulStoneCharacter>(OtherActor);
	if (SoulStoneCharacter) 
	{
		SpawnPickupSound();
		Destroy();
	}
}
