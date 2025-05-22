#include "KeepPushing/Public/Platform//Platform.h"


APlatform::APlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	// setmobility
	
	RootComponent = mesh;
}

void APlatform::BeginPlay()
{
	Super::BeginPlay();

	startPosition = GetActorLocation();
}

void APlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

