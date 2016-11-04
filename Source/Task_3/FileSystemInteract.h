// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FileSystemInteract.generated.h"

UCLASS()
class TASK_3_API AFileSystemInteract : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFileSystemInteract();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = "FileSystem")
	TArray<FString> GetFilesInFolder(FString Directory);

	UFUNCTION(BlueprintCallable, Category = "FileSystem")
	FString GetPathSavesDir();

	UFUNCTION(BlueprintCallable, Category = "FileSystem")
	void PickeUpShot(FString path);
	
	UFUNCTION(BlueprintCallable, Category = "FileSystem")
	UTexture2D* Victory_LoadTexture2D_FromFile(const FString& FullFilePath, int typeFormatEnum, bool& IsValid, int32& Width, int32& Height);
	
	//UFUNCTION(BlueprintCallable, Category = "FileSystem")
	//static EImageFormat::Type GetJoyImageFormat(EFileTypesCode ftype);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category = "FileSystem")
	FString savedGameDir;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FileSystem")
	FString screenshotDir;
};
