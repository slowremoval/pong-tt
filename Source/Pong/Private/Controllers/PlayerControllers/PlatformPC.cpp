// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerControllers/PlatformPC.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Pawns/PlatformPawn.h"

void APlatformPC::SetupMappingContext()
{
	if (IsRunningDedicatedServer())
	{
		return;
	}
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	check(IsValid(Subsystem));
	
	Subsystem->AddMappingContext(MovementMappingContext, 0);
}

void APlatformPC::CachePlayerPawn()
{
	if (IsRunningDedicatedServer())
	{
		return;
	}
	APawn* ControlledPawn = GetPawn();
	PlatformPawn = Cast<APlatformPawn>(ControlledPawn);
}

void APlatformPC::BeginPlay()
{
	Super::BeginPlay();
	
	SetupMappingContext();
	
	CachePlayerPawn();
}

void APlatformPC::BindActions(UEnhancedInputComponent* EnhancedInputComp)
{
#pragma region Action Bindings
	
	EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	
#pragma endregion Action Bindings
}

void APlatformPC::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent))
	{
		BindActions(EnhancedInputComp);
	}
}

void APlatformPC::Move(const FInputActionValue& Value)
{
	if (!IsValid(PlatformPawn))
	{
		return;
	}
	PlatformPawn->Move(Value.Get<FVector>());
}
