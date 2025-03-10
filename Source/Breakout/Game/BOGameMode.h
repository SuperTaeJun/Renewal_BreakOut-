#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BOGameMode.generated.h"

UCLASS()
class BREAKOUT_API ABOGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABOGameMode();
	virtual void BeginPlay() override;
	void StartCineWithBindingFinishFunc();
	virtual void Tick(float DeltaTime) override;

	float StartTime = 10.f;
	float CountdownTime = 10.f;

	void SetDamageInsigator(class ACharacterBase* DamageInsigatorCh);
	class ACharacterBase* GetDamageInsigator() { return  DamageInsigator; }

	void Respawn(class ACharacter* RespawnedCh, class AController* RespawnedController, FName TagName);
	//캐릭터 선택한거 적용
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	//맨처음 캐릭터 시작위치 지정
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	TSubclassOf<class ACharacterBase>Character1;
	TSubclassOf<class ACharacterBase>Character2;
	TSubclassOf<class ACharacterBase>Character3;
	TSubclassOf<class ACharacterBase>Character4;

	FTimerHandle StartTimeHandle;
	UFUNCTION()
	void StartGame();
	bool bStarted;
	class UBOGameInstance* inst;
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AEscapeTool> ItemSpawn;

private:
	TObjectPtr<class ACharacterBase> DamageInsigator;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> BackGroundSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class ULevelSequence> EndGameCine;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class ULevelSequence> StartGameCine;
};
