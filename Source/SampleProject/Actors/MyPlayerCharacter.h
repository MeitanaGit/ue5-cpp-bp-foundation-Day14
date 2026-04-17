// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h" //Geminiが必要と言ったヘッダー
#include "MyStatusComponent.h"
#include "MyGameTypes.h"
#include "MyPlayerCharacter.generated.h"

// 冒頭に前方宣言を追加
class UMyUserWidget;
// 前方宣言
class UMyStatusComponent;


UCLASS()

class SAMPLEPROJECT_API AMyPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyPlayerCharacter();

	//移動処理作成時に作ったやつ　MyInputは"MyInput"の方が良いよ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyInput")
	class UInputMappingContext* DefaultMappingContext;
	//移動処理作成時に作ったやつ　MyInputは"MyInput"の方が良いよ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyInput")
	class UInputAction* MoveAction;	

	// この下が今追加した発射系のコード
	// 発射ボタン用のアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyInput")
	class UInputAction* FireAction;

	// ポーズボタン用のアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyInput")
	class UInputAction* PauseAction;
	
	void Pause();
	void GameOver();
	
	//　弾丸切り替え用アクション、発射ボタンアクションの名前いじっただけ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyInput")
	class UInputAction* SwitchBulletAction;

	// 現在選択中の弾の番号（0番目, 1番目...）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 SelectedBulletIndex = 0;

	// 弾を切り替える関数（必要であれば）
	void SwitchBullet();	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 移動用の関数　Geminiが…
	void Move(const FInputActionValue& Value);
	
	// カメラを支えるアーム（これがないとカメラがキャラに埋まってしまいます）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	// 実際のカメラ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	// 発射関数
	void Fire(const FInputActionValue& Value);

	// ダメージ受け取り用
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
							 class AController* EventInstigator, AActor* DamageCauser) override;
	
	// BP側で「どのWidgetを使うか」選択するための変数
	UPROPERTY(EditAnywhere, Category = "MyUI")
	TSubclassOf<UMyUserWidget> HUDClass;

	// 生成されたUIの実体（インスタンス）
	UPROPERTY()
	UMyUserWidget* HUDInstance;
	
	// レベルアップ時の音
	UPROPERTY(EditAnywhere, Category = "MyEffects")
	class USoundBase* LevelUpSound;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// コンポーネントのポインタを追加
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UBulletManagerComponent* BulletManager;

	// 前回の発射時刻を記録
	float LastFireTime = 0.0f;


	// ビルボード関連
	// ビルボード用最後に移動した方向を保存（初期値は前方向など）
	FVector LastMovementInput = FVector(1.f, 0.f, 0.f);	

	// ステータス系
	// コンポーネントの定義
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom|Components", meta = (AllowPrivateAccess = "true"))
	UMyStatusComponent* StatusComponent;

	// レベルアップ時の処理
	void LevelUp();
	
	// レベルアップ等のシステム
private:
	// コンストラクタでカメラの初期化を行う(BP設定に任せたいのでひとまずコンストラクタで処理)
	void InitCamera();
	// BeginPlayで操作系の初期化を行う
	void InitController();

	
	
public:
	// コンポーネントに処理させる
	void AddScore(int32 Amount);

	// Widgetから呼ばれるパワーアップ適用関数
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	void ApplyUpgrade(int32 UpgradeType);	
};
