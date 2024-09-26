// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlatformPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class APlatformPC;
class UReplicatedPawnMovementComponent;

UCLASS()
class PONG_API APlatformPawn : public APawn
{
	GENERATED_BODY()

public:
	APlatformPawn();

	void Move(FVector MovementInput);

protected:
	virtual void BeginPlay() override;

	void CalculateRelativeMovementDirection(FVector MovementInput, FVector& FinalMovementDirection);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, Category = "Movement Boundaries", meta = (ClampMin = -5000, ClampMax = 5000))
	float MovementBoundary = 1700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = 0, AllowPrivateAccess = "true"))
	float MovementSpeed = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraOffsetX = 2550.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraOffsetZ = 4200.0f;

	void ClampMovement(FVector& NewLocation);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Move(FVector MovementInput);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Move(FVector NewLocation);
};