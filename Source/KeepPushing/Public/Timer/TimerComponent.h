#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KEEPPUSHING_API UTimerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTimerComponent();

	UFUNCTION(BlueprintCallable, Category="Timer")
	void StartTimer();

	UFUNCTION(BlueprintCallable, Category="Timer")
	void StopTimer();

	UFUNCTION(BlueprintCallable, Category="Timer")
	void ResetTimer();

	UFUNCTION(BlueprintCallable, Category="Timer")
	FString GetFormattedTime() const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	float Elapsed = 0.f;
	bool bRunning = false;
};
