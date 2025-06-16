#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Timer.generated.h"

class UTimerUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimerSimple);

UCLASS()
class KEEPPUSHING_API ATimer : public AActor
{
	GENERATED_BODY()

public:
	ATimer();

	UFUNCTION(BlueprintCallable, Category="Timer")
	void StartTimer();

	UFUNCTION(BlueprintCallable, Category="Timer")
	void StopTimer();

	UFUNCTION(BlueprintCallable, Category="Timer")
	void ResetTimer();

	UFUNCTION(BlueprintCallable, Category="Timer")
	FString GetFormattedTime() const;

	UFUNCTION(BlueprintCallable, Category = "Timer")
	float GetElapsed() const { return Elapsed; }

	
	UPROPERTY(BlueprintAssignable, Category="Timer|Events")
	FOnTimerSimple OnStart;

	UPROPERTY(BlueprintAssignable, Category="Timer|Events")
	FOnTimerSimple OnStop;

	UPROPERTY(BlueprintAssignable, Category="Timer|Events")
	FOnTimerSimple OnRestart;
	
	UPROPERTY(EditDefaultsOnly, Category="Timer|UI")
	TSubclassOf<UTimerUserWidget> TimerWidgetClass;


	UPROPERTY()
	TObjectPtr<UTimerUserWidget> TimerWidget;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

private:
	float Elapsed = 0.f;
	bool bRunning = false;
};
