// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileWall.h"
#include "GameProp/Wall.h"
#include "Kismet/GameplayStatics.h"
#include "GameProp/BulletHoleWall.h"
#include "Game/BOGameInstance.h"
#include "ClientSocket.h"
#include"Character/CharacterBase.h"
AProjectileWall::AProjectileWall()
{
	ImpactNiagara = nullptr;

}
void AProjectileWall::BeginPlay()
{
	Super::BeginPlay();
	inst = Cast<UBOGameInstance>(GetGameInstance());
}

void AProjectileWall::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NomalImpulse, const FHitResult& Hit)
{
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());


	if (WallClass && InstigatorPawn!=OtherActor)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = GetOwner();
		SpawnParameters.Instigator = InstigatorPawn;

		FRotator Rotation = InstigatorPawn->GetActorForwardVector().Rotation();
		UWorld* World = GetWorld();
		if (World)
		{
			FVector ConvertLoc = Hit.Location;
			ConvertLoc.Z += 100.f;
			TArray<AActor*> Walls;
			UGameplayStatics::GetAllActorsOfClass(World, WallClass, Walls);

			for (auto wall:Walls)
			{
				ABulletHoleWall* BulletWall = Cast<ABulletHoleWall>(wall);
				if (BulletWall && BulletWall->bUsing)
				{
					//�� ���� ��Ŷ BulletWall->ID�� ConvertLoc, Rotation �̰� 3�� ��Ŷ���� ������ ���Ŀ� �ؿ� 2�� ��Ʈ�ѷ����� ����
					if(inst && Cast<ACharacterBase>(InstigatorPawn)->GetMainController())
						inst->m_Socket->Send_BulletWall_packet(inst->GetPlayerID(), BulletWall->ID, ConvertLoc, Rotation);

					BulletWall->SetActorLocationAndRotation(ConvertLoc, Rotation);
					BulletWall->bUsing = false;
					Destroy();
					return;
				}
				else
				{
					Destroy();
				}
			}

			//World->SpawnActor<ABulletHoleWall>(WallClass, ConvertLoc, Rotation, SpawnParameters);
		}


	}

}
