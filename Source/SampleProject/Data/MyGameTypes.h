#pragma once

#include "CoreMinimal.h"
#include "MyGameTypes.generated.h" // これが必要（コンパイル時に自動生成されます）

// --- 列挙型の定義 ---
UENUM(BlueprintType)
enum class EMenuActionType : uint8
{
	Upgrade_Damage,
	Upgrade_Speed,
	Upgrade_Health,
	Upgrade_FireRate,
	System_Resume = 99,
	System_Quit   = 100,
	System_Retry  = 101,
	
	System_GameStart = 102,		// タイトルから本編へ遷移用	
	System_ReturnTitle = 103	// タイトルに戻る
	
};
//	メニュー表示の種類分け
UENUM(BlueprintType)
enum class EMenuScreenState : uint8
{
	Pause,              // 一時停止
	GameOver,           // ゲームオーバー
	LevelUp,            // レベルアップ
	Clear               // ステージクリア
};
// --- 構造体の定義（もしPlayerCharacter.hから移すなら） ---
USTRUCT(BlueprintType)
struct FUpgradeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
	FText UpgradeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
	int32 UpgradeType;

	// デフォルト値
	FUpgradeData() : UpgradeType(0) {}

};
