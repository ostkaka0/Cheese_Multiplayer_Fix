#include "Player.h"
#include "World.h"
#include "Camera.h"
#include "Projectile.h"
#include <tuple>
#include "MessageType.h"
#include "App.h"
#include "Block.h"
#include "GameUtilityInterface.h"

Player::Player(float X, float Y, short sizeX, short sizeY, bool IsClientControlling, std::string spriteName, int spriteIndex, std::string Name) 
	: Creature(X, Y, sizeX, sizeY, 1536, 0.5, spriteName, spriteIndex, IsClientControlling)
{
	name = Name;
	cameraDelay = 0;
	right = true;
	down = false;
	left = false;
	up = false;
	lmb = false;
	inventory = new Inventory(8, 4, 64);

#ifndef _SERVER
	//auto eventUpdate =  [this](App &app, sf::Event &event, World *world, std::queue<sf::Packet> *packetDataList) { EventUpdate(app, event, world, packetDataList); };
	//eventHandler.AddEventCallback(this, eventUpdate);
#endif
}

/*#ifdef _SERVER
void Player::Update(App &app, World *world, std::queue<sf::Packet> *packetDataList)
#else
void Player::Update(App &app, World *world, std::queue<sf::Packet> *packetDataList, Camera *camera, EventHandler &eventHandler)
#endif*/
void Player::Update(App &app, GameUtilityInterface *GameUtilityInterface)
{
#ifndef _SERVER
	if (isClientControlling)
	{
		if (GameUtilityInterface->getCamera().getEntity() != this)
		{
			if (cameraDelay <= 0)
			{
				GameUtilityInterface->getCamera().setCameraAt(this);
				cameraDelay = 0.5;
			}
			else
			{
				cameraDelay -= app.getDeltaTime();
			}
		}
	}
#endif

	Creature::Update(app, GameUtilityInterface);
/*#ifdef _SERVER
	Creature::Update(app, world, packetDataList);
#else
	Creature::Update(app, world, packetDataList, camera, eventHandler);
#endif*/
}

#ifndef _SERVER

