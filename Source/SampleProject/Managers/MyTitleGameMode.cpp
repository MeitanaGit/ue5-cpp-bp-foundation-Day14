// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTitleGameMode.h"
#include "MyTitleHUD.h"

AMyTitleGameMode::AMyTitleGameMode()
{
	// プレイヤーが勝手にスポーンしないようにする
	DefaultPawnClass = nullptr;
    
	// HUDに作成した AMyTitleHUD を指定
//	HUDClass = AMyTitleHUD::StaticClass();

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}
