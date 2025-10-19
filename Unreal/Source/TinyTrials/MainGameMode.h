// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TINYTRIALS_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	virtual void InitGame(const FString& Map, const FString& Options, FString& Error) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	void InitPlayerAfterJoin(APlayerController* PC);
};
