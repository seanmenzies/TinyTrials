// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerProfile.h"
#include "TinyTrialsPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class TINYTRIALS_API ATinyTrialsPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RequestJoinSession();

	FString GetPlayerDisplayName() const;
	const FConnectedPlayerInfo& GetPlayerProfile() const;
	void OnStartJoin()
	{
		Client_OnStartJoin();
	};
	UFUNCTION(Client, Unreliable)
	void Client_OnStartJoin();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_ShowLoggingInMsg();
	void OnPlayerAuthorised(const FString& PlayerID, const FString& Jwt);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;
	virtual void BeginPlay() override;
    UFUNCTION(Server, Reliable)
    void Server_SubmitDisplayName(const FString& InDisplayName, class ATinyTrialsPlayerController* INRef);
};
