/***************************************************************************
 *   Copyright (C) 2007 by Lothar May                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <boost/asio.hpp>
#include <net/socket_helper.h>
#include <net/clientthread.h>
#include <net/clientstate.h>
#include <net/clientcontext.h>
#include <net/senderhelper.h>
#include <net/receiverhelper.h>
#include <net/downloaderthread.h>
#include <net/clientexception.h>
#include <net/socket_msg.h>
#include <core/avatarmanager.h>
#include <core/loghelper.h>
#include <clientenginefactory.h>
#include <game.h>
#include <qttoolsinterface.h>

#include <boost/lambda/lambda.hpp>
#include <sstream>
#include <memory>
#include <cassert>

#define TEMP_AVATAR_FILENAME "avatar.tmp"
#define CLIENT_AVATAR_LOOP_MSEC	100
#define CLIENT_SEND_LOOP_MSEC	50

using namespace std;
using boost::asio::ip::tcp;


class ClientSenderCallback : public SenderCallback, public SessionDataCallback
{
public:
	ClientSenderCallback() {}
	virtual ~ClientSenderCallback() {}

	virtual void SignalNetError(SessionId /*session*/, int /*errorID*/, int /*osErrorID*/)
	{
	}

	virtual void SignalSessionTerminated(unsigned /*session*/)
	{
	}

private:
};

ClientThread::ClientThread(GuiInterface &gui, AvatarManager &avatarManager)
: m_ioService(new boost::asio::io_service), m_curState(NULL), m_gui(gui),
  m_avatarManager(avatarManager), m_isServerSelected(false),
  m_curGameId(0), m_curGameNum(1), m_guiPlayerId(0), m_sessionEstablished(false),
  m_stateTimer(*m_ioService), m_avatarTimer(*m_ioService)
{
	m_context.reset(new ClientContext);
	m_receiver.reset(new ReceiverHelper);
	myQtToolsInterface.reset(CreateQtToolsWrapper());
	m_senderCallback.reset(new ClientSenderCallback());
	m_senderHelper.reset(new SenderHelper(*m_senderCallback, m_ioService));
}

ClientThread::~ClientThread()
{
}

void
ClientThread::Init(
	const string &serverAddress, const string &serverListUrl,
	bool useServerList, unsigned serverPort, bool ipv6, bool sctp,
	const string &avatarServerAddress, const string &pwd,
	const string &playerName, const string &avatarFile,
	const string &cacheDir)
{
	if (IsRunning())
	{
		assert(false);
		return;
	}

	ClientContext &context = GetContext();

	context.SetSctp(sctp);
	context.SetAddrFamily(ipv6 ? AF_INET6 : AF_INET);
	context.SetServerAddr(serverAddress);
	context.SetServerListUrl(serverListUrl);
	context.SetUseServerList(useServerList);
	context.SetServerPort(serverPort);
	context.SetAvatarServerAddr(avatarServerAddress);
	context.SetPassword(pwd);
	context.SetPlayerName(playerName);
	context.SetAvatarFile(avatarFile);
	context.SetCacheDir(cacheDir);
}

void
ClientThread::SignalTermination()
{
	Thread::SignalTermination();
	m_ioService->stop();
}

void
ClientThread::SendKickPlayer(unsigned playerId)
{
	boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
	packet->GetMsg()->present = PokerTHMessage_PR_kickPlayerRequestMessage;
	KickPlayerRequestMessage_t *netKick = &packet->GetMsg()->choice.kickPlayerRequestMessage;
	netKick->gameId = GetGameId();
	netKick->playerId = playerId;
	m_ioService->post(boost::bind(&ClientThread::SendSessionPacket, shared_from_this(), packet));
}

void
ClientThread::SendLeaveCurrentGame()
{
	boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
	packet->GetMsg()->present = PokerTHMessage_PR_leaveGameRequestMessage;
	m_ioService->post(boost::bind(&ClientThread::SendSessionPacket, shared_from_this(), packet));
}

void
ClientThread::SendStartEvent(bool fillUpWithCpuPlayers)
{
	// Warning: This function is called in the context of the GUI thread.
	// Create a network packet for the server start event.
	boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
	packet->GetMsg()->present = PokerTHMessage_PR_startEventMessage;
	StartEventMessage_t *netStartEvent = &packet->GetMsg()->choice.startEventMessage;
	netStartEvent->gameId = GetGameId();
	netStartEvent->fillWithComputerPlayers = fillUpWithCpuPlayers;
	m_ioService->post(boost::bind(&ClientThread::SendSessionPacket, shared_from_this(), packet));
}

void
ClientThread::SendPlayerAction()
{
	// Warning: This function is called in the context of the GUI thread.
	// Create a network packet containing the current player action.
	boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
	packet->GetMsg()->present = PokerTHMessage_PR_myActionRequestMessage;
	MyActionRequestMessage_t *netMyAction = &packet->GetMsg()->choice.myActionRequestMessage;
	netMyAction->gameId = GetGameId();
	boost::shared_ptr<PlayerInterface> myPlayer = GetGame()->getSeatsList()->front();
	netMyAction->gameState = GetGame()->getCurrentHand()->getCurrentRound();
	netMyAction->myAction = myPlayer->getMyAction();
	// Only send last bet if not fold/checked.
	if (myPlayer->getMyAction() != PLAYER_ACTION_FOLD && myPlayer->getMyAction() != PLAYER_ACTION_CHECK)
		netMyAction->myRelativeBet = myPlayer->getMyLastRelativeSet();
	else
		netMyAction->myRelativeBet = 0;
	// Just dump the packet.
	m_ioService->post(boost::bind(&ClientThread::SendSessionPacket, shared_from_this(), packet));
}

