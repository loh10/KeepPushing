#include "Collectible/Collectible.h"

#include "Car/Car.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ACollectible::ACollectible()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize Sphere Collider
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->InitSphereRadius(100.0f); // Default radius
	RootComponent = SphereCollider;

	// Initialize Mesh Component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	// Enable collision events
	SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereCollider->SetCollisionResponseToAllChannels(ECR_Overlap);

	// Bind the overlap event
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ACollectible::OnOverlapBegin);

	// Default values
	Radius = 100.0f;
	Mesh = nullptr;
}

// Called when the game starts or when spawned
void ACollectible::BeginPlay()
{
	Super::BeginPlay();

	// Apply the radius to the Sphere Collider
	if (SphereCollider)
	{
		SphereCollider->SetSphereRadius(Radius);
	}

	// Apply the mesh to the Mesh Component
	if (Mesh && MeshComponent)
	{
		MeshComponent->SetStaticMesh(Mesh);
	}
}

// Called every frame
void ACollectible::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACollectible::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the overlapping actor is of type Car
	if (OtherActor && OtherActor->IsA(ACar::StaticClass()))
	{
		Collected.Broadcast(this);
	}
}

#if WITH_EDITOR
void ACollectible::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the name of the property that was changed
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Update the Sphere Collider radius if the Radius property was changed
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ACollectible, Radius) && SphereCollider)
	{
		SphereCollider->SetSphereRadius(Radius);
	}

	// Update the Mesh Component if the Mesh property was changed
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ACollectible, Mesh) && MeshComponent)
	{
		MeshComponent->SetStaticMesh(Mesh);
	}
}
#endif