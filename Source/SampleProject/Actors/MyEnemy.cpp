// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemy.h"
#include "Engine/World.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

#include "Kismet/GameplayStatics.h"
#include "MyGameModeBase.h"
#include "MyOrbManager.h"
#include "MyPopUpManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h" // これを追加 (UCapsuleComponent用)
#include "MyPlayerCharacter.h"          // これを追加 (AMyPlayerCharacter用)

// Sets default values
AMyEnemy::AMyEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyEnemy::BeginPlay()
{
	Super::BeginPlay();
	MaxHealth = Health; // 初期値を保存	
	SetMoveSpeed(MoveSpeed);
	bIsDead = true;
	bIsActive = false;

	// 衝突イベントをバインド
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyEnemy::OnOverlapBegin);
	}
	
	// プレイヤー（Pawn）を取得
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
}
//---------------------------
// 敵のスピードを設定する
void AMyEnemy::SetMoveSpeed(float NewSpeed)
{
	// ACharacterが最初から持っているコンポーネントにアクセス
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->MaxWalkSpeed = NewSpeed;
	}
}
// Called every frame
void AMyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 敵が死んでいるか、アクティブじゃないなら必要ない
	if (bIsDead || !bIsActive) { return; }
    
	if (PlayerPawn)
	{
		// 1. プレイヤーへの方向ベクトルを計算
		FVector Direction = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        
		// 2. 移動入力を与える（Z軸方向は無視）
		Direction.Z = 0.0f;
		AddMovementInput(Direction, 1.0f);
	}
}

// 敵の状態を取得するのに使う
bool AMyEnemy::GetIsActive()
{
	return bIsActive;
}
bool AMyEnemy::GetIsDead()
{
	return bIsDead;
}


// Called to bind functionality to input
void AMyEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AMyEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 敵が死んでいるか、アクティブじゃないなら必要ない
	if (bIsDead || !bIsActive) { return ActualDamage; }

	
	Health -= ActualDamage;

	AMyGameModeBase* GM = Cast<AMyGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GM && GM->GetPopUpManager()) 
	{
		FVector PopLocation = GetActorLocation() + DamagePopOffset;		// ダメージ表示の座標補正を加える
		// こちらは数値表示の揺れを表す
		FVector RandomOffset = FVector(FMath::RandRange(-20, 20), FMath::RandRange(-20, 20), 100.f);
		GM->GetPopUpManager()->SpawnPopUpFromPool(PopLocation + RandomOffset, ActualDamage);
	}
	if (Health <= 0.f)
	{
		// デリゲートを呼ぶ（登録している人に通知する）
		OnEnemyDefeated.Broadcast(this);
		// --- ここから撃破演出の追加 ---

		// 1. エフェクト（爆発など）をスポーンさせる
		if (DeathEffect) // BPで設定したエフェクト
		{
			// Niagaraの場合
			FVector SpawnPos = GetActorLocation() + DeathEffectOffset;
			
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			DeathEffect,
			SpawnPos,
			GetActorRotation());
            
			// レガシーなCascadeパーティクルの場合
			// UGameplayStatics::SpawnEmitterAtLocation(this, DeathEffectCascade, GetActorLocation(), GetActorRotation());
		}

		// レベル内のマネージャーを検索（もっと効率化も可能ですが、まずはこれが確実）
		AActor* ManagerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AMyOrbManager::StaticClass());
		AMyOrbManager* OrbManager = Cast<AMyOrbManager>(ManagerActor);

		// 敵が死ぬ時の処理
		if (GM && GM->GetOrbManager())
		{
			GM->GetOrbManager()->SpawnOrbFromPool(GetActorLocation());
		}
		DeactivateEnemy(); // プールへ戻す
	}

	return ActualDamage;
}
void AMyEnemy::ActivateEnemy(FVector Location)
{
	SetActorLocation(Location);
	Health = MaxHealth; // HPリセット
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
	bIsDead = false;
	bIsActive = true;
	SetMoveSpeed(MoveSpeed);
}

void AMyEnemy::DeactivateEnemy()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	PrimaryActorTick.bCanEverTick = false;
	SetActorTickEnabled(false);
	bIsDead = true;
	bIsActive = false;
}
void AMyEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
							  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
							  bool bFromSweep, const FHitResult& SweepResult)
{
	// 敵が死んでいる、またはアクティブでないなら無視
	if (bIsDead || !bIsActive) return;

	// 相手がプレイヤーキャラクターかチェック
	if (OtherActor && OtherActor->IsA(AMyPlayerCharacter::StaticClass()))
	{
		// プレイヤーにダメージを与える
		UGameplayStatics::ApplyDamage(OtherActor, AttackPower, GetController(), this, UDamageType::StaticClass());
        
		// 必要であれば、ここで「一度当たったら少しの間当たらない」などのノックバックや無敵時間の処理を挟む
	}
}
