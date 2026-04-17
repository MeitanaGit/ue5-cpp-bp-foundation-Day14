// Fill out your copyright notice in the Description page of Project Settings.


#include "MyExpOrb.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerCharacter.h"

// Sets default values
AMyExpOrb::AMyExpOrb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 衝突判定の設定
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetSphereRadius(50.0f);
	// 最初はコリジョンをオフにしておく
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// メッシュの設定
	OrbMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrbMesh"));
	OrbMesh->SetupAttachment(RootComponent);
	OrbMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // メッシュは当たり判定なし
	bIsActive = false; // 最初はオフ
}

// Called when the game starts or when spawned
void AMyExpOrb::BeginPlay()
{
	Super::BeginPlay();
	MaxScore = Score;
	// プレイヤーを取得しておく
	TargetPlayer = UGameplayStatics::GetPlayerPawn(this, 0);

	// 保持するために初期速度を渡す
	MaxSpeed = MoveSpeed;

	// オーバーラップイベントをバインド
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMyExpOrb::OnOrbOverlap);
}

// Called every frame
void AMyExpOrb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// まず生存しているかどうかの確認
	if (!bIsActive) return;
	
	// 追加：待機中は計算しない
	//	if (IsHidden()) return;  透明にしたいだけの場合もある可能性があるので一応やらない
	
	
	if (TargetPlayer)
	{
		// プレイヤーとの距離を計算
		float Distance = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());

		if (Distance < 10.0f) // 衝突判定より少し手前で回収
		{
			OnOrbOverlap(nullptr, TargetPlayer, nullptr, 0, false, FHitResult());
			return;
		}
		// 磁石の範囲内に入ったら吸い寄せられる
		else if (Distance <= MagnetRange)
		{
			FVector Direction = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            
			// プレイヤーに向かって移動（フレームレートに依存しないようDeltaTimeを掛ける）
			FVector NewLocation = GetActorLocation() + (Direction * MoveSpeed * DeltaTime);
			SetActorLocation(NewLocation);

			// 距離が非常に近くなったら加速させるなどの演出を入れるとよりヴァンサバっぽくなります
			MoveSpeed += 500.0f * DeltaTime; 
		}
	}
}

void AMyExpOrb::OnOrbOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// まず生存しているかどうかの確認
	if (!bIsActive) return;
	// 追加：オーブが非表示（プール待機中）なら何もしない
//	if (IsHidden()) return;  透明にしたいだけの場合もある可能性があるので一応やらない
	
	// プレイヤーに触れたかチェック
	if (OtherActor && OtherActor == TargetPlayer)
	{
		// TargetPlayerをAMyPlayerCharacter型にキャスト
		AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor);
        
		if (Player)
		{
			//ポイント加算してみる
			Player->AddScore(Score);
		}

		DeactivateOrb(); // Destroy() ではなく非アクティブ化
	}
}

void AMyExpOrb::ActivateOrb(FVector Location)
{
	// 保険処理　環境や負荷でまれにプレイヤーが出る前に動くリスクを考慮
	if (!TargetPlayer) return;

	bIsActive = true; // 有効
	SetActorLocation(Location);
	SetActorHiddenInGame(false);

	// --- 修正ポイント ---
	// 特定の計算フラグではなく、コリジョン設定そのものを戻すのが正解です
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Overlap); 
	// --------------------
		
	SetActorTickEnabled(true);
	// 保持するために初期速度を渡す
	MoveSpeed = MaxSpeed; // スピードを初期値に戻す
	// スコアも戻す
	Score = MaxScore;
}

void AMyExpOrb::DeactivateOrb()
{
	bIsActive = false; // 無効
	SetActorHiddenInGame(true);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorTickEnabled(false);
}

