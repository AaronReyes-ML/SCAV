// Copyright 2016 Mookie. All Rights Reserved.

#include "ScavBullet.h"

FVector AScavBullet::GetWind_Implementation(UWorld* World, FVector Location) const{
	return Wind;
}

float AScavBullet::GetAirDensity_Implementation(UWorld* World, FVector Location) const{
	switch (AtmosphereType) {
		case (EScavAtmosphereType::AT_Curve1): {
			float airmp = SeaLevelAirDensity / GetCurveValue(AirDensityCurve, 0, SeaLevelAirDensity);
			return GetCurveValue(AirDensityCurve, GetAltitude(World, Location) / WorldScale, SeaLevelAirDensity)* airmp;
		}
		case (EScavAtmosphereType::AT_Earth1): {
			return GetAltitudeDensity(GetAltitude(World, Location) / WorldScale / 100.0f);
		}
		default:{
			return SeaLevelAirDensity;
		}
	}
}

float AScavBullet::GetSpeedOfSound_Implementation(UWorld* World, FVector Location) const{
	if (!SpeedOfSoundVariesWithAltitude) {
		return SeaLevelSpeedOfSound * WorldScale;
	}
		
	float Altitude = GetAltitude(World, Location);
	float soundvmp = SeaLevelSpeedOfSound / GetCurveValue(SpeedOfSoundCurve, 0, SeaLevelSpeedOfSound);
	return GetCurveValue(SpeedOfSoundCurve, Altitude, SeaLevelSpeedOfSound)*WorldScale*soundvmp;
}


float AScavBullet::GetAltitudePressure(float AltitudeMeter) const {
	return FMath::Max(SeaLevelAirPressure * FMath::Pow((1 - (0.0000225577 * AltitudeMeter)), 5.25588), 0.0f);
}

float AScavBullet::GetAltitudeTemperature(float AltitudeMeter) const {
	return SeaLevelAirTemperature - (TemperatureLapseRate * FMath::Min(AltitudeMeter, TropopauseAltitude));
}

float AScavBullet::GetAltitudeDensity(float AltitudeMeter) const {
	float Temperature = GetAltitudeTemperature(AltitudeMeter);
	float Pressure = GetAltitudePressure(AltitudeMeter);
	return Pressure * 100.0f / ((Temperature + 273.15) * SpecificGasConstant);
}

float AScavBullet::GetAltitude(UWorld* World, FVector Location) const{
	FVector DistanceFromOrigin = (Location - WorldCenterLocation + FVector(World->OriginLocation));
	if (SphericalAltitude)
	{
		return (DistanceFromOrigin.Size() - SeaLevelRadius);
	}
	else {
		return DistanceFromOrigin.Z;
	}
}