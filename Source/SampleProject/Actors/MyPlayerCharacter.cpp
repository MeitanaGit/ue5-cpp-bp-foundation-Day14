// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputTriggers.h" // これが ETriggerEvent の正体
#include "GameFramework/CharacterMovementComponent.h" // これが必要
// 冒頭に弾丸クラスのヘッダーを追加
#include "MyUserWidget.h" // 作成したWidgetのヘッダー
#include "Kismet/GameplayStatics.h"
#include "MyGameHUD.h"
#include "MyGameTypes.h"
#include "Blueprint/UserWidget.h"
#include "BulletManagerComponent.h" // 追加

// Sets default values
AMyPlayerCharacter::AMyPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// 今は使ってないのでTickを無しに
	PrimaryActorTick.bCanEverTick = false;

	// キャラクターの回転に合わせてカメラが回らないように設定
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 2. 移動方向にキャラクターを自動で向かせる設定
	GetCharacterMovement()->bOrientRotationToMovement = false; 
    
	// 3. 回転の速さを設定（数値が大きいほどクイックに振り向く）
	GetCharacterMovement()->RotationRate = FRotator(0.f, 600.f, 0.f);
	
	// 床との接触判定をより滑らかにする設定（コンストラクタへ）
	GetCharacterMovement()->MaxStepHeight = 45.0f; // 階段などを登る高さ
	GetCharacterMovement()->bMaintainHorizontalGroundVelocity = true;
	GetCharacterMovement()->bUseSeparateBrakingFriction = true; // 追加
	// 床への押し付けを少し弱める、または安定させる
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;	

	InitCamera();
	
	// コンポーネントの作成
	BulletManager = CreateDefaultSubobject<UBulletManagerComponent>(TEXT("BulletManager"));
	// ステータスコンポーネントの作成
	StatusComponent = CreateDefaultSubobject<UMyStatusComponent>(TEXT("StatusComponent"));
}

// Called when the game starts or when spawned
void AMyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	InitController();
	
	// UIを生成して画面に表示（Viewportへ追加）
	if (HUDClass)
	{
		HUDInstance = CreateWidget<UMyUserWidget>(GetWorld(), HUDClass);
		if (HUDInstance)
		{
			HUDInstance->AddToViewport();
			HUDInstance->UpdateScoreDisplay(StatusComponent->CurrentScore); // 初期値0を表示
			// 体力バーのセット
			HUDInstance->UpdateHealthDisplay(StatusComponent->GetCurrentHealth(), StatusComponent->GetMaxHealth());
		}
	}
	// 開始後すぐに一発撃てるようにしておく。
	LastFireTime = -StatusComponent->FireRate;
}
//---------------------------------------------
// ゲーム遷移時にデフォルトのカメラ設定を与える
void AMyPlayerCharacter::InitCamera()
{
	// アーム（CameraBoom）を作成
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;       // キャラとの距離
	CameraBoom->bEnableCameraLag = false;      // ラグを無効化（まずはこれでブレが止まるか確認）
	CameraBoom->bEnableCameraRotationLag = false;
	// もしラグを使いたい場合は、以下の数値を大きく（10.0以上）すると安定します
	// CameraBoom->CameraLagSpeed = 15.0f;

	// カメラアームを完全に独立させる
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	// その代わり、アーム自体の角度を「固定の斜め見下ろし」に設定する
	// (X:0, Y:-45, Z:0) など、好みの角度に調整してください
	CameraBoom->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));	
	
	//　衝突無効処理追加
	CameraBoom->bDoCollisionTest = false; // カメラの衝突判定を無効にする
	
	// カメラを作成し、アームの先端に取り付ける
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

}


//---------------------------------------------
// ゲーム遷移時にコントローラーに操作情報を与える
void AMyPlayerCharacter::InitController()
{
	// --- 【追記】入力モードのリセット ---
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// 1. 入力モードをゲーム専用に戻す
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);

		// 2. マウスカーソルを非表示にする
		PC->bShowMouseCursor = false;
	}
// ----------------------------------

	// プレイヤーコントローラーを取得して、Enhanced Inputのサブシステムに今回の設定(MappingContext)を登録
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// DefaultMappingContext を優先度0で追加
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//---------------------------------------------
// Called every frame
void AMyPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 将来的に毎フレーム処理が必要になった場合ここに追加
	// 現在は bCanEverTick = false のため呼ばれない
}

//---------------------------------------------
// Called to bind functionality to input
void AMyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		// 移動周り
		EnhancedInputComponent->BindAction(
			MoveAction, ETriggerEvent::Triggered, this, &AMyPlayerCharacter::Move);
		// FireAction（ボタン）が押された(Started)時に Fire() を呼ぶ
		EnhancedInputComponent->BindAction(
			FireAction, ETriggerEvent::Started, this, &AMyPlayerCharacter::Fire);
		// SwitchBulletAction（ボタン）が押された(Started)時に SwitchBullet() を呼ぶ
		EnhancedInputComponent->BindAction(SwitchBulletAction,
			ETriggerEvent::Started, this, &AMyPlayerCharacter::SwitchBullet);
		// ポーズ処理
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AMyPlayerCharacter::Pause);
	}
}
//---------------------------------------------
// 実際の移動処理（前述のワールド座標基準）
void AMyPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{

		// カメラやコントローラーの向きを無視して、絶対的な「北・東」を基準に動かす
		// これにより、カメラ設定を変えても移動が逆転しません
		AddMovementInput(FVector::ForwardVector, MovementVector.Y); // Y入力でX軸(前)に移動
		AddMovementInput(FVector::RightVector, MovementVector.X);   // X入力でY軸(右)に移動
		
		// 入力ベクトルを作成
		FVector MoveDir = FVector(MovementVector.Y, MovementVector.X, 0.f);
       
		// 入力がある時だけ、向きを更新（Normalizeして長さを1にする）
		if (!MoveDir.IsNearlyZero())
		{
			LastMovementInput = MoveDir.GetSafeNormal();
		}
	}
}
//---------------------------------------------
// 発射処理の実装
void AMyPlayerCharacter::Fire(const FInputActionValue& Value)
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastFireTime >= StatusComponent->FireRate)
	{
		if (BulletManager)
		{
			// 位置と回転を計算
			FVector SpawnLocation = GetActorLocation() + (LastMovementInput * 10.0f);
			FRotator SpawnRotation = LastMovementInput.Rotation();

			// コンポーネントに依頼するだけ！
			BulletManager->RequestBullet(SelectedBulletIndex, SpawnLocation, SpawnRotation, StatusComponent->BaseDamage);
			LastFireTime = CurrentTime;
		}
	}
}

