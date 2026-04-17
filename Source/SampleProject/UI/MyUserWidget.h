// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h" // 必須
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SAMPLEPROJECT_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// meta = (BindWidget) を付けることで、BP側の同名パーツと自動で紐付きます
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

	// スコアの数字を更新する関数
	void UpdateScoreDisplay(int32 NewScore);

	// HP表示周りテキスト
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateHealthDisplay(float CurrentHP, float MaxHP);

	// Blueprint側でアニメーションなどを再生するためのイベント
	UFUNCTION(BlueprintImplementableEvent, Category = "LevelUp")
	void PlayLevelUpAnimation();

	// レベルの数字も更新したい場合は追加
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	void UpdateLevelDisplay(int32 NewLevel);

	// 参照用のTextBlock（ScoreTextと同様に定義されている前提）
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LevelText;
	
};

