#include "Pawns/ReplicatedPawnMovementComponent.h"

DEFINE_LOG_CATEGORY(LogReplicatedPawnMovementComponent);

void UReplicatedPawnMovementComponent::Server_SetMovement_Implementation(const FVector Movement, bool bForce)
{
	ConsumeInputVector();
	Super::AddInputVector(Movement, bForce);
}

bool UReplicatedPawnMovementComponent::Server_SetMovement_Validate(const FVector Movement, bool bForce)
{
	return true;
}

void UReplicatedPawnMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
	Server_SetMovement(WorldVector, bForce);
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
	Server_SetLocation(GetActorLocation());
}

void UReplicatedPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (IsRunningDedicatedServer())
	{
		Server_MovePawn(DeltaTime);
	}
}

void UReplicatedPawnMovementComponent::Multicast_SetLocation_Implementation(const FVector Vector)
{
	if (!PawnOwner->IsLocallyControlled())
{
	StartLocationInterpolation(Vector);
}
else
{
	GetOwner()->SetActorLocation(Vector);
}
}

void UReplicatedPawnMovementComponent::Server_SetLocation_Implementation(const FVector Vector)
{
	Multicast_SetLocation(Vector);
}

bool UReplicatedPawnMovementComponent::Server_SetLocation_Validate(const FVector Vector)
{
	return true;
}

void UReplicatedPawnMovementComponent::StartLocationInterpolation(const FVector NewTargetLocation)
{
	TargetLocation = NewTargetLocation;

	GetWorld()->GetTimerManager().ClearTimer(InterpolationTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(InterpolationTimerHandle, this, &UReplicatedPawnMovementComponent::InterpolateLocation, InterpDuration, true);
}

void UReplicatedPawnMovementComponent::InterpolateLocation()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	FVector CurrentLocation = Owner->GetActorLocation();

	FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, GetWorld()->GetDeltaSeconds(), InterpSpeed);

	Owner->SetActorLocation(NewLocation);

	if (CurrentLocation.Equals(TargetLocation, KINDA_SMALL_NUMBER))
	{
		GetWorld()->GetTimerManager().ClearTimer(InterpolationTimerHandle);
	}
}