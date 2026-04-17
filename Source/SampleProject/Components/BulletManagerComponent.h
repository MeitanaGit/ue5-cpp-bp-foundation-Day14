// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BulletManagerComponent.generated.h"

USTRUCT(BlueprintType)
struct FBulletPoolWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<class AMyBullet*> Bullets; // "class" を付けて前方宣言
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SAMPLEPROJECT_API UBulletManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBulletManagerComponent();

	// 複数の弾丸クラスを登録できる配列に変更
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<TSubclassOf<class AMyBullet>> BulletClasses;

	// キャラクターから呼ぶためのメイン関数
	// インデックス、発射位置、回転、ダメージを受け取って弾を飛ばす
	AMyBullet* RequestBullet(int32 BulletIndex, FVector Location, FRotator Rotation, float Damage);

	// 弾の種類がいくつあるか返す（SwitchBulletの安全策用）
	int32 GetBulletClassCount() const { return BulletClasses.Num(); }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	//弾丸管理用
	// 生成済みの弾を格納しておくプール（倉庫）
	UPROPERTY()
	// クラス(Key) ごとに 弾のリスト(Value) を保持するマップ
	// TMap<弾の種類, その弾のプール本体>
	TMap<TSubclassOf<class AMyBullet>, FBulletPoolWrapper> MultiBulletPool;
	
	UPROPERTY(EditAnywhere, Category = "BulletComponentCombat")
	int32 BulletPoolSize = 50; // 各種類ごとのプールサイズ

	//負荷スパイクを避ける措置　
	// 時分割生成用のタイマーハンドル
	FTimerHandle SpawnTimerHandle;

	// 現在どの種類の弾を生成中か
	int32 CurrentSpawningClassIndex = 0;
	// その種類の弾を何個まで生成したか
	int32 CurrentSpawningCount = 0;

	// 1回ずつ呼ばれる生成関数
	void IncrementallySpawnBullet();


	// インデックスを指定してプールから弾を取得する関数
	AMyBullet* GetBulletFromPool(int32 BulletIndex);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
