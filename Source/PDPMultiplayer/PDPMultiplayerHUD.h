// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PDPMultiplayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class PDPMULTIPLAYER_API APDPMultiplayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void DrawIU();
	
	UFUNCTION(BlueprintCallable)
	void DeleteUI();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HUD;
	
private:
	UPROPERTY()
	UUserWidget* HUDWidget = nullptr;
};
