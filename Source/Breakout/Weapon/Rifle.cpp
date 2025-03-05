// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Rifle.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Character/CharacterBase.h"
#include "ClientSocket.h"
#include "DrawDebugHelpers.h"
#include "Game/BOGameInstance.h"
#include "GameProp/BulletHoleWall.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Weapon/ProjectileBullet.h"

void ARifle::Fire(const FVector& HitTarget)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;

	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleSocket)
	{
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		FVector ToTarget = bUseScatter ? TraceEndWithScatter(Start, HitTarget) : Start + (HitTarget - Start) * 1.25f;
		ToTarget = ToTarget - SocketTransform.GetLocation();

		FRotator ToTargetRot = ToTarget.Rotation();
		if (ProjectileBulletClass && OwnerPawn)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = OwnerPawn;
			SpawnParameters.Instigator = OwnerPawn;
			UWorld* World = GetWorld();
			AProjectileBullet* FiredBullet = nullptr;
			if (World)
			{
				FiredBullet = World->SpawnActor<AProjectileBullet>(ProjectileBulletClass, SocketTransform.GetLocation(), ToTargetRot, SpawnParameters);

				Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_Fire_Effect(
					Cast<ACharacterBase>(GetOwner())->_SessionId, SocketTransform.GetLocation(), ToTargetRot, 0
				);
				FiredBullet->SetOwner(OwnerPawn);
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
