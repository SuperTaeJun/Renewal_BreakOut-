// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BOGameInstance.generated.h"

enum class ECharacterType : uint8
{
	ECharacter1 UMETA(DisplayName = "Character1"),
	ECharacter2 UMETA(DisplayName = "Character2"),
	ECharacter3 UMETA(DisplayName = "Character3"),
	ECharacter4 UMETA(DisplayName = "Character4"),
};

UCLASS()
class RENEWAL_BREAKOUT_API UBOGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
public:
	class ClientSocket* m_Socket;
	bool connect;
	int playerID;

	bool bNoUI = false;

	ECharacterType CharacterType;

	void SetCharacterType(ECharacterType _CharacterType) { CharacterType = _CharacterType; }
	ECharacterType GetCharacterType() { return CharacterType; }
	inline void SetPlayerID(int id) { playerID = id; }
	inline int GetPlayerID() { return playerID; }

};
