#include "KeepPushing/Public/LifeZone/LifeZone.h"

#include "Car/Car.h"
#include "Components/BoxComponent.h"

ALifeZone::ALifeZone()
{
	PrimaryActorTick.bCanEverTick = true;

	_triggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
	SetRootComponent(_triggerZone);

	_triggerZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_triggerZone->SetCollisionObjectType(ECC_Pawn);
	_triggerZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	_triggerZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	_triggerZone->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	
	//_triggerZone->OnComponentBeginOverlap.AddDynamic(this, &ALifeZone::OnOverlapBegin);
	_triggerZone->OnComponentEndOverlap.AddDynamic(this, &ALifeZone::OnOverlapEnd);
}

void ALifeZone::BeginPlay()
{
	Super::BeginPlay();
}

void ALifeZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALifeZone::OnOverlapEnd(
		UPrimitiveComponent* overlappedComp,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		int32 otherBodyIndex)
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
