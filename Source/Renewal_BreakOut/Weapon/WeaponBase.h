// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

struct WeaponInfo
{
	FString WeaponType;

	int32 MaxAmmo;
	int32 CurAmmo;

	float Damage;
};

UCLASS()
class RENEWAL_BREAKOUT_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();


protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USkeletalMeshComponent> WeaponMesh;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USpotLightComponent> SpotLight;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> FireSound;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> ReloadSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem> ImpactNiagara;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraComponent>DetectNiagara;

	//UPROPERTY(EditAnywhere)
	//TSubclassOf<class AProjectileBullet> ProjectileBulletClass;

	//√—æÀ »Â≈Õ¡¸ ø‰º“µÈ
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius = 100.f;

	//√—æÀ »Â≈Õ¡¸ ªÁøÎ «“¡ˆ∏ª¡ˆ
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = true;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Firerate = 0.15;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Range = 1500.f;

	bool bDebug;
protected:
	virtual void BeginPlay() override;

	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);

public:
	virtual void Fire(const FVector& HitTarget);

public:
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE class USpotLightComponent* GetSpotLight() { return SpotLight; }
	FORCEINLINE float GetRange() { return Range; }
};
