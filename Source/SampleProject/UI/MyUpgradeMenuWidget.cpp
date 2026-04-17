// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUpgradeMenuWidget.h"

#include "Kismet/GameplayStatics.h"
#include "MyGameHUD.h" // HUDのヘッダーを追加
#include "MyPlayerCharacter.h"
void UMyUpgradeMenuWidget::SelectUpgrade(int32 UpgradeType)
{
	if (!bCanClick) return;
		// 1. プレイヤーを取得してパワーアップを適用
	AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(GetOwningPlayerPawn());
	if (Player)
	{
		Player->ApplyUpgrade(UpgradeType);
	}

	// HUDを取得して一括終了
	if (AMyGameHUD* HUD = GetOwningPlayer()->GetHUD<AMyGameHUD>())
	{
		HUD->CloseActiveMenu();
	}
}

