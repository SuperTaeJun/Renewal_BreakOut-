#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SkillInterface.h"
#include "InputActionValue.h"
#include "CharacterBase.generated.h"

class UInputAction;

//애니메이션에서 쓸거
UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	ETIP_Left UMETA(DisplayName = "Turning Left"),
	ETIP_Right UMETA(DisplayName = "Turning Right"),
	ETIP_NotTurning UMETA(DisplayName = "Not Turning"),

	ETIP_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class RENEWAL_BREAKOUT_API ACharacterBase : public ACharacter, public ISkillInterface
{
	GENERATED_BODY()

private:
#pragma region Component
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class UCameraComponent> FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCharacterMovementComponent> Movement;
	UPROPERTY(VisibleAnywhere, Category = Character)
	TObjectPtr<class UPlayerCombatComp> CombatComp;
#pragma endregion

	float JogSpeed =400.f;
	float SprintSpeed = 500.f;
public:
	ACharacterBase();

#pragma region SetFunction

#pragma endregion
#pragma region GetFunction
	FORCEINLINE class UPlayerCombatComp* GetCombatComp() { return CombatComp; }
	FORCEINLINE ETurningInPlace GetTurningType() { return TurningType; }

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
#pragma endregion


private:
	ETurningInPlace TurningType;
	//에임오프셋
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;


	void SetupCharacter();
	void TurnInPlace(float DeltaTime);
	void AimOffset(float DeltaTime);
protected:
	virtual void BeginPlay() override;
	virtual void ActivateSkill() override;
	virtual void Tick(float DeltaTime) override;


#pragma region INPUT
private:
	void InputMpaaing();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class UInputMappingContext> DefalutMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> SprintAction;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> FireAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Sprint_S(const FInputActionValue& Value);
	void Sprint_E(const FInputActionValue& Value);
	void Fire_S(const FInputActionValue& Value);
	void Fire_E(const FInputActionValue& Value);


#pragma endregion

};
