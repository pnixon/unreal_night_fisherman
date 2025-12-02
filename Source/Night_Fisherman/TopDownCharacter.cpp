// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// Sets default values
ATopDownCharacter::ATopDownCharacter()
{
	// Set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f); // Rotation rate for smooth turning
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom (pulls in towards the character if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.0f;
	CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f)); // Top-down angle
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera
	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

// Called when the game starts or when spawned
void ATopDownCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void ATopDownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATopDownCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopDownCharacter::Move);
		}

		// Camera Control
		if (CameraControlAction)
		{
			EnhancedInputComponent->BindAction(CameraControlAction, ETriggerEvent::Triggered, this, &ATopDownCharacter::CameraControl);
		}

		// Interact
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ATopDownCharacter::Interact);
		}

		// Attack
		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ATopDownCharacter::Attack);
		}

		// Heavy Attack
		if (HeavyAttackAction)
		{
			EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ATopDownCharacter::HeavyAttack);
		}

		// Dodge
		if (DodgeAction)
		{
			EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ATopDownCharacter::Dodge);
		}

		// Use Item
		if (UseItemAction)
		{
			EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Started, this, &ATopDownCharacter::UseItem);
		}

		// Pause Menu
		if (PauseMenuAction)
		{
			EnhancedInputComponent->BindAction(PauseMenuAction, ETriggerEvent::Started, this, &ATopDownCharacter::PauseMenu);
		}
	}
}

void ATopDownCharacter::Move(const FInputActionValue& Value)
{
	// Input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// Get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATopDownCharacter::CameraControl(const FInputActionValue& Value)
{
	// Input is a Vector2D
	FVector2D CameraVector = Value.Get<FVector2D>();

	// Implement camera rotation or zoom logic here
	// Example: Rotate camera boom around character
	if (CameraBoom)
	{
		// Rotate the camera boom based on input
		FRotator NewRotation = CameraBoom->GetRelativeRotation();
		NewRotation.Yaw += CameraVector.X * 2.0f; // Horizontal rotation
		
		// Optionally adjust pitch (vertical angle) with limits
		float NewPitch = NewRotation.Pitch + CameraVector.Y * 2.0f;
		NewPitch = FMath::Clamp(NewPitch, -80.0f, -20.0f); // Limit camera angle
		NewRotation.Pitch = NewPitch;
		
		CameraBoom->SetRelativeRotation(NewRotation);
	}
}

void ATopDownCharacter::Interact(const FInputActionValue& Value)
{
	// Implement interaction logic here
	UE_LOG(LogTemp, Warning, TEXT("Interact action triggered!"));
	
	// Example: Perform a line trace to detect interactable objects
	// Then call their interaction function
}

void ATopDownCharacter::Attack(const FInputActionValue& Value)
{
	// Implement attack logic here
	UE_LOG(LogTemp, Warning, TEXT("Attack action triggered!"));
	
	// Example: Play attack animation, spawn projectile, etc.
}

void ATopDownCharacter::HeavyAttack(const FInputActionValue& Value)
{
	// Implement heavy attack logic here
	UE_LOG(LogTemp, Warning, TEXT("Heavy Attack action triggered!"));
	
	// Example: Play heavy attack animation, deal more damage, etc.
}

void ATopDownCharacter::Dodge(const FInputActionValue& Value)
{
	// Implement dodge/roll logic here
	UE_LOG(LogTemp, Warning, TEXT("Dodge action triggered!"));
	
	// Example: Play dodge animation, add temporary invincibility, dash in movement direction
}

void ATopDownCharacter::UseItem(const FInputActionValue& Value)
{
	// Implement item usage logic here
	UE_LOG(LogTemp, Warning, TEXT("Use Item action triggered!"));
	
	// Example: Use currently equipped item (potion, consumable, etc.)
}

void ATopDownCharacter::PauseMenu(const FInputActionValue& Value)
{
	// Implement pause menu logic here
	UE_LOG(LogTemp, Warning, TEXT("Pause Menu action triggered!"));
	
	// Example: Open pause menu widget, pause game
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// Toggle pause or open menu widget
	}
}
