#include "GameStates/GameplayState.h"

#include "GameActors/ReplicatedBall.h"
#include "Kismet/GameplayStatics.h"

AGameplayState::AGameplayState()
{
	GateScores = TMap<int32, int32>();
}

void AGameplayState::UpdateScore(int32 GateIndex)
{
	if (!GateScores.Contains(GateIndex))
	{
		GateScores.Add(GateIndex);
	}
	++GateScores[GateIndex];

	ScoreUpdatedDelegate.Broadcast(GateIndex, GateScores[GateIndex]);
}

bool AGameplayState::GetGameStarted()
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
	bGameStarted = true;
	GameStartedDelegate.Broadcast();
}

void AGameplayState::Multicast_UpdateScore_Implementation(int32 GateIndex)
{
	UpdateScore(GateIndex);
}

void AGameplayState::Server_UpdateScore_Implementation(int32 GateIndex)
{
	Multicast_UpdateScore_Implementation(GateIndex);
}

int32 AGameplayState::GetScore(int32 GateIndex) const
{
	if (GateScores.Contains(GateIndex))
	{
		return *GateScores.Find(GateIndex);
	}
	return 0;
}
