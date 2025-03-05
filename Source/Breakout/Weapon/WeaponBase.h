// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class BREAKOUT_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:
	AWeaponBase();
	virtual void Fire(const FVector& HitTarget) {};
	FORCEINLINE float GetFirerate() { return Firerate; }
	void DetectTool(FVector& HitRes);


	float GetCurAmmo() { return CurAmmo; }
	float GetMaxAmmo() { return MaxAmmo; }
	void SetCurAmmo(float Ammo) {CurAmmo = Ammo;}

	class USoundCue* GetReloadSound() { return ReloadSound; }

	bool bDebug = false;
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBullet> ProjectileBulletClass;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USpotLightComponent> SpotLight;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<class USoundCue> FireSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<class USoundCue> ReloadSound;

	UPROPERTY(EditAnywhere)
	float Damage = 10.f;
	UPROPERTY(EditAnywhere)
	int32 CurAmmo = 10;
	UPROPERTY(EditAnywhere)
	int32 MaxAmmo = 10;

	//�Ѿ� ������ ��ҵ�
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius = 100.f;

	//�Ѿ� ������ ��� ��������
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = true;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Firerate=0.15;

public:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Range = 1500.f;

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);


public:

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem> ImpactNiagara;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraComponent>DetectNiagara;
public:
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	void SetDetectNiagara(bool bUse);
	class USpotLightComponent* GetSpotLight() { return SpotLight; }
};
