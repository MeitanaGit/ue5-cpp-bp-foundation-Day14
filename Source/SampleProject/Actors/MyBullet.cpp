// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBullet.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyEnemy.h" // 敵クラスのヘッダーが必要
#include "NiagaraComponent.h" // これが必要
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AMyBullet::AMyBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 当たり判定の設定
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(15.0f);
	RootComponent = CollisionComponent;
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	
	
	// 2. メッシュコンポーネントを生成して CollisionComponent に取り付ける
	// ★ここが抜けている可能性があります！
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(RootComponent);
	
	// 3. 移動の設定（初速や重力の影響）
	BulletMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("BulletMovement"));
	BulletMovement->UpdatedComponent = CollisionComponent;
	BulletMovement->InitialSpeed = 3000.f;  // 弾速
	BulletMovement->MaxSpeed = 3000.f;
	BulletMovement->bRotationFollowsVelocity = true;
	BulletMovement->ProjectileGravityScale = 0.f; // 重力を無視（見下ろしなので）

	// シミュレーション中に Hit イベントを生成するように設定
	CollisionComponent->SetNotifyRigidBodyCollision(true); 

	// ... ProjectileMovementの設定 ...
	BulletMovement->bShouldBounce = true;
	BulletMovement->Bounciness = 1.0f;
	BulletMovement->Friction = 0.0f;
    
	// 反射時に速度が落ちないようにする設定
	BulletMovement->BounceVelocityStopSimulatingThreshold = 0.f;

	// デフォルトの反射回数を0（通常弾）にしておきます
	BounceCount = 0;

	// 4. 数秒後に自動消滅
	InitialLifeSpan = 0;
	bIsActive = false;
	
	// 念のため初期化
	LastHitEnemy = nullptr;
}

// Called when the game starts or when spawned
void AMyBullet::BeginPlay()
{
	Super::BeginPlay();

	// ゲーム開始時の反射回数を「最大値」として記憶しておく
	MaxBounceCount = BounceCount;
	
	// CollisionComponent（Sphere）を取得してイベントをバインド
	// ※C++で作成した衝突用コンポーネント変数名に合わせてください
	if (CollisionComponent)
	{
		// Hit ではなく Overlap をバインドする
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMyBullet::OnBulletOverlap);
	}
}

// Called every frame
void AMyBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bIsActive) return;

}
// この弾は有効？
bool AMyBullet::GetIsActive()
{
	return bIsActive;
}
// 敵と弾が重なったときに呼ばれる
void AMyBullet::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
								bool bFromSweep, const FHitResult& SweepResult)
{
	if(!bIsActive || !OtherActor || OtherActor == this) return;

	// 敵かどうか判定
	if (OtherActor->IsA(AMyEnemy::StaticClass()) && OtherActor != LastHitEnemy)
	{
		LastHitEnemy = OtherActor;
		UGameplayStatics::ApplyDamage(OtherActor, DamageValue, GetInstigatorController(), this, UDamageType::StaticClass());

		if (BounceCount > 0)
		{
			BounceCount--;
			// 【重要】Overlap設定だと物理で勝手に跳ね返らないので、
			// 反射させたい場合はここで速度ベクトルを手動で反転させる必要があります
			BulletMovement->Velocity = -BulletMovement->Velocity; 
		}
		else
		{
			DeactivateBullet();
		}
	}
}

void AMyBullet::ActivateBullet(FVector Location, FRotator Rotation)
{
	bIsActive = true; // 有効化
	LastHitEnemy = nullptr; // 記憶をリセット

	
	// --- 1. 反射回数をリセット ---
	BounceCount = MaxBounceCount;

	// --- 2. 物理挙動を完全にリセット ---
	// これをしないと、前回の反射時のベクトルが残って変な方向に飛ぶことがあります
	BulletMovement->StopMovementImmediately();
	
	SetActorLocationAndRotation(Location, Rotation);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	// --- Niagaraコンポーネントを探してリセットする ---
	TArray<UNiagaraComponent*> NiagaraComponents;
	GetComponents<UNiagaraComponent>(NiagaraComponents); // このActorに付いているNiagaraを全部取得
	for (UNiagaraComponent* NC : NiagaraComponents)
	{
		if (NC)
		{
			NC->Activate(true); // true でリセットして再開
		}
	}
	
	// 物理移動を再開
	BulletMovement->UpdatedComponent = RootComponent;
	BulletMovement->Velocity = Rotation.Vector() * BulletMovement->InitialSpeed;
	BulletMovement->Activate();

	// 寿命が来たら消す代わりに DeactivateBullet を呼ぶ
	GetWorldTimerManager().SetTimer(DeactivateTimerHandle, this, &AMyBullet::DeactivateBullet, LifeTime, false);
}

void AMyBullet::DeactivateBullet() {
	GetWorldTimerManager().ClearTimer(DeactivateTimerHandle);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	bIsActive = false; // 無効化
	LastHitEnemy = nullptr; // 記憶をリセット
	
	// 移動を停止
	BulletMovement->Deactivate();
	BulletMovement->StopMovementImmediately();

	TArray<UNiagaraComponent*> NiagaraComponents;
	GetComponents<UNiagaraComponent>(NiagaraComponents);
	for (UNiagaraComponent* NC : NiagaraComponents)
	{
		if (NC)
		{
			// NC->Deactivate(); // または NC->DeactivateImmediate();
			//これで消しておかないとエフェクトがワープする
			NC->DeactivateImmediate();
		}
	}

}

