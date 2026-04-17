// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletManagerComponent.h"
#include "MyBullet.h"

// Sets default values for this component's properties
UBulletManagerComponent::UBulletManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBulletManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	// プールの初期化（器だけ用意する）
	for (TSubclassOf<AMyBullet> BulletClass : BulletClasses)
	{
		if (BulletClass)
		{
			MultiBulletPool.Add(BulletClass, FBulletPoolWrapper());
		}
	}

	// 時分割生成を開始（0.01秒間隔で実行。負荷に応じて調整してください）
	if (BulletClasses.Num() > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &UBulletManagerComponent::IncrementallySpawnBullet, 0.01f, true);
	}
	
}


// Called every frame
void UBulletManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
void UBulletManagerComponent::IncrementallySpawnBullet()
{
	// 全てのクラスの生成が終わったらタイマーを止める
	if (!BulletClasses.IsValidIndex(CurrentSpawningClassIndex))
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("All Bullets Pool Initialized.,Class Index = %d"),CurrentSpawningClassIndex);
		UE_LOG(LogTemp, Log, TEXT("Bullet Pool Size = %d"),BulletPoolSize);
		return;
	}

	TSubclassOf<AMyBullet> TargetClass = BulletClasses[CurrentSpawningClassIndex];
    
	if (TargetClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();

		AMyBullet* NewBullet = GetWorld()->SpawnActor<AMyBullet>(TargetClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (NewBullet)
		{
			NewBullet->DeactivateBullet();
			// Mapから該当するWrapperを取得して追加
			MultiBulletPool[TargetClass].Bullets.Add(NewBullet);
		}
	}

	CurrentSpawningCount++;

	// 指定したプールサイズに達したら、次の種類の弾へ
	if (CurrentSpawningCount >= BulletPoolSize)
	{
		CurrentSpawningCount = 0;
		CurrentSpawningClassIndex++;
	}
}

AMyBullet* UBulletManagerComponent::GetBulletFromPool(int32 BulletIndex)
{
	if (!BulletClasses.IsValidIndex(BulletIndex)) return nullptr;

	TSubclassOf<AMyBullet> TargetClass = BulletClasses[BulletIndex];
    
	// 修正ポイント：Findの結果は FBulletPoolWrapper* 型になる
	FBulletPoolWrapper* TargetPoolWrapper = MultiBulletPool.Find(TargetClass);
    
	if (TargetPoolWrapper)
	{
		// 1. まずは既存のプールから非アクティブなものを探す
		for (AMyBullet* Bullet : TargetPoolWrapper->Bullets)
		{
			if (Bullet && !Bullet->GetIsActive())
			{
				return Bullet;
			}
		}

		// 2. もし見つからなかった場合（生成が追いついていない or 撃ちすぎ）
		// その場で1つ新しく生成してプールを拡張する
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		AMyBullet* NewBullet = GetWorld()->SpawnActor<AMyBullet>(TargetClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        
		if (NewBullet)
		{
			NewBullet->DeactivateBullet();
			TargetPoolWrapper->Bullets.Add(NewBullet); // プールに追加しておく
			UE_LOG(LogTemp, Warning, TEXT("Pool Empty! Dynamically expanded: %s"), *TargetClass->GetName());
			return NewBullet;
		}
	}
	
	return nullptr;
}
// キャラクターから呼ばれる関数
AMyBullet* UBulletManagerComponent::RequestBullet(int32 BulletIndex, FVector Location, FRotator Rotation, float Damage)
{
	AMyBullet* Bullet = GetBulletFromPool(BulletIndex);
	if (Bullet)
	{
		Bullet->DamageValue = Damage;
		Bullet->ActivateBullet(Location, Rotation);
		return Bullet;
	}
	return nullptr;
}

