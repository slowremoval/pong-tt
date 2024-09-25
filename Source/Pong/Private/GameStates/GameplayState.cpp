#include "GameStates/GameplayState.h"

#include "GameActors/ReplicatedBall.h"
#include "Kismet/GameplayStatics.h"

AGameplayState::AGameplayState()
{
	GateScores = TMap<int32, int32>();
	bGameStarted = false;
}

int32 AGameplayState::GetScore(int32 GateIndex) const
{
	const int32* Score = GateScores.Find(GateIndex);
	return Score ? *Score : 0;
}

void AGameplayState::UpdateScore(int32 GateIndex)
{
	int32& Score = GateScores.FindOrAdd(GateIndex);
	Score++;

	if (ScoreUpdatedDelegate.IsBound())
	{
		ScoreUpdatedDelegate.Broadcast(GateIndex, Score);
	}
}

void AGameplayState::Server_UpdateScore_Implementation(int32 GateIndex)
{
	Multicast_UpdateScore(GateIndex);
}

void AGameplayState::Multicast_UpdateScore_Implementation(int32 GateIndex)
{
	UpdateScore(GateIndex);
}

bool AGameplayState::IsGameStarted()
{
	if (!bGameStarted)
	{
		TArray<AActor*> FoundPawns;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AReplicatedBall::StaticClass(), FoundPawns);

		if (FoundPawns.Num() > 0)
		{
			Multicast_GameStarted();
			return true;
		}
	}
	return bGameStarted;
}

void AGameplayState::Multicast_GameStarted_Implementation()
{
	if (!bGameStarted)
	{
		bGameStarted = true;
		if (GameStartedDelegate.IsBound())
		{
			GameStartedDelegate.Broadcast();
		}
	}
}