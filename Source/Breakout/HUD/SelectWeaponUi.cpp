// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SelectWeaponUi.h"
#include "Components/Button.h"
#include "Weapon/WeaponBase.h"
#include "GameFramework/PlayerController.h"
#include "Character/CharacterBase.h"

#include "MainHUD.h"
#include "ClientSocket.h"
#include "Network/PacketData.h"
#include "UWeaponManagerComponent.h"

void USelectWeaponUi::NativeConstruct()
{
	Super::NativeConstruct();

	RifleButton->OnClicked.AddDynamic(this, &USelectWeaponUi::RifleButtonPressed);
	ShotgunButton->OnClicked.AddDynamic(this, &USelectWeaponUi::ShotgunButtonPressed);
	LancherButton->OnClicked.AddDynamic(this, &USelectWeaponUi::LancherButtonPressed);

	MyCharacterController = Cast<ACharacterController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}
// 무기 선택 창 패킷 보낼 곳
void USelectWeaponUi::RifleButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("RifleButtonPressed"));
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwningPlayerPawn());
	ACharacterController* Controller = Cast<ACharacterController>(Character->Controller);
	AMainHUD* MainHUD = Cast<AMainHUD>(Controller->GetHUD());

	FInputModeGameOnly GameOnlyInput;
	Controller->SetInputMode(GameOnlyInput);
	MainHUD->RemoveSelectWeapon();
	Controller->bShowMouseCursor = false;
	Controller->bEnableMouseOverEvents = false;

	FName RifleSocketName = FName("RifleSocket");
	Character->GetWeaponManager()->EquipWeapon(Rifle, RifleSocketName);
	//Character->SetWeapon(Rifle, RifleSocketName);
	Controller->SetHUDAmmo(30);

	Character->EnableInput(MyCharacterController);
	//Character->SetWeaponType(EWeaponType::E_Rifle);
	Character->GetWeaponManager()->SetWeaponType(EWeaponType::E_Rifle);
	MyCharacterController->Set_Weapon_Type(EWeaponType::E_Rifle);
}

void USelectWeaponUi::ShotgunButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("ShotgunButtonPressed"));
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwningPlayerPawn());
	ACharacterController* Controller = Cast<ACharacterController>(Character->Controller);
	AMainHUD* MainHUD = Cast<AMainHUD>(Controller->GetHUD());

	FInputModeGameOnly GameOnlyInput;
	Controller->SetInputMode(GameOnlyInput);
	MainHUD->RemoveSelectWeapon();
	Controller->bShowMouseCursor = false;
	Controller->bEnableMouseOverEvents = false;
	Character->EnableInput(MyCharacterController);
	//Character->SetWeaponType(EWeaponType::E_Shotgun);
	Character->GetWeaponManager()->SetWeaponType(EWeaponType::E_Shotgun);
	FName ShotguSocketName = FName("ShotgunSocket");

	//Character->GetWeaponManager()->EquipWeapon(ShotGun, ShotguSocketName);
	Character->GetWeaponManager()->EquipWeapon(ShotGun, ShotguSocketName);
	Controller->SetHUDAmmo(5);

	MyCharacterController->Set_Weapon_Type(EWeaponType::E_Shotgun);
}

void USelectWeaponUi::LancherButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("LancherButtonPressed"));
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwningPlayerPawn());
	ACharacterController* Controller = Cast<ACharacterController>(Character->Controller);
	AMainHUD* MainHUD = Cast<AMainHUD>(Controller->GetHUD());

	FInputModeGameOnly GameOnlyInput;
	Controller->SetInputMode(GameOnlyInput);
	MainHUD->RemoveSelectWeapon();
	Controller->bShowMouseCursor = false;
	Controller->bEnableMouseOverEvents = false;
	Character->EnableInput(MyCharacterController);
	Character->GetWeaponManager()->SetWeaponType(EWeaponType::E_Launcher);
	FName LancherSocketName = FName("LancherSocket");

	//Character->GetWeaponManager()->EquipWeapon(Lancher, LancherSocketName);
	Character->GetWeaponManager()->EquipWeapon(Lancher, LancherSocketName);
	Controller->SetHUDAmmo(2);

	MyCharacterController->Set_Weapon_Type(EWeaponType::E_Launcher);
}
