#include "Widgets/MainMenuWidget.h"

#include "Components/Button.h"
#include "Components/EditableText.h"


void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(ConnectButton))
	{
		ConnectButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnConnectButtonPressed);
	}

	if (IsValid(IPTextBox))
	{
		IPTextBox->OnTextChanged.AddDynamic(this, &UMainMenuWidget::OnIPTextBoxChanged);
	}
}

void UMainMenuWidget::OnConnectButtonPressed()
{
	if (!IsValid(IPTextBox)) { return; }

	FString IPAddress = IPTextBox->GetText().ToString();
	if (IPAddress.IsEmpty()) { return; }

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PlayerController)) { return; }

	PlayerController->ClientTravel(IPAddress, ETravelType::TRAVEL_Absolute);
}

void UMainMenuWidget::OnIPTextBoxChanged(const FText& Text)
{
	FString EnteredText = Text.ToString();

	FString FilteredText;
	for (TCHAR Character : EnteredText)
	{
		if (IsValidIPAddressCharacter(Character))
		{
			FilteredText.AppendChar(Character);
		}
	}

	if (EnteredText != FilteredText)
	{
		IPTextBox->SetText(FText::FromString(FilteredText));
	}
}

bool UMainMenuWidget::IsValidIPAddressCharacter(TCHAR Character) const
{
	return (Character >= '0' && Character <= '9') || Character == '.' || Character == ':';
}
