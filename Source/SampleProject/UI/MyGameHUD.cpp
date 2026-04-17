// Fill out your copyright notice in the Description page of Project Settings.

//　ウィジェットなどのUIの表示を引き受けるクラス、あくまで処理は各々のクラスで行ってもらう


#include "MyGameHUD.h"
#include "Blueprint/UserWidget.h"
#include "MyUpgradeMenuWidget.h"
#include "Kismet/GameplayStatics.h"

void AMyGameHUD::ShowGenericMenu(TArray<FUpgradeData> Options, FText MenuTitle, EMenuScreenState ScreenState)
{
	// 二重ポーズを防ぐ
	if (UGameplayStatics::IsGamePaused(GetWorld())) return;
	APlayerController* PC = GetOwningPlayerController();
	if (!PC || !UpgradeMenuClass) return;

	// ウィジェットがなければ生成
	if (!UpgradeMenuInstance)
	{
		UpgradeMenuInstance = CreateWidget<UMyUpgradeMenuWidget>(GetWorld(), UpgradeMenuClass);
	}

	if (UpgradeMenuInstance)
	{
		// ビューポートに追加（既にある場合は無視される）
		if (!UpgradeMenuInstance->IsInViewport())
		{
			UpgradeMenuInstance->AddToViewport();
		}
		// ここで今のウィンドウを保持する
		CurrentMenuWidget = UpgradeMenuInstance;

		// データを渡す (レベルアップ項目、ポーズ項目など)
		UpgradeMenuInstance->OnSetupOptions(Options, MenuTitle, ScreenState);

		// 共通の「メニュー表示時の設定」
		UGameplayStatics::SetGamePaused(this, true);
		PC->bShowMouseCursor = true;
        
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(UpgradeMenuInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}
}

// ポーズメニュー実装
void AMyGameHUD::ShowPauseMenu()
{
	TArray<FUpgradeData> PauseMenuOptions;
    
	FUpgradeData ResumeOpt;
	ResumeOpt.UpgradeName = FText::FromString(TEXT("ゲーム再開"));
	ResumeOpt.UpgradeType = static_cast<int32>(EMenuActionType::System_Resume); 
	PauseMenuOptions.Add(ResumeOpt);

	FUpgradeData RetryOpt;
	RetryOpt.UpgradeName = FText::FromString(TEXT("リトライ"));
	RetryOpt.UpgradeType = static_cast<int32>(EMenuActionType::System_Retry); 
	PauseMenuOptions.Add(RetryOpt);

	FUpgradeData TitleOpt;
	TitleOpt.UpgradeName = FText::FromString(TEXT("タイトルへ"));
	TitleOpt.UpgradeType = static_cast<int32>(EMenuActionType::System_ReturnTitle);
	PauseMenuOptions.Add(TitleOpt);
	
	
	FUpgradeData QuitOpt;
	QuitOpt.UpgradeName = FText::FromString(TEXT("終了"));
	QuitOpt.UpgradeType = static_cast<int32>(EMenuActionType::System_Quit);
	PauseMenuOptions.Add(QuitOpt);

	// 呼び出し時にタイトルを渡す
	ShowGenericMenu(PauseMenuOptions, FText::FromString(TEXT("一時停止")), EMenuScreenState::Pause);
}
//　ステージクリア 
void AMyGameHUD::ShowAllPhasesCompletedMenu()
{
	TArray<FUpgradeData> PauseMenuOptions;
    
	FUpgradeData RetryOpt;
	RetryOpt.UpgradeName = FText::FromString(TEXT("リトライ"));
	RetryOpt.UpgradeType = static_cast<int32>(EMenuActionType::System_Retry); 
	PauseMenuOptions.Add(RetryOpt);

	FUpgradeData TitleOpt;
	TitleOpt.UpgradeName = FText::FromString(TEXT("タイトルへ"));
	TitleOpt.UpgradeType = static_cast<int32>(EMenuActionType::System_ReturnTitle);
	PauseMenuOptions.Add(TitleOpt);
	
	// 呼び出し時にタイトルを渡す
	ShowGenericMenu(PauseMenuOptions, FText::FromString(TEXT("ゲームクリア！")), EMenuScreenState::Clear);
}
//　ゲームオーバー
void AMyGameHUD::ShowGameOverMenu()
{
	TArray<FUpgradeData> PauseMenuOptions;
    
	FUpgradeData RetryOpt;
	RetryOpt.UpgradeName = FText::FromString(TEXT("リトライ"));
	RetryOpt.UpgradeType = static_cast<int32>(EMenuActionType::System_Retry); 
	PauseMenuOptions.Add(RetryOpt);

	FUpgradeData TitleOpt;
	TitleOpt.UpgradeName = FText::FromString(TEXT("タイトルへ"));
	TitleOpt.UpgradeType = static_cast<int32>(EMenuActionType::System_ReturnTitle);
	PauseMenuOptions.Add(TitleOpt);
	
	// 呼び出し時にタイトルを渡す
	ShowGenericMenu(PauseMenuOptions, FText::FromString(TEXT("ゲームオーバー")), EMenuScreenState::GameOver);
}


void AMyGameHUD::CloseActiveMenu()
{
	if (CurrentMenuWidget)
	{
		CurrentMenuWidget->RemoveFromParent();
		CurrentMenuWidget = nullptr; // 参照をクリア
	}

	// ついでに入力モードもここで戻してあげると、呼び出し側が楽！
	if (APlayerController* PC = GetOwningPlayerController())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}
}