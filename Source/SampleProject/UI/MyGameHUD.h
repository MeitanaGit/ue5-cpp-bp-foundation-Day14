// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyPlayerCharacter.h" // ← FUpgradeDataの定義を読み込むために追加
#include "MyUpgradeMenuWidget.h" // ← Widgetの定義を読み込むために追加
#include "MyGameTypes.h"
#include "MyGameHUD.generated.h"

/**
 * 
 */

UCLASS()
class SAMPLEPROJECT_API AMyGameHUD : public AHUD
{
	GENERATED_BODY()
public:
	// 汎用メニューを表示する関数（ポーズやレベルアップで使い回す）
	void ShowGenericMenu(TArray<FUpgradeData> Options, FText MenuTitle, EMenuScreenState ScreenState);
	
	// ポーズ関連
	void ShowPauseMenu();
	
	// クリア関連
	void ShowAllPhasesCompletedMenu();

	// ゲームオーバー関連
	void ShowGameOverMenu();

protected:
	// エディタでセットするWidgetのクラス
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UMyUpgradeMenuWidget> UpgradeMenuClass;

	// 生成されたWidgetのインスタンス保持用
	UPROPERTY()
	class UMyUpgradeMenuWidget* UpgradeMenuInstance;

	UPROPERTY()
	UUserWidget* CurrentMenuWidget; // 今開いているメニューを記憶しておく
	// 開いているウィンドウを閉じるためのもの
public:
	UFUNCTION(BlueprintCallable)
	void CloseActiveMenu();
};
