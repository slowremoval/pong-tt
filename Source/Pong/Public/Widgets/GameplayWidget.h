#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayWidget.generated.h"

class ABaseGameplayGM;
class AGameplayState;
class UTextBlock;

UCLASS()
class PONG_API UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void InitializeBindings();
	UFUNCTION()
	void CheckIfGameStarted();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Player1ScoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Player2ScoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MessageText;
	FTimerHandle GameStartCheckTimerHandle;
	FTimerHandle GameStartTimerHandle;

	UFUNCTION()
	void UpdatePlayerScore(int32 Player1Score, int32 Player2Score);

	UFUNCTION()
	void UpdateMessage(const FString& NewMessage);
protected:
	TObjectPtr<AGameplayState> GameState;

	UFUNCTION()
	void HandleGameStarted();

	UFUNCTION()
	void HandleScoreUpdated(int32 GateIndex, int32 Score);
};
