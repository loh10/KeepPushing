// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartAndFinish/StartAndFinish.h"
#include "EndingUI.generated.h"

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

	UFUNCTION()
	void BindEndingEvent(const EStartFinishType Type);
	
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	ATimer* Timer = nullptr;
};
