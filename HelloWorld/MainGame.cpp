#include "MainGame.h"

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

// The entry point for a Windows program
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	Play::CentreAllSpriteOrigins();
	Play::LoadBackground("Data\\Backgrounds\\background.png");
	Play::CreateGameObject(TYPE_AGENT8, { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, 40, "agent8_fly");
	Play::MoveSpriteOrigin("agent8_dead", 0, gState.agent8.Dead_Offsety);
	Play::MoveSpriteOrigin("meteor", 0, gState.meteor.Offsety);
	Play::MoveSpriteOrigin("asteroid", 0, gState.asteroid.Offsety);
	Play::MoveSpriteOrigin("agent8_left", 0, gState.agent8.Walk_Offsety);
	Play::MoveSpriteOrigin("agent8_right", 0, gState.agent8.Walk_Offsety);
	SpawnObject(TYPE_METEOR, "meteor", gState.meteor.MAX_METEORS, 40, { DISPLAY_WIDTH / 9, DISPLAY_HEIGHT / 9 }, 0);
	SpawnObject(TYPE_ASTEROID, "asteroid", gState.asteroid.MAX_ASTEROIDS, 60, { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, 0);

	//Play::StartAudioLoop( "music" );
}

// Called by the PlayBuffer once for each frame of the game (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{
	UpdateAgent8();
	UpdateAsteroids();
	UpdateMeteors();
	UpdateAsteroidPiece();
	UpdateGem();
	UpdateRing();
	Draw();
	return Play::KeyDown(VK_ESCAPE);
}


void HandleFlyingControls()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

	SetVelocityObject(obj_agent8, gState.agent8.AGENT8_SPEED);

	if (Play::KeyDown(VK_LEFT))
	{
		obj_agent8.rotation -= 0.05f;
	}
	if (Play::KeyDown(VK_RIGHT))
	{
		obj_agent8.rotation += 0.05f;
	}

	WrapObject(obj_agent8, gState.agent8.AGENT8_RADIUS, gState.agent8.AGENT8_RADIUS);
}

void HandleLandedControls()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	obj_agent8.animSpeed = 0.1f;
	obj_agent8.pos = Play::GetGameObjectByType(TYPE_LANDED_ON).pos; // Getting the object reference from the stored asteroid ID

	Play::SetSprite(obj_agent8, "agent8_left", 0);

	if (Play::KeyDown(VK_LEFT))
	{
		Play::SetSprite(obj_agent8, "agent8_left", 0.5f);
		obj_agent8.rotation -= 0.1f;
	}
	if (Play::KeyDown(VK_RIGHT))
	{
		Play::SetSprite(obj_agent8, "agent8_right", 0.5f);
		obj_agent8.rotation += 0.1f;
	}
	if (Play::KeyPressed(VK_SPACE))
	{
		GameObject& obj_landed_asteroid = Play::GetGameObjectByType(TYPE_LANDED_ON);
		gState.asteroid.LANDED_ASTEROID = obj_landed_asteroid.pos;


		Play::PlayAudio("explode"); // Create Function what happens when asteroid is destroyed


		obj_agent8.pos.x += 10 * gState.agent8.AGENT8_SPEED * sin(obj_agent8.rotation); // This is to make sure that agent8 doesn't look like he's jumping from the centre of the asteroid. This adds the velocity onto the pos. This works because velocity has direction!!!!
		obj_agent8.pos.y += 10 * gState.agent8.AGENT8_SPEED * -cos(obj_agent8.rotation);

		CreateAsteroidPieces();

		gState.agentState = STATE_FLYING;

		Play::SetSprite(obj_agent8, "agent8_fly", 1.0f);

		Play::CreateGameObject(TYPE_GEM, gState.asteroid.LANDED_ASTEROID, 20, "gem");

		Play::DestroyGameObjectsByType(TYPE_LANDED_ON);
	}
}