void
ClientThread::SendChatMessage(const std::string &msg)
{
	// Warning: This function is called in the context of the GUI thread.
	// Create a network packet containing the chat message.
	boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
	packet->GetMsg()->present = PokerTHMessage_PR_chatRequestMessage;
	ChatRequestMessage_t *netChat = &packet->GetMsg()->choice.chatRequestMessage;
	netChat->gameId = GetGameId();
	OCTET_STRING_fromBuf(&netChat->chatText,
						 msg.c_str(),
						 msg.length());

	// Just dump the packet.
	m_ioService->post(boost::bind(&ClientThread::SendSessionPacket, shared_from_this(), packet));
}

void
ClientThread::SendJoinFirstGame(const std::string &password)
{
	// Warning: This function is called in the context of the GUI thread.
	// Create a network packet to request joining a game.
	boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
	packet->GetMsg()->present = PokerTHMessage_PR_joinGameRequestMessage;
	JoinGameRequestMessage_t *netJoinGame = &packet->GetMsg()->choice.joinGameRequestMessage;
	if (!password.empty())
	{
		netJoinGame->password = OCTET_STRING_new_fromBuf(
			&asn_DEF_UTF8String,
			password.c_str(),
			password.length());
	}
	netJoinGame->joinGameAction.present = joinGameAction_PR_joinExistingGame;

	JoinExistingGame_t *joinExisting = &netJoinGame->joinGameAction.choice.joinExistingGame;
	joinExisting->gameId = 1;
	m_ioService->post(boost::bind(&ClientThread::SendSessionPacket, shared_from_this(), packet));
}

void
ClientThread::SendJoinGame(unsigned gameId, const std::string &password)
{
	// Warning: This function is called in the context of the GUI thread.
	// Create a network packet to request joining a game.
	boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
	packet->GetMsg()->present = PokerTHMessage_PR_joinGameRequestMessage;
	JoinGameRequestMessage_t *netJoinGame = &packet->GetMsg()->choice.joinGameRequestMessage;
	if (!password.empty())
	{
		netJoinGame->password = OCTET_STRING_new_fromBuf(
			&asn_DEF_UTF8String,
			password.c_str(),
			password.length());
	}
	netJoinGame->joinGameAction.present = joinGameAction_PR_joinExistingGame;

	JoinExistingGame_t *joinExisting = &netJoinGame->joinGameAction.choice.joinExistingGame;
	joinExisting->gameId = gameId;
	m_ioService->post(boost::bind(&ClientThread::SendSessionPacket, shared_from_this(), packet));
}

void
ClientThread::SendCreateGame(const GameData &gameData, const std::string &name, const std::string &password)
{
	// Warning: This function is called in the context of the GUI thread.
	// Create a network packet to request creating a new game.
	boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
	packet->GetMsg()->present = PokerTHMessage_PR_joinGameRequestMessage;
	JoinGameRequestMessage_t *netJoinGame = &packet->GetMsg()->choice.joinGameRequestMessage;
	if (!password.empty())
	{
		netJoinGame->password = OCTET_STRING_new_fromBuf(
			&asn_DEF_UTF8String,
			password.c_str(),
			password.length());
	}
	netJoinGame->joinGameAction.present = joinGameAction_PR_joinNewGame;

	JoinNewGame_t *joinNew = &netJoinGame->joinGameAction.choice.joinNewGame;
	NetPacket::SetGameData(gameData, &joinNew->gameInfo);
	OCTET_STRING_fromBuf(&joinNew->gameInfo.gameName,
						 name.c_str(),
						 name.length());
	m_ioService->post(boost::bind(&ClientThread::SendSessionPacket, shared_from_this(), packet));
}

void
ClientThread::SendResetTimeout()
{
	boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
	packet->GetMsg()->present = PokerTHMessage_PR_resetTimeoutMessage;
	m_ioService->post(boost::bind(&ClientThread::SendSessionPacket, shared_from_this(), packet));
}

void
ClientThread::SendAskKickPlayer(unsigned playerId)
{
	boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
	packet->GetMsg()->present = PokerTHMessage_PR_askKickPlayerMessage;
	AskKickPlayerMessage_t *netAsk = &packet->GetMsg()->choice.askKickPlayerMessage;
	netAsk->playerId = playerId;
	m_ioService->post(boost::bind(&ClientThread::SendSessionPacket, shared_from_this(), packet));
}

void
ClientThread::SendVoteKick(bool doKick)
{
	boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
	packet->GetMsg()->present = PokerTHMessage_PR_voteKickRequestMessage;
	VoteKickRequestMessage_t *netVote = &packet->GetMsg()->choice.voteKickRequestMessage;
	{
		boost::mutex::scoped_lock lock(m_curPetitionIdMutex);
		netVote->petitionId = m_curPetitionId;
	}
	netVote->voteKick = doKick;
	m_ioService->post(boost::bind(&ClientThread::SendSessionPacket, shared_from_this(), packet));
}

