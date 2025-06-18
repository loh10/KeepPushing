#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "StartAndFinish.generated.h"

UENUM(BlueprintType)
enum class EStartFinishType : uint8
{
	Start UMETA(DisplayName = "Start"),
	Finish UMETA(DisplayName = "Finish"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerEvent, EStartFinishType, triggerType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRaceEndSimple);

UCLASS()
class KEEPPUSHING_API AStartAndFinish : public AActor
{
	GENERATED_BODY()

public:
	AStartAndFinish();

	UPROPERTY(BlueprintAssignable, Category = "Trigger")
	FOnTriggerEvent OnTriggerEvent;

	UPROPERTY(BlueprintAssignable, Category = "Trigger")
	FOnRaceEndSimple OnRaceEndSimple;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	EStartFinishType _triggerType = EStartFinishType::Finish;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
	UBoxComponent* _boxTrigger;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger",
		meta = (ToolTip = "Time in seconds"))
	float _countDown = 3;

	
	UFUNCTION()
	void OnTriggerEnter(
		UPrimitiveComponent* overlappedComponent,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		int32 otherBodyIndex,
		bool bFromSweep,
		const FHitResult& sweepResult
	);
};
