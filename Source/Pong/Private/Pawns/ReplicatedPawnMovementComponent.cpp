#include "Pawns/ReplicatedPawnMovementComponent.h"

DEFINE_LOG_CATEGORY(LogReplicatedPawnMovementComponent);

void UReplicatedPawnMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{

	const FVector ForwardVector = GetPawnOwner()->GetActorForwardVector();
	const FVector RightVector = GetPawnOwner()->GetActorRightVector();

	const float ForwardInput = FVector::DotProduct(WorldVector, ForwardVector);
	const float RightInput = FVector::DotProduct(WorldVector, RightVector);

	FVector InputVector = FVector(RightInput, ForwardInput, 0);
	
	if (IsRunningDedicatedServer())
	{
		Super::AddInputVector(InputVector, bForce);
	}
	else
	{
		Server_SetMovement(InputVector, bForce);
	}
}

void UReplicatedPawnMovementComponent::Server_SetMovement_Implementation(const FVector Movement, bool bForce)
{
	Super::AddInputVector(Movement, bForce);
}

bool UReplicatedPawnMovementComponent::Server_SetMovement_Validate(const FVector Movement, bool bForce)
{
	return true;
}

void UReplicatedPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (IsRunningDedicatedServer())
	{
		Server_MovePawn(DeltaTime);
	}
	else
	{
		InterpolateLocation(DeltaTime);
	}
}

void UReplicatedPawnMovementComponent::Server_MovePawn(float DeltaTime)
{
	FVector InputVector = ConsumeInputVector();
	FVector Movement = InputVector * Speed * DeltaTime;
	FHitResult Hit;

	SafeMoveUpdatedComponent(Movement, UpdatedComponent->GetComponentRotation(), true, Hit);

	if (Hit.IsValidBlockingHit())
	{
		SlideAlongSurface(Movement, 1.0f - Hit.Time, Hit.Normal, Hit);
	}

	FVector CurrentLocation = GetActorLocation();

	const float LocationUpdateThreshold = 5.0f;
	if (FVector::Dist(CurrentLocation, LastServerLocation) > LocationUpdateThreshold)
	{
		Server_SetLocation(CurrentLocation);
		LastServerLocation = CurrentLocation;
	}
}

void UReplicatedPawnMovementComponent::Server_SetLocation_Implementation(const FVector Vector)
{
	Multicast_SetLocation(Vector);
}

void UReplicatedPawnMovementComponent::Multicast_SetLocation_Implementation(const FVector Vector)
{
	StartLocationInterpolation(Vector);
}

void UReplicatedPawnMovementComponent::StartLocationInterpolation(const FVector NewTargetLocation)
{
	TargetLocation = NewTargetLocation;
}

void UReplicatedPawnMovementComponent::InterpolateLocation(float DeltaTime)
{
	if (!TargetLocation.IsZero())
	{
		FVector CurrentLocation = GetActorLocation();
		
		const float DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);
		
		const float SnapThreshold = 1.0f;
		if (DistanceToTarget <= SnapThreshold)
		{
			GetOwner()->SetActorLocation(TargetLocation);
			UE_LOG(LogReplicatedPawnMovementComponent, Log, TEXT("Snapping to target location: %s"), *TargetLocation.ToString());
			return;
		}

		FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, InterpSpeed);

		GetOwner()->SetActorLocation(NewLocation);
		UE_LOG(LogReplicatedPawnMovementComponent, Log, TEXT("Client Interpolating Position: %s -> %s"), *CurrentLocation.ToString(), *NewLocation.ToString());
	}
}