#include "Grid.h"

#include <queue>
#include <vector>
#include "QuadTree.h"
#include "Algo/Reverse.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

// FGrid ������: TArray ����� �׸���� QuadTree �ʱ�ȭ
FGrid::FGrid(const TArray<TArray<int32>>& InGrid)
	: Map(InGrid)
{
	Height = Map.Num();
	Width = (Map.Num() > 0) ? Map[0].Num() : 0;

	QuadTree = new FQuadTree(InGrid);
	BuildClearance();
}

FGrid::~FGrid()
{
	delete QuadTree;
}

TArray<FIntPoint> FGrid::FindPath(int32 StartX, int32 StartY, int32 GoalX, int32 GoalY, int32 EntitySize)
{
	if (!bIsBuilt)
	{
		return TArray<FIntPoint>();
	}

	if (!IsValidPosition(StartX, StartY, EntitySize) || !IsValidPosition(GoalX, GoalY, EntitySize))
	{
		return TArray<FIntPoint>();
	}

	// A*�� ����� ��� �׸��带 ���� (�ʱ�ȭ �� X, Y ��ǥ ����)
	TArray<TArray<FAStarNode>> Nodes;
	Nodes.SetNum(Height);
	for (int32 i = 0; i < Height; i++)
	{
		Nodes[i].SetNum(Width);
		for (int32 j = 0; j < Width; j++)
		{
			Nodes[i][j] = FAStarNode(j, i);
		}
	}

	Nodes[StartY][StartX].G = 0;
	Nodes[StartY][StartX].H = Heuristic(StartX, StartY, GoalX, GoalY);
	Nodes[StartY][StartX].F = Nodes[StartY][StartX].G + Nodes[StartY][StartX].H;

	auto cmp = [](FAStarNode* A, FAStarNode* B) { return A->F > B->F; };
	std::priority_queue<FAStarNode*, std::vector<FAStarNode*>, decltype(cmp)> OpenList(cmp);
	OpenList.push(&Nodes[StartY][StartX]);

	// �湮 ���θ� ǥ���� �׸���
	TArray<TArray<bool>> ClosedList;
	ClosedList.SetNum(Height);
	for (int32 i = 0; i < Height; i++)
	{
		ClosedList[i].SetNum(Width);
		for (int32 j = 0; j < Width; j++)
		{
			ClosedList[i][j] = false;
		}
	}

	// 8���� �̵� (��, ��, ��, �� �� �밢��)
	TArray<TPair<int32, int32>> Dirs = {
		TPair<int32, int32>(0, -1),
		TPair<int32, int32>(0, 1),
		TPair<int32, int32>(-1, 0),
		TPair<int32, int32>(1, 0),
		TPair<int32, int32>(-1, -1),
		TPair<int32, int32>(-1, 1),
		TPair<int32, int32>(1, -1),
		TPair<int32, int32>(1, 1)
	};

	FAStarNode* GoalNode = nullptr;
	while (!OpenList.empty())
	{
		FAStarNode* Current = OpenList.top();
		OpenList.pop();

		int32 Cx = Current->X;
		int32 Cy = Current->Y;

		if (Cx == GoalX && Cy == GoalY)
		{
			GoalNode = Current;
			break;
		}

		ClosedList[Cy][Cx] = true;

		for (const TPair<int32, int32>& Dir : Dirs)
		{
			int32 Nx = Cx + Dir.Key;
			int32 Ny = Cy + Dir.Value;

			if (Nx < 0 || Ny < 0 || Nx >= Width || Ny >= Height)
			{
				continue;
			}

			// �밢 �̵��� ��� ������ ����, ���� ���� üũ
			if (abs(Dir.Key) == 1 && abs(Dir.Value) == 1) {
				// (cx + dir.first, cy)�� (cx, cy + dir.second)�� ��� ��� �������� Ȯ��
				if (!IsValidPosition(Cx + Dir.Key, Cy, EntitySize) ||
					!IsValidPosition(Cx, Cy + Dir.Value, EntitySize))
				{
					continue;  // ���� �� �� �ϳ��� ���� ������ �밢 �̵� ����
				}
			}

			if (!IsValidPosition(Nx, Ny, EntitySize))
			{
				continue;
			}
			if (ClosedList[Ny][Nx])
			{
				continue;
			}

			// ����: ��� 1.0, �밢��: ��� sqrt(2)
			double MoveCost = (FMath::Abs(Dir.Key) + FMath::Abs(Dir.Value)) == 2 ? FMath::Sqrt(2.0) : 1.0;
			double TentativeG = Current->G + MoveCost;
			FAStarNode* Neighbor = &Nodes[Ny][Nx];

			if (TentativeG < Neighbor->G)
			{
				Neighbor->Parent = Current;
				Neighbor->G = TentativeG;
				Neighbor->H = Heuristic(Nx, Ny, GoalX, GoalY);
				Neighbor->F = Neighbor->G + Neighbor->H;
				OpenList.push(Neighbor);
			}
		}
	}

	TArray<FIntPoint> Path;
	if (GoalNode)
	{
		FAStarNode* Current = GoalNode;
		while (Current)
		{
			Path.Add(FIntPoint(Current->X, Current->Y));
			Current = Current->Parent;
		}
		Algo::Reverse(Path);
	}

	return Path;
}

