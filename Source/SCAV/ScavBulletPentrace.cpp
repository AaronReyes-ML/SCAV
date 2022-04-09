// Copyright 2020 Mookie. All Rights Reserved.

#include "ScavBullet.h"

float AScavBullet::PenetrationTrace(FVector StartLocation, FVector EndLocation, TWeakObjectPtr<UPrimitiveComponent, FWeakObjectPtr> Component, EScavPenTraceType PenTraceType, TEnumAsByte<ECollisionChannel> CollisionChannel, FVector &ExitLocation, FVector &ExitNormal) {
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = TraceComplex;
	QueryParams.bFindInitialOverlaps = true;

	FHitResult Result;

switch (PenTraceType) {
	case(EScavPenTraceType::PT_BackTrace1): {
		bool Hit = GetWorld()->LineTraceSingleByChannel(Result, EndLocation, StartLocation, CollisionChannel, QueryParams);
		if (!Hit) return 1.0f;
		ExitNormal = Result.Normal;
		ExitLocation = Result.Location;
		return (1.0f - Result.Time);
	}

	case(EScavPenTraceType::PT_ByComponent1): {
		bool Hit = Component->LineTraceComponent(Result, EndLocation, StartLocation, QueryParams);
		if (!Hit) return 1.0f;
		ExitNormal = Result.Normal;
		ExitLocation = Result.Location;
		return (1.0f - Result.Time);
	}

	case(EScavPenTraceType::PT_TwoSidedGeometry1): {
		bool Hit = GetWorld()->LineTraceSingleByChannel(Result, StartLocation, EndLocation, CollisionChannel, QueryParams);
		if (!Hit) return 1.0f;
		ExitLocation = Result.Location;
		ExitNormal = -Result.Normal;
		return Result.Time;
	}

	default:
		return 1.0f;
	}
}