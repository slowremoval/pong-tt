
#include "HUDs/Gameplay/GameplayHUD.h"

#include "Blueprint/UserWidget.h"


void AGameplayHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(WidgetClass)) { return; }

	APlayerController* PlayerController = GetOwningPlayerController();
	if (!IsValid(PlayerController)) { return; }

	HUDWidget = CreateWidget<UUserWidget>(PlayerController, WidgetClass);
	if (!IsValid(HUDWidget)) { return; }

	HUDWidget->AddToViewport();

	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);

	PlayerController->bShowMouseCursor = false;
}

void AGameplayHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(HUDWidget))
	{
		HUDWidget->RemoveFromParent();
		HUDWidget = nullptr;
	}

	APlayerController* PlayerController = GetOwningPlayerController();

	if (IsValid(PlayerController))
	{
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);

		PlayerController->bShowMouseCursor = true;
	}

	Super::EndPlay(EndPlayReason);
}
