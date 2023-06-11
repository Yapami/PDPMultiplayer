// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PDPMultiplayerCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnAmmoChangedDelegate, const uint8)
DECLARE_DELEGATE_OneParam(FOnHealthChangedDelegate, const float)

UCLASS(config=Game)
class APDPMultiplayerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	APDPMultiplayerCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	FOnAmmoChangedDelegate OnAmmoChangedDelegate;
	FOnHealthChangedDelegate OnHealthChangedDelegate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION(BlueprintCallable)
	AActor* LineTrace();

	virtual void PossessedBy(AController* NewController) override;

protected:
	// Take damage:
	UPROPERTY(ReplicatedUsing = OnHealthChanged, EditDefaultsOnly, Category= "Health")
	float Health = 100;

	UFUNCTION()
	void OnHealthChanged() const;
	
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	bool Server_TakeDamage_Validate(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	void Server_TakeDamage_Implementation(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void Multicast_Ragdoll();
	bool Multicast_Ragdoll_Validate();
	void Multicast_Ragdoll_Implementation();
	
protected:
	// Shot:
	UPROPERTY(ReplicatedUsing = OnAmmoChanged, EditDefaultsOnly, Category= "Health")
	uint8 Ammo = 30;
	
	UFUNCTION()
	void OnAmmoChanged();

	
	bool CanShoot = true;

	UPROPERTY(EditAnywhere, Category= "SFX")
	USoundBase* Shot;

	UPROPERTY(EditAnywhere, Category= "SFX")
	USoundBase* NoAmmo;

	UPROPERTY(EditAnywhere, Category= "SFX")
	USoundAttenuation* WeaponSoundAttenuation;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Shot();
	bool Server_Shot_Validate();
	void Server_Shot_Implementation();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_ShotSFX(USoundBase* Sound);
	bool Multicast_ShotSFX_Validate(USoundBase* Sound);
	void Multicast_ShotSFX_Implementation(USoundBase* Sound);

	UFUNCTION(Client, Unreliable, WithValidation)
	void Client_TraceOnClient();
	bool Client_TraceOnClient_Validate();
	void Client_TraceOnClient_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TraceOnServer();
	bool Server_TraceOnServer_Validate();
	void Server_TraceOnServer_Implementation();

protected:
	// UI:
	UFUNCTION(Client, Reliable, WithValidation)
	void Client_DrawUI();
	bool Client_DrawUI_Validate();
	void Client_DrawUI_Implementation();

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_DeleteUI();
	bool Client_DeleteUI_Validate();
	void Client_DeleteUI_Implementation();
	
protected:
	// Change camera side: 
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_ChangeCameraSideLeft();
	bool Server_ChangeCameraSideLeft_Validate();
	void Server_ChangeCameraSideLeft_Implementation();

	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void Multicast_ChangeCameraLeft();
	bool Multicast_ChangeCameraLeft_Validate();
	void Multicast_ChangeCameraLeft_Implementation();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_ChangeCameraSideRight();
	bool Server_ChangeCameraSideRight_Validate();
	void Server_ChangeCameraSideRight_Implementation();

	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void Multicast_ChangeCameraRight();
	bool Multicast_ChangeCameraRight_Validate();
	void Multicast_ChangeCameraRight_Implementation();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

