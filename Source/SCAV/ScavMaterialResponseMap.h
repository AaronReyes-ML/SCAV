// Copyright 2016 Mookie. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "ScavMaterialResponseMap.generated.h"

UENUM(BlueprintType)
enum class EScavPenTraceType : uint8
{
	PT_BackTrace1 UMETA(DisplayName = "Back Trace1"),
	PT_ByComponent1 UMETA(DisplayName = "By Component1"),
	PT_TwoSidedGeometry1 UMETA(DisplayName = "Double Sided Geometry1"),
};

USTRUCT(BlueprintType)
struct FScavMaterialResponseMapEntry {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Material") EScavPenTraceType PenTraceType = EScavPenTraceType::PT_BackTrace1;
	UPROPERTY(EditAnywhere, Category = "Material") bool NeverPenetrate;
	UPROPERTY(EditAnywhere, Category = "Material") float PenetrationDepthMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Material") float PenetrationNormalization = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Material") float PenetrationNormalizationGrazing = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Material") float PenetrationEntryAngleSpread = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Material") float PenetrationExitAngleSpread = 0.0;
	UPROPERTY(EditAnywhere, Category = "Material") bool NeverRicochet;
	UPROPERTY(EditAnywhere, Category = "Material") float RicochetProbabilityMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Material") float RicochetRestitution = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Material") float RicochetRestitutionInfluence = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Material") float RicochetFriction = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Material") float RicochetFrictionInfluence = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Material") float RicochetSpread = 0.0f;
};

UCLASS(BlueprintType)
class UScavMaterialResponseMap : public UDataAsset{
	GENERATED_BODY()

};