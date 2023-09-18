#ifndef GAME_HEADER_H
#define GAME_HEADER_H

#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

const int DISPLAY_WIDTH = 1280;
const int DISPLAY_HEIGHT = 720;
const int DISPLAY_SCALE = 1;

enum GameObjectTypes
{
	TYPE_AGENT8 = 0,
	TYPE_METEOR,
	TYPE_ASTEROID,
	TYPE_PIECES,
	TYPE_GEM,
	TYPE_RING,
	TYPE_LANDED_ON,
};

enum AgentStates
{
	STATE_FLYING = 0,
	STATE_LANDED,
	STATE_DEAD,
};

enum PlayStates
{
	STATE_WELCOME = 0,
	STATE_PLAY,
	STATE_PAUSE,
	STATE_GAMEOVER,
};

struct Agent8
{
	const int AGENT8_RADIUS{ 40 };
	float AGENT8_SPEED{ 9.0f };
	const Point2D AGENT8_AABB{ 40, 40 };
	int Dead_Offsety{ -45 };
	int Walk_Offsety{ 60 };
	int WrapOnce{ 0 };
};

struct Meteors
{
	const int MAX_METEORS = 1;
	float METEOR_SPEED{ 5.0f };
	const int METEOR_RADIUS{ 40 };
	int Offsety = -55;
};

struct Asteroids
{
	float ASTEROID_SPEED{ 5.0f };

	const int ASTEROID_RADIUS{ 60 };
	const Point2D ASTEROID_AABB{ 60, 60 };
	Point2D LANDED_ASTEROID{ 0, 0 };
	const int MAX_ASTEROIDS = 4; // Consts are things you don't want changed during gameplay
	int Offsety = -15;
};

struct Gems
{
	const int GEM_SCORE{ 500 };
	const int GEM_RADIUS{ 5 };
	int COLLECTED_GEM{ 0 };
};

struct Rings
{
	const int MAX_RINGS = 3;
};

struct AsteroidPieces
{
	float angle;
	float speed;
	const int MAX_ASTEROID_PIECES = 3;
	int ASTEROID_PIECE{ 0 };
	float ASTEROID_PIECE_SPEED{ 10.0f };
};

struct GameState
{
	int score{ 0 };
	AsteroidPieces piece;
	AgentStates agentState = STATE_FLYING;
	Agent8 agent8;
	Meteors meteor;
	Asteroids asteroid;
	Gems gem;
	Rings ring;
	PlayStates pState;
};

extern GameState gState;

// Function prototypes
void Draw();
void HandleFlyingControls();
void HandleLandedControls();
void UpdateMeteors();
void UpdateAsteroids();
void UpdateAsteroidPiece();
void UpdateAgent8();
void CreateAsteroidPieces();
void CreateRings(Point2D pos);
void UpdateGem();
void UpdateRing();
void SetVelocityObject(GameObject& obj, float speed);
void WrapObject(GameObject& obj, int ObjectWidth, int ObjectHeight);
void SpawnObject(int type, const char* spriteName, int MAX_OBJECTS, int collisionRadius, Point2f position, float rotation);
void DrawAllGameObjectsOfType(int type);
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE);
bool MainGameUpdate(float elapsedTime);
int MainGameExit();

#endif // GAME_HEADER_H
