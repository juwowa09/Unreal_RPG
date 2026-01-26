// WonUngJu All Rights Reserved


#include "Items/PickUps/WarriorPickUpBase.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

#include "Widgets/WarriorWidgetBase.h"

// Sets default values
AWarriorPickUpBase::AWarriorPickUpBase()
{
	PrimaryActorTick.bCanEverTick = false;

	PickUpCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpCollisionSphere"));
	SetRootComponent(PickUpCollisionSphere);
	PickUpCollisionSphere->InitSphereRadius(50.f);
	
	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(GetRootComponent());
}

void AWarriorPickUpBase::BeginPlay()
{
	Super::BeginPlay();
	PickUpWidget->SetVisibility(false);
	// Collision 에 오버랩 될 때 호출할 함수 등록
	PickUpCollisionSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnPickUpCollisionSphereBeginOverlap);
	PickUpCollisionSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnPickUpCollisionSphereEndOverlap);
}

void AWarriorPickUpBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                             AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                             const FHitResult& SweepResult)
{
}

void AWarriorPickUpBase::OnPickUpCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

