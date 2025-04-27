#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WeaponData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SphereRadius = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseScatter = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAmmo = 10;
};
