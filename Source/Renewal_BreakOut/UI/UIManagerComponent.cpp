// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIManagerComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

UUIManagerComponent::UUIManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UUIManagerComponent::BeginPlay()
{
    Super::BeginPlay();
}

UUserWidget* UUIManagerComponent::CreateUI(TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder)
{
    if (!WidgetClass) return nullptr;

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (!PlayerController) return nullptr;

    // À§Á¬ »ý¼º
    UUserWidget* NewWidget = CreateWidget<UUserWidget>(PlayerController, WidgetClass);
    if (NewWidget)
    {
        NewWidget->AddToViewport(ZOrder);
        return NewWidget;
    }
    return nullptr;
}

void UUIManagerComponent::ShowUI(FName UIName)
{
    if (ActiveWidgets.Contains(UIName))
    {
        ActiveWidgets[UIName]->SetVisibility(ESlateVisibility::Visible);
    }
}

void UUIManagerComponent::HideUI(FName UIName)
{
    if (ActiveWidgets.Contains(UIName))
    {
        ActiveWidgets[UIName]->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UUIManagerComponent::RemoveUI(FName UIName)
{
    if (ActiveWidgets.Contains(UIName))
    {
        ActiveWidgets[UIName]->RemoveFromParent();
        ActiveWidgets.Remove(UIName);
    }
}