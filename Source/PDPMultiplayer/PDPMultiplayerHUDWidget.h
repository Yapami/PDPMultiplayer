// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PDPMultiplayerHUDWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PDPMULTIPLAYER_API UPDPMultiplayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	void UpdateHealth(const float NewHealth);
	void UpdateAmmo(const uint8 NewAmmo);

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* AmmoText;
};
