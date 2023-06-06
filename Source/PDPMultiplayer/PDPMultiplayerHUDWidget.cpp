// Fill out your copyright notice in the Description page of Project Settings.


#include "PDPMultiplayerHUDWidget.h"
#include "PDPMultiplayerCharacter.h"
#include "Components/TextBlock.h"

void UPDPMultiplayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(APDPMultiplayerCharacter* Player = Cast<APDPMultiplayerCharacter>(GetOwningPlayer()->GetPawn()))
	{
		Player->OnHealthChangedDelegate.BindUObject(this, &UPDPMultiplayerHUDWidget::UpdateHealth);
		Player->OnAmmoChangedDelegate.BindUObject(this, &UPDPMultiplayerHUDWidget::UpdateAmmo);
	}
}

void UPDPMultiplayerHUDWidget::UpdateHealth(const float NewHealth)
{
	HealthText->SetText(FText::FromString(FString::FromInt(static_cast<int>(NewHealth))));
}

void UPDPMultiplayerHUDWidget::UpdateAmmo(const uint8 NewAmmo)
{
	AmmoText->SetText(FText::FromString(FString::FromInt(NewAmmo)));
}
