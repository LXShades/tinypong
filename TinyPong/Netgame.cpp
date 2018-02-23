#include "Netgame.h"
#include "Game.h"

void Netgame::Init() {
	WSADATA wsaData;

	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (ret != 0) {
        return;
    }

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sock == INVALID_SOCKET) {
		return;
	}

	// Setup UDP socket on closest available port
	int attempts = 0;
	for (attempts = 0; attempts < 10; ++attempts) {
		NetAddr localAddr(0, 0, 0, 0, gamePort + attempts);
		sockaddr addr = localAddr.ToSockAddr();

		if (bind(sock, &addr, sizeof (addr)) == 0) {
			localAddr = addr;
			break;
		}
	}

	if (attempts == 10) {
		return;
	}

	// Enable nonblocking
	u_long ahem = 1;
	ioctlsocket(sock, FIONBIO, &ahem);
}

void Netgame::Shutdown() {
	Disconnect();

	WSACleanup();
}

void Netgame::Update() {
	NetMessage message;
	NetAddr sourceAddr;
	Player* sourcePlayer = nullptr;

	// Receive messagees from other players
	while (Receive(&message, &sourceAddr, &sourcePlayer)) {
		if (message.base.type == NmType::RequestConnect) {
			if (netState == Hosting && !(sourceAddr == localAddr)) {
				Player* newPlayer = game.AddPlayer(-1, sourceAddr);
				SendToAddr(sourceAddr, (NetMessage&)NmConfirmConnect(newPlayer->GetId()));
			}
		} else if (message.base.type == NmType::ControlMessage) {
			if (sourcePlayer) {
				sourcePlayer->SubmitControlMessage(message.controlMessage);

				printf("recv from %i obj %i\n", sourcePlayer->GetId(), sourcePlayer->GetPawn()->GetId());
			} else {
				printf("recv from unk\n");
			}
		}
	}

	// Send messages to other players
	if (GetTickCount() >= lastNetSendTime + 16) {
		if (netState == Hosting) {
			for (Player* playerSrc : game.players)  {
				if (!playerSrc->IsAlive() || !playerSrc->GetLastControlMessage()) {
					continue;
				}

				NetMessage message;
				message.controlMessage = *playerSrc->GetLastControlMessage();

				for (Player* playerDest : game.players) {
					if (playerSrc == playerDest || !playerDest->IsAlive()) {
						continue;
					}

					game.netgame.SendToAddr(playerDest->GetAddr(), message);
				}
			}

			lastNetSendTime = GetTickCount();
		} else {
			if (game.localPlayer && game.localPlayer->GetLastControlMessage()) {
				NetMessage message;
				
				message.controlMessage = *game.localPlayer->GetLastControlMessage();

				SendToHost(message);
			}
		}
	}
}

bool Netgame::Connect() {
	sockaddr_in sa;
	INT length;
	
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(gamePort);
	sa.sin_addr.s_addr = INADDR_BROADCAST;

	INT enableBroadcast = 1;
	
	int ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&enableBroadcast, sizeof(enableBroadcast));

	if (ret < 0) {
		printf("Broadcast socket failed\n");
		closesocket(sock);
		return false;
	}

	// Try to find a server and connect to it
	timestamp startWait = GetTickCount();
	NmRequestConnect plzLemmeIn(true);

	sendto(sock, (char*)&plzLemmeIn, sizeof (plzLemmeIn), 0, (sockaddr*)&sa, sizeof (sa));

	while (GetTickCount() - startWait < 1000) {
		NetMessage response;
		NetAddr hostAddr;

		if (Receive(&response, &hostAddr, nullptr)) {
			if (response.base.type == NmType::ConfirmConnect && response.confirmConnect.code == NmConfirmConnect::defCode) {
				printf("Successfully connected\n");

				game.SetLocalPlayerId(response.confirmConnect.playerId);
				game.players[0]->SetAddr(hostAddr);

				netState = Clienting;
				return true;
			}
		}
	}

	return false;
}

bool Netgame::Host() {
	INT enableBroadcast = 1;
	
	int ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&enableBroadcast, sizeof(enableBroadcast));

	netState = Hosting;
	return true;
}

bool Netgame::SendToHost(NetMessage message) {
	message.base.playerId = game.GetLocalPlayer()->GetId();
	return SendToAddr(game.players[0]->GetAddr(), message);
}

bool Netgame::SendToAll(const NetMessage& message) {
	sockaddr_in saBroadcast;

	saBroadcast.sin_family = AF_INET;
	saBroadcast.sin_port = htons(gamePort);
	saBroadcast.sin_addr.s_addr = INADDR_BROADCAST;

	return sendto(sock, (char*)&message, sizeof (message), 0, (sockaddr*)&saBroadcast, sizeof (saBroadcast)) == sizeof (message);
}

bool Netgame::SendToAddr(const NetAddr& addr, const NetMessage& message) {
	sockaddr saAddr = addr.ToSockAddr();

	return sendto(sock, (char*)&message, sizeof (message), 0, &saAddr, sizeof (saAddr)) == sizeof (message);
}

bool Netgame::Receive(NetMessage* messageOut, NetAddr* senderAddr, Player** senderPlayer) {
	if (sock == INVALID_SOCKET)
		return false;

	sockaddr saSenderAddress;
	int addrSize = sizeof(sockaddr);

	int received = recvfrom(sock, (char*)messageOut, sizeof (*messageOut), 0, (sockaddr*)&saSenderAddress, &addrSize);
	
	if (received > 0) {
		NetAddr senderAddress;
		senderAddress = saSenderAddress;

		if (senderAddr) {
			*senderAddr = senderAddress;
		}

		// Find the player that owns this message
		if (netState == Hosting) {
			if (senderPlayer) {
				for (Player* player : game.players) {
					if (player->GetAddr() == senderAddress)
						*senderPlayer = player;
				}
			}
		}
		if (netState == Clienting) {
			// Nevermind--just accept the host's word for it
			Player* p = game.AddPlayer(messageOut->base.playerId, senderAddress);

			if (senderPlayer) {
				*senderPlayer = p;
			}
		}

		return true;
	}

	return false;
}

void Netgame::Disconnect() {
	return;
}

std::string NetAddr::ToString(bool includePort) const {
	// Extremeny inefficient string conversion goes here
	return std::to_string(octets[0]) + "." + std::to_string(octets[1]) + "." + std::to_string(octets[2]) + "." + std::to_string(octets[3]) + ":" + std::to_string(port);
}