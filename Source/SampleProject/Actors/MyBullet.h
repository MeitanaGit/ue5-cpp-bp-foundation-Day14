// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyBullet.generated.h"

UCLASS()
class SAMPLEPROJECT_API AMyBullet : public AActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AMyBullet();
	
	// 当たり判定
	UPROPERTY(VisibleAnywhere, Category = "Custom|Bullet")
	class USphereComponent* CollisionComponent;

	// 移動制御（まっすぐ飛ばすコンポーネント）
	UPROPERTY(VisibleAnywhere, Category = "Custom|Bullet")
	class UProjectileMovementComponent* BulletMovement;

	// 見た目
	UPROPERTY(VisibleAnywhere, Category = "Custom|Bullet")
	class UStaticMeshComponent* BulletMesh;

	// 弾を再利用可能にするための初期化
	void ActivateBullet(FVector Location, FRotator Rotation);

	// 弾をプールに戻す（非表示・衝突無効化）
	void DeactivateBullet();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 衝突時に呼ばれる関数（この形式で宣言する必要があります）
	// 当たった時の命令をこれにする
	UFUNCTION()
	void OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
						 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						 bool bFromSweep, const FHitResult& SweepResult);
	
	// 反射できる残り回数
	// 反射できる初期化用残り回数
	UPROPERTY(EditAnywhere, Category = "Custom|Bullet")
	int32 BounceCount = 0;	

	// BPの「Bounce Count」の初期値を保存しておく変数
	UPROPERTY(EditAnywhere, Category = "Custom|Bullet")
	int32 MaxBounceCount = 0;
	
	// 弾の生存時間計算
	FTimerHandle DeactivateTimerHandle;
	float LifeTime = 3.0f;

private:

	
	// 生存判定用
	bool bIsActive = false;
	
	// 直前にダメージを与えた敵を記憶する
	UPROPERTY()
	AActor* LastHitEnemy = nullptr;
public:	
	bool GetIsActive();
	// 弾が持つダメージ量
	float DamageValue = 1.0f;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
