// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemySpawner.h"
#include "MyGameHUD.h"


// Sets default values
AMyEnemySpawner::AMyEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; // スポナー自体はTick不要
}

// Called when the game starts or when spawned
void AMyEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	EnemyIndex = 0;
	UE_LOG(LogTemp, Warning, TEXT("EnemyClasses count: %d"), EnemyClasses.Num());
	
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	// プールの初期化（器だけ用意する）
	for (TSubclassOf<AMyEnemy> EnemyClassElem : EnemyClasses)
	{
		if (EnemyClassElem)
		{
			MultiEnemyPool.Add(EnemyClassElem, FEnemyPoolWrapper());
		}
	}

	// 時分割生成を開始（0.01秒間隔で実行。負荷に応じて調整してください）
	if (EnemyClasses.Num() > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(IncrementallySpawnTimerHandle, this, &AMyEnemySpawner::IncrementallySpawnEnemy, 0.01f, true);
	}

	// フェーズ設定
	InitPhase();
	
	// 指定した間隔で SpawnEnemy を繰り返し実行するタイマーを開始
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AMyEnemySpawner::SpawnEnemy, SpawnInterval, true);
}

// フェーズの初期化を行う
void AMyEnemySpawner::InitPhase()
{
	CurrentPhaseIndex = 0;
	// 配列が空でないかチェックしてからタイマーを設定
	if (SpawnPhases.IsValidIndex(CurrentPhaseIndex))
	{
		GetWorldTimerManager().SetTimer(PhaseTimerHandle, this, &AMyEnemySpawner::NextPhase, SpawnPhases[CurrentPhaseIndex].PhaseDuration, false);
	}
	LivingBossCount = 0;		// 念のためボス数もリセット
}

