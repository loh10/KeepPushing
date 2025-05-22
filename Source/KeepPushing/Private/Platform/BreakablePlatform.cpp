#include "KeepPushing/Public/Platform/BreakablePlatform.h"


ABreakablePlatform::ABreakablePlatform()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABreakablePlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreakablePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABreakablePlatform::StartBlinking()
{
	
}

void ABreakablePlatform::OnPlayerContact()
{
	
}

void ABreakablePlatform::DestroyPlatform()
{
	
}

void ABreakablePlatform::NotifyPlayerContact()
{
	
}

