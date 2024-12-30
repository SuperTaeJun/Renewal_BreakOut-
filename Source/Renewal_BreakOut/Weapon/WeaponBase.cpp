// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshFinder(TEXT("/Game/BreakoutAsset/Weapon/Rifle/Rifle.Rifle"));
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetSkeletalMeshAsset(SkeletalMeshFinder.Object);
	SetRootComponent(WeaponMesh);

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(RootComponent);
	SpotLight->SetVisibility(false);

}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponBase::Fire(const FVector& HitTarget)
{

	UE_LOG(LogTemp, Warning, TEXT("Fire!!!!"));

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
		//if (ProjectileBulletClass && OwnerPawn)
		//{
		//	//FActorSpawnParameters SpawnParameters;
		//	//SpawnParameters.Owner = OwnerPawn;
		//	//SpawnParameters.Instigator = OwnerPawn;
		//	UWorld* World = GetWorld();
		//	//AProjectileBullet* FiredBullet = nullptr;
		//	if (World)
		//	{
		//		//FiredBullet = World->SpawnActor<AProjectileBullet>(ProjectileBulletClass, SocketTransform.GetLocation(), ToTargetRot, SpawnParameters);

		//		//Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_Fire_Effect(
		//		//	Cast<ACharacterBase>(GetOwner())->_SessionId, SocketTransform.GetLocation(), ToTargetRot, 0
		//		//);
		//		//FiredBullet->SetOwner(OwnerPawn);
		//	}
		//	if (FireSound)
		//	{
		//		UGameplayStatics::PlaySoundAtLocation(
		//			this,
		//			FireSound,
		//			SocketTransform.GetLocation()
		//		);
		//	}

		//}
	}
}

FVector AWeaponBase::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	// 목표 위치와 시작 위치의 방향을 정규화
	FVector DirectionToTarget = (HitTarget - TraceStart).GetSafeNormal();
	// 시작 위치에서 목표 방향으로 DistanceToSphere만큼 이동한 지점을 구의 중심으로 설정합니다.
	FVector SphereCenter = TraceStart + DirectionToTarget * DistanceToSphere;
	// 구의 내부에서 난수로 생성된 벡터를 계산하여 랜덤 위치를 만듭니다.
	FVector RandVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	// 구의 중심에 난수 벡터를 추가하여 최종 위치를 계산합니다.
	FVector EndLoc = SphereCenter + RandVector;
	// 시작 위치에서 최종 위치까지의 벡터를 계산합니다.
	FVector DirToEndLoc = EndLoc - TraceStart;

	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, false, 0.5f);
		DrawDebugSphere(GetWorld(), EndLoc, 4.f, 12, FColor::Orange, false, 0.5f);
		DrawDebugLine(
			GetWorld(),
			TraceStart,
			FVector(TraceStart + DirToEndLoc * 1000.f / DirToEndLoc.Size()),
			FColor::Cyan,
			false,
			0.5f);
	}

	return FVector(TraceStart + DirToEndLoc);
}

void AWeaponBase::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector End = bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;

		World->LineTraceSingleByChannel
		(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility
		);
		if (bDebug)
		{
			DrawDebugLine
			(
				World,
				TraceStart,
				End,
				FColor::Cyan,
				false,
				0.5f
			);
		}
		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
	}
}

