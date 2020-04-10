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

	UFUNCTION(BlueprintCallable,Category = "Weapon")
	void Fire();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UDamageType> damageType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
	USkeletalMeshComponent * MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* defaultImpactEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* fleshImpactEffect = nullptr;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:


	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> fireCameraShake;


	

	void PlayFireEffects(FVector traceEnd);


	
};
