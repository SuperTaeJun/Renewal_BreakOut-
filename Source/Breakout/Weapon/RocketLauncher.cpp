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

void ARocketLauncher::FireInternal(const FVector& HitTarget)
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return;

    const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(TEXT("MuzzleFlash"));
    if (!MuzzleSocket) return;

    FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
    FVector Start = SocketTransform.GetLocation();
    FVector ToTarget = HitTarget - Start;
    FRotator LaunchRotation = ToTarget.Rotation();

    UWorld* World = GetWorld();
    if (World && ProjectileLauncherClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = OwnerPawn;
        SpawnParams.Instigator = OwnerPawn;

        AProjectileBase* Rocket = World->SpawnActor<AProjectileBase>(ProjectileLauncherClass, Start, LaunchRotation, SpawnParams);
        if (Rocket)
        {
            Rocket->SetOwner(OwnerPawn);
        }

        if (ImpactNiagara)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, ImpactNiagara, Start);
        }
    }
}

void ARocketLauncher::SendFirePacket()
{
    ACharacterBase* OwnerCharacter = Cast<ACharacterBase>(GetOwner());
    if (!OwnerCharacter) return;

    const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(TEXT("MuzzleFlash"));
    if (!MuzzleSocket) return;

    FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
    FVector Start = SocketTransform.GetLocation();
    FVector Direction = FVector(1, 0, 0); // 기본 방향 (가짜로 하나 넣음)
    FRotator LaunchRotation = Direction.Rotation();

    if (UBOGameInstance* GI = Cast<UBOGameInstance>(GetGameInstance()))
    {
        if (GI->m_Socket)
        {
            GI->m_Socket->Send_Fire_Effect(OwnerCharacter->_SessionId, Start, LaunchRotation, 1); // Rocket은 EffectType = 1
        }
    }
}