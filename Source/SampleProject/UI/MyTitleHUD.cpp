// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyTitleHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameTypes.h"
#include "Misc/OutputDeviceNull.h" // これが必要

void AMyTitleHUD::BeginPlay()
{
	Super::BeginPlay();


	APlayerController* PC = GetOwningPlayerController();
	UE_LOG(LogTemp, Warning, TEXT("AMyTitleHUD: %p,%p"), PC,*TitleMenuClass);
	if (PC && TitleMenuClass)
	{
		// 1. Widgetの生成
		TitleMenuInstance = CreateWidget<UUserWidget>(GetWorld(), TitleMenuClass);
		if (TitleMenuInstance)
		{
			TitleMenuInstance->AddToViewport();

			// 2. マウスカーソルの表示と入力モードの設定
			PC->bShowMouseCursor = true;
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(TitleMenuInstance->TakeWidget());
			PC->SetInputMode(InputMode);
		}
	}
}

// タイトル画面のボタン選択はここに飛ぶ
void AMyTitleHUD::ApplyTitleButton(int32 ActionType)
{
	// int32 を列挙型にキャストして switch に渡す
	switch (static_cast<EMenuActionType>(ActionType))
	{
	case EMenuActionType::System_GameStart:
		// ステージ1へ移動
		UGameplayStatics::OpenLevel(this, FName(TEXT("Stage1")));
		break;

	case EMenuActionType::System_Quit:
		// ゲーム終了
		UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
		break;

	default:
		// 該当しないアクション（デバッグ用）
		UE_LOG(LogTemp, Warning, TEXT("Unknown ActionType: %d"), ActionType);
		break;
	}
}