#include "GameActors/ReplicatedBall.h"

#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogReplicatedBall, Log, All);

namespace BallConstants
{
    const float DefaultAngleRangeDegrees = 30.0f;
    const float DefaultBallSpeed = 3900.0f;      
    const float DefaultInterpolationSpeed = 7.0f;
    const FRotator ZeroRotation = FRotator(0.0f, 0.0f, 0.0f);
    const int32 NegativeMultiplier = -1;  
    const int32 PositiveMultiplier = 1;   
    const float MinImpactThreshold = 0.0f;
}

void AReplicatedBall::RandomizeBallInitialVelocity()
{
    const FVector ForwardVector = GetActorForwardVector().GetSafeNormal();

    const float AngleRangeDegrees = BallConstants::DefaultAngleRangeDegrees;
    const float RandomAngleDegrees = FMath::RandRange(-AngleRangeDegrees, AngleRangeDegrees);

    const FRotator Rotation = FRotator(BallConstants::ZeroRotation.Pitch, RandomAngleDegrees, BallConstants::ZeroRotation.Roll);

    const int32 RandomXMultiplier = (FMath::RandRange(0, 1) == 0) ? BallConstants::NegativeMultiplier : BallConstants::PositiveMultiplier;
    const int32 RandomYMultiplier = (FMath::RandRange(0, 1) == 0) ? BallConstants::NegativeMultiplier : BallConstants::PositiveMultiplier;

    FVector RandomizedDirection = Rotation.RotateVector(ForwardVector);
    RandomizedDirection.X *= RandomXMultiplier;
    RandomizedDirection.Y *= RandomYMultiplier;

    BallVelocity = RandomizedDirection.GetSafeNormal() * BallSpeed;

    UE_LOG(LogReplicatedBall, Log, TEXT("Ball initial velocity randomized: %s"), *BallVelocity.ToString());
}

AReplicatedBall::AReplicatedBall()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    SetReplicatingMovement(true);

    BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
    RootComponent = BallMesh;

    BallMesh->SetSimulatePhysics(true);
    BallMesh->SetCollisionProfileName(TEXT("BlockAll"));

    BallMesh->OnComponentHit.AddDynamic(this, &AReplicatedBall::OnBallHit);

    BallSpeed = BallConstants::DefaultBallSpeed;
    InterpolationSpeed = BallConstants::DefaultInterpolationSpeed;

    UE_LOG(LogReplicatedBall, Log, TEXT("AReplicatedBall Constructor: Ball Initialized."));
}

void AReplicatedBall::BeginPlay()
{
    Super::BeginPlay();

    RandomizeBallInitialVelocity();

    if (HasAuthority())
    {
        BallMesh->SetPhysicsLinearVelocity(BallVelocity);
        UE_LOG(LogReplicatedBall, Log, TEXT("BeginPlay: Server has authority, setting initial velocity: %s"), *BallVelocity.ToString());
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
    }
}

void AReplicatedBall::OnBallHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit)
{
    if (HasAuthority())
    {
        FVector HitNormal2D = FVector(Hit.ImpactNormal.X, Hit.ImpactNormal.Y, BallConstants::MinImpactThreshold); // Zero Z-axis reflection
        FVector NewVelocity2D = UKismetMathLibrary::GetReflectionVector(FVector(BallVelocity.X, BallVelocity.Y, BallConstants::MinImpactThreshold), HitNormal2D);

        NewVelocity2D = NewVelocity2D.GetSafeNormal2D() * BallSpeed;

        BallVelocity = FVector(NewVelocity2D.X, NewVelocity2D.Y, BallConstants::MinImpactThreshold);

        BallMesh->SetPhysicsLinearVelocity(BallVelocity);

        UE_LOG(LogReplicatedBall, Log, TEXT("OnBallHit: Ball hit something. New Velocity: %s"), *BallVelocity.ToString());
    }
}

void AReplicatedBall::OnRep_BallVelocity()
{
    SetBallVelocityLocally(BallVelocity);
    UE_LOG(LogReplicatedBall, Log, TEXT("OnRep_BallVelocity: Client updated ball velocity: %s"), *BallVelocity.ToString());
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