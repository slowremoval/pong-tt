#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameplayState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStarted);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnScoreUpdated, int32, GateIndex, int32, NewScore);

UCLASS()
class PONG_API AGameplayState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AGameplayState();
	
	UFUNCTION()
	int32 GetScore(int32 GateIndex) const;
	
	FOnScoreUpdated ScoreUpdatedDelegate;
	FOnGameStarted GameStartedDelegate;

private:
	UFUNCTION()
	void UpdateScore(int32 GateIndex);
	
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateScore(int32 GateIndex);

	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameStarted();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	TMap<int32, int32> GateScores;

	bool bGameStarted;
	
public:

	bool GetGameStarted();

	UFUNCTION(Server, Reliable)
	void Server_GameStarted();
	
	UFUNCTION(Server, Reliable)
	void Server_UpdateScore(int32 GateIndex);
};
