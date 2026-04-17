// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyDamagePopUp.h" // これを追加
#include "MyEnemy.generated.h"

// --- ヘッダーの上部に追加 ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDefeated, AMyEnemy*, DefeatedEnemy);

UCLASS()
class SAMPLEPROJECT_API AMyEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyEnemy();

	// 死亡時に実行されるイベント
	UPROPERTY(BlueprintAssignable, Category = "Enemy")
	FOnEnemyDefeated OnEnemyDefeated;
public:

	// 敵の状態を返す
	bool GetIsActive();
	bool GetIsDead();
	
	// 移動力を敵に設定する
	void SetMoveSpeed(float NewSpeed);
	
	// 敵を起動する（位置設定・HPリセット）
	void ActivateEnemy(FVector Location);

	// 敵を非アクティブにする（プールへ戻る準備）
	void DeactivateEnemy();	
private:	
	// 初期HPを保存しておく（再利用時に戻すため）
	float MaxHealth;

	// プレイヤーの情報参照します
	APawn* PlayerPawn;

	bool bIsDead = true;	// 死亡判定
	bool bIsActive = false;	// 画面外などにも対応
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 接触時に呼ばれる関数
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);	
	// 敵のHP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|MyStats")
	float Health = 10.0f;

	// 敵の攻撃力
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|MyStats")
	float AttackPower = 10.0f;

	// エンジン標準のダメージ受け取り関数をオーバーライド
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;	
	
	// 移動スピード（CharacterMovementコンポーネント経由でも調整可能）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|MyStats")
	float MoveSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|MyEffects")
	class UNiagaraSystem* DeathEffect; // Niagaraの場合

	// 撃破時パーティクル絡みのステータスBPで敵ごとに調整できるように UPROPERTY にしておく
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Effects")
	FVector DeathEffectOffset = FVector(0.f, 0.f, -40.f);
	
	// ダメージ表示、BPで敵ごとに調整できるように UPROPERTY にしておく
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|PopUp")
	FVector DamagePopOffset = FVector(0.f, 0.f, -250.f);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
