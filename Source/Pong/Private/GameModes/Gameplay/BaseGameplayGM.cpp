#include "GameModes/Gameplay/BaseGameplayGM.h"

#include "GameActors/Gates.h"
#include "GameFramework/PlayerStart.h"
#include "GameStates/GameplayState.h"
#include "Kismet/GameplayStatics.h"

void ABaseGameplayGM::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentGameState = GetGameState<AGameplayState>();
	
	TArray<AActor*> FoundGates;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGates::StaticClass(), FoundGates);

	for (AActor* Actor : FoundGates)
	{
		AGates* Gate = Cast<AGates>(Actor);
		if (Gate)
		{
			Gates.Add(Gate);
			Gate->OnBallEnter.BindDynamic(this, &ABaseGameplayGM::OnGoalScored);
		}
	}
}

void ABaseGameplayGM::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	Multicast_PlayerJoined(NewPlayer);
}

void ABaseGameplayGM::Multicast_PlayerJoined_Implementation(APlayerController* NewPlayer)
{
	++JoinedPlayers;

	if (JoinedPlayers >= TargetPlayerCount && !bGameStarted)
	{
		bGameStarted = true;
		if (CurrentGameState)
		{
			CurrentGameState->Multicast_GameStarted();
		}

		Server_SpawnBallInCenter();
	}
}

AActor* ABaseGameplayGM::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	for (AActor* PlayerStart : PlayerStarts)
	{
		if (!UsedPlayerStarts.Contains(PlayerStart))
		{
			UsedPlayerStarts.Add(PlayerStart);
			return PlayerStart;
		}
	}

	return PlayerStarts.Num() > 0 ? PlayerStarts[0] : nullptr;
}

APawn* ABaseGameplayGM::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	if (PawnClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = GetInstigator();

		return GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnParams);
	}
	return nullptr;
}

void ABaseGameplayGM::SpawnBallInCenter()
{
	if (IsValid(BallClass))
	{
		if (IsValid(SpawnedBall))
		{
			SpawnedBall->Destroy();
		}

		FVector SpawnLocation = FVector(0.0f, 0.0f, 400.0f);
		FRotator SpawnRotation = FRotator::ZeroRotator;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		SpawnedBall = GetWorld()->SpawnActor<AActor>(BallClass, SpawnLocation, SpawnRotation, SpawnParams);
	}
}

void ABaseGameplayGM::Server_SpawnBallInCenter_Implementation()
{
	Multicast_SpawnBallInCenter();
}

void ABaseGameplayGM::Multicast_SpawnBallInCenter_Implementation()
{
	SpawnBallInCenter();
}

void ABaseGameplayGM::OnGoalScored(int32 GateIndex)
{
	if (CurrentGameState)
	{
		CurrentGameState->Multicast_UpdateScore(GateIndex);
	}

	SpawnBallInCenter();
}