// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollectibleData.h"
#include "GameFramework/SaveGame.h"
#include "CollectibleSave.generated.h"

/**
 * 
 */
UCLASS()
class KEEPPUSHING_API UCollectibleSave : public USaveGame
{
	GENERATED_BODY()

public :
	
	// Données à sauvegarder
	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	TMap<FString, FCollectibleData> SavedCollectionData;
};
