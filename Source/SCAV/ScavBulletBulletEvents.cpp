// Copyright 2016 Mookie. All Rights Reserved.

#include "ScavBullet.h"
#include "ScavMaterialResponseMap.h"
#include "Net/UnrealNetwork.h"

void AScavBullet::VelocityChangeBroadcast_Implementation(FVector_NetQuantize NewLocation, FVector NewVelocity) {
	if (!HasAuthority()) {
		FVector RebasedLocation = UGameplayStatics::RebaseZeroOriginOntoLocal(GetWorld(), NewLocation);
		OnTrajectoryUpdateReceived(RebasedLocation, Velocity, NewVelocity);
		SetActorLocation(RebasedLocation);
		Velocity = NewVelocity;
		CanRetrace = false;
	}
}

void AScavBullet::VelocityChangeBroadcastReliable_Implementation(FVector_NetQuantize NewLocation, FVector NewVelocity) {
	if (!HasAuthority()) {
		FVector RebasedLocation = UGameplayStatics::RebaseZeroOriginOntoLocal(GetWorld(), NewLocation);
		OnTrajectoryUpdateReceived(RebasedLocation, Velocity, NewVelocity);
		SetActorLocation(RebasedLocation);
		Velocity = NewVelocity;
		CanRetrace = false;
	}
}

void AScavBullet::SpawnWithExactVelocity(TSubclassOf<class AScavBullet> BulletClass, AActor* BulletOwner, APawn* BulletInstigator, FVector BulletLocation, FVector BulletVelocity) {

	if (BulletClass != nullptr && BulletOwner != nullptr) {
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = BulletOwner;
		spawnParams.Instigator = BulletInstigator;

		AScavBullet* Default = Cast<AScavBullet>(BulletClass->GetDefaultObject());

		FTransform Transform;
		Transform.SetLocation(BulletLocation);
		Transform.SetScale3D(Default->GetActorScale());

		if (Default->RotateActor) {
			FRotator Rotation = UKismetMathLibrary::MakeRotFromX(BulletVelocity);
			if (Default->RotateRandomRoll) Rotation.Add(0, 0, Default->RandomStream.FRandRange(-180.0f, 180.0f));
			Transform.SetRotation(Rotation.Quaternion());
		}
		else {
			Transform.SetRotation(FQuat(1, 0, 0, 1));
		}

		if (!Default->Shotgun) {
			AScavBullet* bullet = SpawnOrReactivate(BulletOwner->GetWorld(), BulletClass, Transform, BulletVelocity, BulletOwner, BulletInstigator);
		}
		else {
			for (int i = 0; i < Default->ShotCount; i++) {
				float Vel = BulletVelocity.Size()*Default->RandomStream.FRandRange(1.0 - Default->ShotVelocitySpread, 1.0 + Default->ShotVelocitySpread);
				FVector SubmunitionVelocity = Default->RandomStream.VRandCone(BulletVelocity, Default->ShotSpread)*Vel;
				AScavBullet* bullet = SpawnOrReactivate(BulletOwner->GetWorld(), BulletClass, Transform, SubmunitionVelocity, BulletOwner, BulletInstigator);
			}
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Cannot spawn bullet - invalid class or owner"));
	}
}


void AScavBullet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AScavBullet, Velocity, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AScavBullet, RandomStream, COND_InitialOnly);
}

//alternative spawn
void AScavBullet::Spawn(TSubclassOf<class AScavBullet> BulletClass, AActor* BulletOwner, APawn* BulletInstigator, FVector BulletLocation, FVector BulletVelocity) {
	if (BulletClass != nullptr && BulletOwner != nullptr) {

		FActorSpawnParameters spawnParams;
		spawnParams.Owner = BulletOwner;
		spawnParams.Instigator = BulletInstigator;

		AScavBullet* Default = Cast<AScavBullet>(BulletClass->GetDefaultObject());

		FTransform Transform;
		Transform.SetLocation(BulletLocation);
		Transform.SetScale3D(Default->GetActorScale());

		if (Default->RotateActor) {
			if (Default->RotateActor) {
				FRotator Rotation = UKismetMathLibrary::MakeRotFromX(BulletVelocity);
				if (Default->RotateRandomRoll) Rotation.Add(0, 0, Default->RandomStream.FRandRange(-180.0f, 180.0f));
				Transform.SetRotation(Rotation.Quaternion());
			}
		}
		else {
			Transform.SetRotation(FQuat(1, 0, 0, 1));
		}

		//init velocity
		FVector BulletVelocityNew = Default->RandomStream.VRandCone(BulletVelocity, Default->Spread)*BulletVelocity.Size();
		float VelocityMP = FMath::Lerp(Default->MuzzleVelocityMin, Default->MuzzleVelocityMax, Default->RandomStream.FRand());
		BulletVelocityNew = BulletVelocityNew * VelocityMP;

		if (!Default->Shotgun) {
			AScavBullet* bullet = SpawnOrReactivate(BulletOwner->GetWorld(), BulletClass, Transform, BulletVelocityNew, BulletOwner, BulletInstigator);
		}
		else {
			for (int i = 0; i < Default->ShotCount; i++) {
				float Vel = BulletVelocityNew.Size()*Default->RandomStream.FRandRange(1.0 - Default->ShotVelocitySpread, 1.0 + Default->ShotVelocitySpread);
				FVector SubmunitionVelocity = Default->RandomStream.VRandCone(BulletVelocityNew, Default->ShotSpread)*Vel;
				AScavBullet* bullet = SpawnOrReactivate(BulletOwner->GetWorld(), BulletClass, Transform, SubmunitionVelocity, BulletOwner, BulletInstigator);
			}
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Cannot spawn bullet - invalid class or owner"));
	}
}

void AScavBullet::OnImpact_Implementation(bool Ricochet, bool PassedThrough, FVector Location, FVector IncomingVelocity, FVector Normal, FVector ExitLocation, FVector ExitVelocity, FVector Impulse, float PenetrationDepth, AActor* Actor, USceneComponent* Component, FName BoneName, FHitResult HitResult){
	return;
};


void AScavBullet::OnDeactivated_Implementation() {
	return;
};