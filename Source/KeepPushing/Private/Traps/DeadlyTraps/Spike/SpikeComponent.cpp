#include "Traps/DeadlyTraps/Spike/SpikeComponent.h"

#include "Car/Car.h"

ASpikeComponent::ASpikeComponent()
{
	PrimaryActorTick.bCanEverTick = true;

	_rootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	_rootScene->SetupAttachment(RootComponent);
	
	_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//RootComponent = _mesh;
	//SetRootComponent(_mesh);
	_mesh->SetupAttachment(_rootScene);
	
	_mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_mesh->SetCollisionObjectType(ECC_WorldDynamic);
	_mesh->SetCollisionResponseToAllChannels(ECR_Block);
	_mesh->SetNotifyRigidBodyCollision(true);
	_mesh->SetGenerateOverlapEvents(false);
}

void ASpikeComponent::Init(const FVector& direction, UCurveFloat* moveCurve, float lifeTime)
{
	_moveDirection = direction.GetSafeNormal();
	_movementCurve = moveCurve;
	_lifeTime = lifeTime;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, TEXT("Spike Init"));
}

void ASpikeComponent::BeginPlay()
{
	Super::BeginPlay();

	if (_mesh)
	{
		_mesh->OnComponentHit.AddDynamic(this, &ASpikeComponent::OnHit);
	}
	SetLifeSpan(_lifeTime);
}

void ASpikeComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	_timeElapsed += DeltaTime;
	
	float alpha = FMath::Clamp(_timeElapsed / _lifeTime, 0.0f, 1.0f);
	float speed = _movementCurve ? _movementCurve->GetFloatValue(alpha) : 1000.0f;

	SetActorLocation(GetActorLocation() + _moveDirection * speed * DeltaTime);
}

void ASpikeComponent::OnHit(
		UPrimitiveComponent* hitComponent,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		FVector normalImpulse,
		const FHitResult& hit
	)
{
	if (!otherActor || otherActor == this)
	{
		return;
	}

	ACar* vehicule = Cast<ACar>(otherActor);

	if (vehicule)
	{
		vehicule->Kill(vehicule);
		OnKillCar.Broadcast(vehicule);
		Destroy();
	}
}
