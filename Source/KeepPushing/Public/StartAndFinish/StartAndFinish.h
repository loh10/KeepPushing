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

UCLASS()
class KEEPPUSHING_API AStartAndFinish : public AActor
{
	GENERATED_BODY()

public:
	AStartAndFinish();

	
protected:
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	EStartFinishType _triggerType = EStartFinishType::Start;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
	UBoxComponent* _boxTrigger;

	
public:
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(BlueprintAssignable, Category = "Trigger")
	FOnTriggerEvent OnTriggerEvent;
	

protected:
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
