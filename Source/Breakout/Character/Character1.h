// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Character1.generated.h"

/**
 *
 */

struct FCharacterFrameData
{

	FVector Location;
	float DeltaTime;

	FCharacterFrameData()
	{
	};
	FCharacterFrameData(FVector _Location, float _DeltaTime)
	{
		Location = _Location;
		DeltaTime = _DeltaTime;
	};
};

UCLASS()
class BREAKOUT_API ACharacter1 : public ACharacterBase
{
	GENERATED_BODY()

public:
	ACharacter1();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

protected:
	virtual void Destroyed();

	virtual void Skill_S(const FInputActionValue& Value) override;
	virtual void Skill_E(const FInputActionValue& Value) override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem> TimeReplayNiagara;
	float NiagaraSpawnRate = 0.15;
	float NiagaraSpawnSavedTime;
	//�ִ� ����ð�
	UPROPERTY(EditAnywhere)
	float MaxSaveTime = 15.f;
private:

	TDoubleLinkedList<FCharacterFrameData> FrameDatas;
	void StoreFrameData(float DeltaTime);
	void Replay(float DeltaTime);
	bool bTimeReplay = false;
	//out of time data, cannot keep replay
	bool bOutOfData;
	//������ ����Ƚð�
	float RunningTime;
	//�������Ӹ����� ��ŸŸ��
	float LeftRunningTime;
	float RightRunningTime;
	//����� �������Ӹ��ٿ��� ��ϵ� ��ü�ð� 
	float RecordedTime;
	float Temp = 0.2f;

	bool bCoolTimeFinish = true;
	float RecordedCoolTime = 0.f;

	void UpdateHUDCool(float Cool, float MaxCool);


};