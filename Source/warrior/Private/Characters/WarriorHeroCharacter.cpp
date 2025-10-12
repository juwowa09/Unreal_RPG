// WonUngJu All Rights Reserved


#include "Characters/WarriorHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "Component/Input/WarriorInputComponent.h"
#include "WarriorGamePlayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "Component/Combat/HeroCombatComponent.h"

#include "WarriorDebugHelper.h"

AWarriorHeroCharacter::AWarriorHeroCharacter()
{
	//기본 충돌 캡슐
	GetCapsuleComponent()->InitCapsuleSize(42.f,96.f);

	//현재 컨트롤러로 캐릭터 회전 불가능
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// 카메라 봉 생성자
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());	// 루트컴포넌트에 자식으로 붙이기
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->SocketOffset = FVector(0.f,55.f,65.f);	// 자식의 offset 지정(카메라 오프셋)
	CameraBoom->bUsePawnControlRotation = true;		// 컨트롤러에 의해 회전이 되는지 설정

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;	// 이미 카메라봉이 회전하고 있으므로 회전 X

	GetCharacterMovement()->bOrientRotationToMovement = true;	// 캐릭터 wasd 이동시 회전여부
	GetCharacterMovement()->RotationRate = FRotator(0.f,500.f,0.f);	// 캐릭터 회전속도
	GetCharacterMovement()->MaxWalkSpeed = 400.f;	// 최대 걷기 속도
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;	// 이동 정지시 감속

	// CombatComponent 는 확장 컴포넌트 상속 (필요한 기능들 추가)
	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
}

//캐릭터가 컨트롤러에 연결될 때 호출되는 함수
void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// if (WarriorAbilitySystemComponent && WarriorAttributeSet)
	// {
	// 	const FString ASCText = FString::Printf(TEXT("Owner Actor: %s, Avatar Actor: %s"), *WarriorAbilitySystemComponent->GetOwnerActor()->GetActorLabel(), *WarriorAbilitySystemComponent->GetAvatarActor()->GetActorLabel());
	// 	Debug::Print(TEXT("Ability System Component valid.") + ASCText, FColor::Green);
	// 	Debug::Print(TEXT("Attribute System valid.") + ASCText,FColor::Green);
	// }
	if (!CharacterStartUpData.IsNull())
	{
		UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous();
		if (LoadedData)
		{
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
		}
	}
}

//컨트롤러 연결 후 인풋 바인딩에 호출되는 함수
void AWarriorHeroCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset,TEXT("Forgot to asign a vaild data asset as input config"))
	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	
	check(SubSystem);
	SubSystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
	UWarriorInputComponent* WarriorInputComponent = CastChecked<UWarriorInputComponent>(PlayerInputComponent);

	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,
		WarriorGamePlayTags::InputTag_Move,
		ETriggerEvent::Triggered,
		this,
		&ThisClass::Input_Move);
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,
	WarriorGamePlayTags::InputTag_Look,
	ETriggerEvent::Triggered,
	this,
	&ThisClass::Input_Look);
}

void AWarriorHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Debug::Print(TEXT("Working"));
}

void AWarriorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw,0.f);
	
	if (MovementVector.Y != 0.f)
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardDirection, MovementVector.Y);
	}
	
	if (MovementVector.X != 0.f)
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(RightDirection, MovementVector.X);
	}
	
}

void AWarriorHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw,0.f);
	
	if (LookAxisVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}
	
	if (LookAxisVector.Y != 0.f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

