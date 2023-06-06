// Fill out your copyright notice in the Description page of Project Settings.


#include "PDPMultiplayerHUD.h"

#include "Blueprint/UserWidget.h"

void APDPMultiplayerHUD::DrawIU()
{
	HUDWidget = CreateWidget(PlayerOwner, HUD);
	if (HUDWidget)
	{
		HUDWidget->AddToViewport();
	}
}

void APDPMultiplayerHUD::DeleteUI()
{
	if (IsValid(HUDWidget))
	{
		HUDWidget->RemoveFromParent();
	}
	
	HUDWidget = nullptr;
}
