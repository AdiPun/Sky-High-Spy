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
	const int MAX_ASTEROID_PIECES = 3;
	int ASTEROID_PIECE{ 0 };
	const float ASTEROID_PIECE_SPEED{ 10.0f };
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

struct GameState gState;