#include "Character/CharacterBase.h"
//입력
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player/CharacterController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CameraShake/Sprint.h"
#include "CameraShake/Jog.h"
#include "CameraShake/Shoot.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/WeaponBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "HUD/MainHUD.h"
#include "GameFramework/PlayerController.h"
#include "Game/BOGameInstance.h"
#include "Weapon/ProjectileBase.h"
#include "Game/BOGameMode.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "GameProp/EscapeTool.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Animatiom/BOAnimInstance.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Sound/SoundCue.h"
#include "Components/SpotLightComponent.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "Data/WeaponData.h"
#include "UWeaponManagerComponent.h"
#include "Components/SubWeaponManagerComponent.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	TurningType = ETurningInPlace::ETIP_NotTurning;
	CharacterState = ECharacterState::ECS_DEFAULT;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->GetAnimInstance();

	Movement = GetCharacterMovement();
	Movement->MaxWalkSpeed = 400.f;
	Movement->bOrientRotationToMovement = true;
	Movement->JumpZVelocity = 480.f;
	Movement->AirControl = 0.2f;
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = DEFAULTCAMERALENGTH;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->SetWorldLocation(FVector(-60.f, 0.f, 130.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	Grenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade"));
	Grenade->SetupAttachment(GetMesh(), FName("GrandeSocket"));
	Grenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Grenade->bHiddenInGame = true;

	PathSorce = CreateDefaultSubobject<UArrowComponent>(TEXT("GrenadePath"));
	PathSorce->SetupAttachment(GetMesh(), FName("HeadSocket"));

	Aim = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraAim"));
	WeaponManager = CreateDefaultSubobject<UUWeaponManagerComponent>(TEXT("WeaponManager"));
	SubWeaponManager = CreateDefaultSubobject<USubWeaponManagerComponent>(TEXT("SubWeaponManager"));
	SubWeaponManager->InitializeSubWeapon(Grenade, PathSorce, Aim, FollowCamera);

	//스테이트
	MaxHealth = 100.f;
	Health = MaxHealth;
	MaxStamina = 100.f;
	Stamina = MaxStamina;
	StaminaExhaustionState = false;
	ObtainedEscapeToolNum = 0;
	bStarted = false;
	StartedCnt = 5.f;
	bCanEscape = false;

	MDissolveInst = ConstructorHelpers::FObjectFinder<UMaterialInstance>(TEXT("/Game/BreakoutAsset/Character/Character1/Material/MI_Ch1Material_Dissolve.MI_Ch1Material_Dissolve")).Object;
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();



	StartTransform = GetActorTransform();
	inst = Cast<UBOGameInstance>(GetGameInstance());
	inst->bNoUI = true;
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		//입력시스템 매핑
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefalutMappingContext, 0);
		}
	}

	MainController = MainController == nullptr ? Cast<ACharacterController>(Controller) : MainController;

	if (MainController)
	{
		MainController->SetHUDSkill();
		UpdateHpHUD();
		UpdateStaminaHUD();
		UpdateObtainedEscapeTool();
		//MainController->SetHUDBojoImage(Sw);

	}
	if (Aim)
		Aim->SetAutoActivate(false);


	OnTakeAnyDamage.AddDynamic(this, &ACharacterBase::ReciveDamage);
}


