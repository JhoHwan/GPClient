// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

constexpr double INF = std::numeric_limits<double>::infinity();

class GP_API FGrid
{
public:
	enum { Obstacle = 0, Land = 1 };

	// 持失切人 社瑚切
	FGrid(const TArray<TArray<int32>>& InGrid);
	~FGrid();

	// Getter & Setter
public:
	inline const class FQuadTree& GetQuadTree() const { return *QuadTree; }

	// A* Algorithm
public:
	struct FAStarNode
	{
		int X, Y;
		double G, H, F;
		FAStarNode* Parent;

		FAStarNode(int32 InX, int32 InY)
			: X(InX), Y(InY), G(INF), H(0), F(INF), Parent(nullptr)
		{
		}

		FAStarNode() : FAStarNode(0, 0) {}

		FORCEINLINE bool operator< (const FAStarNode& Other) const
		{
			return F < Other.F;
		}
	};

	TArray<FIntPoint> FindPath(int32 StartX, int32 StartY, int32 EndX, int32 EndY, int32 EntitySize);
	TArray<FIntPoint> FindPath(FIntPoint StartPos, FIntPoint GoalPos, int32 EntitySize);

	int32 GetTileType(int32 X, int32 Y, int32 EntitySize) const;
	void UpdateMap(const FIntPoint Inpoint, int32 Walkable);

	bool SaveGridToFile(FString MapName) const;
	static TSharedPtr<FGrid> LoadMapFromFile(FString FilePath);

	inline FIntPoint GetGridSize() const { return FIntPoint(Width, Height); };

private:
	void BuildClearance();
	bool IsValidPosition(int32 X, int32 Y, int32 EntitySize) const;
	static double Heuristic(int32 X1, int32 Y1, int32 X2, int32 Y2);


private:
	bool bIsBuilt = false;
	TArray<TArray<int32>> Map;
	TArray<TArray<int32>> ClearanceMap;
	class FQuadTree* QuadTree;

	int32 Width;
	int32 Height;
};