void
ClientThread::StartAsyncRead()
{
	ReceiveBuffer &buf = GetContext().GetSessionData()->GetReceiveBuffer();
	GetContext().GetSessionData()->GetAsioSocket()->async_read_some(
		boost::asio::buffer(buf.recvBuf + buf.recvBufUsed, RECV_BUF_SIZE - buf.recvBufUsed),
		boost::bind(
			&ClientThread::HandleRead,
			shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void
ClientThread::HandleRead(const boost::system::error_code& ec, size_t bytesRead)
{
	if (!ec)
	{
		ReceiveBuffer &buf = GetContext().GetSessionData()->GetReceiveBuffer();
		buf.recvBufUsed += bytesRead;
		GetReceiver().ScanPackets(buf);

		while (!buf.receivedPackets.empty())
		{
			boost::shared_ptr<NetPacket> packet = buf.receivedPackets.front();
			buf.receivedPackets.pop_front();
			if (packet)
				GetState().HandlePacket(shared_from_this(), packet);
		}
		StartAsyncRead();
	}
	else
	{
		if (ec != boost::asio::error::operation_aborted)
			throw NetException(__FILE__, __LINE__, ERR_SOCK_CONN_RESET, 0);
	}
}

void
ClientThread::SelectServer(unsigned serverId)
{
	boost::mutex::scoped_lock lock(m_selectServerMutex);
	m_isServerSelected = true;
	m_selectedServerId = serverId;
}

ServerInfo
ClientThread::GetServerInfo(unsigned serverId) const
{
	ServerInfo tmpInfo;
	boost::mutex::scoped_lock lock(m_serverInfoMapMutex);
	ServerInfoMap::const_iterator pos = m_serverInfoMap.find(serverId);
	if (pos != m_serverInfoMap.end())
	{
		tmpInfo = pos->second;
	}
	return tmpInfo;
}

GameInfo
ClientThread::GetGameInfo(unsigned gameId) const
{
	GameInfo tmpInfo;
	boost::mutex::scoped_lock lock(m_gameInfoMapMutex);
	GameInfoMap::const_iterator pos = m_gameInfoMap.find(gameId);
	if (pos != m_gameInfoMap.end())
	{
		tmpInfo = pos->second;
	}
	return tmpInfo;
}

PlayerInfo
ClientThread::GetPlayerInfo(unsigned playerId) const
{
	PlayerInfo info;
	if (!GetCachedPlayerInfo(playerId, info))
	{
		ostringstream name;
		name << "#" << playerId;

		info.playerName = name.str();
	}
	return info;
}

bool
ClientThread::GetPlayerIdFromName(const string &playerName, unsigned &playerId) const
{
	bool retVal = false;

	boost::mutex::scoped_lock lock(m_playerInfoMapMutex);
	PlayerInfoMap::const_reverse_iterator i = m_playerInfoMap.rbegin();
	PlayerInfoMap::const_reverse_iterator end = m_playerInfoMap.rend();

	while (i != end)
	{
		if (i->second.playerName == playerName)
		{
			playerId = i->first;
			retVal = true;
			break;
		}
		++i;
	}
	return retVal;
}

ClientCallback &
ClientThread::GetCallback()
{
	return m_gui;
}

GuiInterface &
ClientThread::GetGui()
{
	return m_gui;
}

AvatarManager &
ClientThread::GetAvatarManager()
{
	return m_avatarManager;
}

void
ClientThread::Main()
{
	// Start sub-threads.
	m_avatarDownloader.reset(new DownloaderThread);
	m_avatarDownloader->Run();
	SetState(CLIENT_INITIAL_STATE::Instance());
	RegisterTimers();

	// Main loop.
	boost::asio::io_service::work ioWork(*m_ioService);
	try
	{
		boost::asio::io_service::work ioWork(*m_ioService);
		m_ioService->run(); // Will only be aborted asynchronously.

	} catch (const PokerTHException &e)
	{
		GetCallback().SignalNetClientError(e.GetErrorId(), e.GetOsErrorCode());
	}
	// Close the socket.
	boost::system::error_code ec;
	GetContext().GetSessionData()->GetAsioSocket()->close();
	// Set a state which does not do anything.
	SetState(CLIENT_FINAL_STATE::Instance());
	// Cancel timers.
	GetStateTimer().cancel();
	CancelTimers();
	// Terminate sub-threads.
	m_avatarDownloader->SignalTermination();
	m_avatarDownloader->Join(DOWNLOADER_THREAD_TERMINATE_TIMEOUT);
}

void
ClientThread::RegisterTimers()
{
	m_avatarTimer.expires_from_now(
		boost::posix_time::milliseconds(CLIENT_AVATAR_LOOP_MSEC));
	m_avatarTimer.async_wait(
		boost::bind(
			&ClientThread::TimerCheckAvatarDownloads, shared_from_this(), boost::asio::placeholders::error));
}

void
ClientThread::CancelTimers()
{
	m_avatarTimer.cancel();
}

void
ClientThread::InitGame()
{
	// EngineFactory erstellen
	boost::shared_ptr<EngineFactory> factory(new ClientEngineFactory); // LocalEngine erstellen

	MapPlayerDataList();
	if (GetPlayerDataList().size() != (unsigned)GetStartData().numberOfPlayers)
		throw ClientException(__FILE__, __LINE__, ERR_NET_INVALID_PLAYER_COUNT, 0);
	m_game.reset(new Game(&m_gui, factory, GetPlayerDataList(), GetGameData(), GetStartData(), m_curGameNum++));
	// Initialize GUI speed.
	GetGui().initGui(GetGameData().guiSpeed);
	// Signal start of game to GUI.
	GetCallback().SignalNetClientGameStart(m_game);
}

void
ClientThread::SendSessionPacket(boost::shared_ptr<NetPacket> packet)
{
	// Put packets in a buffer until the session is established.
	if (IsSessionEstablished())
		GetSender().Send(GetContext().GetSessionData(), packet);
	else
		m_outPacketList.push_back(packet);
}

void
ClientThread::SendQueuedPackets()
{
	if (!m_outPacketList.empty())
	{
		NetPacketList::iterator i = m_outPacketList.begin();
		NetPacketList::iterator end = m_outPacketList.end();

		while (i != end)
		{
			GetSender().Send(GetContext().GetSessionData(), *i);
			++i;
		}
		m_outPacketList.clear();
	}
}

bool
ClientThread::GetCachedPlayerInfo(unsigned id, PlayerInfo &info) const
{
	bool retVal = false;

	boost::mutex::scoped_lock lock(m_playerInfoMapMutex);
	PlayerInfoMap::const_iterator pos = m_playerInfoMap.find(id);
	if (pos != m_playerInfoMap.end())
	{
		info = pos->second;
		retVal = true;
	}
	return retVal;
}

void
ClientThread::RequestPlayerInfo(unsigned id, bool requestAvatar)
{
	if (find(m_playerInfoRequestList.begin(), m_playerInfoRequestList.end(), id) == m_playerInfoRequestList.end())
	{
		boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
		packet->GetMsg()->present = PokerTHMessage_PR_playerInfoRequestMessage;
		PlayerInfoRequestMessage_t *netPlayerInfo = &packet->GetMsg()->choice.playerInfoRequestMessage;
		netPlayerInfo->playerId = id;
		GetSender().Send(GetContext().GetSessionData(), packet);

		m_playerInfoRequestList.push_back(id);

	}
	// Remember that we have to request an avatar.
	if (requestAvatar)
	{
		m_avatarShouldRequestList.push_back(id);
	}
}

void
ClientThread::SetPlayerInfo(unsigned id, const PlayerInfo &info)
{
	{
		boost::mutex::scoped_lock lock(m_playerInfoMapMutex);
		// Remove previous player entry with different id
		// for the same player name if it exists.
		// This can only be one entry, since every time a duplicate
		// name is added one is removed.
		// Only erase non computer player entries.
		if (info.playerName.substr(0, sizeof(SERVER_COMPUTER_PLAYER_NAME) - 1) != SERVER_COMPUTER_PLAYER_NAME)
		{
			PlayerInfoMap::iterator i = m_playerInfoMap.begin();
			PlayerInfoMap::iterator end = m_playerInfoMap.end();
			while (i != end)
			{
				if (i->first != id && i->second.playerName == info.playerName)
				{
					m_playerInfoMap.erase(i);
					break;
				}
				++i;
			}
		}
		m_playerInfoMap[id] = info;
	}

	// Update player data for current game.
	boost::shared_ptr<PlayerData> playerData = GetPlayerDataByUniqueId(id);
	if (playerData.get())
	{
		playerData->SetName(info.playerName);
		playerData->SetType(info.ptype);
		if (info.hasAvatar)
		{
			string avatarFile;
			if (GetAvatarManager().GetAvatarFileName(info.avatar, avatarFile))
			{
				playerData->SetAvatarFile(GetQtToolsInterface().stringToUtf8(avatarFile));
			}
		}
	}

	if (find(m_avatarShouldRequestList.begin(), m_avatarShouldRequestList.end(), id) != m_avatarShouldRequestList.end())
	{
		m_avatarShouldRequestList.remove(id);
		// Retrieve avatar if needed.
		RetrieveAvatarIfNeeded(id, info);
	}

	// Remove it from the request list.
	m_playerInfoRequestList.remove(id);

	// Notify GUI
	GetCallback().SignalNetClientPlayerChanged(id, info.playerName);

}

void
ClientThread::SetUnknownPlayer(unsigned id)
{
	// Just remove it from the request list.
	m_playerInfoRequestList.remove(id);
	m_avatarShouldRequestList.remove(id);
	LOG_ERROR("Server reported unknown player id: " << id);
}

void
ClientThread::SetNewGameAdmin(unsigned id)
{
	// Update player data for current game.
	boost::shared_ptr<PlayerData> playerData = GetPlayerDataByUniqueId(id);
	if (playerData.get())
	{
		playerData->SetRights(PLAYER_RIGHTS_ADMIN);
		GetCallback().SignalNetClientNewGameAdmin(id, playerData->GetName());
	}
}

void
ClientThread::RetrieveAvatarIfNeeded(unsigned id, const PlayerInfo &info)
{
	if (find(m_avatarHasRequestedList.begin(), m_avatarHasRequestedList.end(), id) == m_avatarHasRequestedList.end())
	{
		if (info.hasAvatar && !info.avatar.IsZero() && !GetAvatarManager().HasAvatar(info.avatar))
		{
			m_avatarHasRequestedList.push_back(id); // Never remove from this list. Only request once.

			// Download from avatar server if applicable.
			string avatarServerAddress(GetContext().GetAvatarServerAddr());
			if (!avatarServerAddress.empty() && m_avatarDownloader)
			{
				string serverFileName(info.avatar.ToString() + AvatarManager::GetAvatarFileExtension(info.avatarType));
				m_avatarDownloader->QueueDownload(
					id, avatarServerAddress + serverFileName, GetContext().GetCacheDir() + TEMP_AVATAR_FILENAME);
			}
			else
			{
				boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
				packet->GetMsg()->present = PokerTHMessage_PR_avatarRequestMessage;
				AvatarRequestMessage_t *netAvatar = &packet->GetMsg()->choice.avatarRequestMessage;
				netAvatar->requestId = id;
				OCTET_STRING_fromBuf(&netAvatar->avatar,
									 (const char *)info.avatar.data,
									 MD5_DATA_SIZE);
				GetSender().Send(GetContext().GetSessionData(), packet);
			}
		}
	}
}

void
ClientThread::AddTempAvatarFile(unsigned playerId, unsigned avatarSize, AvatarFileType type)
{
	boost::shared_ptr<AvatarFile> tmpAvatar(new AvatarFile);
	tmpAvatar->fileData.reserve(avatarSize);
	tmpAvatar->fileType = type;
	tmpAvatar->reportedSize = avatarSize;

	m_tempAvatarMap[playerId] = tmpAvatar;
}

void
ClientThread::StoreInTempAvatarFile(unsigned playerId, const vector<unsigned char> &data)
{
	AvatarFileMap::iterator pos = m_tempAvatarMap.find(playerId);
	if (pos == m_tempAvatarMap.end())
		throw ClientException(__FILE__, __LINE__, ERR_NET_INVALID_REQUEST_ID, 0);
	// We trust the server (concerning size of the data).
	std::copy(data.begin(), data.end(), back_inserter(pos->second->fileData));
}

void
ClientThread::CompleteTempAvatarFile(unsigned playerId)
{
	AvatarFileMap::iterator pos = m_tempAvatarMap.find(playerId);
	if (pos == m_tempAvatarMap.end())
		throw ClientException(__FILE__, __LINE__, ERR_NET_INVALID_REQUEST_ID, 0);
	boost::shared_ptr<AvatarFile> tmpAvatar = pos->second;
	unsigned avatarSize = (unsigned)tmpAvatar->fileData.size();
	if (avatarSize != tmpAvatar->reportedSize)
		LOG_ERROR("Client received invalid avatar file size!");
	else
		PassAvatarFileToManager(playerId, tmpAvatar);

	// Free memory.
	m_tempAvatarMap.erase(pos);
}

void
ClientThread::PassAvatarFileToManager(unsigned playerId, boost::shared_ptr<AvatarFile> AvatarFile)
{
	PlayerInfo tmpPlayerInfo;
	if (!GetCachedPlayerInfo(playerId, tmpPlayerInfo))
		LOG_ERROR("Client received invalid player id!");
	else
	{
		if (AvatarFile->fileType == AVATAR_FILE_TYPE_UNKNOWN)
			AvatarFile->fileType = tmpPlayerInfo.avatarType;
		if (!GetAvatarManager().StoreAvatarInCache(tmpPlayerInfo.avatar, AvatarFile->fileType, &AvatarFile->fileData[0], AvatarFile->reportedSize, false))
			LOG_ERROR("Failed to store avatar in cache directory.");

		// Update player info, but never re-request avatar.
		SetPlayerInfo(playerId, tmpPlayerInfo);

		string fileName;
		if (GetAvatarManager().GetAvatarFileName(tmpPlayerInfo.avatar, fileName))
		{
			// Dynamically update avatar in GUI.
			GetGui().setPlayerAvatar(playerId, GetQtToolsInterface().stringToUtf8(fileName));
		}
	}
}

void
ClientThread::SetUnknownAvatar(unsigned playerId)
{
	m_tempAvatarMap.erase(playerId);
	LOG_ERROR("Server reported unknown avatar for player: " << playerId);
}

void
ClientThread::TimerCheckAvatarDownloads(const boost::system::error_code& ec)
{
	if (!ec)
	{
		if (m_avatarDownloader && m_avatarDownloader->HasDownloadResult())
		{
			unsigned playerId;
			boost::shared_ptr<AvatarFile> tmpAvatar(new AvatarFile);
			m_avatarDownloader->GetDownloadResult(playerId, tmpAvatar->fileData);
			tmpAvatar->reportedSize = tmpAvatar->fileData.size();
			PassAvatarFileToManager(playerId, tmpAvatar);
		}
		m_avatarTimer.expires_from_now(
			boost::posix_time::milliseconds(CLIENT_AVATAR_LOOP_MSEC));
		m_avatarTimer.async_wait(
			boost::bind(
				&ClientThread::TimerCheckAvatarDownloads, shared_from_this(), boost::asio::placeholders::error));
	}
}

void
ClientThread::UnsubscribeLobbyMsg()
{
	if (GetContext().GetSubscribeLobbyMsg())
	{
		// Send unsubscribe request.
		boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
		packet->GetMsg()->present = PokerTHMessage_PR_subscriptionRequestMessage;
		SubscriptionRequestMessage_t *netRequest = &packet->GetMsg()->choice.subscriptionRequestMessage;
		netRequest->subscriptionAction = subscriptionAction_unsubscribeGameList;
		GetSender().Send(GetContext().GetSessionData(), packet);
		GetContext().SetSubscribeLobbyMsg(false);
	}
}

void
ClientThread::ResubscribeLobbyMsg()
{
	if (!GetContext().GetSubscribeLobbyMsg())
	{
		// Clear game info map as it is outdated.
		ClearGameInfoMap();
		// Send resubscribe request.
		boost::shared_ptr<NetPacket> packet(new NetPacket(NetPacket::Alloc));
		packet->GetMsg()->present = PokerTHMessage_PR_subscriptionRequestMessage;
		SubscriptionRequestMessage_t *netRequest = &packet->GetMsg()->choice.subscriptionRequestMessage;
		netRequest->subscriptionAction = subscriptionAction_resubscribeGameList;
		GetContext().SetSubscribeLobbyMsg(true);
	}
}

const ClientContext &
ClientThread::GetContext() const
{
	assert(m_context.get());
	return *m_context;
}

ClientContext &
ClientThread::GetContext()
{
	assert(m_context.get());
	return *m_context;
}

void
ClientThread::CreateContextSession()
{
	bool validSocket = false;
	// TODO sctp
	try {
		boost::shared_ptr<tcp::socket> newSock;
		if (GetContext().GetAddrFamily() == AF_INET6)
			newSock.reset(new boost::asio::ip::tcp::socket(*m_ioService, tcp::v6()));
		else
			newSock.reset(new boost::asio::ip::tcp::socket(*m_ioService, tcp::v4()));
		boost::asio::socket_base::non_blocking_io command(true);
		newSock->io_control(command);
		newSock->set_option(tcp::no_delay(true));
		newSock->set_option(boost::asio::socket_base::keep_alive(true));

		GetContext().SetSessionData(boost::shared_ptr<SessionData>(new SessionData(
			newSock,
			SESSION_ID_GENERIC,
			*m_senderCallback)));
		GetContext().SetResolver(boost::shared_ptr<boost::asio::ip::tcp::resolver>(
			new boost::asio::ip::tcp::resolver(*m_ioService)));
		validSocket = true;
	} catch (...)
	{
	}
	if (!validSocket)
		throw ClientException(__FILE__, __LINE__, ERR_SOCK_CREATION_FAILED, 0);
}

ClientState &
ClientThread::GetState()
{
	assert(m_curState);
	return *m_curState;
}

void
ClientThread::SetState(ClientState &newState)
{
	if (m_curState)
		m_curState->Exit(shared_from_this());
	m_curState = &newState;
	m_curState->Enter(shared_from_this());
}

boost::asio::deadline_timer &
ClientThread::GetStateTimer()
{
	return m_stateTimer;
}

SenderHelper &
ClientThread::GetSender()
{
	assert(m_senderHelper);
	return *m_senderHelper;
}

ReceiverHelper &
ClientThread::GetReceiver()
{
	assert(m_receiver);
	return *m_receiver;
}

unsigned
ClientThread::GetGameId() const
{
	boost::mutex::scoped_lock lock(m_curGameIdMutex);
	return m_curGameId;
}

void
ClientThread::SetGameId(unsigned id)
{
	boost::mutex::scoped_lock lock(m_curGameIdMutex);
	m_curGameId = id;
}

const GameData &
ClientThread::GetGameData() const
{
	return m_gameData;
}

void
ClientThread::SetGameData(const GameData &gameData)
{
	m_gameData = gameData;
}

const StartData &
ClientThread::GetStartData() const
{
	return m_startData;
}

void
ClientThread::SetStartData(const StartData &startData)
{
	m_startData = startData;
}

unsigned
ClientThread::GetGuiPlayerId() const
{
	return m_guiPlayerId;
}

void
ClientThread::SetGuiPlayerId(unsigned guiPlayerId)
{
	m_guiPlayerId = guiPlayerId;
}

boost::shared_ptr<Game>
ClientThread::GetGame()
{
	return m_game;
}

QtToolsInterface &
ClientThread::GetQtToolsInterface()
{
	assert(myQtToolsInterface.get());
	return *myQtToolsInterface;
}

void
ClientThread::AddPlayerData(boost::shared_ptr<PlayerData> playerData)
{
	if (playerData.get() && !playerData->GetName().empty())
	{
		m_playerDataList.push_back(playerData);
		if (playerData->GetUniqueId() == GetGuiPlayerId())
			GetCallback().SignalNetClientSelfJoined(playerData->GetUniqueId(), playerData->GetName(), playerData->GetRights());
		else
			GetCallback().SignalNetClientPlayerJoined(playerData->GetUniqueId(), playerData->GetName(), playerData->GetRights());
	}
}

void
ClientThread::RemovePlayerData(unsigned playerId, int removeReason)
{
	boost::shared_ptr<PlayerData> tmpData;

	PlayerDataList::iterator i = m_playerDataList.begin();
	PlayerDataList::iterator end = m_playerDataList.end();
	while (i != end)
	{
		if ((*i)->GetUniqueId() == playerId)
		{
			tmpData = *i;
			m_playerDataList.erase(i);
			break;
		}
		++i;
	}

	if (tmpData.get())
	{
		// Remove player from gui.
		GetCallback().SignalNetClientPlayerLeft(tmpData->GetUniqueId(), tmpData->GetName(), removeReason);
	}
}

void
ClientThread::ClearPlayerDataList()
{
	m_playerDataList.clear();
}

void
ClientThread::MapPlayerDataList()
{
	// Retrieve the GUI player.
	boost::shared_ptr<PlayerData> guiPlayer = GetPlayerDataByUniqueId(GetGuiPlayerId());
	assert(guiPlayer.get());
	int guiPlayerNum = guiPlayer->GetNumber();

	// Create a copy of the player list so that the GUI player
	// is player 0. This is mapped because the GUI depends on it.
	PlayerDataList mappedList;

	PlayerDataList::const_iterator i = m_playerDataList.begin();
	PlayerDataList::const_iterator end = m_playerDataList.end();
	int numPlayers = GetStartData().numberOfPlayers;

	while (i != end)
	{
		boost::shared_ptr<PlayerData> tmpData(new PlayerData(*(*i)));
		int numberDiff = tmpData->GetNumber() - guiPlayerNum;
		if (numberDiff >= 0)
			tmpData->SetNumber(numberDiff);
		else
			tmpData->SetNumber(numPlayers + numberDiff);
		mappedList.push_back(tmpData);
		++i;
	}

	// Sort the list by player number.
	mappedList.sort(*boost::lambda::_1 < *boost::lambda::_2);

	m_playerDataList = mappedList;
}

const PlayerDataList &
ClientThread::GetPlayerDataList() const
{
	return m_playerDataList;
}

boost::shared_ptr<PlayerData>
ClientThread::GetPlayerDataByUniqueId(unsigned id)
{
	boost::shared_ptr<PlayerData> tmpPlayer;

	PlayerDataList::const_iterator i = m_playerDataList.begin();
	PlayerDataList::const_iterator end = m_playerDataList.end();

	while (i != end)
	{
		if ((*i)->GetUniqueId() == id)
		{
			tmpPlayer = *i;
			break;
		}
		++i;
	}
	return tmpPlayer;
}

boost::shared_ptr<PlayerData>
ClientThread::GetPlayerDataByName(const std::string &name)
{
	boost::shared_ptr<PlayerData> tmpPlayer;

	if (!name.empty())
	{
		PlayerDataList::const_iterator i = m_playerDataList.begin();
		PlayerDataList::const_iterator end = m_playerDataList.end();

		while (i != end)
		{
			if ((*i)->GetName() == name)
			{
				tmpPlayer = *i;
				break;
			}
			++i;
		}
	}
	return tmpPlayer;
}

void
ClientThread::RemoveDisconnectedPlayers()
{
	// This should only be called between hands.
	if (m_game.get())
	{
		PlayerListIterator it;
		for (it = m_game->getSeatsList()->begin(); it != m_game->getSeatsList()->end(); it++)
		{
			boost::shared_ptr<PlayerInterface> tmpPlayer = *it;
			if (tmpPlayer->getMyActiveStatus())
			{
				// If a player is not in the player data list, it was disconnected.
				if (!GetPlayerDataByUniqueId(tmpPlayer->getMyUniqueID()).get())
				{
					tmpPlayer->setMyCash(0);
					tmpPlayer->setMyActiveStatus(false);
				}
			}
		}
	}
}

void
ClientThread::AddServerInfo(unsigned serverId, const ServerInfo &info)
{
	{
		boost::mutex::scoped_lock lock(m_serverInfoMapMutex);
		m_serverInfoMap.insert(ServerInfoMap::value_type(serverId, info));
	}
	GetCallback().SignalNetClientServerListAdd(serverId);
}

void
ClientThread::ClearServerInfoMap()
{
	{
		boost::mutex::scoped_lock lock(m_serverInfoMapMutex);
		m_serverInfoMap.clear();
	}
	GetCallback().SignalNetClientServerListClear();
}

bool
ClientThread::GetSelectedServer(unsigned &serverId) const
{
	bool retVal = false;
	boost::mutex::scoped_lock lock(m_selectServerMutex);
	if (m_isServerSelected)
	{
		retVal = true;
		serverId = m_selectedServerId;
	}
	return retVal;
}

void
ClientThread::UseServer(unsigned serverId)
{
	ClientContext &context = GetContext();
	ServerInfo useInfo(GetServerInfo(serverId));

	if (context.GetAddrFamily() == AF_INET6)
		context.SetServerAddr(useInfo.ipv6addr);
	else
		context.SetServerAddr(useInfo.ipv4addr);

	context.SetServerPort((unsigned)useInfo.port);
	context.SetAvatarServerAddr(useInfo.avatarServerAddr);
}

unsigned
ClientThread::GetGameIdByName(const std::string &name) const
{
	// Find the game.
	boost::mutex::scoped_lock lock(m_gameInfoMapMutex);
	GameInfoMap::const_iterator i = m_gameInfoMap.begin();
	GameInfoMap::const_iterator end = m_gameInfoMap.end();
	while (i != end)
	{
		if (i->second.name == name)
			break;
		++i;
	}

	if (i == end)
		throw ClientException(__FILE__, __LINE__, ERR_NET_UNKNOWN_GAME, 0);
	return i->first;
}

void
ClientThread::AddGameInfo(unsigned gameId, const GameInfo &info)
{
	{
		boost::mutex::scoped_lock lock(m_gameInfoMapMutex);
		m_gameInfoMap.insert(GameInfoMap::value_type(gameId, info));
	}
	GetCallback().SignalNetClientGameListNew(gameId);
}

void
ClientThread::UpdateGameInfoMode(unsigned gameId, GameMode mode)
{
	bool found = false;
	{
		boost::mutex::scoped_lock lock(m_gameInfoMapMutex);
		GameInfoMap::iterator pos = m_gameInfoMap.find(gameId);
		if (pos != m_gameInfoMap.end())
		{
			found = true;
			(*pos).second.mode = mode;
		}
	}
	if (found)
		GetCallback().SignalNetClientGameListUpdateMode(gameId, mode);
}

void
ClientThread::UpdateGameInfoAdmin(unsigned gameId, unsigned adminPlayerId)
{
	bool found = false;
	{
		boost::mutex::scoped_lock lock(m_gameInfoMapMutex);
		GameInfoMap::iterator pos = m_gameInfoMap.find(gameId);
		if (pos != m_gameInfoMap.end())
		{
			found = true;
			(*pos).second.adminPlayerId = adminPlayerId;
		}
	}
	if (found)
		GetCallback().SignalNetClientGameListUpdateAdmin(gameId, adminPlayerId);
}

void
ClientThread::RemoveGameInfo(unsigned gameId)
{
	bool found = false;
	{
		boost::mutex::scoped_lock lock(m_gameInfoMapMutex);
		GameInfoMap::iterator pos = m_gameInfoMap.find(gameId);
		if (pos != m_gameInfoMap.end())
		{
			found = true;
			m_gameInfoMap.erase(pos);
		}
	}
	if (found)
		GetCallback().SignalNetClientGameListRemove(gameId);
}

void
ClientThread::ModifyGameInfoAddPlayer(unsigned gameId, unsigned playerId)
{
	bool playerAdded = false;
	{
		boost::mutex::scoped_lock lock(m_gameInfoMapMutex);
		GameInfoMap::iterator pos = m_gameInfoMap.find(gameId);
		if (pos != m_gameInfoMap.end())
		{
			pos->second.players.push_back(playerId);
			playerAdded = true;
		}
	}
	if (playerAdded)
		GetCallback().SignalNetClientGameListPlayerJoined(gameId, playerId);
}

void
ClientThread::ModifyGameInfoRemovePlayer(unsigned gameId, unsigned playerId)
{
	bool playerRemoved = false;
	{
		boost::mutex::scoped_lock lock(m_gameInfoMapMutex);
		GameInfoMap::iterator pos = m_gameInfoMap.find(gameId);
		if (pos != m_gameInfoMap.end())
		{
			pos->second.players.remove(playerId);
			playerRemoved = true;
		}
	}
	if (playerRemoved)
		GetCallback().SignalNetClientGameListPlayerLeft(gameId, playerId);
}

void
ClientThread::ClearGameInfoMap()
{
	boost::mutex::scoped_lock lock(m_gameInfoMapMutex);
	m_gameInfoMap.clear();
}

void
ClientThread::StartPetition(unsigned petitionId, unsigned proposingPlayerId, unsigned kickPlayerId, int timeoutSec, int numVotesToKick)
{
	{
		boost::mutex::scoped_lock lock(m_curPetitionIdMutex);
		m_curPetitionId = petitionId;
	}
	GetGui().startVoteOnKick(kickPlayerId, proposingPlayerId, timeoutSec, numVotesToKick);
	if (GetGuiPlayerId() != kickPlayerId
		&& GetGuiPlayerId() != proposingPlayerId)
	{
		GetGui().changeVoteOnKickButtonsState(true);
	}
}

void
ClientThread::UpdatePetition(unsigned petitionId, int /*numVotesAgainstKicking*/, int numVotesInFavourOfKicking, int numVotesToKick)
{
	bool isCurPetition;
	{
		boost::mutex::scoped_lock lock(m_curPetitionIdMutex);
		isCurPetition = m_curPetitionId == petitionId;
	}
	if (isCurPetition)
	{
		GetGui().refreshVotesMonitor(numVotesInFavourOfKicking, numVotesToKick);
	}
}

void
ClientThread::EndPetition(unsigned petitionId)
{
	bool isCurPetition;
	{
		boost::mutex::scoped_lock lock(m_curPetitionIdMutex);
		isCurPetition = m_curPetitionId == petitionId;
	}
	if (isCurPetition)
		GetGui().endVoteOnKick();
}

void
ClientThread::UpdateStatData(const ServerStats &stats)
{
	boost::mutex::scoped_lock lock(m_curStatsMutex);
	if (stats.numberOfPlayersOnServer)
		m_curStats.numberOfPlayersOnServer = stats.numberOfPlayersOnServer;

	if (stats.totalPlayersEverLoggedIn)
		m_curStats.totalPlayersEverLoggedIn = stats.totalPlayersEverLoggedIn;

	if (stats.totalGamesEverCreated)
		m_curStats.totalGamesEverCreated = stats.totalGamesEverCreated;

	GetCallback().SignalNetClientStatsUpdate(m_curStats);
}

ServerStats
ClientThread::GetStatData() const
{
	boost::mutex::scoped_lock lock(m_curStatsMutex);
	return m_curStats;
}

bool
ClientThread::IsSessionEstablished() const
{
	return m_sessionEstablished;
}

void
ClientThread::SetSessionEstablished(bool flag)
{
	if (m_sessionEstablished != flag)
	{
		m_sessionEstablished = flag;
		if (flag)
			SendQueuedPackets();
	}
}

bool
ClientThread::IsSynchronized() const
{
	return m_playerInfoRequestList.empty();
}