void ACharacterBase::UpdateCameraBoom(float DeltaTime)
{
	if (CameraBoom->TargetArmLength <= DEFAULTCAMERALENGTH && (CharacterState == ECharacterState::ECS_RUN || CharacterState == ECharacterState::ECS_IDLE))
	{
		CameraBoom->TargetArmLength += DeltaTime * 400;
	}
	else if (CameraBoom->TargetArmLength >= SPRINTCAMERALENGTH && CharacterState == ECharacterState::ECS_SPRINT)
	{
		CameraBoom->TargetArmLength -= DeltaTime * 400;
	}
}
void ACharacterBase::UpdateStamina(float DeltaTime)
{
	if (StaminaExhaustionState == false)
	{
		if (CharacterState == ECharacterState::ECS_SPRINT && Stamina >= 0.f)
		{
			Stamina -= DeltaTime * 8.f;
			if (Stamina <= 0.f)
				StaminaExhaustionState = true;
		}
		else 	if ((CharacterState == ECharacterState::ECS_RUN || CharacterState == ECharacterState::ECS_IDLE) && Stamina < MaxStamina)
		{
			Stamina += DeltaTime * 10.f;
		}
	}
	else	if (StaminaExhaustionState == true)
	{
		if (Stamina < 50.f)
		{
			Stamina += DeltaTime * 10.f;
		}
		else if (Stamina >= 50.f)
		{
			StaminaExhaustionState = false;
		}
	}
	UpdateStaminaHUD();
}

void ACharacterBase::SetHUDCrosshair(float DeltaTime)
{
	MainController = MainController == nullptr ? Cast<ACharacterController>(Controller) : MainController;
	//MainHUD = MainHUD == nullptr ? Cast<AMainHUD>(MainController->GetHUD()) : MainHUD;

	if (MainController)
	{
		FCrosshairPackage HUDPackage;

		HUDPackage.CrosshairCenter = CrosshairsCenter;
		HUDPackage.CrosshairLeft = CrosshairsLeft;
		HUDPackage.CrosshairRight = CrosshairsRight;
		HUDPackage.CrosshairTop = CrosshairsTop;
		HUDPackage.CrosshairBottom = CrosshairsBottom;

		MainController->SetHUDCrosshair(HUDPackage);
	}

}

void ACharacterBase::UpdateHpHUD()
{
	MainController = MainController == nullptr ? Cast<ACharacterController>(Controller) : MainController;
	if (MainController)
	{
		MainController->SetHUDHealth(FMath::Clamp(Health, 0.f, MaxHealth), MaxHealth);
	}
}

void ACharacterBase::UpdateStaminaHUD()
{
	MainController = MainController == nullptr ? Cast<ACharacterController>(Controller) : MainController;
	if (MainController)
	{
		MainController->SetHUDStamina(FMath::Clamp(Stamina, 0.f, 100.f), MaxStamina);
	}
}

void ACharacterBase::UpdateObtainedEscapeTool()
{
	MainController = MainController == nullptr ? Cast<ACharacterController>(Controller) : MainController;
	if (MainController)
	{
		MainController->SetHUDEscapeTool(ObtainedEscapeToolNum);
	}
}

void ACharacterBase::SetWeaponUi()
{
	if (MainController)
	{
		FInputModeUIOnly UiGameInput;
		MainController->SetInputMode(UiGameInput);
		MainController->DisableInput(MainController);
		//bShowSelectUi = true;
		MainController->bShowMouseCursor = true;
		MainController->bEnableMouseOverEvents = true;

		MainController->showWeaponSelect();
	}

}

void ACharacterBase::SetRespawnUi()
{
	FInputModeUIOnly UiGameInput;
	if (MainController)
	{
		MainController->SetInputMode(UiGameInput);
		MainController->DisableInput(MainController);
		//bShowSelectUi = true;
		MainController->bShowMouseCursor = true;
		MainController->bEnableMouseOverEvents = true;
		MainController->ShowRespawnSelect();
	}
}

void ACharacterBase::SetResetState()
{
	Health = MaxHealth;
	Stamina = 100.f;
	UpdateHpHUD();
	UpdateStaminaHUD();
	WeaponManager->GetCurrentWeapon()->Destroy();
	WeaponManager->SetCurrentWeapon(nullptr);
	bDissolve = false;
	DissolvePercent = -1.f;
	GetMesh()->SetMaterial(0, MDynamicDissolveInst);
	MDynamicDissolveInst->SetScalarParameterValue(FName("Dissolve"), DissolvePercent);

	SubWeaponManager->ResetNum();

	//여기서 패킷
	if (inst)
		inst->m_Socket->Send_Remove_Weapon(inst->GetPlayerID(), true);

}
void ACharacterBase::SetbCanObtainEscapeTool(bool _bCanObtain)
{
	bCanObtainEscapeTool = _bCanObtain;
}

