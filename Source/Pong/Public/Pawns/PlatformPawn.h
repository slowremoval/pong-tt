// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlatformPawn.generated.h"

class APlatformPC;
class UReplicatedPawnMovementComponent;

UCLASS()
class PONG_API APlatformPawn : public APawn
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UReplicatedPawnMovementComponent> PlatformMovementComponent;
	
public:
	APlatformPawn();

	virtual void Tick(float DeltaSeconds) override;
	
	void Move(FVector MovementInput);
	
protected:
	virtual void BeginPlay() override;

};
