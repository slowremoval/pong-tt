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

	virtual void Tick(float DeltaSeconds) override;

	void Move(FVector MovementInput);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UReplicatedPawnMovementComponent> PlatformMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(EditAnywhere, Category = "Movement Boundaries", meta = (ClampMin = -5000, ClampMax = 5000))
	float MovementBoundary = 1700.0f;

	void ClampMovement(FVector& NewLocation);
};