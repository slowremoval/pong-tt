// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ReplicatedPawnMovementComponent.generated.h"

class UInputMappingContext;
DECLARE_LOG_CATEGORY_EXTERN(LogReplicatedPawnMovementComponent, Log, All);

struct FInputActionValue;

UCLASS()
class PONG_API UReplicatedPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	virtual void AddInputVector(FVector WorldVector, bool bForce = false) override;

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetMovement(const FVector Movement, bool bForce);

	UFUNCTION(Server, Reliable)
	void Server_SetLocation(const FVector Vector);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetLocation(const FVector Vector);

	void Server_MovePawn(float DeltaTime);

	void InterpolateLocation(float DeltaTime);
	void StartLocationInterpolation(const FVector NewTargetLocation);

public:
	UPROPERTY(EditAnywhere)
	float Speed = 400;

protected:
	FVector TargetLocation;

	const float InterpSpeed = 15.0f;
};