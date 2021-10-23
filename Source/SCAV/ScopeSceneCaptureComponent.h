// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "ScopeSceneCaptureComponent.generated.h"

/**
 * 
 */
UCLASS(hidecategories = (Collision, Object, Physics, SceneComponent), ClassGroup = Rendering, editinlinenew, meta = (BlueprintSpawnableComponent))
class SCAV_API UScopeSceneCaptureComponent : public USceneCaptureComponent2D
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
	
};
