#include "MainGame.h"

// To Do


// Functions MainGame
void UpdateAgent8();
void UpdateAgent8Dead();
void UpdateAsteroids();
void UpdateLandedAsteroid();
void DestroyAsteroid();
void HandleFlyingControls();
void HandleLandedControls();
void UpdateLaser();
void CreateAsteroidPieces();
void UpdateAsteroidPiece();
void UpdateMeteors();
void UpdateGem();
void CreateRings(Point2D pos);
void UpdateRing();
void CreateClouds(int CloudSpread, int CloudDensity);
void UpdateClouds();
void Pause();

// Functions transferable
void SetVelocitytoObjectDirection(GameObject& obj, float speed);
void WrapObject(GameObject& obj, int ObjectWidth, int ObjectHeight);
void SpawnObject(int type, const char* spriteName, int MAX_OBJECTS, int collisionRadius);
void Draw();
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
	Play::StartAudioLoop( "music" );
}

// Called by the PlayBuffer once for each frame of the game (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{
	switch (gState.pState)
	{
	case STATE_RESET:
		gState.cheat_1 = false;
		gState.cheat_2 = false;
		gState.score = 0;
		Play::DestroyGameObjectsByType(TYPE_METEOR);
		Play::DestroyGameObjectsByType(TYPE_ASTEROID);
		Play::DestroyGameObjectsByType(TYPE_GEM);
		Play::DestroyGameObjectsByType(TYPE_LASER);
		SpawnObject(TYPE_METEOR, "meteor", gState.meteor.MAX_METEORS + (gState.level), 40);
		SpawnObject(TYPE_ASTEROID, "asteroid", gState.asteroid.MAX_ASTEROIDS + gState.level, 60);
		gState.pState = STATE_PLAY;
		break;
	case STATE_PLAY:
		UpdateAgent8();
		UpdateAsteroids();
		UpdateLandedAsteroid();
		UpdateLaser();
		UpdateMeteors();
		UpdateAsteroidPiece();
		UpdateGem();
		UpdateRing();
		UpdateClouds();
		if (Play::KeyPressed(VK_DELETE))
		{
			gState.pState = STATE_PAUSED;
			Play::PlayAudio("pause");
			Play::StopAudioLoop("music");
		}
		break;
	case STATE_PAUSED:
		Pause();
		break;
	case STATE_GAMEOVER:
		UpdateAgent8();
		UpdateAsteroids();
		UpdateMeteors();
		UpdateAsteroidPiece();
		UpdateGem();
		UpdateRing();
		UpdateClouds();
		break;
	}

	Draw();


	return Play::KeyDown(VK_ESCAPE);
}

void UpdateAgent8()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

	switch (gState.agentState)
	{
	case STATE_FLYING:
		CreateClouds(gState.cloud.FlyingSpread, gState.cloud.CloudDensity);
		HandleFlyingControls();
		break;
	case STATE_LANDED:
		HandleLandedControls();
		break;
	case STATE_DEAD:
		UpdateAgent8Dead();

		gState.pState = STATE_GAMEOVER;
		if (Play::KeyPressed(VK_SPACE))
		{
			gState.pState = STATE_RESET;
			gState.agentState = STATE_FLYING;
		}
		break;
	}
	Play::UpdateGameObject(obj_agent8);
}

void UpdateAgent8Dead()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	CreateClouds(gState.cloud.DeadSpread, gState.cloud.DeadCloudDensity);
	Play::SetSprite(obj_agent8, "agent8_dead", 0);
	obj_agent8.animSpeed = 0.1f;
	WrapObject(obj_agent8, 130, 130);
	SetVelocitytoObjectDirection(obj_agent8, gState.agent8.AGENT8_SPEED);
	obj_agent8.acceleration = { 0.05f, 0.05f };
}


void UpdateAsteroids()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	std::vector<int> vAsteroids = Play::CollectGameObjectIDsByType(TYPE_ASTEROID);

	for (int asteroid_id : vAsteroids)
	{
		GameObject& obj_asteroid = Play::GetGameObject(asteroid_id);
		obj_asteroid.animSpeed = 0.1f;
		SetVelocitytoObjectDirection(obj_asteroid, gState.asteroid.ASTEROID_SPEED);
		WrapObject(obj_asteroid, 130, 130);

		if (Play::IsColliding(obj_agent8, obj_asteroid) && gState.agentState == STATE_FLYING)
		{
			gState.agentState = STATE_LANDED;
			obj_asteroid.type = TYPE_LANDED_ON; // Changing the asteroid's type to type landed to call it again
			obj_agent8.rotation += Play::DegToRad(180);
			Play::SetSprite(obj_agent8, "agent8_left", 0);
		}

		Play::UpdateGameObject(obj_asteroid);
	}
}

