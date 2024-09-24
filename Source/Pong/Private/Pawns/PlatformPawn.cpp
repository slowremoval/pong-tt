
#include "Pawns/PlatformPawn.h"
#include "Pawns/ReplicatedPawnMovementComponent.h"

APlatformPawn::APlatformPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	PlatformMovementComponent = CreateDefaultSubobject<UReplicatedPawnMovementComponent>(TEXT("ReplicatedPawnMovementComponent"));
	//bReplicates = true; 	
}

void APlatformPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//PlatformMovementComponent->MovePawn(DeltaSeconds);
}

void APlatformPawn::BeginPlay()
{
	Super::BeginPlay();
}

void APlatformPawn::Move(FVector MovementInput)
{
	
	if (!IsValid(PlatformMovementComponent))
	{
#if 0
		UE_LOG(LogTemp, Warning, TEXT("PlatformMovementComponent is not initialized"));
#endif
		return;
	}
	if (MovementInput.IsNearlyZero())
	{
		return;
	}
	
	PlatformMovementComponent->AddInputVector(MovementInput);
}
