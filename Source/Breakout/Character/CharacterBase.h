#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Type/TurningType.h"

#include "CharacterBase.generated.h"

class UInputAction;
#define DEFAULTCAMERALENGTH 200
#define SPRINTCAMERALENGTH 100

UCLASS()
class BREAKOUT_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();
	int		_SessionId;

protected:
	UPROPERTY(EditAnywhere, Category = Arrow)
	TObjectPtr<class UArrowComponent> PathSorce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UNiagaraComponent> Aim;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCharacterMovementComponent> Movement;


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	EWeaponType CurWeaponType;

	void UpdateCameraBoom(float DeltaTime);
	void UpdateStamina(float DeltaTime);
	void SetHUDCrosshair(float DeltaTime);

	//ĳ���� ����
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float Health = 100.f;
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxStamina = 100.f;
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float Stamina = 100.f;
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float DeadTime = 4.f;

	TObjectPtr<class AMainHUD> MainHUD;
	TObjectPtr<class ACharacterController> MainController;

	bool bCanEscape;
	bool StaminaExhaustionState;
	bool bAlive = true;

	int32 GrendeNum;
	int32 WallGrendeNum;
	int32 BoobyTrapNum;

	ECharacterState CharacterState;
	EBojoMugiType BojoMugiType;
	int32 ObtainedEscapeToolNum;

	FVector SWAimLastLoc;
	FTransform StartTransform;
	bool bSkillUsing = false;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class ULevelSequence> EndGameCine;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class ULevelSequence> StartGameCine;

	TObjectPtr<class AEscapeTool> OverlappingEscapeTool;

	//������
	bool bDissolve = false;
	float DissolvePercent = -1.f;
	TObjectPtr<class UMaterialInstanceDynamic> MDynamicDissolveInst;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UMaterialInstance> MDissolveInst;
	//

public:
	void UpdateHpHUD();
	void UpdateStaminaHUD();
	void UpdateObtainedEscapeTool();

	void SetAO_YAW(float Servao_yaw) { AO_Yaw = Servao_yaw; }
	void SetAO_PITCH(float Servao_pitch) { AO_Pitch = Servao_pitch; }
	void SetResetState();
	void SetWeaponType(EWeaponType Type) { CurWeaponType = Type; }
	void SetbCanEscape(bool _bCanEscape) { bCanEscape = _bCanEscape; }
	void SetEscapeToolNum(int32 Num) { ObtainedEscapeToolNum = Num; }
	void SetWeapon(TSubclassOf<class AWeaponBase> Weapon, FName SocketName);
	void SetWeaponUi();
	void SetRespawnUi();
	void SetbInRespon(bool _bInRespon) { bInRespon = _bInRespon; }
	void SetbShowSelect(bool _bShowSelect) { bShowSelectUi = _bShowSelect; }
	void SetbCanObtainEscapeTool(bool _bCanObtain);
	void SetbDissolve(bool Dissolve) { bDissolve = Dissolve; }
	void SetDissolvePersent(float Dissolve) { DissolvePercent = Dissolve; }
	void SetHealth(float DamagedHp) { Health = DamagedHp; }
	void SetMaxHealth(float MaxHp) { MaxHealth = MaxHp; }
	void SetOverlappingEscapeTool(class AEscapeTool* _OverlappingEscapeTool) { OverlappingEscapeTool = _OverlappingEscapeTool; }
	void SetbAlive(bool _bAlive) {bAlive = _bAlive;}

	class AWeaponBase* GetCurWeapon() const { return CurWeapon; }
	ETurningInPlace GetTurningType() { return TurningType; }
	float GetAO_Yaw() const { return AO_Yaw; }
	float GetAO_Pitch() const { return AO_Pitch; }
	class AEscapeTool* GetOverlappingEscapeTool() { return OverlappingEscapeTool; }
	bool GetbInRespon() { return bInRespon; }
	float GetDissolvePersent() { return DissolvePercent; }
	UAnimMontage* GetDeadMontage() { return DeadMontage; }
	int32 GetEscapeToolNum() { return ObtainedEscapeToolNum; }
	class UMaterialInstanceDynamic* GetDynamicMaterial() { return MDynamicDissolveInst; }
	class AWeaponBase* GetWeapon() { return CurWeapon; }
	class ACharacterController* GetMainController() { return MainController; }
	class ULevelSequence* GetEndGameCine() { return EndGameCine; }
	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }
	float GetStamina() const { return Stamina; }
	float GetMaxStamina() const { return MaxStamina; }
	bool GetbFfirePressed() const { return bFirePressed; }
	FVector GetHitTarget() { return HitTarget; }
	bool GetbAlive() { return bAlive; }


	UFUNCTION()
	void ReciveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	void GrandeThrow();
	void GrandeAim();

	UFUNCTION(BlueprintCallable)
	void GrandeThrowFinish();

	UFUNCTION(BlueprintCallable)
	void SpawnGrenade();

	UFUNCTION(BlueprintCallable)
	void ReloadForMontage();

	void SetSpawnGrenade(TSubclassOf<class AProjectileBase> Projectile);

	void Dead();
	FTimerHandle DeadTimer;
	TObjectPtr<class AWeaponBase> CurWeapon;

