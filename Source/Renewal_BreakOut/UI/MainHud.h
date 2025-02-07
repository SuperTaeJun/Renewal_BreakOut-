// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHud.generated.h"

/**
 * 
 */
UCLASS()
class RENEWAL_BREAKOUT_API AMainHud : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Crosshair)
	UTexture2D* CrosshairCenter;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	UTexture2D* CrosshairLeft;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	UTexture2D* CrosshairRight;

	
private:
	virtual void DrawHUD() override;
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter);
};
