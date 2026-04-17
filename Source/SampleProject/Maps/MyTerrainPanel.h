// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperTileMapComponent.h" // TileMap用
#include "PaperTileMap.h" // これを追加（レイヤー情報にアクセスするため）
#include "MyTerrainPanel.generated.h"

UCLASS()
class SAMPLEPROJECT_API AMyTerrainPanel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyTerrainPanel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// レイヤー数に基づいて位置を補正する関数を追加しておくと管理が楽です
	void AdjustTileMapZOffset();
	
	// 衝突判定用の透明な床
	UPROPERTY(VisibleAnywhere, Category = "Custom|Terrain")
	class UStaticMeshComponent* FloorMesh;

	// 見た目用のタイルマップ
	UPROPERTY(VisibleAnywhere, Category = "Custom|Terrain")
	class UPaperTileMapComponent* TileMapComponent;

	// ループ計算用のパラメータ
	UPROPERTY(EditAnywhere, Category = "Custom|Terrain")
	float PanelSize = 2000.0f; // 1枚のパネルの1辺の長さ
	
	
	UPROPERTY(EditAnywhere, Category = "Custom|Terrain")
	int32 TileLayerCount = 8; // Tiledのレイヤー数（デフォルト8）

	UPROPERTY(EditAnywhere, Category = "Custom|Terrain")
	float TileLayerSeparation = 2.0f; // レイヤー間の間隔
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	APawn* PlayerPawn;
};
