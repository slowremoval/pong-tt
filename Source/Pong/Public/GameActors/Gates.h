// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gates.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnBallEnterGate, int32, GateIndex);

UCLASS()
class PONG_API AGates : public AActor
{
	GENERATED_BODY()
	
public:	
	AGates();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
	int32 GateIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gate")
	class UBoxComponent* GateCollider;

	UFUNCTION(Server, Reliable)
	void Server_HandleBallEnterGate(int32 Index);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HandleBallEnterGate(int32 Index);

	FOnBallEnterGate OnBallEnter;

protected:
	// Function to handle the ball entering the gate's collider
	UFUNCTION()
	void OnGateBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};