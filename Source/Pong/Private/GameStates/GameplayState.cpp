#include "GameStates/GameplayState.h"

AGameplayState::AGameplayState()
{
	GateScores = TMap<int32, int32>();
}

void AGameplayState::UpdateScore(int32 GateIndex)
{
	if (GateScores.Contains(GateIndex))
	{
		++GateScores[GateIndex];

		if (ScoreUpdatedDelegate.IsBound())
		{
			ScoreUpdatedDelegate.Execute(GateIndex, GateScores[GateIndex]);
		}
	}
}

void AGameplayState::Multicast_GameStarted_Implementation()
{
	bGameStarted = true;
	GameStartedDelegate.Broadcast();
}

void AGameplayState::Server_GameStarted_Implementation()
{
	Multicast_GameStarted();
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
	return -1;
}
