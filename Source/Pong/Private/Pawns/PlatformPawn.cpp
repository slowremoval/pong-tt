#include "Pawns/PlatformPawn.h"

#include "Camera/CameraComponent.h"
#include "Pawns/ReplicatedPawnMovementComponent.h"

APlatformPawn::APlatformPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	PlatformMovementComponent = CreateDefaultSubobject<UReplicatedPawnMovementComponent>(
		TEXT("ReplicatedPawnMovementComponent"));

	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootComp;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
}

void APlatformPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	FVector NewLocation = GetActorLocation();

	NewLocation.Y = FMath::Clamp(NewLocation.Y, -MovementBoundary, MovementBoundary);

	SetActorLocation(NewLocation);

#pragma region LookAtPlayer
	// FVector PawnLocation = GetActorLocation();
	//
	// FVector DirectionToPawn = PawnLocation - CameraComponent->GetComponentLocation();
	//
	// FRotator TargetLookAtRotation = FRotationMatrix::MakeFromX(DirectionToPawn).Rotator();
	//
	// float InterpSpeed = 0.9f;  
	// FRotator SmoothRotation = FMath::RInterpTo(CameraComponent->GetComponentRotation(), TargetLookAtRotation, DeltaSeconds, InterpSpeed);
	//
	// CameraComponent->SetWorldRotation(SmoothRotation);
#pragma endregion LookAtPlayer
}

void APlatformPawn::BeginPlay()
{
	Super::BeginPlay();

	const FVector ActorLocation = GetActorLocation();
	const FVector ForwardVector = GetActorForwardVector();

	const FVector Offset = FVector(2550.0f, 0.0f, 4200.0f);

	const FVector CameraWorldLocation = ActorLocation - (ForwardVector * Offset.X) + FVector(0.0f, 0, Offset.Z);

	CameraComponent->SetWorldLocation(CameraWorldLocation);

	const FVector DirectionToPawn = ActorLocation - CameraComponent->GetComponentLocation();

	const FRotator TargetLookAtRotation = FRotationMatrix::MakeFromX(DirectionToPawn).Rotator();
	CameraComponent->SetWorldRotation(TargetLookAtRotation);
}

void APlatformPawn::Move(FVector MovementInput)
{
	if (!IsValid(PlatformMovementComponent))
	{
		return;
	}
	if (MovementInput.IsNearlyZero())
	{
		return;
	}
	//move only along horizontal axis
	MovementInput.Y = 0;
	
	PlatformMovementComponent->AddInputVector(MovementInput);
}
