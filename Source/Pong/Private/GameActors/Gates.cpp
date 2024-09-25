#include "GameActors/Gates.h"

#include "Components/BoxComponent.h"
#include "GameActors/ReplicatedBall.h"


AGates::AGates()
{
	PrimaryActorTick.bCanEverTick = false;

	GateCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("GateCollider"));
	RootComponent = GateCollider;

	GateCollider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	GateCollider->OnComponentBeginOverlap.AddDynamic(this, &AGates::OnGateBeginOverlap);

	GateIndex = 0;
}


void AGates::Server_HandleBallEnterGate_Implementation(int32 Index)
{
	Multicast_HandleBallEnterGate(Index);
}

void AGates::Multicast_HandleBallEnterGate_Implementation(int32 Index)
{
	if (OnBallEnter.IsBound())
	{
		OnBallEnter.Execute(Index);
	}
}

void AGates::OnGateBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(AReplicatedBall::StaticClass()))
	{
		Multicast_HandleBallEnterGate(GateIndex);
	}
}
