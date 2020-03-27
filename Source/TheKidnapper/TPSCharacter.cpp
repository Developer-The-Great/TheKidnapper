// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TPSWeapon.h"
#include "Engine/World.h"
#include "TPSWeapon.h"

// Sets default values
ATPSCharacter::ATPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("USpringComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	//
	
}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{

	Super::BeginPlay();
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	defaultFOV = CameraComponent->FieldOfView;

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	currentWeapon = GetWorld()->SpawnActor<ATPSWeapon>(starterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, params);

	if (currentWeapon)
	{
		currentWeapon->SetOwner(this);
		UE_LOG(LogTemp, Warning, TEXT("Current weapon"))
		currentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponSocket");
	}
}

void ATPSCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector() * value);
}

void ATPSCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector() * value);
}

void ATPSCharacter::BeginCrouch()
{
	UE_LOG(LogTemp,Warning,TEXT("BeginCrouch"))
	Crouch();
}

void ATPSCharacter::EndCrouch()
{
	UE_LOG(LogTemp, Warning, TEXT("EndCrouch"))
	UnCrouch();
}

void ATPSCharacter::BeginFire()
{
	UE_LOG(LogTemp, Warning, TEXT("Begin Fire"))

	if (currentWeapon)
	{

		UE_LOG(LogTemp, Warning, TEXT("Fire"))
		currentWeapon->Fire();
	}
}

void ATPSCharacter::EndFire()
{
}

void ATPSCharacter::BeginZoom()
{
	bIsZooming = true;
}

void ATPSCharacter::EndZoom()
{
	bIsZooming = false;
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	float TargetFOV = bIsZooming ? zoomedFOV : defaultFOV;

	float newFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComponent->SetFieldOfView(newFOV);
}

// Called to bind functionality to input
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ATPSCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ATPSCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATPSCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ATPSCharacter::EndCrouch);


	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ATPSCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ATPSCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATPSCharacter::BeginFire);


}

FVector ATPSCharacter::GetPawnViewLocation() const
{
	if (CameraComponent)
	{
		return CameraComponent->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}



