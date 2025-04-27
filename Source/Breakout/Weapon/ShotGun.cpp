// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ShotGun.h"
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

AShotGun::AShotGun()
{
}

void AShotGun::FireInternal(const FVector& HitTarget)
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return;

    const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(TEXT("MuzzleFlash"));
    if (!MuzzleSocket) return;

    FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
    FVector Start = SocketTransform.GetLocation();

    UWorld* World = GetWorld();
    if (!World) return;

    for (uint32 i = 0; i < NumberOfPellets; i++)
    {
        FVector EndPoint = WeaponData.bUseScatter ? TraceEndWithScatter(Start, HitTarget) : (HitTarget);
        FVector Direction = EndPoint - Start;
        FRotator PelletRotation = Direction.Rotation();

        if (ProjectileBulletClass)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = OwnerPawn;
            SpawnParams.Instigator = OwnerPawn;

            AProjectileBullet* Pellet = World->SpawnActor<AProjectileBullet>(ProjectileBulletClass, Start, PelletRotation, SpawnParams);
            if (Pellet)
            {
                Pellet->SetOwner(OwnerPawn);
            }
        }
    }
}

void AShotGun::SendFirePacket()
{
    ACharacterBase* OwnerCharacter = Cast<ACharacterBase>(GetOwner());
    if (!OwnerCharacter) return;

    const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(TEXT("MuzzleFlash"));
    if (!MuzzleSocket) return;

    FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
    FVector Start = SocketTransform.GetLocation();

    TArray<FRotator> PelletRotations;

    for (uint32 i = 0; i < NumberOfPellets; i++)
    {
        FVector EndPoint = WeaponData.bUseScatter ? TraceEndWithScatter(Start, Start + FVector(1, 0, 0)) : (Start + FVector(1, 0, 0));
        FVector Direction = EndPoint - Start;
        FRotator PelletRotation = Direction.Rotation();
        PelletRotations.Add(PelletRotation);
    }

    if (UBOGameInstance* GI = Cast<UBOGameInstance>(GetGameInstance()))
    {
        if (GI->m_Socket)
        {
            GI->m_Socket->Send_ShotGun_packet(OwnerCharacter->_SessionId, Start, PelletRotations, PelletRotations.Num());
        }
    }
}