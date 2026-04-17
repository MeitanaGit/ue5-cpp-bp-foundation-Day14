// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyStatusComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SAMPLEPROJECT_API UMyStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyStatusComponent();

	// これらを追加して、外部から値を安全に取得できるようにします
	float GetCurrentHealth() const { return CurrentHP; }
	float GetMaxHealth() const { return MaxHP; }
	
	UFUNCTION(BlueprintCallable, Category = "Custom|Stats")
	void ReduceHealth(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Custom|Stats")
	void RestoreFullHealth();

	// ゲッター（HUDなどで使いやすくするため）
	float GetCurrentHP() const { return CurrentHP; }
	float GetMaxHP() const { return MaxHP; }
	
	// 攻撃力
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Stats")
	float BaseDamage = 10.0f;

	// 攻撃発射レート
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Stats")
	float FireRate = 0.5f;

	// 現在のスコアを保持する変数
	UPROPERTY(VisibleAnywhere, Category = "Custom|Stats")
	int32 CurrentScore = 0;

	// 体力に関するパラメーター
	UPROPERTY(EditAnywhere, Category = "Custom|Stats")
	float MaxHP = 100.0f;

	UPROPERTY(VisibleAnywhere, Category = "Custom|Stats")
	float CurrentHP = 100.0f;

	// 自キャラの速度はここで管理
	UPROPERTY(EditAnywhere, Category = "Custom|Stats")
	float BaseMoveSpeed = 150.0f;
	
	// レベルアップに関するパラメータ
	UPROPERTY(VisibleAnywhere, Category = "Custom|Stats")
	int32 CurrentLevel = 1;

	// 次のレベルに必要なスコア
	UPROPERTY(EditAnywhere, Category = "Custom|Stats")
	int32 NextLevelScore = 100;

	// レベルアップごとにどれくらい必要スコアを増やすか（例: 1.5倍）
	UPROPERTY(EditAnywhere, Category = "Custom|Stats")
	float LevelDifficultyMultiplier = 1.5f;

	// アップグレードの実行ロジック
	UFUNCTION(BlueprintCallable, Category = "Custom|Stats")
	void ApplyUpgrade(EMenuActionType Action);

	// 経験値に関わる部分
public:	
	// スコア加算とレベルチェック（戻り値 true でレベルアップ発生）
	UFUNCTION(BlueprintCallable, Category = "Custom|Stats")
	bool AddScore(int32 Amount);

	// アップグレード候補の抽選
	UFUNCTION(BlueprintCallable, Category = "Custom|Stats")
	TArray<FUpgradeData> GenerateUpgradeOptions();
	
private:
	// プールの初期化（内部処理）
	void InitializeUpgradePool();
	TArray<FUpgradeData> AllUpgradePool;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
