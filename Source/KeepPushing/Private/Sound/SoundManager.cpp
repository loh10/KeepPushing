// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/SoundManager.h"

#include "AudioDevice.h"
#include "GameSave/GamePrefSave.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Kismet/GameplayStatics.h"

USoundManager::USoundManager()
{
	static ConstructorHelpers::FObjectFinder<USoundLibrary> SoundLibObj(TEXT("/Game/Audio/SoundLibrary.SoundLibrary"));
	if (SoundLibObj.Succeeded())
	{
		SoundLibrary = SoundLibObj.Object;
	}
}

void USoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//Load all the sounds
	if (SoundLibrary)
	{
		for (const FNamedSound& Entry : SoundLibrary->Sounds)
		{
			if (Entry.Sound)
			{
				if (Entry.Sound->SoundClassObject == nullptr)
				{
					Entry.Sound->SoundClassObject = LoadObject<USoundClass>(nullptr, TEXT("/Game/Audio/Master_SoundClass.Master_SoundClass"));
				}
			}
			SoundMap.Add(Entry.Name,Entry.Sound);
		}
	}

	//Load saved settings
	UGamePrefSave* Save = Cast<UGamePrefSave>(
		UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSlot"),0));

	float Volume = 1.0f;
	if (Save)
	{
		Volume = Save->MasterVolume;
	}

	SetMasterVolume(Volume);
}



void USoundManager::Play2DSound(FName SoundName)
{
	if (USoundBase** Found = SoundMap.Find(SoundName))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), *Found);
	}
}

void USoundManager::PlaySoundAtLocation(FName SoundName, FVector Location)
{
	if (USoundBase** Found = SoundMap.Find(SoundName))
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), *Found, Location);
	}
}

USoundManager* USoundManager::Get(UObject* Context)
{
	if (!Context) return nullptr;

	if (UWorld* World = Context->GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			return GI->GetSubsystem<USoundManager>();
		}
	}

	return nullptr;
}

void USoundManager::SetMasterVolume(float Volume)
{
	static const FName MasterClassName = TEXT("Master_SoundClass");

	USoundMix* MasterMix = LoadObject<USoundMix>(nullptr, TEXT("/Game/Audio/Master_Mix.Master_Mix"));
	USoundClass* MasterClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/Audio/Master_SoundClass.Master_SoundClass"));

	if (!MasterMix || !MasterClass) return;

	UGameplayStatics::SetSoundMixClassOverride(
		this,
		MasterMix,
		MasterClass,
		FMath::Clamp(Volume, 0.f,1.f),
		1.0f,
		0.0f,
		true
		);

	UGameplayStatics::PushSoundMixModifier(this, MasterMix);
	GlobalVolume = Volume;
	
}

void USoundManager::SaveVolumeToDisk()
{
	UGamePrefSave* Save = Cast<UGamePrefSave>(
		UGameplayStatics::CreateSaveGameObject(UGamePrefSave::StaticClass()));

	if (!Save) return;

	Save->MasterVolume = GlobalVolume;
	UGameplayStatics::SaveGameToSlot(Save, TEXT("SettingsSlot"), 0);
}


