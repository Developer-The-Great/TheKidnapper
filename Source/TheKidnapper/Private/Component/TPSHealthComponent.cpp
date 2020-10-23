// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSHealthComponent.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UTPSHealthComponent::UTPSHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	defaultHealth = 100;
	// ...
}


// Called when the game starts
void UTPSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor * owner = GetOwner();

	if (owner)
	{
		owner->OnTakeAnyDamage.AddDynamic(this, &UTPSHealthComponent::OnTakeDamage);
		UE_LOG(LogTemp, Log, TEXT("FOUND OWNER"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("NO OWNER FOUND"));
	}

	health = defaultHealth;


}


// Called every frame
void UTPSHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTPSHealthComponent::OnTakeDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage < 0.0f)
	{
		return;
	}

	health = FMath::Clamp(health - Damage, 0.0f, defaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(health));

	OnHealthChanged.Broadcast(this, health, Damage, DamageType, InstigatedBy, DamageCauser);

}

