// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "PlayerProfile.generated.h"

USTRUCT(BlueprintType)
struct FConnectedPlayerInfo
{
    GENERATED_BODY()

    UPROPERTY()
    TWeakObjectPtr<class ATinyTrialsPlayerController> PlayerRef;

    UPROPERTY(BlueprintReadOnly)
    FString PlayerId; // UUID from backend, string is fine

    UPROPERTY(BlueprintReadOnly)
    FString DisplayName; // user-entered name

    UPROPERTY(BlueprintReadOnly)
    int32 PlayerIndex = -1; // optional: Local or Net index for UI sorting
};