void ACharacterBase::ReloadForMontage()
{
	if (WeaponManager)
	{
		WeaponManager->FinishReload();
	}
}


void ACharacterBase::ReciveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	//Health -= Damage;

	UpdateHpHUD();
	ACharacterBase* DamageInsigatorCh = Cast<ACharacterBase>(DamageCauser);


	if (Health <= 0.0f)
	{
		bDissolve = true;
		/*if (inst)
			inst->m_Socket->Send_Dissolve_packet(inst->GetPlayerID(), 0);*/
		if (DamageInsigatorCh && bAlive && MainController)
		{
			//서버
			if (ObtainedEscapeToolNum > 0 && 10 > ObtainedEscapeToolNum)
			{
				//DamageInsigatorCh->SetEscapeToolNum(DamageInsigatorCh->ObtainedEscapeToolNum + 1);
				ObtainedEscapeToolNum -= 1;
				if (inst) {
					//ui에서 상대방의 아이템 개수 늘려주고
					Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_Increase_item_count_packet(DamageInsigatorCh->_SessionId, 1);
					//// 내꺼는 줄여줌 
					Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_Decrease_item_count_packet(inst->GetPlayerID(), ObtainedEscapeToolNum);
					//UE_LOG(LogTemp, Warning, TEXT("packet"));
				}

			}
			else if (10 <= ObtainedEscapeToolNum)
			{
				DamageInsigatorCh->SetEscapeToolNum(DamageInsigatorCh->ObtainedEscapeToolNum + 4);
				ObtainedEscapeToolNum -= 4;
				if (inst) {
					Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_Increase_item_count_packet(DamageInsigatorCh->_SessionId, DamageInsigatorCh->GetEscapeToolNum());
					Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_Decrease_item_count_packet(inst->GetPlayerID(), ObtainedEscapeToolNum);
				}
			}
			UpdateObtainedEscapeTool();
		}
		//if (CurWeapon)
		//	CurWeapon->SetCurAmmo(0);

		if (bAlive && MainController)
		{
			DisableInput(MainController);
			GetWorld()->GetTimerManager().SetTimer(DeadTimer, this, &ACharacterBase::Dead, DeadTime, false);
			bAlive = false;
			PlayAnimMontage(DeadMontage);
			if (inst)
				Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_Alive_packet(inst->GetPlayerID(), 0);
		}
	}
	else if (Health > 0 && MainController)
	{
		PlayAnimMontage(HitMontage, 1.f);
		//if (inst)
		//	inst->m_Socket->Send_Hit_Anim_packet(inst->GetPlayerID(), true);
	}
}

void ACharacterBase::Dead()
{
	WeaponManager->bCanFire = false;
	SetRespawnUi();

}



