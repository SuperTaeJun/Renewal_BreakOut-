// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "particles/ParticleSystemComponent.h"
#include "Character/CharacterBase.h"
#include "GameProp/BulletHoleWall.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include"GameProp/EscapeTool.h"
#include "Game/BOGameInstance.h"
#include "ClientSocket.h"
#include "Weapon/ProjectileBullet.h"
#include "Sound/SoundCue.h"
#include "Components/SpotLightComponent.h"
//#define TRACE_LENGTH 1000.f

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	DetectNiagara  =CreateDefaultSubobject<UNiagaraComponent>(TEXT("DetectNiagara"));
	DetectNiagara->SetupAttachment(RootComponent);
	DetectNiagara->SetAsset(ConstructorHelpers::FObjectFinder<UNiagaraSystem>(TEXT("/Script/Niagara.NiagaraSystem'/Game/Niagara/Weapon/Detect/NewNiagaraSystem.NewNiagaraSystem'")).Object);
	DetectNiagara->SetAutoActivate(false);

	DetectNiagara->SetWorldRotation(FRotator(90.f, 90.f, 0).Quaternion());
	DetectNiagara->SetWorldLocation(FVector(0.f, 580.f, 0.f));

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(RootComponent);
	SpotLight->SetVisibility(false);
}


// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	SetWeaponData();
}
void AWeaponBase::PlayFireEffect()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}
void AWeaponBase::SendFirePacket()
{
}
void AWeaponBase::Reload()
{
	if (CurAmmo >= WeaponData.MaxAmmo)
	{
		UE_LOG(LogTemp, Warning, TEXT("Already full ammo!"));
		return;
	}

	CurAmmo = WeaponData.MaxAmmo;
}
void AWeaponBase::SetWeaponData()
{

	if (WeaponDataTable)
	{
		const FWeaponData* LoadedData = WeaponDataTable->FindRow<FWeaponData>(WeaponDataID, TEXT("WeaponData Lookup"));

		if (LoadedData)
		{
			WeaponData = *LoadedData; // บนป็
			UE_LOG(LogTemp, Log, TEXT("Weapon data loaded: %s"), *WeaponDataID.ToString());
			CurAmmo = WeaponData.MaxAmmo;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Weapon data not found for ID: %s"), *WeaponDataID.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponDataTable not assigned!"));
	}

}

void AWeaponBase::Fire(const FVector& HitTarget)
{
	if (CurAmmo <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Ammo left!"));
		return;
	}

	CurAmmo--;

	PlayFireEffect();
	SendFirePacket();
	FireInternal(HitTarget);
}

FVector AWeaponBase::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	FVector DirectionToTarget = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + DirectionToTarget * WeaponData.DistanceToSphere;
	FVector RandVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, WeaponData.SphereRadius);
	FVector EndLoc = SphereCenter + RandVector;
	FVector DirToEndLoc = EndLoc - TraceStart;

	return FVector(TraceStart + DirToEndLoc);
}

void AWeaponBase::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (!World) return;

	FVector End = WeaponData.bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) : (TraceStart + (HitTarget - TraceStart) * 1.25f);

	World->LineTraceSingleByChannel(OutHit, TraceStart, End, ECC_Visibility);
}

void AWeaponBase::SetDetectNiagara(bool bUse)
{
	if (DetectNiagara)
	{
		bUse ? DetectNiagara->Activate() : DetectNiagara->Deactivate();
	}
}

void AWeaponBase::DetectTool(FVector& HitRes)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	const USkeletalMeshSocket* MuzzleSocket = WeaponMesh->GetSocketByName(TEXT("MuzzleFlash"));
	if (!MuzzleSocket) return;

	FVector Start = MuzzleSocket->GetSocketTransform(WeaponMesh).GetLocation();
	FHitResult DetectHit;
	TArray<AActor*> IgnoredActors;

	UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		Start,
		Start + (HitRes - Start) * 0.5f,
		30.f,
		ETraceTypeQuery::TraceTypeQuery1,
		true,
		IgnoredActors,
		EDrawDebugTrace::None,
		DetectHit,
		true
	);

	if (DetectHit.bBlockingHit)
	{
		if (ImpactNiagara)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactNiagara, DetectHit.ImpactPoint);
		}
	}
}


