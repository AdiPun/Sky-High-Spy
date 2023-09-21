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
	TYPE_CLOUD,
	TYPE_LASER,
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
	STATE_RESET,
	STATE_PLAY,
	STATE_PAUSED,
	STATE_GAMEOVER,
};

struct Agent8
{
	const int AGENT8_RADIUS{ 40 };
	float AGENT8_SPEED{ 9.0f };
	int Dead_Offsety{ -45 };
	int Walk_Offsety{ 60 };
	float Flying_rotation{ 0.05f };
	float Walking_rotation{ 0.05f };
	float Walking_speed{ 0.2f };
	Point2D Agent8LaserPos{ 0,0 };
};

struct Meteor_info
{
	const int MAX_METEORS = 1;
	float METEOR_SPEED{ 5.0f };
	const int METEOR_RADIUS{ 40 };
	int Offsety = -55;
};

struct Asteroid_info
{
	float ASTEROID_SPEED{ 5.0f };
	const int ASTEROID_RADIUS{ 60 };
	Point2D LANDED_ASTEROID{ 0, 0 };
	const int MAX_ASTEROIDS = 3; // Consts are things you don't want changed during gameplay
	int Offsety = -15;
};

struct Gem_info
{
	const int GEM_SCORE{ 500 };
	const int GEM_RADIUS{ 2 };
	int COLLECTED_GEM{ 0 };
};

struct Ring_info
{
	const int MAX_RINGS = 3;
};

struct AsteroidPieces_info
{
	const int MAX_ASTEROID_PIECES = 3;
	int ASTEROID_PIECE{ 0 };
	const float ASTEROID_PIECE_SPEED{ 10.0f };
};

struct Cloud_info
{
	const int CloudDensity{ 3 };
	const int DeadCloudDensity{ 4 };
	const float FadeRate{ 0.04f };
	const int FlyingSpread{ 10 };
	const int DeadSpread{ 15 };

};

struct TextPositions
{
	Point2f CentrePosition{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 };
	Point2f CentrePosition2{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 10*6 };
	Point2f CentrePosition3{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 10*7 };
	Point2f CentreTop{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 8 };
	Point2f CentreBottomQuarter{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 4 * 3 };
	Point2f CentreBottomFifth{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 5 * 4 };
	Point2f TopRight{ DISPLAY_WIDTH / 10 * 8, DISPLAY_HEIGHT / 10 * 1 };
	Point2f TopRight2{ DISPLAY_WIDTH / 10 * 8, DISPLAY_HEIGHT / 10 * 2 };
};

struct LASER
{
	Point2f obj_agent8_back = { 0,0 };
	int LASER_SPEED = 15;
	Point2f clickPos = { 0,0 };
	float angle = 0;
};

struct GameState // Collection of persisting data to avoid global variables
{
	int score{ 0 };
	bool cheat_1 = false;
	bool cheat_2 = false;
	int level{ 0 };
	int gemsCollected{ 0 };
	int levelTimer{ 0 };
	AsteroidPieces_info piece;
	AgentStates agentState = STATE_FLYING;
	Agent8 agent8;
	Meteor_info meteor;
	Asteroid_info asteroid;
	Gem_info gem;
	Ring_info ring;
	Cloud_info cloud;
	PlayStates pState{ STATE_WELCOME };
	TextPositions position;
	LASER LASER;
};

struct GameState gState;