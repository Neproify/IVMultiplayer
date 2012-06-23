//============== IV: Multiplayer - http://code.iv-multiplayer.com ==============
//
// File: CClientPacketHandler.cpp
// Project: Client.Core
// Author(s): jenksta
// License: See LICENSE in root directory
//
//==============================================================================

#include "CChatWindow.h"
#include <CString.h>
#include "CNetworkManager.h"
#include <Network/PacketIdentifiers.h>
#include "CGameFileChecker.h"
#include "CMainMenu.h"

extern CChatWindow     * g_pChatWindow;
extern String            g_strNick;
extern CNetworkManager * g_pNetworkManager;
extern CMainMenu	   * g_pMainMenu;

void ResetGame();

void CClientPacketHandler::ConnectionRejected(CBitStream * pBitStream, CPlayerSocket * pSenderSocket)
{
	g_pChatWindow->AddInfoMessage("Connection failed! (Rejected)");
	ResetGame();
	g_pNetworkManager->Disconnect();
	g_pMainMenu->ResetNetworkStats();
}

void CClientPacketHandler::ConnectionSucceeded(CBitStream * pBitStream, CPlayerSocket * pSenderSocket)
{
	g_pChatWindow->AddInfoMessage("Connection established, please wait..");
	CBitStream  bsSend;
	bsSend.Write(NETWORK_VERSION);
	bsSend.Write(g_strNick);
	bsSend.WriteBit(!CGameFileChecker::CheckFiles());
	g_pNetworkManager->RPC(RPC_PlayerConnect, &bsSend, PRIORITY_HIGH, RELIABILITY_RELIABLE_ORDERED);
}

void CClientPacketHandler::ConnectionFailed(CBitStream * pBitStream, CPlayerSocket * pSenderSocket)
{
	g_pChatWindow->AddInfoMessage("Connection failed(Timeout), retrying ....");
	g_pNetworkManager->Connect();
	g_pMainMenu->ResetNetworkStats();
}

void CClientPacketHandler::AlreadyConnected(CBitStream * pBitStream, CPlayerSocket * pSenderSocket)
{
	g_pChatWindow->AddInfoMessage("Connection failed(Already connected)!");
}

void CClientPacketHandler::ServerFull(CBitStream * pBitStream, CPlayerSocket * pSenderSocket)
{
	g_pChatWindow->AddInfoMessage("Connection failed(server is full), retrying ....");
	g_pNetworkManager->Connect();
	g_pMainMenu->ResetNetworkStats();
}

void CClientPacketHandler::Disconnected(CBitStream * pBitStream, CPlayerSocket * pSenderSocket)
{
	g_pChatWindow->AddInfoMessage("Server closed the connection!");
	g_pMainMenu->ResetNetworkStats();
}

void CClientPacketHandler::LostConnection(CBitStream * pBitStream, CPlayerSocket * pSenderSocket)
{
	g_pChatWindow->AddInfoMessage("Connetion to the server lost, retrying ....");
	ResetGame();
	g_pMainMenu->ResetNetworkStats();
}

void CClientPacketHandler::Banned(CBitStream * pBitStream, CPlayerSocket * pSenderSocket)
{
	g_pChatWindow->AddInfoMessage("Connection failed(You're banned)!");
}

void CClientPacketHandler::PasswordInvalid(CBitStream * pBitStream, CPlayerSocket * pSenderSocket)
{
	g_pChatWindow->AddInfoMessage("Connection failed(Wrong password)!");
}

void CClientPacketHandler::Register()
{
	AddFunction(PACKET_CONNECTION_REJECTED, ConnectionRejected);
	AddFunction(PACKET_CONNECTION_SUCCEEDED, ConnectionSucceeded);
	AddFunction(PACKET_CONNECTION_FAILED, ConnectionFailed);
	AddFunction(PACKET_ALREADY_CONNECTED, AlreadyConnected);
	AddFunction(PACKET_SERVER_FULL, ServerFull);
	AddFunction(PACKET_DISCONNECTED, Disconnected);
	AddFunction(PACKET_LOST_CONNECTION, LostConnection);
	AddFunction(PACKET_BANNED, Banned);
	AddFunction(PACKET_PASSWORD_INVALID, PasswordInvalid);
}

void CClientPacketHandler::Unregister()
{
	RemoveFunction(PACKET_CONNECTION_REJECTED);
	RemoveFunction(PACKET_CONNECTION_SUCCEEDED);
	RemoveFunction(PACKET_CONNECTION_FAILED);
	RemoveFunction(PACKET_ALREADY_CONNECTED);
	RemoveFunction(PACKET_SERVER_FULL);
	RemoveFunction(PACKET_DISCONNECTED);
	RemoveFunction(PACKET_LOST_CONNECTION);
	RemoveFunction(PACKET_BANNED);
	RemoveFunction(PACKET_PASSWORD_INVALID);
}