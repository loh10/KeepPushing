#pragma once

#include "CoreMinimal.h"
#include "CollectibleData.generated.h"

USTRUCT(BlueprintType)
struct FCollectibleData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Collectibles")
	int MaxCollectibles;

	UPROPERTY(BlueprintReadOnly, Category = "Collectibles")
	int ActualCollectibles;
};
