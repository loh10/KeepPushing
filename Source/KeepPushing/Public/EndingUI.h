// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartAndFinish/StartAndFinish.h"
#include "EndingUI.generated.h"

class UTextBlock;
class ATimer;
/**
 * 
 */
UCLASS(Abstract)
class KEEPPUSHING_API UEndingUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void GetTimer();

	UFUNCTION(BlueprintCallable, Category = "EndingUI")
	float GetGameTimer();	

	UFUNCTION()
	void BindEndingEvent(const EStartFinishType Type);
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* FinalTime = nullptr;

private:
	UPROPERTY()
	ATimer* Timer = nullptr;
};
