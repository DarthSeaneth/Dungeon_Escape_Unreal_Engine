// Copyright Sean Patrick 2021


#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "OpenDoor.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	
	SetupDoorBehavior();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TotalMassOfActors() >= PressurePlateWeightThreshold)
	{
		OpenDoor(DeltaTime);
		WhenDoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if (GetWorld()->GetTimeSeconds() - WhenDoorLastOpened >= DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::SetupDoorBehavior()
{
	CloseAngle = GetOwner()->GetActorRotation().Yaw;
	CurrentAngle = CloseAngle;
	OpenAngle += CloseAngle;
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing Audio Component!"), *GetOwner()->GetName());
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	CurrentAngle = FMath::FInterpTo(GetOwner()->GetActorRotation().Yaw, OpenAngle, DeltaTime, 1.75f);
	FRotator OpenDoor(GetOwner()->GetActorRotation());
	OpenDoor.Yaw = CurrentAngle;
	GetOwner()->SetActorRotation(OpenDoor);

	CloseDoorSound = false;
	if(!AudioComponent){return;}
	if (!OpenDoorSound)
	{
		AudioComponent->Play();
		OpenDoorSound = true;
	}
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentAngle = FMath::FInterpTo(GetOwner()->GetActorRotation().Yaw, CloseAngle, DeltaTime, 3.f);
	FRotator CloseDoor(GetOwner()->GetActorRotation());
	CloseDoor.Yaw = CurrentAngle;
	GetOwner()->SetActorRotation(CloseDoor);
	
	OpenDoorSound = false;
	if (!AudioComponent){return;}
	if (!CloseDoorSound)
	{
		AudioComponent->Play();
		CloseDoorSound = true;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	TArray<AActor*> OverlappingActors;
	if(!PressurePlate) {return TotalMass;}
	PressurePlate->GetOverlappingActors(OUT(OverlappingActors));
	for (AActor* OverlappingActor : OverlappingActors)
	{
		TotalMass += OverlappingActor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	return TotalMass;
}