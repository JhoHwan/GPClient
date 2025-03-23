// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActor.h"
#include "Grid.h"

// Sets default values
AGridActor::AGridActor() : CurrentGrid(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGridActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGridActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AGridActor::SelectGirdWithName(const FString& Name)
{
	auto FindGrid = Grids.Find(Name);
	if (FindGrid == nullptr) return false;

	CurrentGrid = *FindGrid;
	return true;
}

TArray<FIntPoint> AGridActor::FindPath(const FIntPoint& Start, const FIntPoint& Goal, int32 EntitySize)
{
	if (!CurrentGrid.IsValid()) return TArray<FIntPoint>();

	return CurrentGrid->FindPath(Start, Goal, EntitySize);
}

void AGridActor::UpdateGrid(const FIntPoint& Inpoint, int32 Walkable)
{
	if (!CurrentGrid.IsValid()) return;

	CurrentGrid->UpdateMap(Inpoint, Walkable);
}

int32 AGridActor::GetTileType(const FIntPoint& Inpoint)
{
	if (!CurrentGrid.IsValid()) return false;
	return CurrentGrid->GetTileType(Inpoint.X, Inpoint.Y, 1);
}

void AGridActor::SaveGridToFile(const FString& FileName) const
{
	if (!CurrentGrid.IsValid()) return;
	CurrentGrid->SaveGridToFile(FileName);
}

int32 AGridActor::LoadAllMapsFromFolder()
{
	Grids.Empty();
	CurrentGrid = nullptr;

	TArray<FString> Files;
	FString Path = FPaths::ProjectContentDir() + TEXT("Json/GridDatas");
	IFileManager::Get().FindFiles(Files, *Path, TEXT("*.json"));

	if (Files.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Map File"), Grids.Num());
		return 0;
	}

	// 모든 맵 파일을 읽어와서 Grids에 저장
	for (const FString& File : Files)
	{
		FString FullPath = FPaths::Combine(Path, File);
		TSharedPtr<FGrid> NewGrid = FGrid::LoadMapFromFile(FullPath);

		// 맵 로드 실패 시 경고 출력 후 리턴
		if (!NewGrid.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load map from %s"), *FullPath);
			continue;
		}

		FString MapName = FPaths::GetBaseFilename(FullPath);
		Grids.Add(MapName, NewGrid);
	}

	UE_LOG(LogTemp, Warning, TEXT("Sucess Load (%d) Map"), Grids.Num());

	return Grids.Num();
}

TArray<FString> AGridActor::GetGridNames() const
{
	TArray<FString> Names;
	Grids.GetKeys(Names);

	return Names;
}

FIntPoint AGridActor::GetCurrentGridSize() const
{
	if (!CurrentGrid.IsValid()) return FIntPoint(0, 0);
	return CurrentGrid->GetGridSize();
}

void AGridActor::CreateNewGrid(const FIntPoint& MapSize)
{
	TArray<TArray<int32>> NewGrid;
	NewGrid.SetNum(MapSize.Y);
	for (int32 i = 0; i < MapSize.Y; i++)
	{
		NewGrid[i].SetNum(MapSize.X);
		for (int32 j = 0; j < MapSize.X; j++)
		{
			NewGrid[i][j] = 0;
		}
	}

	CurrentGrid = MakeShared<FGrid>(NewGrid);
}
