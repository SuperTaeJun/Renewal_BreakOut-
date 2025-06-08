#include "Components/SubWeaponManagerComponent.h"
#include "Player/CharacterController.h"
#include "GameFramework/Actor.h"
#include "Character/CharacterBase.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "Weapon/ProjectileBase.h"
#include "Weapon/WeaponBase.h"
#include "Animatiom/BOAnimInstance.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Game/BOGameInstance.h"

USubWeaponManagerComponent::USubWeaponManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USubWeaponManagerComponent::InitializeSubWeapon(UStaticMeshComponent* InGrenadeMesh, UArrowComponent* InPathSource, UNiagaraComponent* InAimComp, UCameraComponent* InFollowCam)
{
	GrenadeMesh = InGrenadeMesh;
	PathSource = InPathSource;
	Aim = InAimComp;
	FollowCamera = InFollowCam;
}

void USubWeaponManagerComponent::PredictPath(ACharacterBase* OwnerChar)
{
	if (!OwnerChar || !OwnerChar->GetWeaponManager()->GetCurrentWeapon()) return;
	if (!Aim || !PathSource || !FollowCamera) return;

	Aim->Activate();

	FPredictProjectilePathParams Params;
	Params.StartLocation = PathSource->GetComponentLocation();
	Params.LaunchVelocity = FollowCamera->GetForwardVector() * 1500.f;
	Params.ProjectileRadius = 3.f;
	Params.bTraceWithCollision = true;
	Params.ActorsToIgnore.Add(OwnerChar);

	FPredictProjectilePathResult Result;
	UGameplayStatics::PredictProjectilePath(OwnerChar, Params, Result);

	TArray<FVector> Locs;
	for (auto& D : Result.PathData) Locs.Add(D.Location);

	if (Locs.Num())
	{
		SWAimLastLoc = Locs.Last();
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(
			Aim, FName("PositionArray"), Locs);
	}
}

void USubWeaponManagerComponent::StartThrow(ACharacterBase* OwnerChar)
{
	if (!OwnerChar || !OwnerChar->GetWeaponManager()->GetCurrentWeapon()) return;
	if (Aim) Aim->Deactivate();

	// BoobyTrap 즉시 스폰
	if (CurrentSubWeaponType == EBojoMugiType::E_BoobyTrap)
	{
		SpawnProjectile(OwnerChar);
		return;
	}

	// 그 외: 투척 애니메이션
	if (auto Weapon = OwnerChar->GetWeaponManager()->GetCurrentWeapon())
		Weapon->SetActorHiddenInGame(true);

	if (GrenadeMesh) GrenadeMesh->SetHiddenInGame(false);

	if (auto AnimInst = OwnerChar->GetMesh()->GetAnimInstance())
	{
		// 몽타주 재생 + 서버 패킷
		if (OwnerChar->inst)
			OwnerChar->inst->m_Socket->Send_BojoAnim_packet(OwnerChar->inst->GetPlayerID(), 0);

		OwnerChar->PlayAnimMontage(GrenadeMontage, 1.5f);
		OwnerChar->GetWeaponManager()->bCanFire = false;
	}
}

void USubWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentSubWeaponType = EBojoMugiType::E_Grenade;
	// 캐릭터 컨트롤러 캐싱
	if (ACharacterBase* OwnerChar = Cast<ACharacterBase>(GetOwner()))
	{
		MainController = Cast<ACharacterController>(OwnerChar->GetController());
		MainController->SetHUDBojoImage(CurrentSubWeaponType);
	}
}

void USubWeaponManagerComponent::SpawnProjectile(ACharacterBase* OwnerChar)
{
	if (!OwnerChar || !GrenadeMesh) return;

	UWorld* World = OwnerChar->GetWorld();
	if (!World) return;

	const FVector StartLocation = OwnerChar->GetMesh()->GetSocketLocation(FName("GrandeSocket"));
	FVector ToHitTarget = OwnerChar->GetHitTarget() - StartLocation;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerChar;
	SpawnParams.Instigator = OwnerChar;

	switch (CurrentSubWeaponType)
	{
	case EBojoMugiType::E_Grenade:
		if (GrenadeNum > 0 && GrenadeClass)
		{
			--GrenadeNum;
			World->SpawnActor<AProjectileBase>(GrenadeClass, StartLocation, ToHitTarget.Rotation(), SpawnParams);

			if (OwnerChar->inst)
			{
				OwnerChar->inst->m_Socket->Send_BojoWeapon_packet(
					OwnerChar->inst->GetPlayerID(),
					StartLocation,
					ToHitTarget.Rotation(),
					0  // 수류탄
				);
			}
		}
		break;

	case EBojoMugiType::E_Wall:
		if (WallNum > 0 && WallClass)
		{
			--WallNum;
			World->SpawnActor<AProjectileBase>(WallClass, StartLocation, ToHitTarget.Rotation(), SpawnParams);

			if (OwnerChar->inst)
			{
				OwnerChar->inst->m_Socket->Send_BojoWeapon_packet(
					OwnerChar->inst->GetPlayerID(),
					StartLocation,
					ToHitTarget.Rotation(),
					1  // 벽
				);
			}
		}
		break;
	case EBojoMugiType::E_BoobyTrap:
		if (TrapNum > 0 && BoobyTrapClass)
		{
			--TrapNum;
			World->SpawnActor<AProjectileBase>(BoobyTrapClass, SWAimLastLoc, FRotator::ZeroRotator, SpawnParams);

			if (OwnerChar->inst)
			{
				OwnerChar->inst->m_Socket->Send_BojoWeapon_packet(
					OwnerChar->inst->GetPlayerID(),
					SWAimLastLoc,
					FRotator::ZeroRotator,
					2
				);
			}
		}
		break;
	default:
		break;
	}
}

void USubWeaponManagerComponent::FinishThrow(ACharacterBase* OwnerChar)
{
	// 원래 GrandeThrowFinish 내용 전부 여기
	if (auto AnimInst = OwnerChar->GetMesh()->GetAnimInstance())
		Cast<UBOAnimInstance>(AnimInst)->bUseLeftHand = true;

	if (auto Weapon = OwnerChar->GetWeaponManager()->GetCurrentWeapon())
	{
		Weapon->SetActorHiddenInGame(false);
		OwnerChar->GetWeaponManager()->bCanFire = true;
		if (auto* Socket = OwnerChar->GetMesh()->GetSocketByName(RightHandSocket))
			Socket->AttachActor(Weapon, OwnerChar->GetMesh());
	}

	if (GrenadeMesh)
		GrenadeMesh->SetHiddenInGame(true);

	// HUD 갱신
	if (auto PC = Cast<ACharacterController>(OwnerChar->GetController()))
		PC->SetHUDBojoImage(CurrentSubWeaponType);
}

void USubWeaponManagerComponent::SelectSubWeapon(EBojoMugiType NewType)
{
	CurrentSubWeaponType = NewType;

	if (MainController.IsValid())
	{
		MainController->SetHUDBojoImage(CurrentSubWeaponType);
	}
}


