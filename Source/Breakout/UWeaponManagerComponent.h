// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UWeaponManagerComponent.generated.h"

class AWeaponBase;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	E_Rifle UMETA(DisplayName = "Rifle"),
	E_Shotgun UMETA(DisplayName = "Shotgun"),
	E_Launcher UMETA(DisplayName = "Launcher"),

	ECS_DEFAULT UMETA(DisplayName = "Default")
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BREAKOUT_API UUWeaponManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUWeaponManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	void EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass, FName SocketName);

	void HandleFirePressed();
	void HandleFireReleased();

	void ReloadWeaponWithAnimation();
	void FinishReload();

	void DestroyCurrentWeapon();
	void UpdateAmmoHUD();

	void SetWeaponType(EWeaponType Type) { CurrentWeaponType = Type; }
	EWeaponType GetWeaponType() { return CurrentWeaponType; }
	AWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }
	void SetCurrentWeapon(AWeaponBase* newWeapon) { CurrentWeapon = newWeapon; }

private:
	void FireWeapon(const FVector& HitTarget);
	void StartFireTimer();
	void FireTimerFinished();
	bool CanFire() const;
	bool IsSprinting() const;
	FVector GetCharacterHitTarget() const;
public:
	UPROPERTY()
	TObjectPtr<AWeaponBase> CurrentWeapon;

	UPROPERTY()
	TWeakObjectPtr<ACharacter> OwnerCharacter;

	EWeaponType CurrentWeaponType;
	FName AttachSocketName;

	FTimerHandle FireTimer;

	UPROPERTY()
	bool bFirePressed = false;

	UPROPERTY()
	bool bCanFire = true;

	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> FireActionMontage;
};
