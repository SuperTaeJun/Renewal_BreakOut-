// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BOGameInstance.h"
#include "ClientSocket.h"

void UBOGameInstance::Init()
{
	Super::Init();

	m_Socket = ClientSocket::GetSingleton();

	m_Socket->SetGameInstance(this);
	connect = m_Socket->Connect();

	if (connect)
	{
		m_Socket->StartListen();

		UE_LOG(LogClass, Warning, TEXT("IOCP Server connect success!"));
	}
	else
	{
		UE_LOG(LogClass, Warning, TEXT("IOCP Server connect fail!"))
	};
}
