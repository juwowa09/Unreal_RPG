// WonUngJu All Rights Reserved


#include "Items/WarriorProjectileBase.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

AWarriorProjectileBase::AWarriorProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 대부분 루트 컴포넌트는 콜리젼
	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	// 물리 충돌 X, Overlap, Trace 가능 == QueryOnly
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn,ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);

	// 초기화
	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());

	// 초기화
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 700.f;
	ProjectileMovementComp->MaxSpeed == 900.f;
	ProjectileMovementComp->Velocity = FVector(1.f,0.f,0.f);
	ProjectileMovementComp->ProjectileGravityScale = 0.f;

	// 4초동안 유지
	InitialLifeSpan = 4.f;
}

// Called when the game starts or when spawned
void AWarriorProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

