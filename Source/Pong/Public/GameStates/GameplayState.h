#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameplayState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreUpdated, int32, GateIndex, int32, NewScore);

UCLASS()
class PONG_API AGameplayState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AGameplayState();

	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetScore(int32 GateIndex) const;

	FOnScoreUpdated ScoreUpdatedDelegate;

	FOnGameStarted GameStartedDelegate;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateScore(int32 GateIndex);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameStarted();

	UFUNCTION(Category = "Game")
	bool IsGameStarted();
	
protected:
	UFUNCTION()
	void UpdateScore(int32 GateIndex);

	UFUNCTION(Server, Reliable)
	void Server_UpdateScore(int32 GateIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	TMap<int32, int32> GateScores;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	bool bGameStarted;

};