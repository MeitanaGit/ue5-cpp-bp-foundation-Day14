// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyDamagePopUp.h"
#include "MyPopUpManager.generated.h"

USTRUCT(BlueprintType)
struct FPopUpPoolWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<class AMyDamagePopUp*> PopUpPool; // "class" を付けて前方宣言
};

UCLASS()
class SAMPLEPROJECT_API AMyPopUpManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyPopUpManager();
	
	// 敵から呼ばれる関数：プールから数字を取り出して表示する
	void SpawnPopUpFromPool(FVector Location, float Damage);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// BP登録用
	UPROPERTY(EditAnywhere, Category = "Custom|Pooling")
	TArray<TSubclassOf<class AMyDamagePopUp>> PopUpClasses;

	UPROPERTY(EditAnywhere, Category = "Custom|Pooling")
	int32 PoolSize = 50; // 最大同時表示数

	// ダメージだけじゃなくて回復などにも対応
	int32 PopUpIndex = 0;
	
	//負荷スパイクを避ける措置　
	// 時分割生成用のタイマーハンドル
	FTimerHandle PopUpTimerHandle;
	
	// インデックスを指定してプールからポップアップを取得する関数
	AMyDamagePopUp* GetPopUpFromPool(int32 InPopUpIndex);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// オブジェクトプーリング周り
	UPROPERTY()
	TMap<TSubclassOf<class AMyDamagePopUp>, FPopUpPoolWrapper> MultiPopUpPool;

	// 現在どの種類の弾を生成中か
	int32 CurrentPopUpClassIndex = 0;
	// その種類の弾を何個まで生成したか
	int32 CurrentPopUpCount = 0;
	// 1回ずつ呼ばれる生成関数
	void IncrementallyPopUp();

};
