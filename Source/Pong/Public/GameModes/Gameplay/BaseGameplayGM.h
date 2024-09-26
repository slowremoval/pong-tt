#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameplayGM.generated.h"

class AGameplayState;

UCLASS()
class PONG_API ABaseGameplayGM : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameState")
	TObjectPtr<AGameplayState> CurrentGameState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gates")
	TArray<class AGates*> Gates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball")
	TObjectPtr<AActor> SpawnedBall;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ball")
	TSubclassOf<AActor> BallClass;

	UPROPERTY(EditDefaultsOnly, Category = "Game Settings")
	int32 TargetPlayerCount = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Settings")
	int32 JoinedPlayers = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	bool bGameStarted = false;
	
	const FVector BallSpawnLocation = FVector(0.0f, 0.0f, 400.0f);
	const FRotator BallSpawnRotation = FRotator::ZeroRotator; 
	const int32 DefaultTargetPlayerCount = 2;
	
	TArray<AActor*> UsedPlayerStarts;

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;

	virtual void BeginPlay() override;

	void SpawnBallInCenter();

protected:
	UFUNCTION(Server, Reliable)
	void Server_SpawnBallInCenter();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnBallInCenter();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayerJoined(APlayerController* NewPlayer);

	UFUNCTION()
	void OnGoalScored(int32 GateIndex);
};