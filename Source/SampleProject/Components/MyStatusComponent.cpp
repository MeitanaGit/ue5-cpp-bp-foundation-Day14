// Fill out your copyright notice in the Description page of Project Settings.


#include "MyStatusComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "MyGameTypes.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UMyStatusComponent::UMyStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UMyStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	// --- 追加：初期HPの設定 ---
	CurrentHP = MaxHP;
	
	// 所有者（Character）への参照を取得
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner) return;
	// 移動力の初期化
	Owner->GetCharacterMovement()->MaxWalkSpeed = BaseMoveSpeed;
}


// Called every frame
void UMyStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMyStatusComponent::ApplyUpgrade(EMenuActionType Action)
{
	// 所有者（Character）への参照を取得
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner) return;

	switch (Action)
	{
	case EMenuActionType::Upgrade_Damage:
		BaseDamage *= 1.4f;
		UE_LOG(LogTemp, Warning, TEXT("BaseDamage: %f"), BaseDamage);
		break;
	case EMenuActionType::Upgrade_Speed:
		BaseMoveSpeed *= 1.1f; // 10%アップ
		Owner->GetCharacterMovement()->MaxWalkSpeed = BaseMoveSpeed;
		UE_LOG(LogTemp, Warning, TEXT("BaseMoveSpeed: %f"), BaseMoveSpeed);
		break;
	case EMenuActionType::Upgrade_FireRate:
		FireRate = FMath::Max(0.1f, FireRate * 0.8f);
		UE_LOG(LogTemp, Warning, TEXT("FireRate: %f"), FireRate);
		break;
		// --- 追加：HPアップグレードのロジック ---
	case EMenuActionType::Upgrade_Health:
		MaxHP += 20.0f;
		RestoreFullHealth(); // 最大値が増えたら全回復させる
		UE_LOG(LogTemp, Warning, TEXT("MaxHP: %f"), MaxHP);
		break;
		
		// システム系（Retry/Quit）はここでは処理せず、呼び出し元（PlayerやHUD）に任せる
	default:
		break;
	}
}

bool UMyStatusComponent::AddScore(int32 Amount)
{


	CurrentScore += Amount;
	if (CurrentScore >= NextLevelScore)
	{
		CurrentLevel++;
		CurrentScore -= NextLevelScore; // 余剰分を次に回す
		NextLevelScore *= LevelDifficultyMultiplier;
		return true; // レベルアップ！
	}
	return false;
	
}
// --- 追加：ダメージ処理 ---
void UMyStatusComponent::ReduceHealth(float Amount)
{
	// Clampで 0 ～ MaxHP の間に収める
	CurrentHP = FMath::Clamp(CurrentHP - Amount, 0.0f, MaxHP);
    
	UE_LOG(LogTemp, Log, TEXT("Player HP: %f / %f"), CurrentHP, MaxHP);
}

// --- 追加：全回復処理 ---
void UMyStatusComponent::RestoreFullHealth()
{
	CurrentHP = MaxHP;
}

TArray<FUpgradeData> UMyStatusComponent::GenerateUpgradeOptions()
{
	InitializeUpgradePool();
	TArray<FUpgradeData> SelectedOptions;
	TArray<FUpgradeData> TempPool = AllUpgradePool;

	int32 NumToSelect = FMath::Min(3, TempPool.Num());
	for (int32 i = 0; i < NumToSelect; i++)
	{
		int32 RandomIndex = FMath::RandRange(0, TempPool.Num() - 1);
		SelectedOptions.Add(TempPool[RandomIndex]);
		TempPool.RemoveAt(RandomIndex);
	}
	return SelectedOptions;
}

void UMyStatusComponent::InitializeUpgradePool()
{
	AllUpgradePool.Empty();

	// 0: 攻撃力
	FUpgradeData Opt0;
	Opt0.UpgradeName = FText::FromString(TEXT("攻撃力アップ"));
	Opt0.Description = FText::FromString(TEXT("ダメージが増加します"));
	Opt0.UpgradeType = static_cast<int32>(EMenuActionType::Upgrade_Damage);
	AllUpgradePool.Add(Opt0);

	// 1: 移動速度
	FUpgradeData Opt1;
	Opt1.UpgradeName = FText::FromString(TEXT("移動速度アップ"));
	Opt1.Description = FText::FromString(TEXT("足が少し速くなります"));
	Opt1.UpgradeType = static_cast<int32>(EMenuActionType::Upgrade_Speed);
	AllUpgradePool.Add(Opt1);

	// 2: 連射速度
	FUpgradeData Opt2;
	Opt2.UpgradeName = FText::FromString(TEXT("連射速度アップ"));
	Opt2.Description = FText::FromString(TEXT("弾を撃つ間隔が短くなります"));
	Opt2.UpgradeType = static_cast<int32>(EMenuActionType::Upgrade_FireRate);
	AllUpgradePool.Add(Opt2);

	// 3: HP増加 (新規追加)
	FUpgradeData Opt3;
	Opt3.UpgradeName = FText::FromString(TEXT("最大HPアップ"));
	Opt3.Description = FText::FromString(TEXT("最大HPが20増加し、全回復します"));
	Opt3.UpgradeType = static_cast<int32>(EMenuActionType::Upgrade_Health);
	AllUpgradePool.Add(Opt3);
}
