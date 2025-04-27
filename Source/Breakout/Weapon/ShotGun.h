#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "ShotGun.generated.h"

UCLASS()
class BREAKOUT_API AShotGun : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	AShotGun();
protected:
	virtual void FireInternal(const FVector& HitTarget) override;
	virtual void SendFirePacket() override;
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	uint32 NumberOfPellets = 6; //한번에 발사량
};
