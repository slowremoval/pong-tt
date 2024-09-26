#include "Pawns/PlatformPawn.h"

#include "Camera/CameraComponent.h"

APlatformPawn::APlatformPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicatingMovement(true);

	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootComp;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
}

void APlatformPawn::BeginPlay()
{
	Super::BeginPlay();

	const FVector ActorLocation = GetActorLocation();
	const FVector ForwardVector = GetActorForwardVector();

	const FVector CameraWorldLocation = ActorLocation - (ForwardVector * CameraOffsetX) + FVector(0.0f, 0.0f, CameraOffsetZ);
	CameraComponent->SetWorldLocation(CameraWorldLocation);

	const FVector DirectionToPawn = ActorLocation - CameraComponent->GetComponentLocation();
	const FRotator TargetLookAtRotation = FRotationMatrix::MakeFromX(DirectionToPawn).Rotator();
	CameraComponent->SetWorldRotation(TargetLookAtRotation);
}

void APlatformPawn::CalculateRelativeMovementDirection(FVector MovementInput, FVector& FinalMovementDirection)
{
	const FVector ForwardVector = GetActorForwardVector();
	const FVector RightVector = GetActorRightVector();

	const float ForwardInput = FVector::DotProduct(MovementInput, ForwardVector);
	const float RightInput = FVector::DotProduct(MovementInput, RightVector);

	FVector InputVector = FVector(RightInput, ForwardInput, 0.0f);
	InputVector *= MovementSpeed * GetWorld()->GetDeltaSeconds();

	FinalMovementDirection = InputVector;
}

void APlatformPawn::Move(FVector MovementInput)
{
	if (MovementInput.IsNearlyZero())
	{
		return;
	}

	MovementInput.Y = 0;

	FVector FinalMovementDirection;
	CalculateRelativeMovementDirection(MovementInput, FinalMovementDirection);

	if (HasAuthority())
	{
		FVector NewLocation = GetActorLocation() + FinalMovementDirection;
		ClampMovement(NewLocation);
		SetActorLocation(NewLocation);
		Multicast_Move(NewLocation); 
	}
	else
	{
		Server_Move(FinalMovementDirection);
	}
}

void APlatformPawn::ClampMovement(FVector& NewLocation)
{
	NewLocation.Y = FMath::Clamp(NewLocation.Y, -MovementBoundary, MovementBoundary);
}

void APlatformPawn::Server_Move_Implementation(FVector MovementInput)
{
	FVector NewLocation = GetActorLocation() + MovementInput;
	ClampMovement(NewLocation);
	SetActorLocation(NewLocation);

	Multicast_Move(NewLocation);
}

bool APlatformPawn::Server_Move_Validate(FVector MovementInput)
{
	return true;
}

void APlatformPawn::Multicast_Move_Implementation(FVector NewLocation)
{
	if (!HasAuthority())
	{
		SetActorLocation(NewLocation);
	}
}