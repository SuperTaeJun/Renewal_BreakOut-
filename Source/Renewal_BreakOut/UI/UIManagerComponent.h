// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UIManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RENEWAL_BREAKOUT_API UUIManagerComponent : public UActorComponent
{
	GENERATED_BODY()

	public:
    UUIManagerComponent();

protected:
    virtual void BeginPlay() override;

public:
    // UI ���� �Լ�
    UUserWidget* CreateUI(TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder = 0);

    // UI ���� �Լ�
    void ShowUI(FName UIName);

    // UI �ݱ� �Լ�
    void HideUI(FName UIName);

    // UI ���� �Լ�
    void RemoveUI(FName UIName);

private:
    // ���� Ȱ��ȭ�� UI ���
    TMap<FName, UUserWidget*> ActiveWidgets;
		
};