void UpdateLandedAsteroid()
{
	GameObject& obj_landed = Play::GetGameObjectByType(TYPE_LANDED_ON);
	obj_landed.animSpeed = 0.1f;
	SetVelocitytoObjectDirection(obj_landed, gState.asteroid.ASTEROID_SPEED);
	WrapObject(obj_landed, 130, 130);
	Play::UpdateGameObject(obj_landed);
}

void DestroyAsteroid()
{
	
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	GameObject& obj_landed_asteroid = Play::GetGameObjectByType(TYPE_LANDED_ON);
	

	Play::PlayAudio("explode"); // Create Function what happens when asteroid is destroyed
	CreateAsteroidPieces();


	Play::CreateGameObject(TYPE_GEM, gState.asteroid.LANDED_ASTEROID, gState.gem.GEM_RADIUS, "gem");


	Play::DestroyGameObjectsByType(TYPE_LANDED_ON);
}

void HandleFlyingControls()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	Play::SetSprite(obj_agent8, "agent8_fly", 0);

	SetVelocitytoObjectDirection(obj_agent8, gState.agent8.AGENT8_SPEED);

	if (Play::KeyDown(VK_LEFT))
	{
		obj_agent8.rotation -= gState.agent8.Flying_rotation;
	}
	if (Play::KeyDown(VK_RIGHT))
	{
		obj_agent8.rotation += gState.agent8.Flying_rotation;
	}

	WrapObject(obj_agent8, gState.agent8.AGENT8_RADIUS, gState.agent8.AGENT8_RADIUS);
}

void HandleLandedControls()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	GameObject& obj_landed_asteroid = Play::GetGameObjectByType(TYPE_LANDED_ON);
	obj_agent8.pos = Play::GetGameObjectByType(TYPE_LANDED_ON).pos; // Getting the object reference from the stored asteroid ID

	if (Play::KeyDown(VK_LEFT))
	{
		Play::SetSprite(obj_agent8, "agent8_left", gState.agent8.Walking_speed);
		obj_agent8.animSpeed = gState.agent8.Walking_speed;
		obj_agent8.rotation -= gState.agent8.Walking_rotation;
	}
	else if (Play::KeyDown(VK_RIGHT))
	{
		Play::SetSprite(obj_agent8, "agent8_right", gState.agent8.Walking_speed);
		obj_agent8.animSpeed = gState.agent8.Walking_speed;
		obj_agent8.rotation += gState.agent8.Walking_rotation;
	}
	else
	{
		obj_agent8.animSpeed = 0;
	}

	if (Play::KeyPressed(VK_SPACE))
	{
		gState.agentState = STATE_FLYING;
		Play::SetSprite(obj_agent8, "agent8_fly", 1.0f);
		gState.asteroid.LANDED_ASTEROID = obj_landed_asteroid.pos;
		obj_agent8.pos.x += 10 * gState.agent8.AGENT8_SPEED * sin(obj_agent8.rotation); // This is to make sure that agent8 doesn't look like he's jumping from the centre of the asteroid. This adds the velocity onto the pos. This works because velocity has direction!!!!
		obj_agent8.pos.y += 10 * gState.agent8.AGENT8_SPEED * -cos(obj_agent8.rotation);
		DestroyAsteroid();
	}
	if (gState.cheat_1 == true)
	{
		if (Play::KeyDown(VK_LBUTTON))
		{	
			int laser_id = Play::CreateGameObject(TYPE_LASER, obj_agent8.pos, 2, "laser");
			GameObject& laser_obj = Play::GetGameObject(laser_id);
			
			Vector2D laserdirection = Play::GetMousePos() - obj_agent8.pos;

			laserdirection.Normalize();

			laser_obj.velocity = laserdirection * gState.LASER.LASER_SPEED;

			laser_obj.rotation = -atan2(laserdirection.x, laserdirection.y);
		}
	}
}

