// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TPSWeapon.h"
#include "Engine/World.h"
#include "TPSWeapon.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"

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

}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	//GetMovementComponent()->can

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

	averageDeltaTime.Init(0, maxArrayElements);
}

void ATPSCharacter::MoveForward(float value)
{
	//UE_LOG(LogTemp, Warning, TEXT("before move  %s"), *GetActorLocation().ToString());

	AddMovementInput(GetActorForwardVector() * value);

	//updateSocketPositions();

	//UE_LOG(LogTemp, Warning, TEXT("after move  %s"), *GetActorLocation().ToString());
}

void ATPSCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector() * value);

	//updateSocketPositions();

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


	if (currentWeapon)
	{
		currentWeapon->BeginFire();	
	}
}

void ATPSCharacter::EndFire()
{
	if (currentWeapon)
	{
		currentWeapon->EndFire();
	}
}

void ATPSCharacter::updateSocketPositions()
{
	FVector speedOffset = GetVelocity() * currentAverageDeltaTime;

	if (currentWeapon)
	{
		

		if (bIsCrouched)
		{
			firstHandSocketLocation = GetMesh()->GetSocketLocation("crouchTargetWeaponLocation") + speedOffset;
		}
		else
		{
			firstHandSocketLocation = GetMesh()->GetSocketLocation("targetWeaponLocation") + speedOffset;
		}

		rightElbowPlacementLocation = GetMesh()->GetSocketLocation("rightElbowPlacement");

		auto weaponMeshComp = currentWeapon->GetSkeletalMeshComponent();

		secondHandSocketLocation = weaponMeshComp->GetSocketLocation("secondHand") + speedOffset;
		leftElbowPlacementLocation = GetMesh()->GetSocketLocation("leftElbowPlacement");

		//GetVelocity()
		
		
		currentWeapon->CheckDefaultSubobjects();
		currentWeapon->CheckDefaultSubobjects();
		currentWeapon->CheckDefaultSubobjects();
		//currentWeapon->CheckForErrors();
		//currentWeapon->CheckForErrors();

		//UE_LOG(LogTemp, Warning, TEXT("Velocity %s"),*(GetVelocity() * currentAverageDeltaTime).ToString());
		
		UE_LOG(LogTemp, Warning, TEXT("currentAverageDeltaTime %f"), currentAverageDeltaTime);
		UE_LOG(LogTemp, Warning, TEXT("a %f"), currentAverageDeltaTime);
		UE_LOG(LogTemp, Warning, TEXT("b %f"), currentAverageDeltaTime);
		UE_LOG(LogTemp, Warning, TEXT("c %f"), currentAverageDeltaTime);
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("No weapon"));
	}
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
	//storedDeltaTime = DeltaTime;
	//-------------------------- set current character field of view ------------------------------------------//

	float TargetFOV = bIsZooming ? zoomedFOV : defaultFOV;

	float newFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComponent->SetFieldOfView(newFOV);

	//--------------------------- 
	
	averageDeltaTime.Add(DeltaTime);

	if (averageDeltaTime.Num() > maxArrayElements)
	{
		averageDeltaTime.RemoveAt(0);
	}

	currentAverageDeltaTime = 0;

	for (float deltaTick : averageDeltaTime)
	{
		currentAverageDeltaTime += deltaTick;
	}

	currentAverageDeltaTime /= maxArrayElements;


	
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
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATPSCharacter::EndFire);

}

FVector ATPSCharacter::GetPawnViewLocation() const
{
	if (CameraComponent)
	{
		return CameraComponent->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}



