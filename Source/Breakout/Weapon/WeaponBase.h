// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/WeaponData.h"
#include "WeaponBase.generated.h"

UCLASS(Abstract)
class BREAKOUT_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:
	AWeaponBase();
	virtual void Fire(const FVector& HitTarget);
	void DetectTool(FVector& HitRes);

	FORCEINLINE float GetCurAmmo() const { return CurAmmo; }
	FORCEINLINE float GetMaxAmmo() const { return WeaponData.MaxAmmo; }
	FORCEINLINE const FWeaponData& GetWeaponData() const { return WeaponData; }

	class USoundCue* GetReloadSound() { return ReloadSound; }
	FWeaponData GetWeaponData() {return WeaponData;}
	bool bDebug = false;

protected:
	// DataTable 파일 자체 (CSV 가져온 거)
	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	TObjectPtr<class UDataTable> WeaponDataTable;

	// 사용할 Row 이름 (ex: "Rifle", "Shotgun" 같은 ID)
	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	FName WeaponDataID;
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
	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<class UNiagaraSystem> ImpactNiagara;
	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<class UNiagaraComponent>DetectNiagara;


	int32 CurAmmo = 10;
	FWeaponData WeaponData;
protected:
	virtual void BeginPlay() override;
	virtual void SetWeaponData();

	virtual void FireInternal(const FVector& HitTarget) PURE_VIRTUAL(AWeaponBase::FireInternal, );
	virtual void PlayFireEffect();
	virtual void SendFirePacket();

	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);


public:
	virtual void Reload();

public:
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	void SetDetectNiagara(bool bUse);
	class USpotLightComponent* GetSpotLight() { return SpotLight; }
};
