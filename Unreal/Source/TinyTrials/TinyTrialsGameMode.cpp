// Copyright Epic Games, Inc. All Rights Reserved.

#include "TinyTrialsGameMode.h"
#include "TinyTrialsCharacter.h"
#include "TinyTrialsPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

ATinyTrialsGameMode::ATinyTrialsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

void ATinyTrialsGameMode::PostLogin(APlayerController* NewLogin)
{
	Super::PostLogin(NewLogin);

    auto C = Cast<ATinyTrialsPlayerController>(NewLogin);
    if (!C)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Game Mode] New log in not expected class."));
        return;
    }
    // keep a queue for new logins so we only process one at a time
    ProcessedControllersQueue.AddUnique(C);
    if (ProcessedControllersQueue.Num() <= 1) ProcessLogin(C);
}

void ATinyTrialsGameMode::ProcessLogin(ATinyTrialsPlayerController* NewLogin)
{
    // 1) Build the payload
    auto Json = GenerateJsonObj(NewLogin);
    FString Body;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
    FJsonSerializer::Serialize(Json, Writer);

    // 2) Create request
    auto& Http = FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = Http.CreateRequest();
    BuildHttpRequest(Req, Body);

    Req->OnProcessRequestComplete().BindUObject(this, &ATinyTrialsGameMode::OnRequestComplete);
    Req->ProcessRequest();

    PutPlayerInPendingAuthState(NewLogin);
}

void ATinyTrialsGameMode::ManageLoginQueue()
{
    ProcessedControllersQueue.RemoveAt(0);
    if (ProcessedControllersQueue.IsEmpty()) return;

    TArray<int32> StaleControllers;
    ATinyTrialsPlayerController* NextController = nullptr;
    for (int32 i = 0; i < ProcessedControllersQueue.Num(); ++i)
    {
        if (!ProcessedControllersQueue[i].IsValid())
        {
            StaleControllers.Add(i);
        }
        else
        {
            NextController = ProcessedControllersQueue[i].Get();
            break;
        }
    }
    for (const auto& ContIdx : StaleControllers) ProcessedControllersQueue.RemoveAt(ContIdx);
    if (NextController) ProcessLogin(NextController);
}

TSharedRef<FJsonObject> ATinyTrialsGameMode::GenerateJsonObj(APlayerController* Controller) const
{
    const FString DeviceId = TEXT("") /*GetOrMakeStableDeviceIdFor(Controller)*/;
    const FString DisplayName = TEXT("") /*GetDisplayNameFor(Controller)*/;

    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    Json->SetStringField(TEXT("device_id"), DeviceId);
    Json->SetStringField(TEXT("display_name"), DisplayName);
    return Json;
}

void ATinyTrialsGameMode::BuildHttpRequest(TSharedRef<class IHttpRequest, ESPMode::ThreadSafe> Req, const FString& Body)
{
    // in reality, we'd make a getter/generator but for this project we just set some default base values
    FHttpProfile Profile;

    Req->SetURL(Profile.BaseUrl);
    Req->SetVerb(Profile.Verb);
    Req->SetHeader(TEXT("Content-Type"), Profile.ContentType);
    for (const auto& Kvp : Profile.Headers)
    {
        Req->SetHeader(Kvp.Key, Kvp.Value);
    }
    Req->SetTimeout(Profile.TimeoutSeconds);
    Req->SetContentAsString(Body);
}

void ATinyTrialsGameMode::OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
    if (!ProcessedControllersQueue.IsEmpty()) return;

    if (!ProcessedControllersQueue[0].IsValid())
    {
        ManageLoginQueue();
        return;
    }
    auto ProcessingController = ProcessedControllersQueue[0].Get();

    if (!bSucceeded || !Response.IsValid())
    {
        //HandleAuthFailure(ControllerBeingProcessed.Get(), TEXT("Network/No response"));
        return;
    }

    if (Response->GetResponseCode() != 200)
    {
        //HandleAuthFailure(ControllerBeingProcessed.Get(), FString::Printf(TEXT("HTTP %d"), Response->GetResponseCode()));
        return;
    }

    // 3) Parse JSON: { "playerId": "...", "jwt": "..." }
    TSharedPtr<FJsonObject> JsonResp;
    const auto Payload = Response->GetContentAsString();
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Payload);
    if (!FJsonSerializer::Deserialize(Reader, JsonResp) || !JsonResp.IsValid())
    {
        //HandleAuthFailure(ProcessedControllersQueue.Get(), TEXT("Bad JSON"));
        return;
    }

    const FString PlayerId = JsonResp->GetStringField(TEXT("playerId"));
    const FString Jwt = JsonResp->GetStringField(TEXT("jwt"));

    InitializeAuthorizedPlayer(ProcessingController, PlayerId, Jwt);

    ManageLoginQueue();
}

void ATinyTrialsGameMode::PutPlayerInPendingAuthState(ATinyTrialsPlayerController* PendingController)
{
    // set bool gate to prevent initialisation
}

void ATinyTrialsGameMode::InitializeAuthorizedPlayer(ATinyTrialsPlayerController* AuthController, const FString& ID, const FString& Token)
{
    // set bool gate to allow initialisation
}

