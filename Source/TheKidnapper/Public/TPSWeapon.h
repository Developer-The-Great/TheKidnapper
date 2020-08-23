// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TPSWeapon.generated.h"


class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;


UCLASS()
class THEKIDNAPPER_API ATPSWeapon : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATPSWeapon();

	USkeletalMeshComponent* GetSkeletalMeshComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UDamageType> damageType;

	void BeginFire();
	void EndFire();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
	USkeletalMeshComponent * MeshComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent * sceneComp = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* tracerEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* defaultImpactEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* fleshImpactEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float baseDamage = 20.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float HeadShotDamage = 80.0f;

	UFUNCTION(BlueprintImplementableEvent, Category = "FiringEvent")
		void OnFire();

	void Fire();
	
public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	FVector lookAtSpot = FVector(0,0,0);

	float lastFireTime = 0.0f;

	/* RPM - Bullets per minute fired by weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float rateOfFire = 600.0f;

	float timeBetweenShots;

	FTimerHandle TimerHandle_RateBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> fireCameraShake;


	

	void PlayFireEffects(FVector traceEnd);


	
};
