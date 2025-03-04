// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WeaponSelectionWidget.h"
#include "Components/Button.h"
#include "Weapon/WeaponBase.h"
#include "GameFramework/PlayerController.h"
#include "Character/CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "MainHud.h"
#include "ClientSocket.h"


void UWeaponSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RifleButton->OnClicked.AddDynamic(this, &UWeaponSelectionWidget::RifleButtonPressed);
	ShotgunButton->OnClicked.AddDynamic(this, &UWeaponSelectionWidget::ShotgunButtonPressed);
	LancherButton->OnClicked.AddDynamic(this, &UWeaponSelectionWidget::LancherButtonPressed);

	MyCharacterController = Cast<ACharacterController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}
// 무기 선택 창 패킷 보낼 곳
void UWeaponSelectionWidget::RifleButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("RifleButtonPressed"));
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwningPlayerPawn());
	ACharacterController* Controller = Cast<ACharacterController>(Character->Controller);

}

void UWeaponSelectionWidget::ShotgunButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("ShotgunButtonPressed"));
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwningPlayerPawn());
	ACharacterController* Controller = Cast<ACharacterController>(Character->Controller);

}

void UWeaponSelectionWidget::LancherButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("LancherButtonPressed"));
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwningPlayerPawn());
	ACharacterController* Controller = Cast<ACharacterController>(Character->Controller);

}
