// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/RespawnSelect.h"
#include "Components/Button.h"
#include "Character/CharacterBase.h"
#include "Player/CharacterController.h"
#include "GameFramework/PlayerController.h"
#include "MainHUD.h"
#include"Game/BOGameMode.h"
#include "Sound/SoundCue.h"

void URespawnSelect::NativeConstruct()
{
	Super::NativeConstruct();

	Respawn1->OnClicked.AddDynamic(this, &URespawnSelect::Respawn1Pressed);
	Respawn2->OnClicked.AddDynamic(this, &URespawnSelect::Respawn2Pressed);
	Respawn3->OnClicked.AddDynamic(this, &URespawnSelect::Respawn3Pressed);
	Respawn4->OnClicked.AddDynamic(this, &URespawnSelect::Respawn4Pressed);
}

void URespawnSelect::Respawn1Pressed()
{
	if (ClickSound)
		PlaySound(ClickSound);

	FName TagName = FName(TEXT("PlayerStart1"));
	SelectButton(TagName);

}

void URespawnSelect::Respawn2Pressed()
{
	if (ClickSound)
		PlaySound(ClickSound);

	FName TagName = FName(TEXT("PlayerStart2"));
	SelectButton(TagName);

}

void URespawnSelect::Respawn3Pressed()
{
	if (ClickSound)
		PlaySound(ClickSound);

	FName TagName = FName(TEXT("PlayerStart3"));
	SelectButton(TagName);


}

void URespawnSelect::Respawn4Pressed()
{
	if (ClickSound)
		PlaySound(ClickSound);

	FName TagName = FName(TEXT("PlayerStart4"));
	SelectButton(TagName);
}

void URespawnSelect::SelectButton(FName TagName)
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwningPlayerPawn());
	ACharacterController* Controller = Cast<ACharacterController>(Character->Controller);
	ABOGameMode* BOGameMode = Cast<ABOGameMode>(GetWorld()->GetAuthGameMode());
	AMainHUD* MainHUD = Cast<AMainHUD>(Controller->GetHUD());

	FInputModeGameOnly GameOnlyInput;
	Controller->SetInputMode(GameOnlyInput);
	Controller->bShowMouseCursor = false;
	Controller->bEnableMouseOverEvents = false;

	MainHUD->RemoveRespawnSelect();

	if (BOGameMode)
	{
		BOGameMode->Respawn(Character, Controller, TagName);
	}
}

