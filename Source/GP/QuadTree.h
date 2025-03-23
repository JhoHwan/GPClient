// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class GP_API FQuadTree
{
public:
	// 내부 노드 클래스 선언
	class FNode;

public:
	// 생성자: 주어진 그리드를 사용하여 QuadTree 객체를 초기화
	FQuadTree(const TArray<TArray<int32>>& Grid);

	// 복사 생성자 및 대입 연산자 삭제
	FQuadTree(const FQuadTree& Other) = delete;
	void operator=(const FQuadTree& Other) = delete;

	// 소멸자
	~FQuadTree();

	// 루트 노드 Getter 함수
	inline const FNode& GetRoot() const { return *Root; }

	// 리프 노드들 Getter 함수
	inline const TArray<FNode*>& GetLeafNodes() const { return LeafNodes; }

private:
	// 주어진 영역이 균일한지 확인
	// Grid: 그리드 데이터
	// X, Y: 영역의 시작 좌표
	// Width, Height: 영역의 크기
	// Value: 균일한 값 (출력 매개변수)
	// 반환값: 영역이 균일하면 true, 그렇지 않으면 false
	bool IsUniform(const TArray<TArray<int32>>& Grid, int32 X, int32 Y, int32 Width, int32 Height, int32& Value) const;

	// 주어진 영역을 기반으로 QuadTree를 빌드
	// Grid: 그리드 데이터
	// X, Y: 영역의 시작 좌표
	// Width, Height: 영역의 크기
	// 반환값: 생성된 QuadTree 노드
	FNode* Build(const TArray<TArray<int32>>& Grid, int32 X, int32 Y, int32 Width, int32 Height);

private:
	// QuadTree의 루트 노드
	FNode* Root;

	// 리프 노드들
	TArray<FNode*> LeafNodes;
};

class FQuadTree::FNode
{
public:
	// 생성자: 주어진 좌표와 크기, Walkable 값을 사용하여 노드를 초기화
	FNode(int32 X, int32 Y, int32 Width, int32 Height, int32 Walkable = -1);

	// 소멸자
	~FNode();

public:
	// 노드의 시작 좌표
	int32 X;
	int32 Y;

	// 노드의 크기
	int32 Width;
	int32 Height;

	// 노드의 Walkable 값
	int32 Walkable;

	// 자식 노드들 (0: 좌상, 1: 우상, 2: 좌하, 3: 우하 순서 등으로 사용 가능)
	FNode* Children[4];
};