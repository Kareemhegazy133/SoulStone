// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SoulStoneCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AWeapon::AWeapon() 
{
	WeaponHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon HitBox"));
	WeaponHitBox->SetupAttachment(GetRootComponent());
	WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponHitBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponHitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	HitBoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Hit Box Trace Start"));
	HitBoxTraceStart->SetupAttachment(GetRootComponent());

	HitBoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Hit Box Trace End"));
	HitBoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay() 
{
	Super::BeginPlay();

	WeaponHitBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnHitBoxOverlap);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
	AttachMeshToSocket(InParent, InSocketName);
	ItemState = EItemState::EIS_Equipped;
	if (EquipSound) 
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}

	if (Sphere) {
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}

void AWeapon::OnHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector Start = HitBoxTraceStart->GetComponentLocation();
	const FVector End = HitBoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FHitResult TraceHits;

	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, FVector(5.f, 5.f, 5.f),
		HitBoxTraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, 
		false, ActorsToIgnore, EDrawDebugTrace::ForDuration, TraceHits, true);
}
