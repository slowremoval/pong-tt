#include "GameActors/ReplicatedBall.h"

#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


DEFINE_LOG_CATEGORY_STATIC(LogReplicatedBall, Log, All);

void AReplicatedBall::RandomizeBallInitialVelocity()
{
	const FVector ForwardVector = GetActorForwardVector().GetSafeNormal();

	const float AngleRangeDegrees = 30.0f;

	const float RandomAngleDegrees = FMath::RandRange(-AngleRangeDegrees, AngleRangeDegrees);
	const FRotator Rotation = FRotator(0.0f, RandomAngleDegrees, 0.0f);

	const int32 RandomXMultiplier = (FMath::RandRange(0, 1) == 0) ? -1 : 1;
	const int32 RandomYMultiplier = (FMath::RandRange(0, 1) == 0) ? -1 : 1;

	FVector RandomizedDirection = Rotation.RotateVector(ForwardVector);

	RandomizedDirection.X *= RandomXMultiplier;
	RandomizedDirection.Y *= RandomYMultiplier;

	BallVelocity = RandomizedDirection.GetSafeNormal() * BallSpeed;

	UE_LOG(LogTemp, Log, TEXT("Ball initial velocity randomized: %s"), *BallVelocity.ToString());
}

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
	InterpolationSpeed = 7.0f;

	TargetPosition = FVector::ZeroVector;

	UE_LOG(LogReplicatedBall, Log, TEXT("AReplicatedBall Constructor: Ball Initialized with Velocity: %s"),
	       *BallVelocity.ToString());
}

void AReplicatedBall::BeginPlay()
{
	Super::BeginPlay();

	RandomizeBallInitialVelocity();

	if (HasAuthority())
	{
		BallMesh->SetPhysicsLinearVelocity(BallVelocity);
		UE_LOG(LogReplicatedBall, Log, TEXT("BeginPlay: Server has authority, setting initial velocity: %s"),
		       *BallVelocity.ToString());
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

		Multicast_SetBallState(GetActorLocation(), BallVelocity);
	}
	else
	{
		InterpolatePosition(DeltaTime);
	}
}

void AReplicatedBall::InterpolatePosition(float DeltaTime)
{
	if (!TargetPosition.IsZero())
	{
		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetPosition, DeltaTime, InterpolationSpeed);

		SetActorLocation(NewLocation);
		UE_LOG(LogReplicatedBall, Verbose, TEXT("Client Interpolating Position: %s -> %s"), *CurrentLocation.ToString(),
		       *NewLocation.ToString());
	}
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

		Multicast_SetBallState(GetActorLocation(), BallVelocity);

		UE_LOG(LogReplicatedBall, Log, TEXT("OnBallHit: Ball hit something. New Velocity: %s"),
		       *BallVelocity.ToString());
	}
}

void AReplicatedBall::OnRep_BallVelocity()
{
	SetBallVelocityLocally(BallVelocity);
	UE_LOG(LogReplicatedBall, Log, TEXT("OnRep_BallVelocity: Client updated ball velocity: %s"),
	       *BallVelocity.ToString());
}

void AReplicatedBall::Multicast_SetBallState_Implementation(const FVector& NewLocation, const FVector& NewVelocity)
{
	TargetPosition = NewLocation;
	SetBallVelocityLocally(NewVelocity);

	UE_LOG(LogReplicatedBall, Log,
	       TEXT("Multicast_SetBallState: Position and Velocity updated. Target Position: %s, New Velocity: %s"),
	       *NewLocation.ToString(), *NewVelocity.ToString());
}

void AReplicatedBall::SetBallVelocityLocally(const FVector& NewVelocity)
{
	BallVelocity = NewVelocity;
	BallMesh->SetPhysicsLinearVelocity(NewVelocity);
	UE_LOG(LogReplicatedBall, Verbose, TEXT("SetBallVelocityLocally: Local velocity updated: %s"),
	       *NewVelocity.ToString());
}

void AReplicatedBall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AReplicatedBall, BallVelocity);
	UE_LOG(LogReplicatedBall, Log, TEXT("GetLifetimeReplicatedProps: Ball velocity marked for replication."));
}