void ACharacterBase::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 30.f)
	{
		TurningType = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -30.f)
	{
		TurningType = ETurningInPlace::ETIP_Left;
	}
	if (TurningType != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningType = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ACharacterBase::AimOffset(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool IsFalling = GetCharacterMovement()->IsFalling();
	//UE_LOG(LogTemp, Log, TEXT("%f"), AO_Yaw);


	if (!IsFalling && Speed == 0.f) //점프 아니고 서있을때
	{
		//UE_LOG(LogTemp, Log, TEXT("%f"), Speed);
		FRotator CurAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurAimRotation, StartingAimRotation);
		AO_Yaw = DeltaRotation.Yaw;
		if (TurningType == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (IsFalling || Speed > 0.f) // 뛰거나 점프일때
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningType = ETurningInPlace::ETIP_NotTurning;
	}
	AO_Pitch = GetBaseAimRotation().Pitch + 20.f;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ACharacterBase::TraceUnderCrossHiar(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	//크로스헤어 뷰포트에서의 좌표
	FVector2D CrossHairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	FVector CrossHairPostion;
	FVector CrossHairDirection;

	//주어진 2D 화면 공간 좌표를 3D 세계 공간 지점과 방향으로 변환
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
	(
		UGameplayStatics::GetPlayerController(this, 0),
		CrossHairLocation,
		CrossHairPostion,
		CrossHairDirection
	);
	if (bScreenToWorld)
	{
		FVector Start = CrossHairPostion;
		if (this)
		{
			float DistanceToCharacter = (this->GetActorLocation() - Start).Size();
			Start += CrossHairDirection * (DistanceToCharacter + 100.f);
		}

		if (WeaponManager->GetCurrentWeapon())
		{
			FVector End = Start + CrossHairDirection * WeaponManager->GetCurrentWeapon()->GetWeaponData().Range;

			GetWorld()->LineTraceSingleByChannel(
				TraceHitResult,
				Start,
				End,
				ECollisionChannel::ECC_Visibility
			);
			if (!TraceHitResult.bBlockingHit)
			{
				TraceHitResult.ImpactPoint = End;
			}
			else
			{
				//DrawDebugSphere(GetWorld(), TraceHitResult.ImpactPoint, 12.f, 12, FColor::Red);
			}
		}
	}

}
void ACharacterBase::SetSprint()
{
	Movement->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
}
void ACharacterBase::SetRun()
{
	Movement->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;
}

void ACharacterBase::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotaion(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDir = FRotationMatrix(YawRotaion).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDir, MovementVector.Y);
	const FVector RightDir = FRotationMatrix(YawRotaion).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDir, MovementVector.X);
}
void ACharacterBase::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerYawInput(LookAxis.X);
		AddControllerPitchInput(LookAxis.Y);
	}
}

