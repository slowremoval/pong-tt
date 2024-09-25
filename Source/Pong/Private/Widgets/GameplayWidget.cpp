#include "Widgets/GameplayWidget.h"

#include "Components/TextBlock.h"
#include "GameStates/GameplayState.h"
#include "Kismet/GameplayStatics.h"


void UGameplayWidget::InitializeBindings()
{
	GameState = Cast<AGameplayState>(UGameplayStatics::GetGameState(this));
	
	if (IsValid(GameState))
	{
		GameState->ScoreUpdatedDelegate.AddDynamic(this, &UGameplayWidget::HandleScoreUpdated);
		GameState->GameStartedDelegate.AddDynamic(this, &UGameplayWidget::HandleGameStarted);
	}
	GetWorld()->GetTimerManager().SetTimer(GameStartTimerHandle, this, &UGameplayWidget::CheckIfGameStarted, 1.0f, false);
}

void UGameplayWidget::CheckIfGameStarted()
{
	if (GameState->GetGameStarted())
	{
		HandleGameStarted();
	}
}

void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UpdateMessage(TEXT("Waiting for the opponent..."));

	InitializeBindings();
}


void UGameplayWidget::HandleGameStarted()
{
	UpdateMessage(TEXT(""));
	HandleScoreUpdated(0, 0);
}

void UGameplayWidget::HandleScoreUpdated(int32 GateIndex, int32 Score)
{
	if (IsValid(GameState))
	{
		int32 Player1Score = GameState->GetScore(1); 
		int32 Player2Score = GameState->GetScore(2); 
		UpdatePlayerScore(Player1Score, Player2Score);
	}
}

void UGameplayWidget::UpdatePlayerScore(int32 Player1Score, int32 Player2Score)
{
	if (IsValid((Player1ScoreText)))
	{
		Player1ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Player 1: %d"), Player1Score)));
	}

	if (IsValid(Player2ScoreText))
	{
		Player2ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Player 2: %d"), Player2Score)));
	}
}

void UGameplayWidget::UpdateMessage(const FString& NewMessage)
{
	if (IsValid(MessageText))
	{
		MessageText->SetText(FText::FromString(NewMessage));
	}
}