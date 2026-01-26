// WonUngJu All Rights Reserved


#include "Items/WarriorProjectileBase.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGamePlayTags.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "WarriorDebugHelper.h"

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

	// Box Delegate 등록
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);

	// 초기화
	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());

	// 초기화
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 700.f;
	ProjectileMovementComp->MaxSpeed = 900.f;
	ProjectileMovementComp->Velocity = FVector(1.f,0.f,0.f);
	ProjectileMovementComp->ProjectileGravityScale = 0.f;

	// 4초동안 유지
	InitialLifeSpan = 4.f;
}

// Called when the game starts or when spawned
void AWarriorProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileDamagePolicy == EProjectileDamagePolicy::OnBeginOverlap)
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	}
}

// OnHit 델리게이트에 등록할 함수
void AWarriorProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);
	
	APawn* HitPawn = Cast<APawn>(OtherActor);

	// Hostile 인지 체크
	if (!HitPawn || !UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(),HitPawn))
	{
		Destroy();
		return;
	}

	bool bIsValidBlock = false;
	const bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitPawn, WarriorGamePlayTags::Player_Status_Blocking);

	if (bIsPlayerBlocking)
	{
		// 유효한 블럭방향인지 확인
		bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(this, HitPawn);
	}

	FGameplayEventData Data;
	Data.Instigator = this;
	Data.Target = HitPawn;

	if (bIsValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitPawn,
			WarriorGamePlayTags::Player_Event_SuccessfulBlock,
			Data
			);
	}
	else
	{
		// Apply Projectile Damage
		HandleApplyProjectileDamage(HitPawn, Data);
	}

	Destroy();
}

// Overlap 델리게이트에 등록할 함수
void AWarriorProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedActors.Contains(OtherActor))
	{
		return;
	}

	OverlappedActors.AddUnique(OtherActor);

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		FGameplayEventData Data;
		Data.Instigator = GetInstigator();
		Data.Target = HitPawn;
		
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(),HitPawn))
		{
			HandleApplyProjectileDamage(HitPawn,Data);
			BP_OnSpawnProjectileHitFX(SweepResult.ImpactPoint);
			Destroy();
		}
	}
}

void AWarriorProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
	checkf(ProjectileDamageEffectSpecHandle.IsValid(),TEXT("Forgot to assign a valid spec handle to the projectile: %s"),*GetActorNameOrLabel());

	// Instigator -> Target으로 ASC Effect 적용
	const bool bWasApplied = UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(),InHitPawn,ProjectileDamageEffectSpecHandle);

	if (bWasApplied)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			InHitPawn,
			WarriorGamePlayTags::Shared_Event_HitReact,
			InPayload
			);
	}
}