void ACharacterBase::Sprint_S(const FInputActionValue& Value)
{
	if (!StaminaExhaustionState && WeaponManager->GetCurrentWeapon())
	{
		CharacterState = ECharacterState::ECS_SPRINT;

		SetSprint();
	}
	else
	{
		CharacterState = ECharacterState::ECS_RUN;

		SetRun();
	}

}
void ACharacterBase::Sprint_E(const FInputActionValue& Value)
{
	CharacterState = ECharacterState::ECS_RUN;
	Movement->MaxWalkSpeed = 400;

	SetRun();
}
void ACharacterBase::Fire_S(const FInputActionValue& Value)
{
	if (WeaponManager) WeaponManager->HandleFirePressed();
}
void ACharacterBase::Fire_E(const FInputActionValue& Value)
{
	if (WeaponManager) WeaponManager->HandleFireReleased();
}
void ACharacterBase::Inter(const FInputActionValue& Value)
{
	if (bCanObtainEscapeTool && OverlappingEscapeTool)
	{
		ObtainedEscapeToolNum += 1;
		//패킷(id, num)
		if (inst && MainController)
		{
			Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_Item_packet(inst->GetPlayerID(), ObtainedEscapeToolNum);
			inst->m_Socket->Send_Destroyed_item_packet(OverlappingEscapeTool->ItemID, inst->GetPlayerID());
			inst->m_Socket->Send_item_Anim_packet(inst->GetPlayerID(), 1);
		}
		UpdateObtainedEscapeTool();
		OverlappingEscapeTool->SetHideMesh();
		OverlappingEscapeTool = nullptr;
	}
	else if (!bCanObtainEscapeTool && OverlappingEscapeTool)
	{
		OverlappingEscapeTool->bOverlap = 1;
	}

	if (bCanEscape)
	{
		UE_LOG(LogTemp, Warning, TEXT("endgame"));
		bCrosshiar = false;
		bStamina = false;
		FMovieSceneSequencePlaybackSettings PlaybackSettings;
		PlaybackSettings.bHideHud = true;
		PlaybackSettings.bHidePlayer = true;
		PlaybackSettings.bDisableMovementInput = true;
		PlaybackSettings.bDisableLookAtInput = true;
		ALevelSequenceActor* SequenceActor;
		ULevelSequencePlayer* LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
			GetWorld(),
			EndGameCine,
			PlaybackSettings,
			SequenceActor
		);

		if (LevelSequencePlayer)
		{
			if (inst)
				Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_End_Game_packet(inst->GetPlayerID(), true);

			MainController->MainHUD->RemoveToolNumUi();
			MainController->MainHUD->RemoveCharacterOverlay();
			MainController->ShowMatchingUi();
			MainController->SetHUDMatchingUi(true);
			LevelSequencePlayer->Play();
			LevelSequencePlayer->OnFinished.AddDynamic(this, &ACharacterBase::SendEnd);
			while (!inst->m_Socket->Tempname.empty()) {
				inst->m_Socket->Tempname.pop();
			}
		}
	}
}
void ACharacterBase::Inter_Start(const FInputActionValue& Value)
{
	if (!bCanObtainEscapeTool && OverlappingEscapeTool)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		Cast<UBOAnimInstance>(AnimInstance)->bUseLeftHand = false;
		if (InterMontage) {
			PlayAnimMontage(InterMontage);
			if (inst && MainController) {
				inst->m_Socket->Send_item_Anim_packet(inst->GetPlayerID(), 0);
				inst->m_Socket->Send_Mopp_Sync_packet(OverlappingEscapeTool->ItemID, 1, inst->GetPlayerID());
			}
		}
	}
}
void ACharacterBase::Inter_End(const FInputActionValue& Value)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	Cast<UBOAnimInstance>(AnimInstance)->bUseLeftHand = true;
	if (InterMontage)
		StopAnimMontage(InterMontage);
	if (OverlappingEscapeTool)
	{
		OverlappingEscapeTool->bOverlap = 0;
		if (inst && MainController) {
			inst->m_Socket->Send_Mopp_Sync_packet(OverlappingEscapeTool->ItemID, 0, inst->GetPlayerID());
			inst->m_Socket->Send_item_Anim_packet(inst->GetPlayerID(), 1);
		}
	}

}
void ACharacterBase::EToolTranfrom(const FInputActionValue& Value)
{
	OverlappingEscapeTool->TransformMesh(GetWorld()->GetDeltaSeconds(), false, false);
}
void ACharacterBase::Reroad(const FInputActionValue& Value)
{
	if (WeaponManager)
	{
		WeaponManager->ReloadWeaponWithAnimation();
	}
}

void ACharacterBase::GrandeFire_Aiming(const FInputActionValue& Value)
{
	if (SubWeaponManager)
	{
		SubWeaponManager->PredictPath(this);
	}
}

void ACharacterBase::GrandeFire(const FInputActionValue& Value)
{
	if (SubWeaponManager)
	{
		SubWeaponManager->StartThrow(this);
	}
}

void ACharacterBase::SelectGrande(const FInputActionValue& Value)
{
	if (SubWeaponManager)
	{
		SubWeaponManager->SelectSubWeapon(EBojoMugiType::E_Grenade);
	}
}

void ACharacterBase::SelectWall(const FInputActionValue& Value)
{
	if (SubWeaponManager)
	{
		SubWeaponManager->SelectSubWeapon(EBojoMugiType::E_Wall);
	}
}

void ACharacterBase::SelectTrap(const FInputActionValue& Value)
{
	if (SubWeaponManager)
	{
		SubWeaponManager->SelectSubWeapon(EBojoMugiType::E_BoobyTrap);
	}
}

void ACharacterBase::StartJump(const FInputActionValue& Value)
{
	if (bCanJump && WeaponManager->GetCurrentWeapon())
		Super::Jump();

	bCanJump = false;

}

