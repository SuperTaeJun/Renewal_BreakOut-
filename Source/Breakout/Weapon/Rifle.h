#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "Rifle.generated.h"

UCLASS()
class BREAKOUT_API ARifle : public AWeaponBase
{
	GENERATED_BODY()

protected:
	virtual void FireInternal(const FVector& HitTarget) override;
	virtual void SendFirePacket() override;
};
