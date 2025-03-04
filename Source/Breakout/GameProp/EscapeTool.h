// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameProp/PropBase.h"
#include "EscapeTool.generated.h"

/**
 * 
 */
UCLASS()
class BREAKOUT_API AEscapeTool : public APropBase
{
	GENERATED_BODY()
	
public:
	AEscapeTool();
	UPROPERTY(EditAnywhere)
	int ItemID = -1;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime);
	virtual void Destroyed() override;
	UFUNCTION(BlueprintCallable)
	void TransformMesh(float DeltaTime, bool Clamp, bool TransformReverse);
	void SetHideMesh();

	void SetbDetected(bool _bDetected) { bDetected = _bDetected; }
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> MeshA;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> MeshB;

	int32 LoopNum = 0;
	int32 RandNum = 0;

	MeshData DataA;
	MeshData DataB;
	MeshData InterpData;


	float Time = 0.f;
	float MorphingSpeed = 0.3f;
	float Cur = 0.f;

	bool bDetected;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UMaterialInstance> OldMaterial;
	TObjectPtr<class UMaterialInstanceDynamic> DynamicMaterial;

	void SetProcMesh();

	UFUNCTION()
	virtual void OnSphereOverlap
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION()
	void OnSphereEndOverlap
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	TObjectPtr<class ACharacterBase> OverlapedCharacter;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UWidgetComponent> PercentBar;

	void UpdatePercent(float Percent);
	class UBOGameInstance* inst;
public:
	//0 = �⺻����
	//1 = a->b�� �������� ����
	//2 = b->a�� �ٽ� ���ư��� ����
	//�̳�Ŭ������ �ٽ� ����� 
	int32 bOverlap=0;
};
