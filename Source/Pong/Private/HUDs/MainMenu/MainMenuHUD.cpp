#include "HUDs/MainMenu/MainMenuHUD.h"

#include "Blueprint/UserWidget.h"

void AMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(WidgetClass)) { return; }

	APlayerController* PlayerController = GetOwningPlayerController();
	if (!IsValid(PlayerController)) { return; }

	HUDWidget = CreateWidget<UUserWidget>(PlayerController, WidgetClass);
	if (!IsValid(HUDWidget)) { return; }

	HUDWidget->AddToViewport();

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(HUDWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);

	PlayerController->bShowMouseCursor = true;
}

void AMainMenuHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

		PlayerController->bShowMouseCursor = false;
	}

	Super::EndPlay(EndPlayReason);
}
