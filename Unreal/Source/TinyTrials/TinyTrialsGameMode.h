// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HttpModule.h"
#include "TinyTrialsGameMode.generated.h"

class FJsonObject;
class ATinyTrialsPlayerController;

USTRUCT(BlueprintType)
struct FHttpProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString BaseUrl = TEXT("http://127.0.0.1:8000");

    // e.g., "/register-or-login"
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Endpoint = TEXT("/register-or-login");

    // "GET", "POST", ...
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Verb = TEXT("POST");

    // Almost always JSON for your API
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ContentType = TEXT("application/json");

    // Optional: extra headers (e.g., Authorization)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> Headers;

    // Timeouts (seconds)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TimeoutSeconds = 10;

    // Simple retry policy (0 = no retries)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxRetries = 0;

    // Backoff between retries (seconds)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RetryBackoffSeconds = 0.5f;

    // Convenience: builds full URL
    FString Url() const
    {
        if (Endpoint.StartsWith(TEXT("/")))
            return BaseUrl + Endpoint;
        return BaseUrl + TEXT("/") + Endpoint;
    }
};

UCLASS(minimalapi)
class ATinyTrialsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATinyTrialsGameMode();

protected:
	virtual void PostLogin(APlayerController* NewLogin) override;

private:
	void ProcessLogin(ATinyTrialsPlayerController* NewLogin);
	void ManageLoginQueue();
	UPROPERTY()
	TArray<TWeakObjectPtr<ATinyTrialsPlayerController>> ProcessedControllersQueue;
	TSharedRef<FJsonObject> GenerateJsonObj(APlayerController* Controller) const;
	void BuildHttpRequest(TSharedRef<class IHttpRequest, ESPMode::ThreadSafe> Req, const FString& Body);
	void OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded);

	void PutPlayerInPendingAuthState(ATinyTrialsPlayerController* PendingController);
	void InitializeAuthorizedPlayer(ATinyTrialsPlayerController* AuthController, const FString& ID, const FString& Token);
};



