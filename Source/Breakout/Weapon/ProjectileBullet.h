// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBullet.generated.h"


UCLASS()
class BREAKOUT_API AProjectileBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectileBullet();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	virtual void OnHit
	(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
		FVector NomalImpulse, const FHitResult& Hit);
	virtual void Destroyed() override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraComponent> BeamNiagaraMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem>ImpactNiagara;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> ImpactSound;

	UPROPERTY(EditAnywhere)
	float Damage=10.f;

	UPROPERTY(EditAnywhere)
	//�Ѿ��� �̵��ӵ� �������� ������� �ð��� ����� // �����ð� ���� ��� 100m/s�� ���ư� ex) ���Ͻ��� 2��� ��Ÿ��� 200m
	float Distance = 2.f; 

	FTimerHandle DistanceTimerHandle;
	void DistanceTimer();

	bool bHit = false;
};
