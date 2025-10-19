// Copyright Epic Games, Inc. All Rights Reserved.


#include "TinyTrialsPlayerController.h"
#include "TinyTrialsGameMode.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "TinyTrialsPlayerState.h"

void ATinyTrialsPlayerController::Server_RequestJoinSession_Implementation()
{
    if (auto GM = GetWorld()->GetAuthGameMode<ATinyTrialsGameMode>())
    {
        GM->RequestJoinSession(this);
    }
}

FString ATinyTrialsPlayerController::GetPlayerDisplayName() const
{
    if (ATinyTrialsPlayerState* PS = GetPlayerState<ATinyTrialsPlayerState>())
    {
        return PS->MyProfile.DisplayName;
    }
    return TEXT("");
}

const FConnectedPlayerInfo& ATinyTrialsPlayerController::GetPlayerProfile() const
{
    return GetPlayerState<ATinyTrialsPlayerState>()->MyProfile;
}

void ATinyTrialsPlayerController::Client_OnStartJoin_Implementation()
{
    BP_ShowLoggingInMsg();
}

void ATinyTrialsPlayerController::OnPlayerAuthorised(const FString& PlayerID, const FString& Jwt)
{
    if (ATinyTrialsPlayerState* PS = GetPlayerState<ATinyTrialsPlayerState>())
    {
        PS->Server_ReceiveServerAuthorisation(PlayerID, Jwt);
        // InitNewGame();
    }
}

void ATinyTrialsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

void ATinyTrialsPlayerController::Server_SubmitDisplayName_Implementation(const FString& InDisplayName, ATinyTrialsPlayerController* INRef)
{
    if (ATinyTrialsPlayerState* PS = GetPlayerState<ATinyTrialsPlayerState>())
    {
        PS->Server_SetUsernameAndController(InDisplayName, INRef);
    }
}
