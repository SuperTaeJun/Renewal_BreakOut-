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


void ARifle::FireInternal(const FVector& HitTarget)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(TEXT("MuzzleFlash"));
	if (!MuzzleSocket) return;

	FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
	FVector Start = SocketTransform.GetLocation();
	FVector Direction = WeaponData.bUseScatter ? TraceEndWithScatter(Start, HitTarget) - Start : (HitTarget - Start) * 1.25f;
	FRotator ProjectileRotation = Direction.Rotation();

	if (ProjectileBulletClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerPawn;
		SpawnParams.Instigator = OwnerPawn;

		AProjectileBullet* Bullet = GetWorld()->SpawnActor<AProjectileBullet>(ProjectileBulletClass, Start, ProjectileRotation, SpawnParams);
		if (Bullet)
		{
			Bullet->SetOwner(OwnerPawn);
		}
	}
}

void ARifle::SendFirePacket()
{
	ACharacterBase* OwnerCharacter = Cast<ACharacterBase>(GetOwner());
	if (!OwnerCharacter) return;

	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(TEXT("MuzzleFlash"));
	if (!MuzzleSocket) return;

	FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
	FVector Start = SocketTransform.GetLocation();
	FVector Direction = WeaponData.bUseScatter ? TraceEndWithScatter(Start, Start + FVector(1, 0, 0)) - Start : FVector(1, 0, 0);
	FRotator ProjectileRotation = Direction.Rotation();

	if (UBOGameInstance* GI = Cast<UBOGameInstance>(GetGameInstance()))
	{
		if (GI->m_Socket)
		{
			GI->m_Socket->Send_Fire_Effect(OwnerCharacter->_SessionId, Start, ProjectileRotation, 0);
		}
	}
}
