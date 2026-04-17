// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyDamagePopUp.generated.h"

UCLASS()
class SAMPLEPROJECT_API AMyDamagePopUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyDamagePopUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 敵から受け取るダメージ量。Blueprintから読み取れるように設定
	UPROPERTY(BlueprintReadOnly, Category = "Custom|MyDamage")
	float DamageAmount;
	// Blueprint側で中身を書き込めるイベント（関数）を作る
	UFUNCTION(BlueprintImplementableEvent, Category = "Custom|MyDamage")
	void OnDamageAmountReceived();
	
	// --- 追加：プールに戻すための処理 ---
	UFUNCTION(BlueprintCallable, Category = "Custom|MyPool")
	void DeactivatePopUp();
};
