// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyExpOrb.generated.h"

UCLASS()
class SAMPLEPROJECT_API AMyExpOrb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyExpOrb();
	// プーリング対応
	void ActivateOrb(FVector Location);
	void DeactivateOrb();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 衝突判定（これに触れたら取得）
	UPROPERTY(VisibleAnywhere, Category = "Custom|Components")
	class USphereComponent* CollisionComponent;

	// 見た目
	UPROPERTY(VisibleAnywhere, Category = "Custom|Components")
	class UStaticMeshComponent* OrbMesh;

	// スコア(経験値)
	UPROPERTY(EditAnywhere, Category = "Custom|Orb Stats")
	int32 Score = 10;

	int32 MaxScore = 10;

	// 吸引が開始される距離
	UPROPERTY(EditAnywhere, Category = "Custom|Orb Stats")
	float MagnetRange = 100.0f;

	// 吸引スピード
	UPROPERTY(EditAnywhere, Category = "Custom|Orb Stats")
	float MoveSpeed = 200.0f;
	float MaxSpeed = 0.0f; //保持して後から渡す用

	// プレイヤーへの参照
	UPROPERTY()
	class APawn* TargetPlayer;

	// 衝突時のイベント関数
	UFUNCTION()
	void OnOrbOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	bool bIsActive = false; // 明示的な生存フラグ
};
