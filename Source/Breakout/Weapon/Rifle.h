// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "Rifle.generated.h"

/**
 * 
 */
UCLASS()
class BREAKOUT_API ARifle : public AWeaponBase
{
	GENERATED_BODY()
	
protected:
	virtual void Fire(const FVector& HitTarget) override;
};
