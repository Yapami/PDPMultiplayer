// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PDPStartPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PDPMULTIPLAYER_API APDPStartPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> StartWidget;
	
protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION(Client, Reliable)
	void Client_DrawStartUI();
	void Client_DrawStartUI_Implementation();

	UFUNCTION(Client, Reliable)
	void Client_DeleteStartUI();
	void Client_DeleteStartUI_Implementation();
};
