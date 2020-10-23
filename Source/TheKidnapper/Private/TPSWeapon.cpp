// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShake.h"
#include "Particles/ParticleSystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TheKidnapper.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
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
	sceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	MeshComp->AttachTo(sceneComp);
	RootComponent = sceneComp;

	MuzzleSocketName = "MuzzleSocket";


}

USkeletalMeshComponent * ATPSWeapon::GetSkeletalMeshComponent()
{
	return MeshComp;
}

void ATPSWeapon::BeginFire()
{

	float firstDelay = FMath::Max(lastFireTime + timeBetweenShots - GetWorld()->TimeSeconds,0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_RateBetweenShots,this,&ATPSWeapon::Fire, timeBetweenShots, true, firstDelay);


}

void ATPSWeapon::EndFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_RateBetweenShots);
}

// Called when the game starts or when spawned
void ATPSWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	timeBetweenShots = 60.0f / rateOfFire;


}

void ATPSWeapon::Fire()
{
	AActor * MyOwner = GetOwner();
	//if there is an owner
	UE_LOG(LogTemp, Warning, TEXT("Fire()"));

	if (MyOwner)
	{
		OnFire();
		//get start position 
		

		FVector startLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(startLocation, EyeRotation);

		

		FCollisionQueryParams QueryParams;

		//QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.bReturnPhysicalMaterial = true;
		//QueryParams.AddIgnoredActor(this);

		QueryParams.bTraceComplex = true;

		FVector shotDirection = EyeRotation.Vector();
		sprayBasedDirectionModify(shotDirection);

		FVector endLocation = startLocation + shotDirection * 10000;

		FHitResult HitResult;
		bool bulletHitSomething = GetWorld()->LineTraceSingleByChannel(HitResult, startLocation, endLocation,ECC_Visibility,QueryParams);

		FVector tracerEnd = endLocation;

		if (bulletHitSomething)
		{
			
			AActor * HitActor = HitResult.GetActor();
			tracerEnd = HitResult.ImpactPoint;
			
			//auto test = HitResult.PhysMaterial.Get();

			EPhysicalSurface surface =  UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

			UParticleSystem* selectedEffect = nullptr;

			UE_LOG(LogTemp, Warning, TEXT("Shooting %s"), *HitResult.Actor->GetFName().ToString())
				UE_LOG(LogTemp, Warning, TEXT("SurfaceType %d"), (int)surface)

			float damage = baseDamage;
			if (surface == SURFACE_FLESHVULNERABLE)
			{
				damage = HeadShotDamage;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, damage, EyeRotation.Vector(), HitResult, MyOwner->GetInstigatorController(), this, damageType);

			//surface->
			switch (surface)
			{

			case SURFACE_FLESHDEFAULT:
				UE_LOG(LogTemp, Warning, TEXT("SURFACE_FLESHDEFAULT"))
				selectedEffect = fleshImpactEffect;
				break;

			case SURFACE_FLESHVULNERABLE:
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


		if (tracerEffect)
		{
			FVector MuzzleLocation = GetSkeletalMeshComponent()->GetSocketLocation(MuzzleSocketName);
			UParticleSystemComponent * tracer =  UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), tracerEffect, MuzzleLocation);

			if (tracer)
			{
				tracer->SetVectorParameter("BeamEnd", tracerEnd);
			}

		}
		




		lastFireTime = GetWorld()->TimeSeconds;

	}
}

// Called every frame
void ATPSWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AActor * MyOwner = GetOwner();

	if (MyOwner)
	{
		//UE_LOG(LogTemp, Warning, TEXT("raycastHitSomething"));
		FVector startLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(startLocation, EyeRotation);

		FVector endLocation = startLocation + EyeRotation.Vector() * 10000;

		FCollisionQueryParams QueryParams;

		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);

		FHitResult HitResult;
		bool raycastHitSomething = GetWorld()->LineTraceSingleByChannel(HitResult, startLocation, endLocation, ECC_Visibility, QueryParams);

		FRotator Rot;

		Rot = FRotationMatrix::MakeFromYZ(endLocation - startLocation, FVector::UpVector).Rotator();

		this->SetActorRotation(Rot);
	}

	

}

// Called to bind functionality to input
void ATPSWeapon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATPSWeapon::sprayBasedDirectionModify(FVector & directionToModify)
{
	FVector up;

	FVector right = FVector::CrossProduct(directionToModify, FVector::UpVector);
	

	if (FMath::IsNearlyZero(right.Size()))
	{
		right = FVector(0,1,0);
		up = FVector(1, 0, 0);
	}
	else 
	{
		right.Normalize();
		up = FVector::CrossProduct(directionToModify, right);
		up.Normalize();
	}

	float horizontalRot = FMath::FRandRange(-maxHorizontalSprayAngle, maxHorizontalSprayAngle);
	float verticalRot = FMath::FRandRange(-maxVerticalSprayAngle, maxVerticalSprayAngle);

	directionToModify = directionToModify.RotateAngleAxis(verticalRot, right);
	directionToModify = directionToModify.RotateAngleAxis(horizontalRot, up);
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