// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"

void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (EnemySpawnerClass)
	{
		// ゲーム開始時に敵スポナーを作成する
		GetWorld()->SpawnActor<AActor>(EnemySpawnerClass, FVector::ZeroVector, FRotator::ZeroRotator);
	}
	// PopUpManagerの生成（追加）
	if (PopUpManagerClass)
	{
		PopUpManagerInstance = GetWorld()->SpawnActor<AMyPopUpManager>(PopUpManagerClass);
	}
	
	if (OrbManagerClass)
	{
		// レベル開始時にオーブマネージャーを1つだけ生成する
		OrbManagerInstance = GetWorld()->SpawnActor<AMyOrbManager>(OrbManagerClass);
	}
}