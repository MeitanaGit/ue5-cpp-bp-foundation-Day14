// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyTitleHUD.generated.h"


/**
 * 
 */
UCLASS()
class SAMPLEPROJECT_API AMyTitleHUD : public AHUD
{
	GENERATED_BODY()
	// ゲーム開始時に呼ばれる
	virtual void BeginPlay() override;

protected:
	// タイトル画面用のWidgetクラス（BPで割り当てる用）
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> TitleMenuClass;

	// 生成したインスタンスを保持
	UPROPERTY()
	class UUserWidget* TitleMenuInstance;

public:
	// Widgetから呼ばれるパワーアップ適用関数
	UFUNCTION(BlueprintCallable, Category = "TitleSelect")
	void ApplyTitleButton(int32 ActionType);	

};
