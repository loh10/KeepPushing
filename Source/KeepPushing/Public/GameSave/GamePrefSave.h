// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GamePrefSave.generated.h"

/**
 * 
 */
UCLASS()
class KEEPPUSHING_API UGamePrefSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float MasterVolume = 1.0f;
};
