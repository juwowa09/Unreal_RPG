// WonUngJu All Rights Reserved


#include "Characters/WarriorEnemyCharacter.h"

#include "Component/Combat/EnemyCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AWarriorEnemyCharacter::AWarriorEnemyCharacter()
{
	// AI Controller가 이걸 소유함
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 오른쪽 스틱으로 인한 회전 이루어지지 않도록함
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	GetCharacterMovement()->bUseControllerDesiredRotation = false; // 마우스 돌아도 캐릭터 회전 X
	GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터 이동시 해당 방향으로 회전 O
	GetCharacterMovement()->RotationRate = FRotator(0.f,180.f,0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;

	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>("EnemyCombatComponent");
}
