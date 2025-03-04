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
    // UI 생성 함수
    UUserWidget* CreateUI(TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder = 0);

    // UI 열기 함수
    void ShowUI(FName UIName);

    // UI 닫기 함수
    void HideUI(FName UIName);

    // UI 제거 함수
    void RemoveUI(FName UIName);

private:
    // 현재 활성화된 UI 목록
    TMap<FName, UUserWidget*> ActiveWidgets;
		
};
