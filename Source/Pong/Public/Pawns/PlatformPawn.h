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
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UReplicatedPawnMovementComponent> PlatformMovementComponent;
	FVector CameraWorldLocation;

public:
	APlatformPawn();

	virtual void Tick(float DeltaSeconds) override;
	
	void Move(FVector MovementInput);
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;
};
