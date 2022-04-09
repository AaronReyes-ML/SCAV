#include "ScavBullet.h"

void AScavBullet::Deactivate() {
	//server only
	if (!HasAuthority()) { return; }
	OnDeactivated();
	this->DeactivateToPool();
	DeactivationBroadcast();
}

AScavBullet* AScavBullet::GetFromPool(UWorld* World, UClass* BulletClass) {
	AScavBullet* Pool = Cast<AScavBullet>(BulletClass->GetDefaultObject());

	if (Pool) {
		//find first of correct class;
		bool CleanupRequired=false;

		int32 FoundIndex = Pool->Pooled.IndexOfByPredicate(
			[&](auto InItem) {
			if (InItem.IsValid() && InItem->GetWorld() == World) {
				return true;
			}
			else {
				CleanupRequired = true;
				return false;
			}
		});

		TWeakObjectPtr<AScavBullet> Found = nullptr;
		if (FoundIndex != INDEX_NONE) {
			Found = Pool->Pooled[FoundIndex];
			Pool->Pooled.RemoveAtSwap(FoundIndex);
		}

		if (CleanupRequired) {
#ifdef WITH_EDITOR
		if (Pool->DebugPooling) {
			GEngine->AddOnScreenDebugMessage(2, 2, FColor::White, TEXT("Invalid reference in pool, cleaning up"));
		}
#endif
			Pool->Pooled.RemoveAll([&](auto InItem) {
				if (InItem.IsValid() && InItem->GetWorld() == World) {
					return false;
				}
				else {
					return true;
				}
			});
		}

		return(Found.Get());
	}
	else {
		return nullptr;
	}
}

AScavBullet* AScavBullet::SpawnOrReactivate(UWorld* World, TSubclassOf<class AScavBullet> BulletClass, const FTransform& Transform, FVector BulletVelocity, AActor* BulletOwner, APawn* BulletInstigator) {
	AScavBullet* bullet;

	AScavBullet* Recycled = GetFromPool(World, BulletClass); 

	if (Recycled) {
		AScavBullet* Default = Cast<AScavBullet>(BulletClass->GetDefaultObject());

		Recycled->Reset();

		Recycled->SetOwner(BulletOwner);
		Recycled->SetInstigator(BulletInstigator);
		Recycled->SetActorTransform(Transform,false,nullptr,ETeleportType::TeleportPhysics);
		Recycled->Velocity = BulletVelocity;
		Recycled->SetActorHiddenInGame(Default->IsHidden());
		Recycled->SetActorTickEnabled(true);
		Recycled->CanRetrace = false;
		Recycled->IgnoredActors = Default->IgnoredActors;
		Recycled->SafeDelay = Default->SafeDelay;
		Recycled->SetLifeSpan(Default->InitialLifeSpan);
		if (!Recycled->HasActorBegunPlay()){ Recycled->BeginPlay(); }
		Recycled->ReactivationBroadcast(UGameplayStatics::RebaseLocalOriginOntoZero(Recycled->GetWorld(), Transform.GetLocation()), BulletVelocity, BulletOwner, BulletInstigator);
#ifdef WITH_EDITOR
		if (Recycled->DebugPooling) {
			GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Recycling pooled bullet"));
		}
#endif
		return Recycled;
	}
	else {
		bullet = Cast<AScavBullet>(World->SpawnActorDeferred<AScavBullet>(BulletClass, Transform, BulletOwner, BulletInstigator));
		bullet->RandomStream.GenerateNewSeed();
		bullet->Velocity = BulletVelocity;
		UGameplayStatics::FinishSpawningActor(bullet, Transform);
#ifdef WITH_EDITOR
		if (bullet->DebugPooling) {
			GEngine->AddOnScreenDebugMessage(0, 2, FColor::Orange, TEXT("Spawning new bullet"));
		}
#endif
		return bullet;
	}
}

void AScavBullet::ReactivationBroadcast_Implementation(FVector_NetQuantize NewLocation, FVector NewVelocity, AActor* BulletOwner, APawn* BulletInstigator) {
	if (!HasAuthority()) {
		AScavBullet* Default = Cast<AScavBullet>(this->StaticClass()->GetDefaultObject());

		SetOwner(BulletOwner);
		SetInstigator(BulletInstigator);

		SetActorLocation(UGameplayStatics::RebaseZeroOriginOntoLocal(GetWorld(), NewLocation));
		Velocity = NewVelocity;
		CanRetrace = false;

		SetActorHiddenInGame(Default->IsHidden());
		SetActorTickEnabled(Default->IsActorTickEnabled());
		SafeDelay = Default->SafeDelay;
		OwnerSafe = Default->SafeLaunch;
		BeginPlay();
	}
}

void AScavBullet::DeactivationBroadcast_Implementation() {
	if (!HasAuthority()) {
		OnDeactivated();
		this->DeactivateToPool();
	}
}

void AScavBullet::LifeSpanExpired() {
	Deactivate();
}

void AScavBullet::DeactivateToPool() {
	AScavBullet* Pool = Cast<AScavBullet>(GetClass()->GetDefaultObject());

	if (Pool && EnablePooling) {
		SetActorHiddenInGame(true);
		SetActorTickEnabled(false);
		Pool->Pooled.Add(this);
		EndPlay(EEndPlayReason::RemovedFromWorld);

		if (Pool->Pooled.Num() > MaxPoolSize) {
			AScavBullet* Oldest = (Pool->Pooled[0].Get());
			Pool->Pooled.RemoveAtSwap(0);
			if (Oldest) { Oldest->Destroy(); }
		}

#ifdef WITH_EDITOR
		if (DebugPooling) {
			GEngine->AddOnScreenDebugMessage(2, 2, FColor::White, FString("Bullet pooled: ") + FString::FromInt(Pool->Pooled.Num()));
		}
#endif
	}
	else {
		Destroy();
	}
}