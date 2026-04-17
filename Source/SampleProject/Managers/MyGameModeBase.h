// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyOrbManager.h"
#include "MyPopUpManager.h"
#include "MyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SAMPLEPROJECT_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	// 外部（敵）からマネージャーを取得するための関数
	class AMyOrbManager* GetOrbManager() const { return OrbManagerInstance; }
	// ゲームモードからポップアップマネージャーの情報を受け取るもの
	class AMyPopUpManager* GetPopUpManager() const { return PopUpManagerInstance; }
	
protected:
	virtual void BeginPlay() override;	
	// スポナー（敵用）
	UPROPERTY(EditAnywhere, Category = "Custom|Management")
	TSubclassOf<AActor> EnemySpawnerClass;

	// オーブマネージャー用
	UPROPERTY(EditAnywhere, Category = "Custom|Management")
	TSubclassOf<class AMyOrbManager> OrbManagerClass;
	
	// ポップアップマネージャー用
	UPROPERTY(EditAnywhere, Category = "Custom|Management")
	TSubclassOf<class AMyPopUpManager> PopUpManagerClass;
	
private:
	// 生成された実体を保持する
	UPROPERTY()
	class AMyOrbManager* OrbManagerInstance;

	UPROPERTY()
	class AMyPopUpManager* PopUpManagerInstance; // 追加
};
