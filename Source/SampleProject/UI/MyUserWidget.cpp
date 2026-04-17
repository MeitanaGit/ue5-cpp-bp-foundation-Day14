// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"

void UMyUserWidget::UpdateScoreDisplay(int32 NewScore)
{
	if (ScoreText)
	{
		// 数字をテキストに変換してセット
		ScoreText->SetText(FText::AsNumber(NewScore));
	}
}



void UMyUserWidget::UpdateLevelDisplay(int32 NewLevel)
{
	if (LevelText)
	{
		LevelText->SetText(FText::Format(NSLOCTEXT("UI", "LevelFormat", "Lv: {0}"), NewLevel));
	}
}