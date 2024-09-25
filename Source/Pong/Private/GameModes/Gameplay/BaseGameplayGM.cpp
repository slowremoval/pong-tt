#include "GameModes/Gameplay/BaseGameplayGM.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

APawn* ABaseGameplayGM::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
                                                                   const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo);
	if (!ResultPawn)
	{
		UE_LOG(LogGameMode, Warning, TEXT("SpawnDefaultPawnAtTransform: Couldn't spawn Pawn of type %s at %s"),
		       *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
	}
	return ResultPawn;
}

AActor* ABaseGameplayGM::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	for (AActor* PlayerStart : PlayerStarts)
	{
		if (!UsedPlayerStarts.Contains(PlayerStart))
		{
			UsedPlayerStarts.Add(PlayerStart);
			return PlayerStart;
		}
	}

	if (PlayerStarts.Num() > 0)
	{
		return PlayerStarts[0];
	}

	return nullptr;
}
