// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/PlayerAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/CharacterBase.h"
#include "Character/PlayerCombatComp.h"
#include "Weapon/WeaponBase.h"
void UPlayerAnimInstance::NativeInitializeAnimation()
{
	BaseCharacter = Cast<ACharacterBase>(GetOwningActor());
	if (BaseCharacter)
	{
		Movement = BaseCharacter->GetCharacterMovement();

		bUseLeftHand = true;
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (Movement)
	{
		FVector Velocity = Movement->Velocity;
		Speed = Velocity.Size2D();

		bIsFalling = Movement->IsFalling();
		bIsAccelerating = BaseCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
		TurningType = BaseCharacter->GetTurningType();
		CurEquipWeapon = BaseCharacter->GetCombatComp()->GetCurWeapon();

		//Yaw오프셋 값
		FRotator AimRotation = BaseCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BaseCharacter->GetVelocity());
		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
		DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 6.f);
		YawOffset = DeltaRotation.Yaw;

		//Lean오프셋 값
		CharacterRotationLastFrame = CharacterRotation;
		CharacterRotation = BaseCharacter->GetActorRotation();
		const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
		const float Target = Delta.Yaw / DeltaSeconds;
		const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f);
		Lean = FMath::Clamp(Interp, -90.f, 90.f);

		AO_Yaw = BaseCharacter->GetAO_Yaw();
		AO_Pitch = BaseCharacter->GetAO_Pitch();


		if (CurEquipWeapon)
		{
			LeftHandTransform = CurEquipWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
			FVector OutPosition;
			FRotator OutRotation;
			BaseCharacter->GetMesh()->TransformToBoneSpace(FName("Bip001-R-Hand"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
			LeftHandTransform.SetLocation(OutPosition);
			LeftHandTransform.SetRotation(FQuat(OutRotation));

			FTransform RightHandTransform = CurEquipWeapon->GetWeaponMesh()->GetSocketTransform(FName("Bip001-R-Hand"), ERelativeTransformSpace::RTS_World);
			RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), BaseCharacter->GetCombatComp()->GetHitTarget());

			FTransform MuzzleTransform = CurEquipWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
			FVector MuzzleX(FRotationMatrix(MuzzleTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));

		}
	}
}
