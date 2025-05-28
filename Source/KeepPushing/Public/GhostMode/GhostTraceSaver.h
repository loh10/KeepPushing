#pragma once

#include "CoreMinimal.h"
#include "GhostModeData.h"
#include "GameFramework/SaveGame.h"
#include "GhostTraceSaver.generated.h"

UCLASS()
class KEEPPUSHING_API UGhostTraceSaver : public USaveGame
{
	GENERATED_BODY()

public:
	UGhostTraceSaver();

public:
	UPROPERTY(VisibleAnywhere)
	TMap<FString, FTrace> TracesPoints;
};