// Called every frame
void AMyEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyEnemySpawner::NextPhase()
{
	// 配列 SpawnPhases に対してチェックを行う
	if (SpawnPhases.IsValidIndex(CurrentPhaseIndex + 1))
	{
		CurrentPhaseIndex++;
		GetWorldTimerManager().SetTimer(PhaseTimerHandle, this, &AMyEnemySpawner::NextPhase, SpawnPhases[CurrentPhaseIndex].PhaseDuration, false);
		UE_LOG(LogTemp, Log, TEXT("Phase Switched to: %s"), *SpawnPhases[CurrentPhaseIndex].PhaseName);
	}
	else
	{
		// 【ここがポイント】次のフェーズがない ＝ 全フェーズ終了（全クリア）
		OnAllPhasesCompleted();
	}
}
void AMyEnemySpawner::SpawnEnemy()
{
	// 敵がいなさそうであれば戻る
	if (!PlayerPawn || EnemyClasses.Num() == 0 || !SpawnPhases.IsValidIndex(CurrentPhaseIndex)) return;

	// 1. 現在のフェーズ情報を取得
	const FSpawnPhase& CurrentPhase = SpawnPhases[CurrentPhaseIndex];

	// 2. ボスフェーズかどうかの判定
	if (CurrentPhase.bIsBossPhase)
	{
		// --- 追加：ボスフェーズに入ったら時間経過によるフェーズ遷移を止める ---
		GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
		// 登録しているボスの数だけ回す
		for (int32 BossClassIndex : CurrentPhase.AllowedBossIndices)
		{
			SpawnBoss(BossClassIndex);
		}
		
		// ボス出現後はザコのスポーンタイマーを止める、
		// もしくは間隔を極端に遅くするなどの処理をここに入れる
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle); 
		return;
	}
	
	//----------------------------------
	// 3. フェーズ内の AllowedEnemyIndices が空でないか確認
	if (CurrentPhase.AllowedEnemyIndices.Num() == 0) return;
	
	// 4. そのフェーズで許可されたインデックスの中からランダムに選ぶ
	int32 RandomListIndex = FMath::RandRange(0, CurrentPhase.AllowedEnemyIndices.Num() - 1);
	int32 EnemyClassIndex = CurrentPhase.AllowedEnemyIndices[RandomListIndex];
    
	// 5. 選ばれたインデックスでプールから取得
	AMyEnemy* EnemyToUse = GetEnemyFromPool(EnemyClassIndex);

	if (EnemyToUse)
	{
		float RandomAngle = FMath::FRandRange(0.f, 2.f * PI);
		FVector Offset = FVector(FMath::Cos(RandomAngle), FMath::Sin(RandomAngle), 0.f) * SpawnRadius;
		FVector SpawnLocation = PlayerPawn->GetActorLocation() + Offset;
		SpawnLocation.Z = PlayerPawn->GetActorLocation().Z;

		EnemyToUse->ActivateEnemy(SpawnLocation);
	}

	// スポーン間隔の短縮処理
	SpawnInterval = FMath::Max(0.1f, SpawnInterval * 0.99f);
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AMyEnemySpawner::SpawnEnemy, SpawnInterval, false);

}
// ボス出現専用
void AMyEnemySpawner::SpawnBoss(int32 InBossIndex)
{
	AMyEnemy* BossEnemy = GetEnemyFromPool(InBossIndex);
	if (BossEnemy)
	{
		// ボス死亡イベントに関数を登録する
		// 「BossEnemyが死んだら、私の OnBossDefeated を呼んでね」という約束
		BossEnemy->OnEnemyDefeated.AddDynamic(this, &AMyEnemySpawner::OnBossDefeated);
        
		LivingBossCount++; // 生きているボスをカウントアップ
		
		// ボス専用の出現演出（カメラシェイクやエフェクト）をここに書ける
		UE_LOG(LogTemp, Warning, TEXT("BOSS APPEARED!"));
		UE_LOG(LogTemp, Warning, TEXT("LivingBossCount : %d"),LivingBossCount);

		float RandomAngle = FMath::FRandRange(0.f, 2.f * PI);
		FVector Offset = FVector(FMath::Cos(RandomAngle), FMath::Sin(RandomAngle), 0.f) * SpawnRadius;
		FVector SpawnLocation = PlayerPawn->GetActorLocation() + Offset;
		SpawnLocation.Z = PlayerPawn->GetActorLocation().Z;

		BossEnemy->ActivateEnemy(SpawnLocation);
	}
}
AMyEnemy* AMyEnemySpawner::GetEnemyFromPool(int32 InEnemyIndex)
{
	if (!EnemyClasses.IsValidIndex(InEnemyIndex)) return nullptr;

	TSubclassOf<AMyEnemy> TargetClass = EnemyClasses[InEnemyIndex];
	FEnemyPoolWrapper* TargetPoolWrapper = MultiEnemyPool.Find(TargetClass);
    
	
	if (TargetPoolWrapper)
	{
		for (AMyEnemy* Enemy : TargetPoolWrapper->EnemyPool)
		{
			// 敵が死んでいたら使える
			if (Enemy && Enemy->GetIsDead())
			{
				// 今使われてない敵だから使って良いよの合図
				return Enemy;
			}
		}
		// --- 追加：プールが空（生成が間に合っていない）場合のバックアップ ---
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AMyEnemy* NewEnemy = GetWorld()->SpawnActor<AMyEnemy>(TargetClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (NewEnemy)
		{
			NewEnemy->DeactivateEnemy();
			TargetPoolWrapper->EnemyPool.Add(NewEnemy);
			return NewEnemy;
		}
	}
	
	return nullptr;
}

void AMyEnemySpawner::IncrementallySpawnEnemy()
{
	// 全てのクラスの生成が終わったらタイマーを止める
	if (!EnemyClasses.IsValidIndex(CurrentSpawningClassIndex))
	{
		GetWorld()->GetTimerManager().ClearTimer(IncrementallySpawnTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("All Enemys Pool Initialized."));
		return;
	}

	TSubclassOf<AMyEnemy> TargetClass = EnemyClasses[CurrentSpawningClassIndex];
    
	if (TargetClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		// 「衝突していても関係なく強制的にスポーンさせる」という設定です
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AMyEnemy* NewEnemy = GetWorld()->SpawnActor<AMyEnemy>(TargetClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (NewEnemy)
		{
			NewEnemy->DeactivateEnemy();
			// Mapから該当するWrapperを取得して追加
			MultiEnemyPool[TargetClass].EnemyPool.Add(NewEnemy);
		}
	}

	CurrentSpawningCount++;

	// 指定したプールサイズに達したら、次の種類の敵へ
	if (CurrentSpawningCount >= EnemyPoolSize)
	{
		CurrentSpawningCount = 0;
		CurrentSpawningClassIndex++;
	}
}

// ボスが死ぬたびにここが呼ばれる
void AMyEnemySpawner::OnBossDefeated(AMyEnemy* DefeatedBoss)
{
	// 死んだボスからの登録を解除（念のため）
	DefeatedBoss->OnEnemyDefeated.RemoveDynamic(this, &AMyEnemySpawner::OnBossDefeated);

	LivingBossCount--;

	UE_LOG(LogTemp, Warning, TEXT("OnBossDefeated : %d"),LivingBossCount);

	// 全てのボス（1体でも3体でも）を倒したかチェック
	if (LivingBossCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("All Bosses Defeated! Moving to Next Phase."));
        
		// ボスフェーズは時間切れを待たずに次へ進む
		GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
		NextPhase();
		LivingBossCount = 0;
        
		// 止めていた雑魚スポーンタイマーを再開する
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AMyEnemySpawner::SpawnEnemy, SpawnInterval, true);
	}
}
void AMyEnemySpawner::OnAllPhasesCompleted()
{
	UE_LOG(LogTemp, Error, TEXT("ALL PHASES COMPLETED! GAME CLEAR!"));

	// 1. スポーンタイマーを完全に止める
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	GetWorldTimerManager().ClearTimer(PhaseTimerHandle);

	// こっちは敵でPawnのプレイヤーのようにGetController()を持っていないので、こちらにする。
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		AMyGameHUD* HUD = Cast<AMyGameHUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->ShowAllPhasesCompletedMenu();
		}
	}
}