// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraActor.h"
#include "Skill4Actor.generated.h"

/**
 * 
 */
UCLASS()
class BREAKOUT_API ASkill4Actor : public ANiagaraActor
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void Tick(float Delta) override;
	bool bTimerStart = false;
	FTimerHandle TelpoTimer;
	void Dead();
};
