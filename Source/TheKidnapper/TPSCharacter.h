// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ATPSWeapon;

UCLASS()
class THEKIDNAPPER_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);

	void MoveRight(float value);

	void BeginCrouch();
	void EndCrouch();

	void BeginFire();
	void EndFire();



	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
	UCameraComponent* CameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent * SpringArmComponent;

	void BeginZoom();
	void EndZoom();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	virtual FVector GetPawnViewLocation() const override;

private:

	

	float defaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float zoomedFOV;
	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0 , ClampMax = 100))
	float ZoomInterpSpeed;

	UPROPERTY(EditAnywhere,Category = "Player")
	ATPSWeapon * currentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ATPSWeapon> starterWeaponClass;



	float currentFOV;

	bool bIsZooming;

};
