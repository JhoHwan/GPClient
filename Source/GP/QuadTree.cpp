// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadTree.h"
#include "Containers/Array.h"

FQuadTree::FNode::FNode(int32 InX, int32 InY, int32 InWidth, int32 InHeight, int32 InWalkable)
	: X(InX), Y(InY), Width(InWidth), Height(InHeight), Walkable(InWalkable)
{
	for (int32 i = 0; i < 4; i++)
	{
		Children[i] = nullptr;
	}
}

FQuadTree::FNode::~FNode()
{
	// 자식 노드 메모리 해제
	for (int32 i = 0; i < 4; i++)
	{
		if (Children[i] != nullptr)
		{
			delete Children[i];
			Children[i] = nullptr;
		}
	}
}

FQuadTree::FQuadTree(const TArray<TArray<int32>>& Grid)
{
	int32 Height = Grid.Num();
	int32 Width = (Grid.Num() > 0) ? Grid[0].Num() : 0;

	// 루트 노드 생성
	Root = Build(Grid, 0, 0, Width, Height);
}

FQuadTree::~FQuadTree()
{
	if (Root == nullptr)
	{
		return;
	}
	// 루트 노드 메모리 해제
	delete Root;
}

bool FQuadTree::IsUniform(const TArray<TArray<int32>>& Grid, int32 X, int32 Y, int32 Width, int32 Height, int32& OutValue) const
{
	// 첫 번째 값으로 초기화
	OutValue = Grid[Y][X];

	// 주어진 영역이 균일한지 확인
	for (int32 i = Y; i < Y + Height; i++)
	{
		for (int32 j = X; j < X + Width; j++)
		{
			if (Grid[i][j] != OutValue)
			{
				return false;
			}
		}
	}
	return true;
}

struct FBuildTask
{
	int32 X, Y, Width, Height;
	FQuadTree::FNode* Parent; // 부모 노드 (루트인 경우 nullptr)
	int32 ChildIndex;         // 부모의 Children 배열에서 할당할 인덱스 (-1은 루트)
};

FQuadTree::FNode* FQuadTree::Build(const TArray<TArray<int32>>& Grid, int32 X, int32 Y, int32 Width, int32 Height)
{
	FNode* RootNode = nullptr;
	TArray<FBuildTask> Tasks;
	Tasks.Add({ X, Y, Width, Height, nullptr, -1 });

	while (Tasks.Num() > 0)
	{
		FBuildTask Task = Tasks.Last();
		Tasks.Pop();

		int32 Value;
		if (IsUniform(Grid, Task.X, Task.Y, Task.Width, Task.Height, Value))
		{
			// 균일한 영역이면 리프 노드를 생성
			FNode* Leaf = new FNode(Task.X, Task.Y, Task.Width, Task.Height, Value);
			if (Task.Parent)
			{
				Task.Parent->Children[Task.ChildIndex] = Leaf;
				LeafNodes.Add(Leaf);
			}
			else
			{
				RootNode = Leaf;
			}
			continue;
		}

		// 균일하지 않은 영역이면 내부 노드 생성 (Walkable = -1)
		FNode* Node = new FNode(Task.X, Task.Y, Task.Width, Task.Height, -1);
		if (Task.Parent)
		{
			Task.Parent->Children[Task.ChildIndex] = Node;
		}
		else
		{
			RootNode = Node;
		}

		// 영역이 더 이상 분할할 수 있는지 확인
		int32 MidWidth = Task.Width / 2;
		int32 MidHeight = Task.Height / 2;
		if (MidWidth == 0 || MidHeight == 0)
		{
			continue;  // 더 이상 분할 불가
		}

		// 네 영역에 대해 작업을 스택에 push (DFS 순서 유지)
		// 좌상
		Tasks.Add({ Task.X, Task.Y, MidWidth, MidHeight, Node, 0 });
		// 우상
		Tasks.Add({ Task.X + MidWidth, Task.Y, Task.Width - MidWidth, MidHeight, Node, 1 });
		// 좌하
		Tasks.Add({ Task.X, Task.Y + MidHeight, MidWidth, Task.Height - MidHeight, Node, 2 });
		// 우하
		Tasks.Add({ Task.X + MidWidth, Task.Y + MidHeight, Task.Width - MidWidth, Task.Height - MidHeight, Node, 3 });
	}

	return RootNode;
}