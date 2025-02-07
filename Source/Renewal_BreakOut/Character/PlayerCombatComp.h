// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCombatComp.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RENEWAL_BREAKOUT_API UPlayerCombatComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerCombatComp();

private:
	class ACharacterBase* Player;
	class AWeaponBase* CurrentWeapon;

	FVector HitTarget;

private:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateHitTarget();
	void TraceUnderCrossHiar(FHitResult& TraceHitResult);
public:
	void Fire();
public:
	void SetWeapon(TSubclassOf<class AWeaponBase> Weapon, FName SocketName);
	class AWeaponBase* GetCurWeapon() { return CurrentWeapon; }
	FVector GetHitTarget() { return HitTarget; }
};
