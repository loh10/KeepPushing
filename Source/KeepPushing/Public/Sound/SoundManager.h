// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoundLibrary.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sound/SoundBase.h"
#include "SoundManager.generated.h"

/**
 * 
 */
UCLASS()
class KEEPPUSHING_API USoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()


public:
	USoundManager();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable)
	void Play2DSound(FName SoundName);

	UFUNCTION(BlueprintCallable)
	void PlaySoundAtLocation(FName SoundName, FVector Location);

	UFUNCTION(BlueprintPure)
	static USoundManager* Get(UObject* Context);

	UFUNCTION(BlueprintCallable)
	void SetMasterVolume(float Volume);

	UFUNCTION(BlueprintCallable)
	void SaveVolumeToDisk();

	UPROPERTY(BlueprintReadWrite)
	float GlobalVolume;

private:
	UPROPERTY(EditDefaultsOnly)
	USoundLibrary* SoundLibrary;

	TMap<FName, USoundBase*> SoundMap;
};
