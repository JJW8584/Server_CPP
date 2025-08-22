#pragma once

class GameSesison;
class Room;

class Player : public enable_shared_from_this<Player>
{
public:
	Player();
	virtual ~Player();

public:
	Protocol::PlayerInfo* playerInfo;
	weak_ptr<GameSession> session; // cycle

public:
	atomic<weak_ptr<Room>> room;
};