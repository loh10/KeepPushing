#include "VoidZone/VoidZone.h"

#include "Car/Car.h"
#include "Components/BoxComponent.h"

AVoidZone::AVoidZone()
{
	PrimaryActorTick.bCanEverTick = true;

	_triggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
	SetRootComponent(_triggerZone);

	_triggerZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_triggerZone->SetCollisionObjectType(ECC_WorldDynamic);
	_triggerZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	_triggerZone->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	_triggerZone->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	
	_triggerZone->OnComponentBeginOverlap.AddDynamic(this, &AVoidZone::OnOverlapBegin);
}

void AVoidZone::BeginPlay()
{
	Super::BeginPlay();
}

void AVoidZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVoidZone::OnOverlapBegin(
	UPrimitiveComponent* overlappedComp,
	AActor* otherActor,
	UPrimitiveComponent* otherComp,
	int32 otherBodyIndex,
	bool bFromSweep,
	const FHitResult& sweepResult)
{
	if (!otherActor || otherActor == this)
	{
		return;
	}

	const ACar* vehicle = Cast<ACar>(otherActor);

	if (!vehicle)
	{
		return;
	}

	OnVoidZoneTouched.Broadcast(otherActor);
}
