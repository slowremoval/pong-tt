// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReplicatedBall.generated.h"

UCLASS()
class PONG_API AReplicatedBall : public AActor
{
	GENERATED_BODY()

public:
	AReplicatedBall();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball")
	float BallSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball", ReplicatedUsing = OnRep_BallVelocity)
	FVector BallVelocity;

	UFUNCTION()
	void OnRep_BallVelocity();

	UFUNCTION()
	void OnBallHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BallMesh;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetBallVelocity(const FVector& NewVelocity);

	void SetBallVelocityLocally(const FVector& NewVelocity);
};