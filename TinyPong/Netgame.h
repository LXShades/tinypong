#pragma once
#include <string>
#include <WinSock2.h>

#include "Math.h"

class NetAddr {
public:
	NetAddr() = default;
	NetAddr(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned __int16 port_) : octets{a, b, c, d}, port(port_) {};

	union {
		unsigned __int8 octets[4];
		unsigned __int32 ip;
	};
	unsigned __int16 port;

	std::string ToString(bool includePort = true) const;
	sockaddr ToSockAddr() const {
		sockaddr_in addr;
		
		memset(&addr, 0, sizeof (addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.S_un.S_addr = ip;

		return *((sockaddr*)&addr);
	}

	bool operator==(const NetAddr& other) const {
		return ip == other.ip && port == other.port;
	}

	void operator=(const sockaddr& addr_) {
		sockaddr_in* addr = (sockaddr_in*)&addr_;
		ip = addr->sin_addr.S_un.S_addr;
		port = ntohs(addr->sin_port);
	}
};

class Netgame {
public:
	enum NetState {
		Offline = 0,
		Hosting = 1,
		Clienting = 2,
	};

public:
	void Init();
	void Shutdown();

	void Update();

	bool Connect();
	void Disconnect();
	bool Host();

	bool SendToHost(struct NetMessage message);
	bool SendToAll(const struct NetMessage& message);
	bool SendToAddr(const class NetAddr& addr, const struct NetMessage& message);

	bool Receive(NetMessage* messageOut, class NetAddr* senderAddr, class Player** senderPlayer);

private:
	SOCKET sock = -1;
	NetState netState;
	NetAddr localAddr;
	unsigned int lastNetSendTime;

	const int gamePort = 15382;
};

#pragma pack(push, 1)
enum NmType : unsigned char {
	RequestConnect = 1,
	ConfirmConnect = 2,
	ControlMessage = 3,
	RequestQuit = 4,
	ConfirmQuit = 5
};

struct NmBase {
	NmBase() = default;
	NmBase(unsigned int size_, NmType type_) : size(size_), type(type_), playerId(0) {};

	unsigned char size;
	NmType type;
	int playerId;
};

// Net messages
struct NmRequestConnect {
	NmRequestConnect() = default;
	NmRequestConnect(bool initialise) : code(defCode), base(sizeof (NmRequestConnect), NmType::RequestConnect) {};

	NmBase base;
	unsigned short code;
	static const unsigned short defCode = 0xD00D;
};

struct NmConfirmConnect {
	NmConfirmConnect() = default;
	NmConfirmConnect(unsigned short playerId_) : code(0xDEAD), playerId(playerId_), base(sizeof (NmConfirmConnect), NmType::ConfirmConnect) {};

	NmBase base;

	unsigned short code;
	unsigned short playerId;
	static const unsigned short defCode = 0xDEAD;
};

struct NmControlMessagePaddle {
public:
	NmControlMessagePaddle() = default;
	NmControlMessagePaddle(float newPosition, float targetPosition) : position(newPosition), targetPosition(targetPosition) {};

	float position;
	float targetPosition;
};

struct NmControlMessageBall {
	NmControlMessageBall() = default;
	NmControlMessageBall(const Vec2& xy_, const Vec2& velocity_) : xy(xy_), velocity(velocity_) {};

	Vec2 xy;
	Vec2 velocity;
};

struct NmControlMessage {
public:
	enum Type {
		Paddle = 0,
		Ball = 1
	};

public:
	NmControlMessage() = default;
	NmControlMessage(const NmControlMessagePaddle& paddle_) : base(sizeof (NmControlMessage), ControlMessage), paddle(paddle_), objType(Paddle) {};
	NmControlMessage(const NmControlMessageBall& ball_) : base(sizeof (NmControlMessage), ControlMessage), ball(ball_), objType(Ball) {};

public:
	NmBase base;
	Type objType;

	union {
		NmControlMessagePaddle paddle;
		NmControlMessageBall ball;
	};
};

struct NetMessage {
	NetMessage() = default;

	union {
		NmBase base;
		NmControlMessage controlMessage;
		NmRequestConnect requestConnect;
		NmConfirmConnect confirmConnect;
	};
};
#pragma pack(pop)