void UpdateMeteors()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	std::vector<int> vMeteors = Play::CollectGameObjectIDsByType(TYPE_METEOR);
	for (int meteor_id : vMeteors)
	{
		GameObject& obj_meteor = Play::GetGameObject(meteor_id);
		obj_meteor.animSpeed = 0.1f;
		SetVelocityObject(obj_meteor, gState.meteor.METEOR_SPEED);
		WrapObject(obj_meteor, 130, 130);

		if (Play::IsColliding(obj_agent8, obj_meteor) && gState.agentState == STATE_FLYING)
		{
			gState.agentState = STATE_DEAD;
		}

		Play::UpdateGameObject(obj_meteor);
	}
}

void UpdateAsteroids()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	std::vector<int> vAsteroids = Play::CollectGameObjectIDsByType(TYPE_ASTEROID);

	for (int asteroid_id : vAsteroids)
	{
		GameObject& obj_asteroid = Play::GetGameObject(asteroid_id);
		obj_asteroid.animSpeed = 0.1f;
		SetVelocityObject(obj_asteroid, gState.asteroid.ASTEROID_SPEED);
		WrapObject(obj_asteroid, 130, 130);

		if (Play::IsColliding(obj_agent8, obj_asteroid) && gState.agentState == STATE_FLYING)
		{
			gState.agentState = STATE_LANDED;
			obj_asteroid.type = TYPE_LANDED_ON; // Changing the asteroid's type to type landed to call it again
			obj_agent8.rotation += Play::DegToRad(180);
		}

		Play::UpdateGameObject(obj_asteroid);
	}

	GameObject& obj_landed = Play::GetGameObjectByType(TYPE_LANDED_ON);
	obj_landed.animSpeed = 0.1f;
	SetVelocityObject(obj_landed, gState.asteroid.ASTEROID_SPEED);
	WrapObject(obj_landed, 130, 130);
	Play::UpdateGameObject(obj_landed);
}

void UpdateAgent8()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

	switch (gState.agentState)
	{
	case STATE_FLYING:
		HandleFlyingControls();
		break;
	case STATE_LANDED:
		HandleLandedControls();
		break;
	case STATE_DEAD:
		Play::SetSprite(obj_agent8, "agent8_dead", 0); // Add a agent8 struct that's called landed = true active = true
		obj_agent8.animSpeed = 0.1f;
		WrapObject(obj_agent8, 130, 130);
		SetVelocityObject(obj_agent8, gState.agent8.AGENT8_SPEED);
		obj_agent8.acceleration = { 0.05f, 0.05f };
		gState.pState = STATE_GAMEOVER;
		break;
	}
	Play::UpdateGameObject(obj_agent8);
}



void UpdateGem()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	std::vector<int> vGems = Play::CollectGameObjectIDsByType(TYPE_GEM);

	for (int gem_id : vGems)
	{
		GameObject& obj_gem = Play::GetGameObject(gem_id);
		WrapObject(obj_gem, gState.gem.GEM_RADIUS, gState.gem.GEM_RADIUS);

		if (Play::IsColliding(obj_agent8, obj_gem) && gState.agentState == STATE_FLYING)
		{
			CreateRings(obj_gem.pos);
			Play::PlayAudio("reward");
			gState.score += gState.gem.GEM_SCORE;
			Play::DestroyGameObject(gem_id);
		}

		Play::UpdateGameObject(obj_gem);
	}
}

void CreateRings(Point2D pos)
{
	for (int i = 0; i < gState.ring.MAX_RINGS; i++)
	{
		int ring_id = Play::CreateGameObject(TYPE_RING, pos, 0, "blue_ring");
		GameObject& obj_ring = Play::GetGameObject(ring_id);
		obj_ring.scale = 1 + 0.2 * i;
	}
}

void UpdateRing()
{
	std::vector<int> vRings = Play::CollectGameObjectIDsByType(TYPE_RING);

	for (int ring : vRings)
	{
		GameObject& obj_ring = Play::GetGameObject(ring);

		obj_ring.scale += 0.01f; // Every time this function is called, the scale increase
		obj_ring.opacity -= 0.05f;

		Play::UpdateGameObject(obj_ring);

		if (obj_ring.opacity < 0)
		{
			Play::DestroyGameObject(ring);
		}
	}
}

