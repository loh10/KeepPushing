#include "StartAndFinish/StartAndFinish.h"


AStartAndFinish::AStartAndFinish()
{
	PrimaryActorTick.bCanEverTick = true;

	_boxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrigger"));
	SetRootComponent(_boxTrigger);

	_boxTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_boxTrigger->SetCollisionObjectType(ECC_WorldDynamic);
	_boxTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	_boxTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	_boxTrigger->SetGenerateOverlapEvents(true);
}

void AStartAndFinish::BeginPlay()
{
	Super::BeginPlay();

	if (_boxTrigger)
	{
		_boxTrigger->OnComponentBeginOverlap.AddDynamic(this, &AStartAndFinish::OnTriggerEnter);
	}
}

void AStartAndFinish::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AStartAndFinish::OnTriggerEnter(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (otherActor && otherActor != this)
	{
		switch (_triggerType)
		{
		case EStartFinishType::Start:
			OnTriggerEvent.Broadcast(EStartFinishType::Start);
			UE_LOG(LogTemp, Warning, TEXT("Event Start Race"));
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Green,
				FString::Printf(TEXT("Start Collided with: %s"), *otherActor->GetName())
				);
			break;

		case EStartFinishType::Finish:
			OnTriggerEvent.Broadcast(EStartFinishType::Finish);
			UE_LOG(LogTemp, Warning, TEXT("Event Finish Race"));
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Blue,
				FString::Printf(TEXT("Finish Collided with: %s"), *otherActor->GetName())
				);
			break;

		default:
			break;
		}
	}
}