void ACharacterBase::StopJump(const FInputActionValue& Value)
{
	Super::StopJumping();
}

void ACharacterBase::Detect_E(const FInputActionValue& Value)
{
	if (WeaponManager->GetCurrentWeapon())
	{
		WeaponManager->GetCurrentWeapon()->SetDetectNiagara(false);
	}
}

void ACharacterBase::LightOnOff(const FInputActionValue& Value)
{
	if (WeaponManager->GetCurrentWeapon() && WeaponManager->GetCurrentWeapon()->GetSpotLight())
	{
		UE_LOG(LogTemp, Log, TEXT("ONOFF"));

		if (WeaponManager->GetCurrentWeapon()->GetSpotLight()->IsVisible())
		{	//라이트 패킷 ID랑,false보내기 그 후 컨트롤러에서 밑에줄 실행
			if (inst)
				inst->m_Socket->Send_Light_On_packet(inst->GetPlayerID(), false);
			WeaponManager->GetCurrentWeapon()->GetSpotLight()->SetVisibility(false);
			bCurLight = false;
		}
		else
		{			//라이트 패킷 ID,랑true보내기 그 후 컨트롤러에서 밑에줄 실행
			if (inst)
				inst->m_Socket->Send_Light_On_packet(inst->GetPlayerID(), true);
			WeaponManager->GetCurrentWeapon()->GetSpotLight()->SetVisibility(true);
			bCurLight = true;
		}
	}
}

void ACharacterBase::Quit(const FInputActionValue& Value)
{
	if (inst) {
		inst->m_Socket->Send_Logout_packet(inst->GetPlayerID());
		inst->m_Socket->CloseSocket();
	}
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);

}

void ACharacterBase::OnDebug(const FInputActionValue& Value)
{
	if (WeaponManager->GetCurrentWeapon())
	{
		UE_LOG(LogTemp, Warning, TEXT("DEBUG"));
		if (WeaponManager->GetCurrentWeapon()->bDebug)
			WeaponManager->GetCurrentWeapon()->bDebug = false;
		else
			WeaponManager->GetCurrentWeapon()->bDebug = true;
	}

	SetEscapeToolNum(2);
}