void Player::EventUpdate(App &app, const sf::Event &event, GameUtilityInterface* gameUtilityInterface)
{
	if (isClientControlling)
	{
		bool
			rDown = right,
			dDown = down,
			lDown = left,
			uDown = up;

		switch(event.type)
		{
		case sf::Event::KeyPressed:
			switch(event.key.code)
			{
			case sf::Keyboard::D:
Right:
				rDown = true;
				break;
			case sf::Keyboard::S:
Down:
				dDown = true;
				break;
			case sf::Keyboard::A:
Left:
				lDown = true;
				break;
			case sf::Keyboard::W:
Up:
				uDown = true;
				break;

			case sf::Keyboard::Space:
				if (true)//(speedX == 0 || speedY == 0)
				{
					float xSpeed2 = 0;
					float ySpeed2 = 0;

					Block *block = gameUtilityInterface->getCurrentWorld()->getBlock((long)x+8>>4, (long)y+8>>4, 2);
					if (block != nullptr)
					{
						block->CreatureJump(app, this, xSpeed2, ySpeed2, gameUtilityInterface->getCurrentWorld()->getBlockAndMetadata((long)x+8>>4, (long)y+8>>4, 2).second);
					}

					if (xSpeed2 != 0 && speedX != 0)
						break;
					else if (ySpeed2 != 0 && speedY != 0)
						break;

					if (CheckCollision(app, gameUtilityInterface->getCurrentWorld(), (xSpeed2 > 0)? -1:1, (ySpeed2 > 0)? -1:1))
					{
						if (speedX == 0)
							speedX = xSpeed2;
						
						if (speedY == 0)
							speedY = ySpeed2;
					}

					if (block != nullptr)
						block->OnEntityHover(app, this, xSpeed2, ySpeed2, speedX, speedY, gameUtilityInterface->getCurrentWorld()->getBlockAndMetadata((long)x+8>>4, (long)y+8>>4, 2).second);
				}
				break;
			case sf::Keyboard::Q:
				{
					float thing1 = (sf::Mouse::getPosition().y - app.getPosition().y) - app.getView().getSize().y/2;
					float thing2 = (sf::Mouse::getPosition().x - app.getPosition().x) - app.getView().getSize().x/2;
					double angle = atan2(thing1, thing2) * 180 / 3.1415926536;
					if (angle < 0)
						angle = angle + 360;

					double deltaSpeedX = cos(angle*3.1415926536)*speedX;
					double deltaSpeedY = sin(angle*3.1415926536)*speedY;

					if (angle > 180)
						deltaSpeedX *= -1;

					if (angle < 90 || angle > 270)
						deltaSpeedY *= -1;


					Projectile *projectile = new Projectile(x+8, y+8, 32, 32, angle, 1024, 0.03125F, "arrow.png", 0, false);
					gameUtilityInterface->getCurrentWorld()->AddEntity(projectile);
				}
				break;

			case sf::Keyboard::Right:
				goto Right;

			case sf::Keyboard::Down:
				goto Down;

			case sf::Keyboard::Left:
				goto Left;

			case sf::Keyboard::Up:
				goto Up;
			}
			break;

		case sf::Event::KeyReleased:
			switch(event.key.code)
			{
			case sf::Keyboard::D:
RightR:
				rDown = false;
				break;
			case sf::Keyboard::S:
DownR:
				dDown = false;
				break;
			case sf::Keyboard::A:
LeftR:
				lDown = false;
				break;
			case sf::Keyboard::W:
UpR:
				uDown = false;
				break;

			case sf::Keyboard::Right:
				goto RightR;

			case sf::Keyboard::Down:
				goto DownR;

			case sf::Keyboard::Left:
				goto LeftR;

			case sf::Keyboard::Up:
				goto UpR;
			}
			break;

		case sf::Event::MouseButtonPressed:
			switch(event.key.code)
			{
			case sf::Mouse::Left:
					//inventory->AddItem(new NormalItem("Cow", "cowtexture"), 2);
					//inventory->AddItem(new NormalItem("Goat", "cofsafwtexture"), 8);
					//inventory->AddItem(new NormalItem("Chicken", "cowtexture"), 20);
					//inventory->get(std::cout);
					
				if(!lmb && (lmb=true))


				lmb = true;
				break;

			case sf::Mouse::Right:
				break;
			}

		case sf::Event::MouseButtonReleased:
			{
				lmb = false;
			}
			break;
		}
		KeyUpdate(rDown, dDown, lDown, uDown, gameUtilityInterface);
	}
}

void Player::Draw(App &app, GameUtilityInterface *gameUtilityInterface)
{

	inventory->Draw(16, 16, app, gameUtilityInterface->getTextureContainer()); 
	Creature::Draw(app, gameUtilityInterface);
}
#endif

void Player::KeyUpdate(bool Right, bool Down, bool Left, bool Up, GameUtilityInterface* gameUtilityInterface)
{
	if (Right != right || Down != down || Left != left || Up != up)
	{
		horizontal = 0;
		vertical = 0;

		if (Right)  horizontal += speed;
		if (Down)   vertical += speed;
		if (Left)   horizontal -= speed;
		if (Up)     vertical -= speed;

		//std::cout << "bools: " <<  Right << " " << Down<< " " << Left<< " " << Up<< " " << std::endl;
		//std::cout << "variables: " <<  horizontal << " " << vertical << " " << speedX << " " << speedY << std::endl;

		right = Right;
		down = Down;
		left = Left;
		up = Up;



		if (isClientControlling)
		{
			sf::Packet packet;
			packet << (sf::Uint16)PlayerMove << x << y << speedX << speedY << angle << horizontal << vertical;
			gameUtilityInterface->SendPacket(packet);
		}
	}
}

void Player::setCameraDelay(float delay)
{
	cameraDelay = delay;
}

std::string Player::getTextureName()
{
	return "graywizard.png";
}

char Player::getTextureId()
{
	return 0;
}