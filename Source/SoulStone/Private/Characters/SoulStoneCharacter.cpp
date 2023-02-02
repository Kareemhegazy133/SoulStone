// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SoulStoneCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GroomComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Animation/AnimMontage.h"
#include "HUD/SoulStoneHUD.h"
#include "HUD/SoulStoneOverlay.h"


// Sets default values
ASoulStoneCharacter::ASoulStoneCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}

void ASoulStoneCharacter::Tick(float DeltaTime)
{
	if (Attributes && SoulStoneOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		SoulStoneOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

// Called to bind functionality to input
void ASoulStoneCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASoulStoneCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASoulStoneCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASoulStoneCharacter::Jump);
		EnhancedInputComponent->BindAction(FKeyAction, ETriggerEvent::Triggered, this, &ASoulStoneCharacter::FKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASoulStoneCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASoulStoneCharacter::Dodge);
	}
}

void ASoulStoneCharacter::Jump()
{
	if (IsUnoccupied()) Super::Jump();
}

float ASoulStoneCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void ASoulStoneCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

void ASoulStoneCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASoulStoneCharacter::AddSouls(ASoul* Soul)
{
	UE_LOG(LogTemp, Warning, TEXT("ASoulStoneCharacter::AddSouls"));
	if (Attributes && SoulStoneOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SoulStoneOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ASoulStoneCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SoulStoneOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SoulStoneOverlay->SetGold(Attributes->GetGold());
	}
}

// Called when the game starts or when spawned
void ASoulStoneCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("EngageableTarget"));

	InitializeSoulStoneOverlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(SoulStoneMappingContext, 0);
		}
	}
}

void ASoulStoneCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ASoulStoneCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	if (GetController()) {
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void ASoulStoneCharacter::FKeyPressed() 
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon) 
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else 
	{
		if (CanDisarm()) {
			Disarm();
		}

		else if (CanArm()) {
			Arm();
		}
	}
}

void ASoulStoneCharacter::Attack()
{
	Super::Attack();

	if (CanAttack()) {
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
	
}

void ASoulStoneCharacter::Dodge()
{
	if (IsOccupied() || !HasEnoughStamina()) return;
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if (Attributes && SoulStoneOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		SoulStoneOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASoulStoneCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

void ASoulStoneCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASoulStoneCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	ActionState = EActionState::EAS_Unoccupied;
}

bool ASoulStoneCharacter::CanAttack() 
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASoulStoneCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied && 
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASoulStoneCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

void ASoulStoneCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASoulStoneCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASoulStoneCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASoulStoneCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ASoulStoneCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage) 
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASoulStoneCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

bool ASoulStoneCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

bool ASoulStoneCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}
void ASoulStoneCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASoulStoneCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASoulStoneCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ASoulStoneCharacter::InitializeSoulStoneOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		ASoulStoneHUD* SoulStoneHUD = Cast<ASoulStoneHUD>(PlayerController->GetHUD());
		if (SoulStoneHUD)
		{
			SoulStoneOverlay = SoulStoneHUD->GetSoulStoneOverlay();
			if (SoulStoneOverlay && Attributes)
			{
				SoulStoneOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SoulStoneOverlay->SetStaminaBarPercent(1.f);
				SoulStoneOverlay->SetGold(0);
				SoulStoneOverlay->SetSouls(0);
				SoulStoneOverlay->SetLevel(1);
			}
		}
	}
}

void ASoulStoneCharacter::SetHUDHealth()
{
	if (SoulStoneOverlay && Attributes)
	{
		SoulStoneOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}