void ACharacterBase::Detect_S(const FInputActionValue& Value)
{
	if (WeaponManager->GetCurrentWeapon())
	{
		WeaponManager->GetCurrentWeapon()->DetectTool(HitTarget);
		WeaponManager->GetCurrentWeapon()->SetDetectNiagara(true);
	}
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetVelocity().Size() <= 0.f)
		CharacterState = ECharacterState::ECS_IDLE;
	switch (CharacterState)
	{
	case ECharacterState::ECS_IDLE:
		UpdateCameraBoom(DeltaTime);
		break;
	case ECharacterState::ECS_RUN:
		Movement->MaxWalkSpeed = 400.f;
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(UJog::StaticClass());
		UpdateCameraBoom(DeltaTime);
		break;
	case ECharacterState::ECS_SPRINT:
		if (!StaminaExhaustionState)
		{
			if (!bSkillUsing)
			{
				Movement->MaxWalkSpeed = 600.f;
				UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(USprint::StaticClass());
			}
		}
		UpdateCameraBoom(DeltaTime);
		break;
	case ECharacterState::ECS_DEFAULT:
		Movement->MaxWalkSpeed = 400.f;
		UpdateCameraBoom(DeltaTime);
		break;
	}

	if (!Movement->IsFalling())
	{
		bCanJump = true;
	}
	if (bStamina)
		UpdateStamina(DeltaTime);
	AimOffset(DeltaTime);
	AimOffset(DeltaTime);

	if (bCrosshiar)
		SetHUDCrosshair(DeltaTime);
	FHitResult HitResult;
	TraceUnderCrossHiar(HitResult);
	HitTarget = HitResult.ImpactPoint;

	if (!PathSorce->bHiddenInGame)
	{
		PathSorce->SetHiddenInGame(true);
	}

	//캐릭터 디졸브
	if (bDissolve && Health <= 0.f)
	{

		if (MDissolveInst)
		{
			MDynamicDissolveInst = UMaterialInstanceDynamic::Create(MDissolveInst, this);
			if (MDynamicDissolveInst)
			{
				DissolvePercent += DeltaTime / 4;
				GetMesh()->SetMaterial(0, MDynamicDissolveInst);
				MDynamicDissolveInst->SetScalarParameterValue(FName("Dissolve"), DissolvePercent);
			}
		}
	}

	if (MainController)
		MainController->SeverHpSync(bAlive, Health, inst->GetPlayerID());

}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacterBase::Move);// trigger 매 틱마다
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACharacterBase::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacterBase::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacterBase::StopJump);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ACharacterBase::Sprint_S);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACharacterBase::Sprint_E);
		EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Triggered, this, &ACharacterBase::Inter);
		EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Started, this, &ACharacterBase::Inter_Start);
		EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Completed, this, &ACharacterBase::Inter_End);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ACharacterBase::Fire_S);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ACharacterBase::Fire_E);
		EnhancedInputComponent->BindAction(ReRoadAction, ETriggerEvent::Triggered, this, &ACharacterBase::Reroad);
		EnhancedInputComponent->BindAction(GrandeFireAction, ETriggerEvent::Triggered, this, &ACharacterBase::GrandeFire_Aiming);
		EnhancedInputComponent->BindAction(GrandeFireAction, ETriggerEvent::Completed, this, &ACharacterBase::GrandeFire);
		EnhancedInputComponent->BindAction(SelectGrandeAction, ETriggerEvent::Triggered, this, &ACharacterBase::SelectGrande);
		EnhancedInputComponent->BindAction(SelectWallAction, ETriggerEvent::Triggered, this, &ACharacterBase::SelectWall);
		EnhancedInputComponent->BindAction(SelectTrapAction, ETriggerEvent::Triggered, this, &ACharacterBase::SelectTrap);
		EnhancedInputComponent->BindAction(DetectAction, ETriggerEvent::Triggered, this, &ACharacterBase::Detect_S);
		EnhancedInputComponent->BindAction(DetectAction, ETriggerEvent::Completed, this, &ACharacterBase::Detect_E);
		EnhancedInputComponent->BindAction(LightAction, ETriggerEvent::Started, this, &ACharacterBase::LightOnOff);
		EnhancedInputComponent->BindAction(QuitAction, ETriggerEvent::Started, this, &ACharacterBase::Quit);
		EnhancedInputComponent->BindAction(DebugAction, ETriggerEvent::Started, this, &ACharacterBase::OnDebug);
	}
}

void ACharacterBase::SendEnd()
{
	GetWorld()->ServerTravel(FString("/Game/Maps/GameRoom"), false, true);
}


void ACharacterBase::StartGame()
{
	bCrosshiar = true;
	bStamina = true;
	UE_LOG(LogTemp, Warning, TEXT("STARTGAME"));
	Movement->Velocity = FVector::ZeroVector;
	MainController = MainController == nullptr ? Cast<ACharacterController>(Controller) : MainController;
	if (MainController)
	{
		if (inst)
			inst->m_Socket->Send_Start_game_packet(inst->GetPlayerID());
		//MainController->MainHUD->AddToolNumUi();
		//MainController->MainHUD->AddCharacterOverlay();
		MainController->MainHUD->StartSetVisibility();
		// num 계수, name 처리 
		MainController->SetNum();
		MainController->SetChName();
		SetActorTransform(StartTransform);
		//bStarted = false;
		EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		SetWeaponUi();
		if (inst)
			inst->m_Socket->bName = true;
	}

}

void ACharacterBase::Server_PlayDeadAnim()
{
	PlayAnimMontage(DeadMontage);
}
