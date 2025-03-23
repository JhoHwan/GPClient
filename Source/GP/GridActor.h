// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridActor.generated.h"

UCLASS()
class GP_API AGridActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool SelectGirdWithName(const FString& Name);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	TArray<FIntPoint> FindPath(const FIntPoint& Start, const FIntPoint& Goal, int32 EntitySize);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void UpdateGrid(const FIntPoint& Inpoint, int32 Walkable);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	int32 GetTileType(const FIntPoint& Inpoint);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void SaveGridToFile(const FString& FileName) const;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	int32 LoadAllMapsFromFolder();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	TArray<FString> GetGridNames() const;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	FIntPoint GetCurrentGridSize() const;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void CreateNewGrid(const FIntPoint& MapSize);

private:
	TMap<FString, TSharedPtr<class FGrid>> Grids;
	TSharedPtr<class FGrid> CurrentGrid;

};
