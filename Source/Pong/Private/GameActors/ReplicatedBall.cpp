// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActors/ReplicatedBall.h"

#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


DEFINE_LOG_CATEGORY_STATIC(LogReplicatedBall, Log, All);

AReplicatedBall::AReplicatedBall()
{
    PrimaryActorTick.bCanEverTick = true;
    
    bReplicates = true;
    
    BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
    RootComponent = BallMesh;

    BallMesh->SetSimulatePhysics(true);
    BallMesh->SetCollisionProfileName(TEXT("BlockAll"));

    BallMesh->OnComponentHit.AddDynamic(this, &AReplicatedBall::OnBallHit);

    BallSpeed = 3900.0f;

    BallVelocity = FVector(BallSpeed, 0.0f, 0.0f);

    UE_LOG(LogReplicatedBall, Log, TEXT("AReplicatedBall Constructor: Ball Initialized with Velocity: %s"), *BallVelocity.ToString());
}

void AReplicatedBall::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        BallMesh->SetPhysicsLinearVelocity(BallVelocity);
        UE_LOG(LogReplicatedBall, Log, TEXT("BeginPlay: Server has authority, setting initial velocity: %s"), *BallVelocity.ToString());
    }
    else
    {
        UE_LOG(LogReplicatedBall, Log, TEXT("BeginPlay: Client initialized. Waiting for updates from server."));
    }
}

void AReplicatedBall::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (HasAuthority())
    {
        FVector Velocity = BallMesh->GetPhysicsLinearVelocity();
        Velocity = Velocity.GetClampedToMaxSize(BallSpeed);
        BallMesh->SetPhysicsLinearVelocity(Velocity);

        BallVelocity = Velocity;

        Multicast_SetBallPositionAndVelocity(GetActorLocation(), BallVelocity);

        UE_LOG(LogReplicatedBall, Log, TEXT("Tick: Server updating ball position: %s, velocity: %s"), *GetActorLocation().ToString(), *BallVelocity.ToString());
    }
    else
    {
        UE_LOG(LogReplicatedBall, Verbose, TEXT("Tick: Client receiving ball updates from server."));
    }
}

void AReplicatedBall::OnBallHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit)
{
    if (HasAuthority())
    {
        FVector HitNormal2D = FVector(Hit.ImpactNormal.X, Hit.ImpactNormal.Y, 0.0f);
        FVector NewVelocity2D = UKismetMathLibrary::GetReflectionVector(FVector(BallVelocity.X, BallVelocity.Y, 0.0f), HitNormal2D);
        
        NewVelocity2D = NewVelocity2D.GetSafeNormal2D() * BallSpeed;

        BallVelocity = FVector(NewVelocity2D.X, NewVelocity2D.Y, 0.0f);

        BallMesh->SetPhysicsLinearVelocity(BallVelocity);

        Multicast_SetBallVelocity(BallVelocity);

        UE_LOG(LogReplicatedBall, Log, TEXT("OnBallHit: Ball hit something. New Velocity: %s"), *BallVelocity.ToString());
    }
}

void AReplicatedBall::OnRep_BallVelocity()
{
    SetBallVelocityLocally(BallVelocity);
    UE_LOG(LogReplicatedBall, Log, TEXT("OnRep_BallVelocity: Client updated ball velocity: %s"), *BallVelocity.ToString());
}

void AReplicatedBall::Multicast_SetBallVelocity_Implementation(const FVector& NewVelocity)
{
    SetBallVelocityLocally(NewVelocity);
    UE_LOG(LogReplicatedBall, Log, TEXT("Multicast_SetBallVelocity: Velocity replicated to clients: %s"), *NewVelocity.ToString());
}

void AReplicatedBall::Multicast_SetBallPositionAndVelocity_Implementation(const FVector& NewLocation, const FVector& NewVelocity)
{
    SetActorLocation(NewLocation);
    SetBallVelocityLocally(NewVelocity);
    UE_LOG(LogReplicatedBall, Log, TEXT("Multicast_SetBallPositionAndVelocity: Position and velocity replicated. New Position: %s, New Velocity: %s"), *NewLocation.ToString(), *NewVelocity.ToString());
}

void AReplicatedBall::SetBallVelocityLocally(const FVector& NewVelocity)
{
    BallVelocity = NewVelocity;
    BallMesh->SetPhysicsLinearVelocity(NewVelocity);
    UE_LOG(LogReplicatedBall, Verbose, TEXT("SetBallVelocityLocally: Local velocity updated: %s"), *NewVelocity.ToString());
}

void AReplicatedBall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AReplicatedBall, BallVelocity);
    UE_LOG(LogReplicatedBall, Log, TEXT("GetLifetimeReplicatedProps: Ball velocity marked for replication."));
}