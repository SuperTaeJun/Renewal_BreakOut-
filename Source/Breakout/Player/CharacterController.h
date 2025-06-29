
// Fill out your copyright notice in the Description page of Project Settings.


#pragma once
#include "CoreMinimal.h"
#include <queue>
#include "ClientSocket.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include <memory>
#include "Character/CharacterBase.h"
#include "UWeaponManagerComponent.h"
#include "Components/SubWeaponManagerComponent.h"
#include "CharacterController.generated.h"

//ClientSocket* c_socket = nullptr;
class CPlayer;
class Citem;
class ClientSocket;
class CPlayerInfo;
class UBOGameInstance;
/**
 *
 */
struct RotAndLoc
{
	FVector Loc;
	FRotator Rot;
	RotAndLoc(FVector A, FRotator B)
	{
		Loc = A;
		Rot = B;
	}
};


UCLASS()
class BREAKOUT_API ACharacterController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> SkMeshAsset1;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> SkMeshAsset2;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> SkMeshAsset3;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> SkMeshAsset4;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UMaterialInstance> OldMaterial;

	TObjectPtr<class UMaterialInstanceDynamic> DynamicMaterial;

	// 무기 동기화용 
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> LauncherRef;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> GrenadeRef;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> WallRef;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> BoobyTrapRef;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBullet> BulletRef;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBullet> ShotgunRef;

	//애니메이션 동기화 용
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> SyncDeadMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> SyncReloadMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> SyncReloadMontageCh3;

	//아이템 획득 애니메이션
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> SyncInterMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> SyncHitMontage;

	// 플레이어 동기화 용
	UPROPERTY(EditAnywhere)
	TObjectPtr<UClass> Anim1;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UClass> Anim2;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UClass> Anim3;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UClass> Anim4;
	// 나이아가라 스킬 생성 동기화 용 
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem> TimeReplayNiagaraRef;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> NiagaraActorRef;

	// 보조무기 생산 시 애니메이션 동기화 용
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> GrenadeMontage;

	/*UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> GrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> GrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> GrenadeMontage;*/
	//-------------------------------------------

	UPROPERTY(EditAnywhere, Category = "Combat System", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeaponBase> Rifle;

	UPROPERTY(EditAnywhere, Category = "Combat System", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeaponBase> ShotGun;

	UPROPERTY(EditAnywhere, Category = "Combat System", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeaponBase> Lancher;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem>ChargeNiagara;
public:
	ACharacterController();
	CPlayer initplayer;
	// 다른 캐릭터들의 정보
	std::queue<std::shared_ptr<CPlayer>> NewPlayer;
	std::queue<std::shared_ptr<CItem>> NewItem;
	//int my_session_id;
	//bool ProcessPacket(char* p);

	//virtual void OnPossess(APawn* InPawn) override;
	void SetChName();
	void SetNum();

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDStamina(float Stamina, float MaxStamina);
	void SetHUDAmmo(int32 Ammo);
	void SetHUDEscapeTool(int32 EscapeTool);
	void SetHUDBojoImage(EBojoMugiType Type);
	void SetHUDCrosshair(const struct FCrosshairPackage& Package);
	//skill
	void SetHUDSkill();
	//스킬 아이콘 투명도
	void SetHUDSkillOpacity(float Opacity);
	//skill1,3,4 쿨타임
	void SetHUDCool(float Cool, float MaxCool);
	//skill2 대쉬포인트
	void SetHUDCool(int32 Cool);
	void SetHUDCoolVisibility(bool bVisibility);
	void SetHUDMatchingUi(bool bVictory);
	float StartCnt = 5.f;
	void showWeaponSelect();
	void ShowRespawnSelect();
	void ShowMatchingUi();
	// 초기 플레이어 저장
	//void SetPlayerID(const int playerid) {
	//	UE_LOG(LogTemp, Warning, TEXT("%d -> my_id"), playerid);
	//	id = playerid; }
	void SetPlayerInfo(CPlayerInfo* p_info) {
		if (p_info != nullptr)
			PlayerInfo = p_info;
	}
	void SetInitPlayerInfo(const CPlayer& owner_player);
	//-----------------------------------------------------
	void SetNewCharacterInfo(std::shared_ptr<CPlayer> InitPlayer);
	void SetNewItemInfo(std::shared_ptr<CItem> InitPlayer);
	void SetAttack(int _id);
	void SetHitEffect(int _id);
	void UpdatePlayer();
	//동기화 용
	void UpdateSyncPlayer();
	void UpdateSyncItem();
	void SetHp(float DamagedHp);
	void SetDestroyItemid(int itemid) { Escapeid = itemid; };
	void SetMoppItemID(int moppID) { MoppID = moppID; };
	bool UpdateWorld();
	float damaged = 0;
	//초기 컨트롤러 세팅
	
	void InitPlayer();
	//Tick함수
	virtual void Tick(float DeltaTime);

	// 스폰시킬 다른 캐릭터
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class ACharacterBase> ToSpawn;
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AEscapeTool> ItemSpawn;
	void Set_Weapon_Type(EWeaponType Type);
	//void UpdateWeaponMesh();
	virtual void OnPossess(APawn* InPawn) override;

	void AllHud();
public:

	TObjectPtr<class AMainHUD> MainHUD;
	int id;
	int Escapeid = -1;
	int MoppID;
	bool bNewPlayerEntered = false;
	bool bNewWeaponEntered = false;
	bool bInitPlayerSetting = false;
	//ClientSocket* c_socket;
	CPlayerInfo* PlayerInfo;
	CItemInfo* ItemInfo;
	int p_cnt;
	bool connect;
	bool Set_Weapon;
	char data[BUFSIZE] = { 0 };
	int remainData = 0;
	bool login_cond;
	class AActor* ServerTemp;
	UBOGameInstance* inst;
	bool bSync = false;
	class ABOGameMode* m_GameMode;
	TArray<class ABulletHoleWall*> BulletWalls;
	TArray<class AEscapeTool*> EscapeTools;
	void SeverHpSync(bool bAlive, float hp, int myid);
	void ServerSetDissolve(bool dissolve, class ACharacterBase* player);
	UFUNCTION()
	void ServerSendEnd();
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<class ULevelSequence> EndGameCine;

	// 이동 패킷 주기 설정
	FTimerHandle FMovePacketTimer;
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABulletHoleWall> WallClass;

};
