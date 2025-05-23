#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerComponent.h"
#include "Components/Button.h"
#include "TimerUserWidget.generated.h"

class UTextBlock;

UCLASS()
class KEEPPUSHING_API UTimerUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Timer")
	void InitTimer(UTimerComponent* InTimer) { Timer = InTimer; }

protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TimerTextBlock = nullptr;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UButton* StartButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* PauseButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* ResetButton = nullptr;

	virtual void NativeConstruct() override; // Add this override

	UFUNCTION()
	void OnStartClicked();

	UFUNCTION()
	void OnPauseClicked();

	UFUNCTION()
	void OnResetClicked();

private:
	UPROPERTY()
	UTimerComponent* Timer = nullptr;
};
