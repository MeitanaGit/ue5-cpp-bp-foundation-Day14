// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyPlayerCharacter.h" // FUpgradeDataの定義を読み込むため
#include "MyUpgradeMenuWidget.generated.h"

/**
 * レベルアップ時のアップグレード選択メニュー用基底クラス
 */
UCLASS()
class SAMPLEPROJECT_API UMyUpgradeMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** * 3つのアップグレード選択肢をセットアップするイベント
	 * 処理の中身はBlueprint側（デザイン側）でボタンに文字を入れるために使います
	 */
	// HUDから呼ばれ、BP側で「タイトル書き換え」や「ボタン生成」を行うイベント
	UFUNCTION(BlueprintImplementableEvent, Category = "LevelUp")
	void OnSetupOptions(const TArray<FUpgradeData>& Options, const FText& Title, EMenuScreenState ScreenState);

protected:
	/**
	 * ボタンがクリックされた時にBlueprintから呼び出す関数
	 * @param UpgradeType 選択されたパワーアップの種類
	 */
	
	// どういった状態でUIが開いているのかを判別する
public:
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	void SelectUpgrade(int32 UpgradeType);
	
protected:
	// Blueprint側で読み書きできるように「BlueprintReadWrite」をつける
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	bool bCanClick = false; // 初期値はfalse	
};