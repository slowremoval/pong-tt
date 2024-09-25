// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameplayGM.generated.h"

/**
 * 
 */
UCLASS()
class PONG_API ABaseGameplayGM : public AGameModeBase
{
	GENERATED_BODY()
protected:
	TArray<AActor*> UsedPlayerStarts;

public:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	public:
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
};
