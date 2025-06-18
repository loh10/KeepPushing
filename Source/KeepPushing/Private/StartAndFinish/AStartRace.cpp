#include "StartAndFinish/AStartRace.h"


#include "Kismet/GameplayStatics.h"
#include "Sound/SoundManager.h"
#include "Timer/Timer.h"

AAStartRace::AAStartRace()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAStartRace::BeginPlay()
{
	Super::BeginPlay();

	SetPlayerInputEnabled(false);

	BeginCountdown();
}

void AAStartRace::BeginCountdown()
{
	_currentCount = _countdownStart + _offsetCountdown;
	UpdateCountdown();
	GetWorldTimerManager().SetTimer(_countdownTimer, this, &AAStartRace::UpdateCountdown, 1.f, true);
}

void AAStartRace::UpdateCountdown()
{
	if (_currentCount > 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Countdown: %d"), _currentCount));
		FText countdownText = FText::AsNumber(_currentCount);
		OnCountdownUpdated(countdownText);
		--_currentCount;
	}
	else
	{
		FText goText = FText::FromString("GO");
		OnCountdownUpdated(goText);
		FinishCountdown();
	}
}

void AAStartRace::FinishCountdown()
{
	GetWorldTimerManager().ClearTimer(_countdownTimer);
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("GO!"));
	OnCountdownGo();
	SetPlayerInputEnabled(true);

	OnStart.Broadcast();
	
	// === Lancer tous les timers ===
	TArray<AActor*> Timers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATimer::StaticClass(), Timers);
	for (AActor* TimerActor : Timers)
	{
		if (ATimer* Timer = Cast<ATimer>(TimerActor))
		{
			Timer->StartTimer();
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Start Timer !"));
	FTimerHandle HideUITimer;
	GetWorldTimerManager().SetTimer(
		HideUITimer,
		this,
		&AAStartRace::TriggerCountdownHide,
		_countdownHideDelay,   // Délai avant de cacher (1 seconde ici, ajuste si besoin)
		false
	);

	USoundManager::Get(this)->PlayMusic("GameMusic");
}

void AAStartRace::TriggerCountdownHide()
{
	OnCountdownHide();
}

void AAStartRace::SetPlayerInputEnabled(bool bEnabled)
{
	// Pour tous les PlayerControllers (ou juste le 0 si solo)
	UWorld* World = GetWorld();
	if (!World) return;

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC)
		{
			PC->SetIgnoreMoveInput(!bEnabled);
			PC->SetIgnoreLookInput(!bEnabled);

			// Ou, pour du Pawn possédé :
			APawn* Pawn = PC->GetPawn();
			if (Pawn)
			{
				Pawn->DisableInput(PC);
				if (bEnabled) Pawn->EnableInput(PC);
			}
		}
	}
}