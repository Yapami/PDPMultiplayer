// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDPMultiplayerCharacter.h"

#include "DrawDebugHelpers.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

//////////////////////////////////////////////////////////////////////////
// APDPMultiplayerCharacter

APDPMultiplayerCharacter::APDPMultiplayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	OnTakeAnyDamage.AddDynamic(this, &APDPMultiplayerCharacter::Server_TakeDamage);
}

//////////////////////////////////////////////////////////////////////////
// Input

void APDPMultiplayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &APDPMultiplayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APDPMultiplayerCharacter::MoveRight);

	// Change camera side
	PlayerInputComponent->BindAction("ChangeCameraLeft", IE_Pressed, this, &APDPMultiplayerCharacter::Server_ChangeCameraSideLeft);
	PlayerInputComponent->BindAction("ChangeCameraRight", IE_Pressed, this, &APDPMultiplayerCharacter::Server_ChangeCameraSideRight);

	// Shot
	PlayerInputComponent->BindAction("Shot", IE_Pressed, this, &APDPMultiplayerCharacter::Server_Shot);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APDPMultiplayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APDPMultiplayerCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &APDPMultiplayerCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &APDPMultiplayerCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &APDPMultiplayerCharacter::OnResetVR);
}


void APDPMultiplayerCharacter::OnResetVR()
{
	// If PDPMultiplayer is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in PDPMultiplayer.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void APDPMultiplayerCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void APDPMultiplayerCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

AActor* APDPMultiplayerCharacter::LineTrace()
{
	auto World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
	
	FHitResult HitResult;
	const FVector TraceStart = FollowCamera->GetComponentLocation();
	const FVector TraceEnd = FollowCamera->GetForwardVector() * 30000.0f + TraceStart;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);
	DrawDebugLine(World, TraceStart, TraceEnd, FColor::Green, true);
	
	return HitResult.Actor.Get();
}

bool APDPMultiplayerCharacter::Server_TakeDamage_Validate(AActor* DamagedActor, float Damage,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	return DamagedActor != nullptr && Damage > 0.0f;
}

void APDPMultiplayerCharacter::Server_TakeDamage_Implementation(AActor* DamagedActor, float Damage,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (APDPMultiplayerCharacter* Player = Cast<APDPMultiplayerCharacter>(DamagedActor))
	{
		Player->Health -= Damage;

		UE_LOG(LogTemp, Error, TEXT("Health: %i"), (int)Player->Health);
	}
}

void APDPMultiplayerCharacter::OnHealthChanged() const
{
	OnHealthChangedDelegate.ExecuteIfBound(Health);
}

bool APDPMultiplayerCharacter::Server_Shot_Validate()
{
	return true;
}

void APDPMultiplayerCharacter::Server_Shot_Implementation()
{
	if (CanShoot)
	{
		if (Ammo > 0)
		{
			Multicast_ShotSFX(Shot);
			CanShoot = false;
			--Ammo;
			Client_TraceOnClient();
			// TODO: Delay(0.3)
			CanShoot = true;
		}
		else
		{
			Multicast_ShotSFX(NoAmmo);
		}
	}
}

bool APDPMultiplayerCharacter::Multicast_ShotSFX_Validate(USoundBase* Sound)
{
	return Sound != nullptr;
}

void APDPMultiplayerCharacter::Multicast_ShotSFX_Implementation(USoundBase* Sound)
{
	UGameplayStatics::SpawnSoundAttached(Sound, RootComponent, NAME_None, FVector(ForceInit), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, 1, 1, 0, WeaponSoundAttenuation)->Play();
}

bool APDPMultiplayerCharacter::Client_TraceOnClient_Validate()
{
	return true;
}

void APDPMultiplayerCharacter::Client_TraceOnClient_Implementation()
{
	if (AActor* HitActor = LineTrace())
	{
		if (HitActor->ActorHasTag("Player"))
		{
			Server_TraceOnServer();
		}
	}
}

bool APDPMultiplayerCharacter::Server_TraceOnServer_Validate()
{
	return true;
}

void APDPMultiplayerCharacter::Server_TraceOnServer_Implementation()
{
	if (AActor* HitActor = LineTrace())
	{
		HitActor->TakeDamage(20, FDamageEvent(), nullptr, nullptr);
	}
}

void APDPMultiplayerCharacter::OnAmmoChanged()
{
	OnAmmoChangedDelegate.ExecuteIfBound(Ammo);
}

bool APDPMultiplayerCharacter::Server_ChangeCameraSideLeft_Validate()
{
	return true;
}

void APDPMultiplayerCharacter::Server_ChangeCameraSideLeft_Implementation()
{
	Multicast_ChangeCameraLeft();
}

bool APDPMultiplayerCharacter::Multicast_ChangeCameraLeft_Validate()
{
	return true;
}

void APDPMultiplayerCharacter::Multicast_ChangeCameraLeft_Implementation()
{
	static const FVector LeftSideCamera = {110.0f, -60.0, 70.0f};
	
	FollowCamera->SetRelativeLocation(LeftSideCamera);
}

bool APDPMultiplayerCharacter::Server_ChangeCameraSideRight_Validate()
{
	return true;
}

void APDPMultiplayerCharacter::Server_ChangeCameraSideRight_Implementation()
{
	Multicast_ChangeCameraRight();
}

bool APDPMultiplayerCharacter::Multicast_ChangeCameraRight_Validate()
{
	return true;
}

void APDPMultiplayerCharacter::Multicast_ChangeCameraRight_Implementation()
{
	static const FVector RightSideCamera = {110.0f, 60.0, 70.0f};
	
	FollowCamera->SetRelativeLocation(RightSideCamera);
}


void APDPMultiplayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APDPMultiplayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APDPMultiplayerCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APDPMultiplayerCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void APDPMultiplayerCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APDPMultiplayerCharacter, Health);
	DOREPLIFETIME(APDPMultiplayerCharacter, Ammo);
}
