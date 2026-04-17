// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDamagePopUp.h"

// Sets default values
AMyDamagePopUp::AMyDamagePopUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 生成時はまず止めておく
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void AMyDamagePopUp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyDamagePopUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ダメージ表示が終わったらこれを呼ぶこと！
void AMyDamagePopUp::DeactivatePopUp()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
    
	// 必要であれば、ここで座標をリセットしたり速度を0にしたりします
}
