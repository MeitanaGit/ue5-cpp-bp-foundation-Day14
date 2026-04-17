// Fill out your copyright notice in the Description page of Project Settings.


#include "MyOrbManager.h"

// Sets default values
AMyOrbManager::AMyOrbManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyOrbManager::BeginPlay()
{
	Super::BeginPlay();
	// 時分割生成開始
	GetWorldTimerManager().SetTimer(IncrementallySpawnTimerHandle, this, &AMyOrbManager::IncrementallySpawnOrb, 0.02f, true);	
}

// Called every frame
void AMyOrbManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyOrbManager::IncrementallySpawnOrb()
{
	if (CurrentSpawningCount >= PoolSize)
	{
		GetWorldTimerManager().ClearTimer(IncrementallySpawnTimerHandle);
		return;
	}
	
	FActorSpawnParameters SpawnParams;

	// スポーン時に「テンプレート」から値を引き継ぐ設定や、初期化を確実にする
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AMyExpOrb* NewOrb = GetWorld()->SpawnActor<AMyExpOrb>(OrbClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (NewOrb)
	{
		NewOrb->DeactivateOrb();
		// 念のため、初期位置をプレイヤーが絶対に行かない場所（はるか遠くの下方など）へ飛ばす
		NewOrb->SetActorLocation(FVector(0.f, 0.f, -50000.f));
		
		OrbPool.Add(NewOrb);
		CurrentSpawningCount++;
	}
}

void AMyOrbManager::SpawnOrbFromPool(FVector Location)
{
	for (AMyExpOrb* Orb : OrbPool)
	{
		if (Orb && Orb->IsHidden()) // 非表示＝プールに眠っている
		{
			Orb->ActivateOrb(Location);
			return;
		}
	}
	// 足りない場合の動的生成（予備）
	AMyExpOrb* NewOrb = GetWorld()->SpawnActor<AMyExpOrb>(OrbClass, Location, FRotator::ZeroRotator);
	if(NewOrb) OrbPool.Add(NewOrb);
}
