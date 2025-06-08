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
	// 초기화: CharacterBase에서 컴포넌트들 넘겨주기
	void InitializeSubWeapon(
		class UStaticMeshComponent* InGrenadeMesh,
		class UArrowComponent* InPathSource,
		class UNiagaraComponent* InAimComp,
		class UCameraComponent* InFollowCam);

	/** 1) 버튼 누른 채 조준 궤적 표시 */
	void PredictPath(ACharacterBase* OwnerChar);

	/** 2) 버튼 떼면 투척 애니메이션 시작 */
	void StartThrow(ACharacterBase* OwnerChar);

	/** 3) AnimNotify_SpawnProjectile 에서 호출할 실제 스폰 */
	UFUNCTION(BlueprintCallable)  // Blueprint Notify로도 부를 수 있게
		void SpawnProjectile(ACharacterBase* OwnerChar);
	UFUNCTION(BlueprintCallable)
	void FinishThrow(ACharacterBase* OwnerChar);

	void ResetNum() { GrenadeNum = 3; WallNum = 3; TrapNum = 3; }

	void SelectSubWeapon(EBojoMugiType NewType);
	EBojoMugiType GetCurrentSubWeaponType() const { return CurrentSubWeaponType; }

protected:
	virtual void BeginPlay() override;

private:
	// ▶ 보조무기 수량 (에디터에서 조정 가능)
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

	// ▶ 내부 참조용 컴포넌트들
	class UStaticMeshComponent* GrenadeMesh = nullptr;
	class UArrowComponent* PathSource = nullptr;
	class UNiagaraComponent* Aim = nullptr;
	class UCameraComponent* FollowCamera = nullptr;

	// ▶ 조준선 최종 위치
	FVector SWAimLastLoc;

	// ▶ HUD 컨트롤러 캐시 (기존처럼)
	TWeakObjectPtr<class ACharacterController> MainController;

	// ▶ 현재 선택된 서브무기 타입
	EBojoMugiType CurrentSubWeaponType;

	// 우클릭/애니메이션 재생용
	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimMontage* GrenadeMontage;

	FName                 RightHandSocket;
};