//---------------------------------------------
void AMyPlayerCharacter::SwitchBullet()
{
	// 例外処理
	if (SelectedBulletIndex < 0)
	{
		SelectedBulletIndex = 0;
	}
	if (BulletManager)
	{
		int32 MaxCount = BulletManager->GetBulletClassCount();
		if (MaxCount > 0)
		{
			SelectedBulletIndex = (SelectedBulletIndex + 1) % MaxCount;
			UE_LOG(LogTemp, Warning, TEXT("Bullet Index: %d"), SelectedBulletIndex);
		}
	}
}
//---------------------------------------------
// コンポーネントに任せるスコア加算処理
void AMyPlayerCharacter::AddScore(int32 Amount)
{
	if (!StatusComponent) return;

	// 1. スコア加算とレベルアップ判定をコンポーネントに任せる
	if (StatusComponent->AddScore(Amount))
	{
		LevelUp();
	}

	// 2. UI更新（表示用データもコンポーネントから取る）
	if (HUDInstance)
	{
		HUDInstance->UpdateScoreDisplay(StatusComponent->CurrentScore);
	}
}

//---------------------------------------------
void AMyPlayerCharacter::LevelUp() 
{
	// 1. 抽選もコンポーネントに任せる
	TArray<FUpgradeData> SelectedOptions = StatusComponent->GenerateUpgradeOptions();

	// 2. HUD表示（ここはPlayerの窓口としての仕事）
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		if (AMyGameHUD* HUD = Cast<AMyGameHUD>(PC->GetHUD()))
		{
			HUD->ShowGenericMenu(SelectedOptions, FText::FromString(TEXT("レベルアップ！")), EMenuScreenState::LevelUp);
		}
	}

	// 3. 演出（UI・SE）
	if (HUDInstance)
	{
		HUDInstance->UpdateLevelDisplay(StatusComponent->CurrentLevel);
		HUDInstance->PlayLevelUpAnimation();
	}
	if (LevelUpSound)
	{
		UGameplayStatics::PlaySound2D(this, LevelUpSound);
	}
}


//---------------------------------------------
//強化項目を選ぶ
void AMyPlayerCharacter::ApplyUpgrade(int32 UpgradeType)
{
	EMenuActionType Action = static_cast<EMenuActionType>(UpgradeType);
	// レジューム時に使う
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	// ウィジェットにリクエストを出すのでHUDを一度だけ取得してキャストしておく
	AMyGameHUD* HUD = Cast<AMyGameHUD>(PC->GetHUD());	
	
	switch (Action)
	{
		// ゲームに戻るよ
	case EMenuActionType::System_Resume :
		// ポーズ解除
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		InitController();
		break;
		//リトライ
	case EMenuActionType::System_Retry :
		UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
		return;
		//終了
	case EMenuActionType::System_Quit :
		UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
		return;
		//タイトルに戻る
	case EMenuActionType::System_ReturnTitle :
		UGameplayStatics::OpenLevel(this, FName(TEXT("TitleMap")));
		return;
	default :
		// 該当しないアクション（デバッグ用） ステータスアップでもこっちに入るよ
		UE_LOG(LogTemp, Warning, TEXT("Unknown ActionType: %d"), Action);
		break;
		
	}

	// 2. ステータスアップならコンポーネントに丸投げ
	if (StatusComponent) {
		StatusComponent->ApplyUpgrade(Action);
	}
	// 体力バーのセット
	HUDInstance->UpdateHealthDisplay(StatusComponent->GetCurrentHealth(), StatusComponent->GetMaxHealth());

	// 親ウィンドウは閉じる
	if (HUD) HUD->CloseActiveMenu();
}

//---------------------------------------------
// ポーズ処理
void AMyPlayerCharacter::Pause()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		AMyGameHUD* HUD = Cast<AMyGameHUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->ShowPauseMenu();
		}
	}
}
//---------------------------------------------
// ゲームオーバー処理
void AMyPlayerCharacter::GameOver()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		AMyGameHUD* HUD = Cast<AMyGameHUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->ShowGameOverMenu();
		}
	}
}


// ダメージを受ける処理
float AMyPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, 
									 AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (StatusComponent)
	{
		// StatusComponentにダメージを伝える
		StatusComponent->ReduceHealth(ActualDamage);

		// UIを更新（HPバーなどがある場合）
		if (HUDInstance)
		{
			HUDInstance->UpdateHealthDisplay(StatusComponent->GetCurrentHealth(), StatusComponent->GetMaxHealth());
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("HUDInstance is NULL!"));
		}

		// HPが0になったら死亡処理
		if (StatusComponent->GetCurrentHealth() <= 0.f)
		{
			// ゲームオーバーメニュー表示など
			GameOver();
		}
	}

	return ActualDamage;
}