protected:

	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> FireActionMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> GrenadeMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> DeadMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> ReloadMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> InterMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> HitMontage;


	//����ź
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> Grenade;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> GrenadeClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> WallClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> BoobyTrapClass;

	//idle turn
	ETurningInPlace TurningType;
	void TurnInPlace(float DeltaTime);

	//���ӿ�����
	void AimOffset(float DeltaTime);
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	FName RightHandSocketName;

	//fire
	bool bCanFire;
	bool bFirePressed;
	FTimerHandle FireTimer;
	void StartFireTimer();
	void FireTimerFinished();
	void Fire();
	void FirePressd(bool _Pressd);
	void TraceUnderCrossHiar(FHitResult& TraceHitResult);
	void PlayFireActionMontage();

	bool bCanJump = true;
	bool bInRespon;
	bool bShowSelectUi;
	bool bCanObtainEscapeTool;
	bool bUsingThrowMontage = false;
	//���ؼ�
	UPROPERTY(EditAnywhere, Category = Crosshair)
	TObjectPtr<class UTexture2D> CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	TObjectPtr<class UTexture2D> CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	TObjectPtr<class UTexture2D> CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	TObjectPtr<class UTexture2D> CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	TObjectPtr<class UTexture2D> CrosshairsBottom;

	//ũ�ν����������� ��Ÿ���ŭ ���������� ������ �浹��ġ
	FVector HitTarget;

	void SetSprint();
	void SetRun();


	//�Է°�
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> DefalutMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SkillAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> InterAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> ReRoadAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> GrandeFireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SelectGrandeAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SelectWallAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SelectTrapAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> DetectAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> LightAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> QuitAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> DebugAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Sprint_S(const FInputActionValue& Value);
	void Sprint_E(const FInputActionValue& Value);
	void Fire_S(const FInputActionValue& Value);
	void Fire_E(const FInputActionValue& Value);
	void Inter(const FInputActionValue& Value);
	void Inter_Start(const FInputActionValue& Value);
	void Inter_End(const FInputActionValue& Value);
	void EToolTranfrom(const FInputActionValue& Value);
	void Reroad(const FInputActionValue& Value);
	void GrandeFire_Aiming(const FInputActionValue& Value);
	void GrandeFire(const FInputActionValue& Value);
	void SelectGrande(const FInputActionValue& Value);
	void SelectWall(const FInputActionValue& Value);
	void SelectTrap(const FInputActionValue& Value);
	void StartJump(const FInputActionValue& Value);
	void StopJump(const FInputActionValue& Value);
	virtual void Skill_S(const FInputActionValue& Value) {};
	virtual void Skill_E(const FInputActionValue& Value) {};
	void Detect_S(const FInputActionValue& Value);
	void Detect_E(const FInputActionValue& Value);
	void LightOnOff(const FInputActionValue& Value);
	void Quit(const FInputActionValue& Value);
	void OnDebug(const FInputActionValue& Value);

public:
	//������ �����ϴ� �Լ���
	UFUNCTION()
	void SendEnd();
	void SpawnBeam(FVector StartBeam, FVector EndBeam);
	void SpawnHitImpact(FVector HitLoc, FRotator HitRot);
	bool bStarted;
	FTimerHandle StartHandle;
	UFUNCTION()
	void StartGame();
	float StartedCnt;
	bool bDeadAnim = false;
	class UBOGameInstance* inst;
	class UCameraComponent* GetCamera() { return FollowCamera; }
	void Server_PlayDeadAnim();

	bool bCrosshiar = false;
	bool bStamina = false;
	bool bCurLight = false;
};

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_IDLE UMETA(DisplayName = "Idle"),
	ECS_RUN UMETA(DisplayName = "Run"),
	ECS_SPRINT UMETA(DisplayName = "Sprint"),
	ECS_FALLING UMETA(DisplayName = "Falling"),

	ECS_DEFAULT UMETA(DisplayName = "Default")
};

UENUM(BlueprintType)
enum class EBojoMugiType : uint8
{
	E_Grenade UMETA(DisplayName = "Grenade"),
	E_Wall UMETA(DisplayName = "Wall"),
	E_BoobyTrap UMETA(DisplayName = "BoobyTrap"),

	ECS_DEFAULT UMETA(DisplayName = "Default")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	E_Rifle UMETA(DisplayName = "Rifle"),
	E_Shotgun UMETA(DisplayName = "Shotgun"),
	E_Launcher UMETA(DisplayName = "Launcher"),

	ECS_DEFAULT UMETA(DisplayName = "Default")
};