TArray<FIntPoint> FGrid::FindPath(FIntPoint StartPos, FIntPoint GoalPos, int32 EntitySize)
{
	return FindPath(StartPos.X, StartPos.Y, GoalPos.X, GoalPos.Y, EntitySize);
}

void FGrid::UpdateMap(const FIntPoint Inpoint, int32 Walkable)
{
	Map[Inpoint.Y][Inpoint.X] = Walkable;
	BuildClearance();
}

bool FGrid::SaveGridToFile(FString MapName) const
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	JsonObject->SetStringField(TEXT("Name"), MapName);
	JsonObject->SetNumberField(TEXT("Width"), Width);
	JsonObject->SetNumberField(TEXT("Height"), Height);

	TArray<TSharedPtr<FJsonValue>> MapArray;
	for (const TArray<int32>& Row : Map)
	{
		TArray<TSharedPtr<FJsonValue>> RowArray;
		for (int32 Value : Row)
		{
			RowArray.Add(MakeShareable(new FJsonValueNumber(Value)));
		}
		MapArray.Add(MakeShareable(new FJsonValueArray(RowArray)));
	}

	JsonObject->SetArrayField(TEXT("Map"), MapArray);

	FString JsonStr;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonStr);
	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
	{
		FString FilePath = FPaths::ProjectContentDir() + TEXT("Json/GridDatas/") + MapName + TEXT(".json");
		FFileHelper::SaveStringToFile(JsonStr, *FilePath);
		UE_LOG(LogTemp, Warning, TEXT("Map saved to %s"), *FilePath);
		return true;
	}

	return false;
}

TSharedPtr<FGrid> FGrid::LoadMapFromFile(FString FilePath)
{
	int32 Width, Height;
	TArray<TArray<int32>> Map;
	FString JsonString;

	// ������ �о�ͼ� JsonString�� ����
	if (FFileHelper::LoadFileToString(JsonString, *FilePath) == false) return nullptr;

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);

	// JsonString�� JsonObject�� ��ȯ
	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid()) return nullptr;

	// �ʿ��� �ʵ尡 ������ nullptr ��ȯ
	if (!JsonObject->TryGetNumberField(TEXT("Width"), Width) || !JsonObject->TryGetNumberField(TEXT("Height"), Height)) return nullptr;

	// �� �����͸� �о�ͼ� Map�� ����
	const TArray<TSharedPtr<FJsonValue>>* MapArray;
	if (JsonObject->TryGetArrayField(TEXT("Map"), MapArray))
	{
		for (const TSharedPtr<FJsonValue>& RowValue : *MapArray)
		{
			const TArray<TSharedPtr<FJsonValue>>* RowArray;
			if (RowValue->TryGetArray(RowArray))
			{
				TArray<int32> Row;
				for (const TSharedPtr<FJsonValue>& Value : *RowArray)
				{
					Row.Add(static_cast<int32>(Value->AsNumber()));
				}
				Map.Add(Row);
			}
		}
		return MakeShareable(new FGrid(Map));
	}
	
	return nullptr;
}

void FGrid::BuildClearance()
{
	bIsBuilt = true;

	// ClearanceMap ũ�� �ʱ�ȭ
	ClearanceMap.SetNum(Height);
	for (int32 i = 0; i < Height; i++)
	{
		ClearanceMap[i].SetNum(Width);
		for (int32 j = 0; j < Width; j++)
		{
			ClearanceMap[i][j] = 0;
		}
	}

	// ���� �ϴ� �����ʺ��� clearance �� ���
	for (int32 i = Height - 1; i >= 0; i--)
	{
		for (int32 j = Width - 1; j >= 0; j--)
		{
			if (Map[i][j] == 1)
			{
				continue;
			}

			if (i == Height - 1 || j == Width - 1)
			{
				ClearanceMap[i][j] = 1;
			}
			else
			{
				int32 MinVal = FMath::Min(ClearanceMap[i + 1][j], FMath::Min(ClearanceMap[i][j + 1], ClearanceMap[i + 1][j + 1]));
				ClearanceMap[i][j] = MinVal + 1;
			}
		}
	}
}

bool FGrid::IsValidPosition(int32 X, int32 Y, int32 EntitySize) const
{
	if (X < 0 || Y < 0 || X >= Width || Y >= Height)
	{
		return false;
	}
	return (ClearanceMap[Y][X] >= EntitySize);
}

int32 FGrid::GetTileType(int32 X, int32 Y, int32 EntitySize) const
{
	return Map[Y][X];
}

double FGrid::Heuristic(int32 X1, int32 Y1, int32 X2, int32 Y2)
{
	int32 Dx = FMath::Abs(X1 - X2);
	int32 Dy = FMath::Abs(Y1 - Y2);
	double D = 1.0;             // ���� �̵� ���
	double D2 = FMath::Sqrt(2.0); // �밢�� �̵� ���

	return D * (Dx + Dy) - ((2 * D - D2) * FMath::Min(Dx, Dy));
}
