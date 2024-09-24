#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Pawns/PlatformPawn.h"
#include "PlatformPC.generated.h"

class UInputMappingContext;
class UInputAction;

struct FInputActionValue;

UCLASS()
class PONG_API APlatformPC : public APlayerController
{
	GENERATED_BODY()

public:
	void SetupMappingContext();
	void CachePlayerPawn();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	void BindActions(UEnhancedInputComponent* EnhancedInputComp);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> MovementMappingContext;
	
	UPROPERTY()
	TObjectPtr<APlatformPawn> PlatformPawn;

protected:
    void Move(const FInputActionValue& Value);
};
