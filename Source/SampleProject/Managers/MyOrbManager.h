// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyExpOrb.h"
#include "MyOrbManager.generated.h"

UCLASS()
class SAMPLEPROJECT_API AMyOrbManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyOrbManager();
	// 敵から呼ばれる関数
	void SpawnOrbFromPool(FVector Location);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Custom|Orb")
	TSubclassOf<AMyExpOrb> OrbClass;

	UPROPERTY(EditAnywhere, Category = "Custom|Orb")
	int32 PoolSize = 100;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	TArray<AMyExpOrb*> OrbPool;

	FTimerHandle IncrementallySpawnTimerHandle;
	int32 CurrentSpawningCount = 0;
	void IncrementallySpawnOrb();
};