void CreateAsteroidPieces()
{
	for (int i = 0; i < gState.piece.MAX_ASTEROID_PIECES; i++)
	{
		int piece_id = Play::CreateGameObject(TYPE_PIECES, gState.asteroid.LANDED_ASTEROID, 0, "asteroid_pieces");
		GameObject& obj_piece = Play::GetGameObject(piece_id);
		Play::SetGameObjectDirection(obj_piece, gState.piece.ASTEROID_PIECE_SPEED, -i * Play::RadToDeg(120));
		obj_piece.frame = i;
	}
}

void UpdateAsteroidPiece()
{
	std::vector<int> vPieces = Play::CollectGameObjectIDsByType(TYPE_PIECES);
	for (int piece_id : vPieces)
	{
		GameObject& obj_piece = Play::GetGameObject(piece_id);

		Play::UpdateGameObject(Play::GetGameObject(piece_id));

		if (!Play::IsVisible(Play::GetGameObject(piece_id))) // IsVisible waits for the sprite to completely be off screen vs Isleavingdsiplay area
		{
			Play::DestroyGameObject(piece_id);
		}
	}
}
// Takes a game object and what speed you want and makes it move in the direction it is facing
void SetVelocityObject(GameObject& obj, float speed)
{
	float x = sin(obj.rotation);
	float y = cos(obj.rotation);

	obj.velocity.x = x * speed;
	obj.velocity.y = -y * speed;
}

// Takes a game object, width and height and loops the object around the screen
void WrapObject(GameObject& obj, int ObjectWidth, int ObjectHeight)
{
	if (obj.pos.x + ObjectWidth < 0)
	{
		obj.pos.x = DISPLAY_WIDTH + ObjectWidth;
	}
	if (obj.pos.x - ObjectWidth > DISPLAY_WIDTH)
	{
		obj.pos.x = 0 - ObjectWidth;
	}
	if (obj.pos.y + ObjectHeight < 0)
	{
		obj.pos.y = DISPLAY_HEIGHT + ObjectHeight;
	}
	if (obj.pos.y - ObjectHeight > DISPLAY_HEIGHT)
	{
		obj.pos.y = 0 - ObjectHeight;
	}
}


// Spawns game objects at the quantity you want
void SpawnObject(int type, const char* spriteName, int MAX_OBJECTS, int collisionRadius, Point2f position = { DISPLAY_HEIGHT / 2,DISPLAY_WIDTH / 2 }, float rotation = (0))
{
	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		int id = Play::CreateGameObject(type, position, collisionRadius, spriteName);
		GameObject& obj = Play::GetGameObject(id);
		obj.rotation = Play::DegToRad(rotation);
	}
}

void Draw()
{
	Play::DrawBackground();
	DrawAllGameObjectsOfType(TYPE_METEOR);
	DrawAllGameObjectsOfType(TYPE_ASTEROID);
	DrawAllGameObjectsOfType(TYPE_LANDED_ON);
	DrawAllGameObjectsOfType(TYPE_PIECES);
	DrawAllGameObjectsOfType(TYPE_GEM);
	DrawAllGameObjectsOfType(TYPE_RING);


	Play::DrawObjectRotated(Play::GetGameObjectByType(TYPE_AGENT8));
	Play::DrawFontText("font151px", "SCORE: " + std::to_string(gState.score), { DISPLAY_WIDTH / 2,DISPLAY_HEIGHT / 6 }, Play::CENTRE);
	Play::PresentDrawingBuffer();
}

// For game objects that have multiple instances of them, collects all the gameobject ids of the type
// Draws them rotated
void DrawAllGameObjectsOfType(int type)
{
	for (int id : Play::CollectGameObjectIDsByType(type))
	{
		GameObject& obj = Play::GetGameObject(id);
		Play::DrawObjectRotated(obj, obj.opacity); // by default it already has opacity and it's 1. We're telling it to draw object at the object's opacity. We can still reuse this function because every other object's opacity is 1 anyway. We're going to edit the ring's opacity.
	}
}

// Gets called once when the player quits the game
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}


