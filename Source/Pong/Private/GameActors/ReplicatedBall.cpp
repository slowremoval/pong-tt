// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActors/ReplicatedBall.h"

#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

AReplicatedBall::AReplicatedBall()
{
	PrimaryActorTick.bCanEverTick = true;

	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	RootComponent = BallMesh;

	BallMesh->SetSimulatePhysics(true);
	BallMesh->SetCollisionProfileName(TEXT("BlockAll"));

	BallMesh->OnComponentHit.AddDynamic(this, &AReplicatedBall::OnBallHit);

	BallSpeed = 3900.0f;

	bReplicates = true;
	SetReplicatingMovement(true);

	BallVelocity = FVector(BallSpeed, 0.0f, BallSpeed);
}

void AReplicatedBall::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		BallMesh->SetPhysicsLinearVelocity(BallVelocity);
	}
}

void AReplicatedBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// if (HasAuthority())
	// {
	// 	FVector Velocity = BallMesh->GetPhysicsLinearVelocity();
	// 	Velocity = Velocity.GetClampedToMaxSize(BallSpeed);
	// 	BallMesh->SetPhysicsLinearVelocity(Velocity);
	//
	// 	BallVelocity = Velocity;
	// }
}

void AReplicatedBall::OnBallHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit)
{
	if (HasAuthority())
	{
		FVector HitNormal2D = FVector(Hit.ImpactNormal.X, Hit.ImpactNormal.Y, 0.0f);

		FVector NewVelocity2D = UKismetMathLibrary::GetReflectionVector(FVector(BallVelocity.X, BallVelocity.Y, 0.0f),
		                                                                HitNormal2D);

		NewVelocity2D = NewVelocity2D.GetSafeNormal2D() * BallSpeed;

		BallVelocity = FVector(NewVelocity2D.X, NewVelocity2D.Y, 0.0f);

		BallMesh->SetPhysicsLinearVelocity(BallVelocity);

		Multicast_SetBallVelocity(BallVelocity);
	}
}

void AReplicatedBall::OnRep_BallVelocity()
{
	SetBallVelocityLocally(BallVelocity);
}

void AReplicatedBall::Multicast_SetBallVelocity_Implementation(const FVector& NewVelocity)
{
	SetBallVelocityLocally(NewVelocity);
}

void AReplicatedBall::SetBallVelocityLocally(const FVector& NewVelocity)
{
	BallVelocity = NewVelocity;
	BallMesh->SetPhysicsLinearVelocity(NewVelocity);
}

void AReplicatedBall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AReplicatedBall, BallVelocity);
}
