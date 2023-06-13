// Fill out your copyright notice in the Description page of Project Settings.


#include "PDPStartPlayerController.h"
#include "Blueprint/UserWidget.h"

void APDPStartPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	Client_DrawStartUI();
}

void APDPStartPlayerController::Destroyed()
{
	Super::Destroyed();

	Client_DeleteStartUI();
}

void APDPStartPlayerController::Client_DrawStartUI_Implementation()
{
	UUserWidget* Widget = CreateWidget(this, StartWidget);
	if (Widget)
	{
		Widget->AddToViewport();
	}
	
	SetShowMouseCursor(true);

	SetInputMode(FInputModeUIOnly());
}

void APDPStartPlayerController::Client_DeleteStartUI_Implementation()
{
	SetShowMouseCursor(true);
	
	SetInputMode(FInputModeGameOnly());
}
