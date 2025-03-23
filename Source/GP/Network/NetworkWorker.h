// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "GP.h"

struct GP_API FPacketHeader
{
	FPacketHeader() : FPacketHeader(0, 0)
	{
	}

	FPacketHeader(uint16 PacketSize, uint16 PacketID) : PacketSize(PacketSize), PacketID(PacketID)
	{
	}

	friend FArchive& operator<<(FArchive& Ar, FPacketHeader& Header)
	{
		Ar << Header.PacketSize;
		Ar << Header.PacketID;
		return Ar;
	}

	uint16 PacketSize;
	uint16 PacketID;
};

class GP_API RecvWorker : public FRunnable 
{
public:
	RecvWorker(class FSocket* Socket, TSharedPtr<class PacketSession> SessionRef);
	~RecvWorker();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Exit();

	void Destroy();

private:
	bool ReceivePacket(TArray<uint8>& OutPacket);
	bool ReceiveDesiredBytes(uint8* Results, int32 Size);

protected:
	FRunnableThread* Thread = nullptr;
	bool bIsRunning = false;
	class FSocket* Socket;
	TWeakPtr<class PacketSession> SessionRef;
};

class GP_API SendWorker : public FRunnable
{
public:
	SendWorker(class FSocket* Socket, TSharedPtr<class PacketSession> SessionRef);
	~SendWorker();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Exit();

	void Destroy();

private:
	bool SendPacket(TSharedPtr<SendBuffer> SendBuffer);
	bool SendDesiredBytes(const uint8* Buffer, int32 Size);

protected:
	FRunnableThread* Thread = nullptr;
	bool bIsRunning = false;
	class FSocket* Socket;
	TWeakPtr<class PacketSession> SessionRef;
};