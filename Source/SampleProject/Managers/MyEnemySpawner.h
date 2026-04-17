// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyEnemy.h"

#include "MyEnemySpawner.generated.h"



// 1. USTRUCTマクロを忘れずに追加
USTRUCT(BlueprintType)
struct FEnemyPoolWrapper
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<class AMyEnemy*> EnemyPool; // "class" を付けて前方宣言
	
	// 明示的にコンストラクタを追加
    FEnemyPoolWrapper()
    {
        EnemyPool.Empty();
    }
};
USTRUCT(BlueprintType)
struct FSpawnPhase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString PhaseName;

	// このフェーズで出現対象にする EnemyClasses のインデックスリスト
	UPROPERTY(EditAnywhere)
	TArray<int32> AllowedEnemyIndices;

	// 次のフェーズに移行するまでの時間（秒）
	UPROPERTY(EditAnywhere)
	float PhaseDuration = 0.0f;

	// --- 追加：ボス出現フラグ ---
	UPROPERTY(EditAnywhere)
	bool bIsBossPhase = false;

	// ボスとして出現させる EnemyClasses のインデックス 複数出現対応
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsBossPhase"))
	TArray<int32> AllowedBossIndices;
	// --- コンストラクタを追加して確実に初期化 ---
	FSpawnPhase()
		: PhaseName(TEXT("New Phase"))
		, PhaseDuration(0.0f)
		, bIsBossPhase(false)
	{
	}
};
UCLASS()
class SAMPLEPROJECT_API AMyEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	// スポーン間隔（秒）
	UPROPERTY(EditAnywhere, Category = "Custom|Spawning")
	float SpawnInterval = 1.0f;

	// プレイヤーからの距離（画面外に設定）
	UPROPERTY(EditAnywhere, Category = "Custom|Spawning")
	float SpawnRadius = 1000.0f;

	UPROPERTY(VisibleAnywhere, Category = "Custom|Spawning")
	APawn* PlayerPawn;
	
	// 実際にスポーンさせる関数
	void SpawnEnemy();
	
	void SpawnBoss(int32 InBossIndex);

	// 敵出現のタイマーハンドル
	FTimerHandle SpawnTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 出現させる敵のインデックス
	int32 EnemyIndex = 0;

	// 複数の敵クラスを登録できる配列に変更
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	TArray<TSubclassOf<class AMyEnemy>> EnemyClasses;
	
	
	// オブジェクトプーリング用
private:
	// 事前に生成しておく数
	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 PoolSize = 100;

	//敵の種類管理用
	// 生成済みの弾を格納しておくプール（倉庫）
	// クラス(Key) ごとに 弾のリスト(Value) を保持するマップ
	// TMap<敵の種類, その敵のプール本体>
	UPROPERTY()
	TMap<TSubclassOf<class AMyEnemy>, FEnemyPoolWrapper> MultiEnemyPool;
	
	UPROPERTY(EditAnywhere, Category = "Enemy")
	int32 EnemyPoolSize = 50; // 各種類ごとのプールサイズ

	// インデックスを指定してプールから敵を取得する関数
	AMyEnemy* GetEnemyFromPool(int32  InEnemyIndex);
	
	//負荷スパイクを避ける措置
	// 時分割生成用のタイマーハンドル
	FTimerHandle IncrementallySpawnTimerHandle;

	// 現在どの種類の弾を生成中か
	int32 CurrentSpawningClassIndex = 0;
	// その種類の弾を何個まで生成したか
	int32 CurrentSpawningCount = 0;

	// 1回ずつ呼ばれる生成関数
	void IncrementallySpawnEnemy();

	//-----------------------------
	// 敵出現フェーズの管理
	int32 CurrentPhaseIndex = 0;
	FTimerHandle PhaseTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Custom|Spawning")
	TArray<FSpawnPhase> SpawnPhases;
	
	// 敵出現のフェーズ処理の初期化
	void InitPhase();
	// 次のフェーズへ
	void NextPhase();

	//--------------------------------
	// ボスに関連する処理
protected:
	// ボスが死んだときに呼び出される関数（UFUNCTIONが必要）
	UFUNCTION()
	void OnBossDefeated(AMyEnemy* DefeatedBoss);

private:
	int32 LivingBossCount = 0;
	
	// フェイズ全終了
	void OnAllPhasesCompleted();
};
