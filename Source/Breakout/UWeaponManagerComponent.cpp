#include "UWeaponManagerComponent.h"
#include "GameFramework/Character.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapon/WeaponBase.h"
#include "Character/CharacterBase.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "CameraShake/Shoot.h"
#include "Player/CharacterController.h"
#include "Game//BOGameInstance.h"
#include "Sound/SoundCue.h"

UUWeaponManagerComponent::UUWeaponManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UUWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	CurrentWeaponType = EWeaponType::ECS_DEFAULT;
}

void UUWeaponManagerComponent::EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass, FName SocketName)
{
	if (!WeaponClass || !OwnerCharacter.IsValid()) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Cast<AActor>(OwnerCharacter.Get());
	SpawnParams.Instigator = Cast<APawn>(OwnerCharacter.Get());

	AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);
	if (NewWeapon)
	{
		CurrentWeapon = NewWeapon;
		AttachSocketName = SocketName;
		USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
		if (Mesh)
		{
			const USkeletalMeshSocket* Socket = Mesh->GetSocketByName(SocketName);
			if (Socket)
			{
				Socket->AttachActor(CurrentWeapon, Mesh);
			}
		}
	}
}

void UUWeaponManagerComponent::HandleFirePressed()
{
	bFirePressed = true;
	if (!CanFire()) return;
	FireWeapon(GetCharacterHitTarget());
	StartFireTimer();
}

void UUWeaponManagerComponent::HandleFireReleased()
{
	bFirePressed = false;
}

bool UUWeaponManagerComponent::CanFire() const
{
	return bCanFire && CurrentWeapon && CurrentWeapon->GetCurAmmo() > 0 && !IsSprinting();
}

bool UUWeaponManagerComponent::IsSprinting() const
{
	if (ACharacterBase* Character = Cast<ACharacterBase>(OwnerCharacter.Get()))
	{
		return Character->GetCharacterState() == ECharacterState::ECS_SPRINT;
	}
	return false;
}

FVector UUWeaponManagerComponent::GetCharacterHitTarget() const
{
	if (ACharacterBase* Character = Cast<ACharacterBase>(OwnerCharacter.Get()))
	{
		return Character->GetHitTarget();
	}
	return FVector::ZeroVector;
}

void UUWeaponManagerComponent::FireWeapon(const FVector& HitTarget)
{
	if (!CurrentWeapon || !OwnerCharacter.IsValid()) return;

	bCanFire = false;
	CurrentWeapon->Fire(HitTarget);

	ACharacterBase* Character = Cast<ACharacterBase>(OwnerCharacter.Get());
	if (Character)
	{
		Character->PlayAnimMontage(FireActionMontage);
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(UShoot::StaticClass());

		switch (CurrentWeaponType)
		{
		case EWeaponType::E_Rifle:
			Character->AddControllerPitchInput(-0.7f);
			break;
		case EWeaponType::E_Shotgun:
			Character->AddControllerPitchInput(-3.f);
			break;
		case EWeaponType::E_Launcher:
			Character->AddControllerPitchInput(-6.f);
			break;
		default:
			break;
		}

		if (ACharacterController* Controller = Cast<ACharacterController>(Character->GetController()))
		{
			Controller->SetHUDAmmo(CurrentWeapon->GetCurAmmo());
		}
	}
}

void UUWeaponManagerComponent::StartFireTimer()
{
	if (!CurrentWeapon) return;

	const float FireRate = CurrentWeapon->GetWeaponData().FireRate;
	GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &UUWeaponManagerComponent::FireTimerFinished, FireRate, false);
}

void UUWeaponManagerComponent::FireTimerFinished()
{
	bCanFire = true;
	if (bFirePressed)
	{
		HandleFirePressed();
	}
}

void UUWeaponManagerComponent::ReloadWeaponWithAnimation()
{
	if (!OwnerCharacter.IsValid() || !CurrentWeapon) return;

	ACharacterBase* Character = Cast<ACharacterBase>(OwnerCharacter.Get());
	if (!Character) return;

	if (ReloadMontage)
	{
		Character->PlayAnimMontage(ReloadMontage);
	}

	if (CurrentWeapon->GetReloadSound())
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentWeapon->GetReloadSound(), OwnerCharacter->GetActorLocation());
	}

	if (UBOGameInstance* Inst = Cast<UBOGameInstance>(Character->GetGameInstance()))
	{
		Inst->m_Socket->Send_Reload_packet(Inst->GetPlayerID(), true);
	}

	bCanFire = false;
}

void UUWeaponManagerComponent::FinishReload()
{
	if (!CurrentWeapon) return;

	CurrentWeapon->Reload();
	bCanFire = true;

	if (ACharacterBase* Character = Cast<ACharacterBase>(OwnerCharacter.Get()))
	{
		if (ACharacterController* Controller = Cast<ACharacterController>(Character->GetController()))
		{
			Controller->SetHUDAmmo(CurrentWeapon->GetCurAmmo());
		}
	}
}

void UUWeaponManagerComponent::DestroyCurrentWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
}

void UUWeaponManagerComponent::UpdateAmmoHUD()
{
	if (!CurrentWeapon || !OwnerCharacter.IsValid()) return;

	ACharacterBase* Character = Cast<ACharacterBase>(OwnerCharacter.Get());
	if (ACharacterController* Controller = Cast<ACharacterController>(Character->GetController()))
	{
		Controller->SetHUDAmmo(CurrentWeapon->GetCurAmmo());
	}
}
