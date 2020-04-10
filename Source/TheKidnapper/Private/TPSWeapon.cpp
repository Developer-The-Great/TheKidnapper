// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShake.h"
#include "Particles/ParticleSystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TheKidnapper.h"

static int32 DebugWeaponDrawing = 0;

FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw debug lines for weapons"),
	ECVF_Cheat);
// Sets default values
ATPSWeapon::ATPSWeapon()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";


}

// Called when the game starts or when spawned
void ATPSWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATPSWeapon::Fire()
{
	//We would like to now if the weapon has an owner

	AActor * MyOwner = GetOwner();
	//if there is an owner
	UE_LOG(LogTemp, Warning, TEXT("Fire()"));

	if (MyOwner)
	{
		//get start position 
		

		FVector startLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(startLocation, EyeRotation);

		FVector endLocation = startLocation + EyeRotation.Vector() * 10000;

		FCollisionQueryParams QueryParams;

		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.bReturnPhysicalMaterial = true;
		QueryParams.AddIgnoredActor(this);

		QueryParams.bTraceComplex = true;

		FHitResult HitResult;
		bool bulletHitSomething = GetWorld()->LineTraceSingleByChannel(HitResult, startLocation, endLocation,ECC_Visibility,QueryParams);

		if (bulletHitSomething)
		{
			AActor * HitActor = HitResult.GetActor();
			
			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, EyeRotation.Vector(), HitResult, MyOwner->GetInstigatorController(), this, damageType);

			//auto test = HitResult.PhysMaterial.Get();

			EPhysicalSurface surface =  UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

			UParticleSystem* selectedEffect = nullptr;

			UE_LOG(LogTemp, Warning, TEXT("Shooting %s"),*HitResult.Actor->GetFName().ToString())
			UE_LOG(LogTemp, Warning, TEXT("SurfaceType %d"), (int)surface)
			//surface->
			switch (surface)
			{

			case SurfaceType1:
				UE_LOG(LogTemp, Warning, TEXT("SURFACE_FLESHDEFAULT"))
				selectedEffect = fleshImpactEffect;
				break;

			case SurfaceType2:
				UE_LOG(LogTemp, Warning, TEXT("SURFACE_FLESHVULNERABLE"))
				selectedEffect = fleshImpactEffect;
				break;

			default:
				UE_LOG(LogTemp, Warning, TEXT("default"))
				selectedEffect = defaultImpactEffect;
				break;
			}

			if (selectedEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), selectedEffect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
			}

		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), startLocation, endLocation, FColor::White, false, 1.0f, 0, 1.0f);
		}
		
		PlayFireEffects(endLocation);

	}
}

// Called every frame
void ATPSWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATPSWeapon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATPSWeapon::PlayFireEffects(FVector traceEnd)
{


	APawn * MyOwner = Cast<APawn>(GetOwner());

	if (MyOwner)
	{
		APlayerController * PC = Cast<APlayerController>(MyOwner->GetController());

		if (PC)
		{

			UE_LOG(LogTemp, Warning, TEXT("MyCharacter's Name is %s"), *MyOwner->GetName());
			UE_LOG(LogTemp, Warning, TEXT("Camera Shake"));

			PC->ClientPlayCameraShake(fireCameraShake,2.0f);
		
			if (!fireCameraShake)
			{
				UE_LOG(LogTemp, Warning, TEXT("no fire camera Shake"));
			}

			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(fireCameraShake, 1.0f);

			


		}
	}

	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}


}