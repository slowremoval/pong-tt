#include "GameModes/Gameplay/BaseGameplayGM.h"

#include "GameActors/Gates.h"
#include "GameFramework/PlayerStart.h"
#include "GameStates/GameplayState.h"
#include "Kismet/GameplayStatics.h"

APawn* ABaseGameplayGM::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
                                                                   const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo);
	if (!ResultPawn)
	{
		UE_LOG(LogGameMode, Warning, TEXT("SpawnDefaultPawnAtTransform: Couldn't spawn Pawn of type %s at %s"),
		       *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
	}
	return ResultPawn;
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

	if (PlayerStarts.Num() > 0)
	{
		return PlayerStarts[0];
	}

	return nullptr;
}


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

void ABaseGameplayGM::Multicast_PlayerJoined_Implementation(APlayerController* NewPlayer)
{
	++JoinedPlayers;
	
	if (JoinedPlayers >= TargetPlayerCount)
	{
		CurrentGameState->Multicast_GameStarted();
		Server_SpawnBallInCenter();
	}
}

void ABaseGameplayGM::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    Multicast_PlayerJoined(NewPlayer);
}

void ABaseGameplayGM::SpawnBallInCenter()
{
    if (IsValid(BallClass))
    {
        if (IsValid(SpawnedBall))
        {
            SpawnedBall->Destroy();
        }

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
    	
        const FVector SpawnLocation = FVector(0.0f, 0.0f, 400.0f); 
        const FRotator SpawnRotation = FRotator::ZeroRotator;

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
	CurrentGameState->Multicast_UpdateScore(GateIndex);
	SpawnBallInCenter();
}
