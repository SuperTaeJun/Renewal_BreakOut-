#include "Character/CharacterBase.h"
#pragma region INPUT
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#pragma endregion
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Character/PlayerCombatComp.h"
#include "Weapon/WeaponBase.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#define DEFAULTCAMERALENGTH 200
#define SPRINTCAMERALENGTH 100

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	TurningType = ETurningInPlace::ETIP_NotTurning;
	SetupCharacter();
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	InputMpaaing();
	FName RifleSocketName = FName("RifleSocket");
	CombatComp->SetWeapon(AWeaponBase::StaticClass(), RifleSocketName);
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);
}

void ACharacterBase::ActivateSkill()
{
}
void ACharacterBase::SetupCharacter()
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(75.f, true);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshFinder(TEXT("/Game/BreakoutAsset/Character/Character1/SKM_Character1.SKM_Character1"));
	GetMesh()->SetSkeletalMeshAsset(SkeletalMeshFinder.Object);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 15.f), FRotator(0.f, -90.f, 0.f));

	Movement = GetCharacterMovement();
	Movement->MaxWalkSpeed = 400.f;
	Movement->bOrientRotationToMovement = true;
	Movement->JumpZVelocity = 480.f;
	Movement->AirControl = 0.2f;
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = DEFAULTCAMERALENGTH;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->SetWorldLocation(FVector(-60.f, 0.f, 60.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	CombatComp = CreateDefaultSubobject<UPlayerCombatComp>(TEXT("CombatComp"));
}

void ACharacterBase::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 30.f)
	{
		TurningType = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -30.f)
	{
		TurningType = ETurningInPlace::ETIP_Left;
	}
	if (TurningType != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningType = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ACharacterBase::AimOffset(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool IsFalling = GetCharacterMovement()->IsFalling();
	//UE_LOG(LogTemp, Log, TEXT("%f"), AO_Yaw);


	if (!IsFalling && Speed == 0.f) //점프 아니고 서있을때
	{
		//UE_LOG(LogTemp, Log, TEXT("%f"), Speed);
		FRotator CurAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurAimRotation, StartingAimRotation);
		AO_Yaw = DeltaRotation.Yaw;
		if (TurningType == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (IsFalling || Speed > 0.f) // 뛰거나 점프일때
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningType = ETurningInPlace::ETIP_NotTurning;
	}
	AO_Pitch = GetBaseAimRotation().Pitch + 20.f;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

#pragma region INPUT

void ACharacterBase::InputMpaaing()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		//입력시스템 매핑
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefalutMappingContext, 0);
		}
	}
}
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacterBase::Move);// trigger 매 틱마다
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACharacterBase::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacterBase::StopJump);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ACharacterBase::Sprint_S);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACharacterBase::Sprint_E);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ACharacterBase::Fire_S);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ACharacterBase::Fire_E);
	}
}
void ACharacterBase::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotaion(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDir = FRotationMatrix(YawRotaion).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDir, MovementVector.Y);
	const FVector RightDir = FRotationMatrix(YawRotaion).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDir, MovementVector.X);
}
void ACharacterBase::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerYawInput(LookAxis.X);
		AddControllerPitchInput(LookAxis.Y);
	}
}
void ACharacterBase::Sprint_S(const FInputActionValue& Value)
{

}
void ACharacterBase::Sprint_E(const FInputActionValue& Value)
{
}
void ACharacterBase::Fire_S(const FInputActionValue& Value)
{
	CombatComp->Fire();
}
void ACharacterBase::Fire_E(const FInputActionValue& Value)
{
}
#pragma endregion


