// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RocketLauncher.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapon/ProjectileBase.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Game/BOGameInstance.h"
#include "ClientSocket.h"
#include "Character/CharacterBase.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

ARocketLauncher::ARocketLauncher()
{
}

void ARocketLauncher::Fire(const FVector& HitTarget)
{
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket && InstigatorPawn)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		StartPos = SocketTransform.GetLocation();
		CurWeaponRot = TargetRotation;
		if (ProjectileLancherClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = GetOwner();
			SpawnParameters.Instigator = InstigatorPawn;
			SpawnParm = SpawnParameters;
			UWorld* World = GetWorld();
			AProjectileBase* Projectile = nullptr;
			if (World)
			{
				Projectile=World->SpawnActor<AProjectileBase>(ProjectileLancherClass, SocketTransform.GetLocation(), TargetRotation, SpawnParameters);
				Projectile->SetOwner(GetOwner());
				if (Cast<UBOGameInstance>(GetGameInstance()))
					Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_Fire_Effect(Cast<ACharacterBase>(GetOwner())->_SessionId, SocketTransform.GetLocation(), TargetRotation, 1);
			}
			if (ImpactNiagara)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation
				(
					World,
					ImpactNiagara,
					SocketTransform.GetLocation()
				);
			}

			if (FireSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					FireSound,
					SocketTransform.GetLocation()
				);
			}


		}

	}
}
