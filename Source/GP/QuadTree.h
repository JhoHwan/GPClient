// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class GP_API FQuadTree
{
public:
	// ���� ��� Ŭ���� ����
	class FNode;

public:
	// ������: �־��� �׸��带 ����Ͽ� QuadTree ��ü�� �ʱ�ȭ
	FQuadTree(const TArray<TArray<int32>>& Grid);

	// ���� ������ �� ���� ������ ����
	FQuadTree(const FQuadTree& Other) = delete;
	void operator=(const FQuadTree& Other) = delete;

	// �Ҹ���
	~FQuadTree();

	// ��Ʈ ��� Getter �Լ�
	inline const FNode& GetRoot() const { return *Root; }

	// ���� ���� Getter �Լ�
	inline const TArray<FNode*>& GetLeafNodes() const { return LeafNodes; }

private:
	// �־��� ������ �������� Ȯ��
	// Grid: �׸��� ������
	// X, Y: ������ ���� ��ǥ
	// Width, Height: ������ ũ��
	// Value: ������ �� (��� �Ű�����)
	// ��ȯ��: ������ �����ϸ� true, �׷��� ������ false
	bool IsUniform(const TArray<TArray<int32>>& Grid, int32 X, int32 Y, int32 Width, int32 Height, int32& Value) const;

	// �־��� ������ ������� QuadTree�� ����
	// Grid: �׸��� ������
	// X, Y: ������ ���� ��ǥ
	// Width, Height: ������ ũ��
	// ��ȯ��: ������ QuadTree ���
	FNode* Build(const TArray<TArray<int32>>& Grid, int32 X, int32 Y, int32 Width, int32 Height);

private:
	// QuadTree�� ��Ʈ ���
	FNode* Root;

	// ���� ����
	TArray<FNode*> LeafNodes;
};

class FQuadTree::FNode
{
public:
	// ������: �־��� ��ǥ�� ũ��, Walkable ���� ����Ͽ� ��带 �ʱ�ȭ
	FNode(int32 X, int32 Y, int32 Width, int32 Height, int32 Walkable = -1);

	// �Ҹ���
	~FNode();

public:
	// ����� ���� ��ǥ
	int32 X;
	int32 Y;

	// ����� ũ��
	int32 Width;
	int32 Height;

	// ����� Walkable ��
	int32 Walkable;

	// �ڽ� ���� (0: �»�, 1: ���, 2: ����, 3: ���� ���� ������ ��� ����)
	FNode* Children[4];
};