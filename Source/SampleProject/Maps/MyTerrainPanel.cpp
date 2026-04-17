// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTerrainPanel.h"
#include "PaperTileMapComponent.h"
#include "PaperTileMap.h"
#include "PaperTileLayer.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyTerrainPanel::AMyTerrainPanel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ルートコンポーネントを作成
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// 1. 床メッシュの設定
	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
	FloorMesh->SetupAttachment(Root);
	// ここで Visible を切っておけば、ゲーム中は見えなくなります
	FloorMesh->SetHiddenInGame(true);

	// 2. タイルマップの設定
	TileMapComponent = CreateDefaultSubobject<UPaperTileMapComponent>(TEXT("TileMapComponent"));
	TileMapComponent->SetupAttachment(Root);
	// タイルマップ側のコリジョンは「No Collision」に設定することを忘れずに！
	TileMapComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AMyTerrainPanel::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);	

	// タイルマップのZ位置を補正する
	AdjustTileMapZOffset();
}

// Called every frame
void AMyTerrainPanel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerPawn) return;

	FVector PlayerLoc = PlayerPawn->GetActorLocation();
	FVector MyLoc = GetActorLocation();

	// 3枚並んでいるうち、1.5枚分（半分以上）離れたら移動する判定
	float Threshold = PanelSize * 1.5f;
	FVector NewLoc = MyLoc;

	// X軸のループ
	if (PlayerLoc.X - MyLoc.X > Threshold)      NewLoc.X += PanelSize * 3.0f;
	else if (MyLoc.X - PlayerLoc.X > Threshold) NewLoc.X -= PanelSize * 3.0f;

	// Y軸のループ
	if (PlayerLoc.Y - MyLoc.Y > Threshold)      NewLoc.Y += PanelSize * 3.0f;
	else if (MyLoc.Y - PlayerLoc.Y > Threshold) NewLoc.Y -= PanelSize * 3.0f;

	if (NewLoc != MyLoc)
	{
		SetActorLocation(NewLoc);
	}
}

// レイヤーの層を見てタイルマップを補正する
void AMyTerrainPanel::AdjustTileMapZOffset()
{
	if (!TileMapComponent) return;

	// エンジン側のメンバ変数（Layers等）を一切使わず、
	// 自分で定義した TileLayerCount と TileLayerSeparation だけで計算する
	if (TileLayerCount > 0)
	{
		// 最上段（コリジョン層）を Z=0 にするためのオフセット計算
		float ZOffset = (TileLayerCount - 1) * TileLayerSeparation;

		FVector RelativeLoc = TileMapComponent->GetRelativeLocation();
		RelativeLoc.Z = ZOffset;
        
		TileMapComponent->SetRelativeLocation(RelativeLoc);
        
		UE_LOG(LogTemp, Log, TEXT("TileMap Adjusted: Z=%f (Layers: %d)"), ZOffset, TileLayerCount);
	}
}