void UpdateLaser()
{
	std::vector<int> vAsteroids = Play::CollectGameObjectIDsByType(TYPE_ASTEROID);
	std::vector<int> vMeteors = Play::CollectGameObjectIDsByType(TYPE_METEOR);
	std::vector<int> vLASERS = Play::CollectGameObjectIDsByType(TYPE_LASER);

	for (int id_laser : vLASERS)
	{
		GameObject& obj_laser = Play::GetGameObject(id_laser);
		bool hasCollided = false;

		for (int id_asteroid : vAsteroids)
		{
			GameObject& obj_asteroid = Play::GetGameObject(id_asteroid);
			if (Play::IsColliding(obj_laser, obj_asteroid) && gState.cheat_1 == true)
			{
				hasCollided = true;
				Play::PlayAudio("explode");
				for (int i = 0; i < gState.piece.MAX_ASTEROID_PIECES; i++)
				{
					int piece_id = Play::CreateGameObject(TYPE_PIECES, obj_asteroid.pos, 0, "asteroid_pieces");
					GameObject& obj_piece = Play::GetGameObject(piece_id);
					Play::SetGameObjectDirection(obj_piece, gState.piece.ASTEROID_PIECE_SPEED, -i * Play::RadToDeg(120));
					obj_piece.frame = i;
				}
				Play::CreateGameObject(TYPE_GEM, obj_asteroid.pos, gState.gem.GEM_RADIUS, "gem");
				Play::DestroyGameObject(id_asteroid);
			}
		}
		for (int id_meteor : vMeteors)
		{
			GameObject& obj_meteor = Play::GetGameObject(id_meteor);
			if (Play::IsColliding(obj_laser, obj_meteor) && gState.cheat_1 == true)
			{
				hasCollided = true;
				Play::PlayAudio("clang"); 
				Play::CreateGameObject(TYPE_GEM, obj_meteor.pos, gState.gem.GEM_RADIUS, "gem");
				Play::DestroyGameObject(id_meteor);
			}
		}

		Play::UpdateGameObject(obj_laser);

		if (!Play::IsVisible(obj_laser) || hasCollided)
		{
			Play::DestroyGameObject(id_laser);
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

void UpdateMeteors()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	std::vector<int> vMeteors = Play::CollectGameObjectIDsByType(TYPE_METEOR);
	for (int meteor_id : vMeteors)
	{
		GameObject& obj_meteor = Play::GetGameObject(meteor_id);
		obj_meteor.animSpeed = 0.1f;
		SetVelocitytoObjectDirection(obj_meteor, gState.meteor.METEOR_SPEED);
		WrapObject(obj_meteor, 130, 130);

		if (Play::IsColliding(obj_agent8, obj_meteor) && gState.agentState == STATE_FLYING)
		{
			gState.agentState = STATE_DEAD;
			Play::PlayAudio("combust");
		}

		Play::UpdateGameObject(obj_meteor);
	}
}

void UpdateGem()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	std::vector<int> vGems = Play::CollectGameObjectIDsByType(TYPE_GEM);

	for (int gem_id : vGems)
	{
		GameObject& obj_gem = Play::GetGameObject(gem_id);
		WrapObject(obj_gem, gState.gem.GEM_RADIUS, gState.gem.GEM_RADIUS);
		obj_gem.rotSpeed = 0.01f;

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

void CreateClouds(int CloudSpread, int CloudDensity)
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	for (int i = 0; i < CloudDensity; i++)
	{
		Play::CreateGameObject(TYPE_CLOUD,
			{ obj_agent8.oldPos.x + Play::RandomRollRange(CloudSpread,CloudSpread),
			obj_agent8.oldPos.y + Play::RandomRollRange(-CloudSpread,	CloudSpread) },
			0, "particle");
	}
}

void UpdateClouds()
{
	// for each item in TYPE_CLOUDS
	// function that takes gametimer and uses it to fade the cloud out
	// if the opacity is =< 0, destroy the cloud_id
	std::vector<int> vClouds = Play::CollectGameObjectIDsByType(TYPE_CLOUD);
	for (int cloud_id : vClouds)
	{
		GameObject& cloud_obj = Play::GetGameObject(cloud_id);

		cloud_obj.opacity -= gState.cloud.FadeRate;

		Play::UpdateGameObject(cloud_obj);

		if (cloud_obj.opacity <= 0)
		{
			Play::DestroyGameObject(cloud_id);
		}
	}
}

void Pause()
{
	if (Play::KeyPressed(VK_DELETE) && gState.pState == STATE_PAUSED)
	{
		gState.pState = STATE_PLAY;
		Play::PlayAudio("pause");
		Play::StartAudioLoop("music");
	}
	if (Play::KeyPressed(VK_CONTROL) && gState.pState == STATE_PAUSED && gState.cheat_1 == false)
	{
		Play::PlayAudio("cheat");
		gState.cheat_1 = true;
	}
	else if (Play::KeyPressed(VK_CONTROL) && gState.pState == STATE_PAUSED && gState.cheat_1 == true)
	{
		Play::PlayAudio("cheat");
		gState.cheat_1 = false;
	}

	if (Play::KeyPressed(VK_DOWN) && gState.pState == STATE_PAUSED && gState.cheat_1 == false)
	{
		Play::PlayAudio("cheat");
		gState.cheat_2 = true;
	}
	else if (Play::KeyPressed(VK_DOWN) && gState.pState == STATE_PAUSED && gState.cheat_2 == true)
	{
		Play::PlayAudio("cheat");
		gState.cheat_2 = false;
	}
}

// Transferable Functions

// Takes a game object and what speed you want and makes it move in the direction it is facing
void SetVelocitytoObjectDirection(GameObject& obj, float speed)
{
	obj.velocity.x = sin(obj.rotation) * speed;
	obj.velocity.y = -cos(obj.rotation) * speed;
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
void SpawnObject(int type, const char* spriteName, int MAX_OBJECTS, int collisionRadius)
{
	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		int id = Play::CreateGameObject(type, { Play::RandomRollRange(0,DISPLAY_WIDTH),
			Play::RandomRollRange(0,DISPLAY_HEIGHT) }, collisionRadius, spriteName);
		GameObject& obj = Play::GetGameObject(id);
		obj.rotation = Play::RandomRollRange(0, Play::DegToRad(360));
	}
}

// collect game objects by type if nothing will return a length of 0 use .size
// if 0 that means all gems collected

void Draw()
{
	Play::DrawBackground();
	DrawAllGameObjectsOfType(TYPE_METEOR);
	DrawAllGameObjectsOfType(TYPE_ASTEROID);
	DrawAllGameObjectsOfType(TYPE_LANDED_ON);
	DrawAllGameObjectsOfType(TYPE_PIECES);
	DrawAllGameObjectsOfType(TYPE_GEM);
	DrawAllGameObjectsOfType(TYPE_RING);
	DrawAllGameObjectsOfType(TYPE_CLOUD);
	DrawAllGameObjectsOfType(TYPE_LASER);
	Play::DrawObjectRotated(Play::GetGameObjectByType(TYPE_AGENT8));

	if (gState.pState == STATE_PAUSED)
	{
		Play::DrawFontText("font151px", "SCORE: " + std::to_string(gState.score), { DISPLAY_WIDTH / 2,DISPLAY_HEIGHT / 6 }, Play::CENTRE);
	}

	if (gState.pState == STATE_PAUSED)
	{
		Play::DrawFontText("font151px", "PAUSED", gState.position.CentrePosition, Play::CENTRE);
	}
	if (gState.pState == STATE_GAMEOVER)
	{
		Play::DrawFontText("font151px",
			"GAME OVER",
			gState.position.CentrePosition,
			Play::CENTRE);
		Play::DrawFontText("font151px",
			"Press space to play again",
			gState.position.CentreBottomQuarter,
			Play::CENTRE);
	}
	if (gState.pState == STATE_WELCOME)
	{
		Play::DrawFontText("font105px", "Arrow keys to rotate, space to jump, DEL to pause\nDestroy all the asteroids and collect all gems to get to the next level\nAvoid the meteor!", gState.position.CentrePosition, Play::CENTRE);
		Play::DrawFontText("font151px", "welcome to SkyHighSpy", gState.position.CentreTOP, Play::CENTRE);
		Play::DrawFontText("font64px",
			"Press space to play",
			gState.position.CentreBottomQuarter,
			Play::CENTRE);

	}

	if (gState.cheat_1 == true)
	{
		Play::DrawFontText("font64px",
			"cheat 1: LASERS ON",
			gState.position.TopRight,
			Play::CENTRE);

	}if (gState.cheat_2 == true)
	{
		Play::DrawFontText("font64px",
			"cheat 2: YEE HAW",
			gState.position.TopRight2,
			Play::CENTRE);
	}

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


