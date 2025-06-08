// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SubWeaponManagerComponent.generated.h"


UENUM(BlueprintType)
enum class EBojoMugiType : uint8
{
	E_Grenade UMETA(DisplayName = "Grenade"),
	E_Wall UMETA(DisplayName = "Wall"),
	E_BoobyTrap UMETA(DisplayName = "BoobyTrap"),

	ECS_DEFAULT UMETA(DisplayName = "Default")
};

class ACharacterController;
class ACharacterBase;
class AProjectileBase;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BREAKOUT_API USubWeaponManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USubWeaponManagerComponent();
	// �ʱ�ȭ: CharacterBase���� ������Ʈ�� �Ѱ��ֱ�
	void InitializeSubWeapon(
		class UStaticMeshComponent* InGrenadeMesh,
		class UArrowComponent* InPathSource,
		class UNiagaraComponent* InAimComp,
		class UCameraComponent* InFollowCam);

	/** 1) ��ư ���� ä ���� ���� ǥ�� */
	void PredictPath(ACharacterBase* OwnerChar);

	/** 2) ��ư ���� ��ô �ִϸ��̼� ���� */
	void StartThrow(ACharacterBase* OwnerChar);

	/** 3) AnimNotify_SpawnProjectile ���� ȣ���� ���� ���� */
	UFUNCTION(BlueprintCallable)  // Blueprint Notify�ε� �θ� �� �ְ�
		void SpawnProjectile(ACharacterBase* OwnerChar);
	UFUNCTION(BlueprintCallable)
	void FinishThrow(ACharacterBase* OwnerChar);

	void ResetNum() { GrenadeNum = 3; WallNum = 3; TrapNum = 3; }

	void SelectSubWeapon(EBojoMugiType NewType);
	EBojoMugiType GetCurrentSubWeaponType() const { return CurrentSubWeaponType; }

protected:
	virtual void BeginPlay() override;

private:
	// �� �������� ���� (�����Ϳ��� ���� ����)
	UPROPERTY(EditAnywhere, Category = "SubWeapon|Count")
	int32 GrenadeNum = 3;

	UPROPERTY(EditAnywhere, Category = "SubWeapon|Count")
	int32 WallNum = 3;

	UPROPERTY(EditAnywhere, Category = "SubWeapon|Count")
	int32 TrapNum = 3;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> GrenadeClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> WallClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> BoobyTrapClass;

	// �� ���� ������ ������Ʈ��
	class UStaticMeshComponent* GrenadeMesh = nullptr;
	class UArrowComponent* PathSource = nullptr;
	class UNiagaraComponent* Aim = nullptr;
	class UCameraComponent* FollowCamera = nullptr;

	// �� ���ؼ� ���� ��ġ
	FVector SWAimLastLoc;

	// �� HUD ��Ʈ�ѷ� ĳ�� (����ó��)
	TWeakObjectPtr<class ACharacterController> MainController;

	// �� ���� ���õ� ���깫�� Ÿ��
	EBojoMugiType CurrentSubWeaponType;

	// ��Ŭ��/�ִϸ��̼� �����
	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimMontage* GrenadeMontage;

	FName                 RightHandSocket;
};
