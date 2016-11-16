// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Engine.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Runtime/Core/Public/Misc/Base64.h"
#include "ActorGraphApi.generated.h"

UCLASS()
class TASK_3_API AActorGraphApi : public AActor
{
	GENERATED_BODY()
	
public:	
	FHttpModule* Http;
	FString responseJson;

	FString uploadFilePath;
	FString myId;
	int64 fileSize;
	int64 file_offset;
	FString access_token;
	FString uploadSession;
	FString hUpload;

	// Sets default values for this actor's properties
	AActorGraphApi();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = "Facebook library")
	void CreatingUploadSession(FString token, FString file_path);
	void OnResponseUploadSession(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void UploadingFileData();
	void OnResponseDoneUpload(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "Facebook library")
		void GetUploadStatus(FString token, FString file_path);
	void OnResponseUploadStatus(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void ExampleFeedUploadPhoto();
	void OnResponseFeed(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "Facebook library")
	void CrutchWithCurl(FString token, FString filePath, FString caption);
	
	bool bWhaitUpload;
	bool firstGetIds;

	TArray<FString> photosId;
	FString idCurrPhoto;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString urlPhoto;

	UFUNCTION(BlueprintCallable, Category = "Facebook library")
	void GetAllPhoto(FString token);
	void OnResponsePhoto(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

};
