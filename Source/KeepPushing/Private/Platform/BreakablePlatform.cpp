#include "KeepPushing/Public/Platform/BreakablePlatform.h"


ABreakablePlatform::ABreakablePlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	mesh -> SetNotifyRigidBodyCollision(true);
}

void ABreakablePlatform::BeginPlay()
{
	Super::BeginPlay();

	if (mesh)
	{
		//UE_LOG(LogTemp, Warning, TEXT("BIND OK"));
		mesh->OnComponentHit.AddDynamic(this, &ABreakablePlatform::OnComponentHit);
	}
}

void ABreakablePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_isActivated)
	{
		_currentTimer += DeltaTime;

		// Start blinking in the last second
		if (_currentTimer >= _timerToDestroy - 1.f)
		{
			_isBlinking = true;
			StartBlinking();
		}

		if (_currentTimer >= _timerToDestroy)
		{
			DestroyPlatform();
		}
	}
}


void ABreakablePlatform::StartBlinking()
{
	blinkTimer += GetWorld()->GetDeltaSeconds();

	if (blinkTimer >= 0.1f)
	{
		blinkTimer = 0.f;
		mesh->SetVisibility(!mesh->IsVisible());
	}
}

void ABreakablePlatform::OnComponentHit(
			UPrimitiveComponent* hitComponent,
			AActor* otherActor,
			UPrimitiveComponent* otherComp,
			FVector normalImpulse,
			const FHitResult& hit
		)
{

	UE_LOG(LogTemp, Warning, TEXT("Unter Overlap"));

	
	if (!_isActivated && otherActor && otherActor != this)
	{
		_isActivated = true;
		// Jouer un son si besoin :
		// UGameplayStatics::PlaySoundAtLocation(this, soundToPlay, GetActorLocation());

		// Affichage à l'écran (durée 2s, couleur rouge, id unique)
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Red,
				FString::Printf(TEXT("Touched by: %s"), *otherActor->GetName())
			);
		}
		
	}
}

void ABreakablePlatform::DestroyPlatform()
{
	Destroy();
}

