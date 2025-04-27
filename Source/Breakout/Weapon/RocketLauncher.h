// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "RocketLauncher.generated.h"

/**
 * 
 */
UCLASS()
class BREAKOUT_API ARocketLauncher : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	ARocketLauncher();
protected:
	virtual void FireInternal(const FVector& HitTarget) override;
	virtual void SendFirePacket() override;
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> ProjectileLauncherClass;

	FVector StartPos;
	FRotator CurWeaponRot;
	FActorSpawnParameters SpawnParm;
};
