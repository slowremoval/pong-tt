// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameplayGM.generated.h"

class AGameplayState;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGateTriggered, int32, GateIndex);

UCLASS()
class PONG_API ABaseGameplayGM : public AGameModeBase
{
	GENERATED_BODY()
protected:
	TArray<AActor*> UsedPlayerStarts;
	int32 JoinedPlayers;
	bool bGameStarted;
	
	TObjectPtr<AGameplayState> CurrentGameState;

public:
	int32 GetJoinedPlayers() const
	{
		return JoinedPlayers;
	}

	bool GetGameStarted() const
	{
		return bGameStarted;
	}

protected:
	const int32 TargetPlayerCount {2};

public:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	void SpawnBallInCenter();

	virtual void BeginPlay() override;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayerJoined(APlayerController* NewPlayer);

	FOnGateTriggered OnGateTriggered;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gates")
	TArray<class AGates*> Gates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball")
	TSubclassOf<AActor> BallClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball")
	TObjectPtr<AActor>SpawnedBall;

	UFUNCTION(Server, Reliable)
	void Server_SpawnBallInCenter();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnBallInCenter();

	UFUNCTION()
	void OnGoalScored(int32 GateIndex);
};