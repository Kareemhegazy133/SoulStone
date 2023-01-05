// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;

UCLASS()
class SOULSTONE_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	AWeapon();
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);
	TArray<AActor*> IgnoreActors;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void OnHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:
	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "WeaponProperties")
	UBoxComponent* WeaponHitBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* HitBoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* HitBoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	float Damage;

public:
	FORCEINLINE UBoxComponent* GetWeaponHitBox() const { return WeaponHitBox; }
};
