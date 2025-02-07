#include "Character/PlayerCombatComp.h"
#include "Character/CharacterBase.h"
#include "Weapon/WeaponBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"

UPlayerCombatComp::UPlayerCombatComp()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPlayerCombatComp::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<ACharacterBase>(GetOwner());
}

void UPlayerCombatComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateHitTarget();
}

void UPlayerCombatComp::UpdateHitTarget()
{
	FHitResult HitResult;
	TraceUnderCrossHiar(HitResult);
	HitTarget = HitResult.ImpactPoint;
}

void UPlayerCombatComp::TraceUnderCrossHiar(FHitResult& TraceHitResult)
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
		if (Player)
		{
			float DistanceToCharacter = (Player->GetActorLocation() - Start).Size();
			Start += CrossHairDirection * (DistanceToCharacter + 100.f);
		}

		if (CurrentWeapon)
		{
			FVector End = Start + CrossHairDirection * CurrentWeapon->GetRange();

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

void UPlayerCombatComp::Fire()
{
	CurrentWeapon->Fire(HitTarget);
}


void UPlayerCombatComp::SetWeapon(TSubclassOf<class AWeaponBase> Weapon, FName SocketName)
{
	if (!CurrentWeapon)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = Player;
		SpawnParameters.Instigator = Player;
		AActor* SpawnWeapon = GetWorld()->SpawnActor<AWeaponBase>(Weapon, SpawnParameters);
		CurrentWeapon = Cast<AWeaponBase>(SpawnWeapon);

		const USkeletalMeshSocket* WeaponSocket = Player->GetMesh()->GetSocketByName(SocketName);


		if (WeaponSocket && CurrentWeapon)
		{
			WeaponSocket->AttachActor(CurrentWeapon, Player->GetMesh());
			CurrentWeapon->SetOwner(Player);
		}

	}

}
