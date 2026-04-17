// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPopUpManager.h"

// Sets default values
AMyPopUpManager::AMyPopUpManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PrimaryActorTick.bCanEverTick = false;
	CurrentPopUpClassIndex = 0;
	CurrentPopUpCount = 0;

}

// Called when the game starts or when spawned
void AMyPopUpManager::BeginPlay()
{
	Super::BeginPlay();
	
	// 1. まずMapの器（Wrapper）だけを全てのクラス分用意する
	for (TSubclassOf<AMyDamagePopUp> PopUpClass : PopUpClasses)
	{
		if (PopUpClass)
		{
			MultiPopUpPool.Add(PopUpClass, FPopUpPoolWrapper());
		}
	}

	// 2. 時分割生成を開始
	if (PopUpClasses.Num() > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(PopUpTimerHandle, this, &AMyPopUpManager::IncrementallyPopUp, 0.01f, true);
	}
}

// Called every frame
void AMyPopUpManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyPopUpManager::SpawnPopUpFromPool(FVector Location, float Damage)
{
	// 改修ポイント：GetPopUpFromPool で適切なインスタンスを取得
	AMyDamagePopUp* PopUpToUse = GetPopUpFromPool(PopUpIndex);

	// 改修ポイント
	if (PopUpToUse)
	{
		Damage = floor(Damage * 10.0f) / 10.0f;
		PopUpToUse->SetActorLocation(Location);
		PopUpToUse->DamageAmount = Damage;
		PopUpToUse->SetActorHiddenInGame(false);
		PopUpToUse->SetActorTickEnabled(true);
		PopUpToUse->OnDamageAmountReceived();
	}
}

AMyDamagePopUp* AMyPopUpManager::GetPopUpFromPool(int32 InPopUpIndex)
{
	// インデックスの安全チェック
	if (!PopUpClasses.IsValidIndex(InPopUpIndex)) return nullptr;

	TSubclassOf<AMyDamagePopUp> TargetClass = PopUpClasses[InPopUpIndex];
	FPopUpPoolWrapper* TargetPoolWrapper = MultiPopUpPool.Find(TargetClass);
    
	if (TargetPoolWrapper)
	{
		for (AMyDamagePopUp* PopUp : TargetPoolWrapper->PopUpPool)
		{
			if (PopUp && PopUp->IsHidden())
			{
				return PopUp;
			}
		}
	}
	return nullptr;
}

void AMyPopUpManager::IncrementallyPopUp()
{
	// 全てのクラスの生成が終わったかチェック
	if (!PopUpClasses.IsValidIndex(CurrentPopUpClassIndex))
	{
		GetWorld()->GetTimerManager().ClearTimer(PopUpTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("All PopUp Pools Fully Initialized."));
		return;
	}

	TSubclassOf<AMyDamagePopUp> TargetClass = PopUpClasses[CurrentPopUpClassIndex];
    
	if (TargetClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		AMyDamagePopUp* NewPopUp = GetWorld()->SpawnActor<AMyDamagePopUp>(TargetClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (NewPopUp)
		{
			// 非アクティブ化
			NewPopUp->DeactivatePopUp(); 
            
			// MapからWrapperを取得して配列に追加
			FPopUpPoolWrapper* Wrapper = MultiPopUpPool.Find(TargetClass);
			if (Wrapper)
			{
				Wrapper->PopUpPool.Add(NewPopUp);
			}
		}
	}

	// カウントアップ
	CurrentPopUpCount++;

	// 指定したプールサイズに達したら、次のクラスへ
	if (CurrentPopUpCount >= PoolSize)
	{
		CurrentPopUpCount = 0;
		CurrentPopUpClassIndex++;
	}
}
