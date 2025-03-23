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
	// �ڽ� ��� �޸� ����
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

	// ��Ʈ ��� ����
	Root = Build(Grid, 0, 0, Width, Height);
}

FQuadTree::~FQuadTree()
{
	if (Root == nullptr)
	{
		return;
	}
	// ��Ʈ ��� �޸� ����
	delete Root;
}

bool FQuadTree::IsUniform(const TArray<TArray<int32>>& Grid, int32 X, int32 Y, int32 Width, int32 Height, int32& OutValue) const
{
	// ù ��° ������ �ʱ�ȭ
	OutValue = Grid[Y][X];

	// �־��� ������ �������� Ȯ��
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
	FQuadTree::FNode* Parent; // �θ� ��� (��Ʈ�� ��� nullptr)
	int32 ChildIndex;         // �θ��� Children �迭���� �Ҵ��� �ε��� (-1�� ��Ʈ)
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
			// ������ �����̸� ���� ��带 ����
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

		// �������� ���� �����̸� ���� ��� ���� (Walkable = -1)
		FNode* Node = new FNode(Task.X, Task.Y, Task.Width, Task.Height, -1);
		if (Task.Parent)
		{
			Task.Parent->Children[Task.ChildIndex] = Node;
		}
		else
		{
			RootNode = Node;
		}

		// ������ �� �̻� ������ �� �ִ��� Ȯ��
		int32 MidWidth = Task.Width / 2;
		int32 MidHeight = Task.Height / 2;
		if (MidWidth == 0 || MidHeight == 0)
		{
			continue;  // �� �̻� ���� �Ұ�
		}

		// �� ������ ���� �۾��� ���ÿ� push (DFS ���� ����)
		// �»�
		Tasks.Add({ Task.X, Task.Y, MidWidth, MidHeight, Node, 0 });
		// ���
		Tasks.Add({ Task.X + MidWidth, Task.Y, Task.Width - MidWidth, MidHeight, Node, 1 });
		// ����
		Tasks.Add({ Task.X, Task.Y + MidHeight, MidWidth, Task.Height - MidHeight, Node, 2 });
		// ����
		Tasks.Add({ Task.X + MidWidth, Task.Y + MidHeight, Task.Width - MidWidth, Task.Height - MidHeight, Node, 3 });
	}

	return RootNode;
}