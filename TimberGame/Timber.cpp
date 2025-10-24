#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>
#include <ctime>
#include <iostream>

using namespace sf;

//============================================================
// ENUMERACIONS I CONSTANTS GLOBALS
//============================================================
const int NUM_CLOUDS = 3;
const float timeBarStartWidth = 400;
const float timeBarHeight = 80;
const int NUM_BRANCHES = 6;
enum class Side { LEFT, RIGHT, NONE };

struct NPC {
	Sprite sprite;
	bool active;
	float speed;
	int maxHeight;
	int maxSpeed;
	int sentit;
	float posicioInicialX;

	NPC(Texture& texture, int maxHeight_, int maxSpeed_, int sentit_, float posicioInicialX_)
		: sprite(texture), active(false), speed(0),
		maxHeight(maxHeight_), maxSpeed(maxSpeed_),
		sentit(sentit_), posicioInicialX(posicioInicialX_) {
	}
};

//============================================================
// PROTOTIPS DE FUNCIONS
//============================================================
void updateNPC(NPC&, float);
void updateBranchSprites(Side[], Sprite[]);
void updateBranches(Side branchPositions[], int seed);

//======================================
// FUNCIÓ PRINCIPAL DEL JOC
//======================================
int main()
{
	//Variables del joc
	bool paused = true;
	Clock clock;
	int score = 0;
	float timeRemaining = 6.0f;
	RectangleShape timeBar(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition({ 1920 / 2 - timeBarStartWidth / 2, 980 });
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;
	Side playerSide = Side::LEFT;

	//--------------------------------------------------------
	// CREACIÓ I ELEMENTS VISUALS
	//--------------------------------------------------------
	VideoMode vm({ 1920, 1080 });
	RenderWindow window(vm, "Timber!!!", State::Fullscreen);
	Texture textureBackground("graphics/background.png");
	Sprite spriteBackground(textureBackground);
	Texture texturePlayer("graphics/player.png");
	Sprite spritePlayer(texturePlayer);
	spritePlayer.setPosition({ 580, 720 });

	//Gravestone
	Texture textureGravestone("graphics/rip.png");
	Sprite spriteRIP(textureGravestone);
	spriteRIP.setPosition({ 1000, 900 });

	//Axe
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;
	Texture textureAxe("graphics/axe.png");
	Sprite spriteAxe(textureAxe);
	spriteAxe.setPosition({ AXE_POSITION_LEFT, 500 });

	//Arbre
	Texture textureTree1("graphics/tree.png");
	Sprite spriteTree1(textureTree1);
	spriteTree1.setPosition({ 810, 0 });

	//Tronc volador
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	Texture textureLog("graphics/log.png");
	Sprite spriteLog(textureLog);


	//--------------------------------------------------------
	// TEXTOS I MARCADOR
	//--------------------------------------------------------
	Font font("fonts/KOMIKAP_.ttf");
	Text messageText(font);
	messageText.setString("Press Enter to start!");
	messageText.setCharacterSize(75);
	messageText.setFillColor(Color::White);
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin({ textRect.position.x + textRect.size.x / 2.0f,
	textRect.position.y + textRect.size.y / 2.0f });
	messageText.setPosition({ 1920 / 2, 1080 / 2  });
	Text scoreText(font);
	scoreText.setCharacterSize(100);
	scoreText.setString("Score = 0");
	scoreText.setFillColor(Color::White);
	scoreText.setPosition({ 20, 20 });

	//Arbre atrezzo
	Texture textureTreeBackground("graphics/tree-background.png");
	Sprite treeBackground1(textureTreeBackground);
	Sprite treeBackground2(textureTreeBackground);
	Sprite treeBackground3(textureTreeBackground);
	treeBackground1.setPosition({ 1200, 550 });
	treeBackground2.setPosition({ 400, 700 });
	treeBackground3.setPosition({ 1600, 600 });

	//Abella
	Texture  textureBee("graphics/bee.png");
	NPC bee(textureBee, 500, 400, -1, 2000);
	
	//Cloud
	Texture textureCloud("graphics/cloud.png");
	NPC clouds[NUM_CLOUDS] = {
		NPC(textureCloud, 100, 200, 1, -200),
		NPC(textureCloud, 250, 200, 1, -200),
		NPC(textureCloud, 500, 200, 1, -200)
	};

	Texture textureBranch("graphics/branch.png");
	Side branchPositions[NUM_BRANCHES];
	Sprite branches[NUM_BRANCHES] = {
	Sprite(textureBranch),Sprite(textureBranch),Sprite(textureBranch),
	Sprite(textureBranch),Sprite(textureBranch),Sprite(textureBranch) };
	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition({ -2000, -2000 });
		// Set the sprite's origin to dead centre
		// We can then spin it round without changing its position
		branches[i].setOrigin({ 220, 20 });
		branchPositions[i] = Side::LEFT;
	}

	// Comptador de FPS
	Text fpsText(font);
	fpsText.setCharacterSize(50);
	fpsText.setFillColor(Color::Yellow);
	fpsText.setPosition({ 1600, 20 });

	Clock fpsClock;
	int frameCount = 0;
	float fpsTime = 0.0f;
	float fpsValue = 0.0f;

	//--------------------------------------------------------
	// BUCLE PRINCIPAL DEL JOC
	//--------------------------------------------------------
	while (window.isOpen()) {
			// GESTIÓ D’ESDEVENIMENTS
			while (const std::optional event = window.pollEvent()) {
				if (event->is<Event::Closed>()) window.close();
				if (const auto* key = event->getIf<Event::KeyPressed>()) {
					if (key->scancode == Keyboard::Scancode::Escape)
						window.close();
					if (key->scancode == Keyboard::Scancode::Enter) {
						paused = false;
						score = 0;
						timeRemaining = 6;
						for (int i = 0; i < NUM_BRANCHES; i++)
							branchPositions[i] = Side::NONE;
						spriteRIP.setPosition({ 675, 2000 });
						spritePlayer.setPosition({ 675, 660 });

					}

					if (key->scancode == Keyboard::Scancode::Right) {
						// Make sure the player is on the right
						playerSide = Side::RIGHT;
						score++;
						// Add to the amount of time remaining
						timeRemaining += (2 / score) + .15;
						spriteAxe.setPosition({ AXE_POSITION_RIGHT,
						spriteAxe.getPosition().y });
						spritePlayer.setPosition({ 1200, 720 });
						updateBranches(branchPositions, score);
						// Set the log flying to the left
						spriteLog.setPosition({ 810, 720 });
						logSpeedX = -5000;
						logActive = true;
					}

					if (key->scancode == Keyboard::Scancode::Left) {
						// Make sure the player is on the left
						playerSide = Side::LEFT;
						score++;
						// Add to the amount of time remaining
						timeRemaining += (2 / score) + .15;
						spriteAxe.setPosition({ AXE_POSITION_RIGHT,
						spriteAxe.getPosition().y });
						spritePlayer.setPosition({ 600, 720 });
						updateBranches(branchPositions, score);
						// Set the log flying to the left
						spriteLog.setPosition({ 810, 720 });
						logSpeedX = -5000;
						logActive = true;
					}

				}
			}
		//-------------------------------------------------------
		// UPDATE
		//-------------------------------------------------------
		if(!paused){
			Time dt = clock.restart();
			timeRemaining -= dt.asSeconds();

			// FPS update
			frameCount++;
			fpsTime += fpsClock.restart().asSeconds();
			if (fpsTime >= 1.0f) {
				fpsValue = frameCount / fpsTime;
				frameCount = 0;
				fpsTime = 0.0f;

				std::stringstream fpsStream;
				fpsStream << "FPS: " << static_cast<int>(fpsValue);
				fpsText.setString(fpsStream.str());
			}

			timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, 80));
			if (timeRemaining <= 0) {
				paused = true;
				messageText.setString("Out of time!");
				textRect = messageText.getLocalBounds();
				messageText.setOrigin({ (textRect.position.x + (textRect.size.x / 2.0f)),
				(textRect.position.y + (textRect.size.y / 2.0f)) });
			}

			// Entitats mòbils
			updateNPC(clouds[0], dt.asSeconds());
			updateNPC(clouds[1], dt.asSeconds());
			updateNPC(clouds[2], dt.asSeconds());
			updateNPC(bee, dt.asSeconds());
			

			updateBranchSprites(branchPositions, branches);

			if (branchPositions[5] == playerSide) {
				paused = true;  // Pausar el joc
				spriteRIP.setPosition({ 525, 760 });  // Posar la tomba (gravestone)
				spritePlayer.setPosition({ 2000, 660 });  // Moure el jugador fora de la pantalla
				messageText.setString("SQUISHED!");  // Missatge de mort
				textRect = messageText.getLocalBounds();
				messageText.setOrigin({ (textRect.position.x + (textRect.size.x / 2.0f)),
				(textRect.position.y + (textRect.size.y / 2.0f)) });
			}

			updateBranchSprites(branchPositions, branches);

			// Update the score text
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());
		}
		//-------------------------------------------------------
		// RENDER
		//-------------------------------------------------------
		window.clear();
		window.draw(spriteBackground);
		for (int i = 0; i < NUM_CLOUDS; i++) {
			window.draw(clouds[i].sprite);
		}

		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}

		window.draw(spriteTree1);
		window.draw(spritePlayer);
		window.draw(bee.sprite);
		window.draw(treeBackground1);
		window.draw(treeBackground2);
		window.draw(treeBackground3);
		window.draw(timeBar);
		window.draw(scoreText);
		if (paused) {
			window.draw(messageText);
		}
		window.draw(fpsText);
		window.display();
	}
	return 0;
}

void updateNPC(NPC& npc, float dt) {
	if (!npc.active) {
		npc.speed = (rand() % npc.maxSpeed) * npc.sentit;
		float height = rand() % npc.maxHeight;
		npc.sprite.setPosition({ npc.posicioInicialX, height });
		npc.active = true;
	}
	else {
		npc.sprite.setPosition({
		npc.sprite.getPosition().x + npc.speed * dt,
		npc.sprite.getPosition().y
			});
		if (npc.sprite.getPosition().x < -200 ||
			npc.sprite.getPosition().x > 2000)
			npc.active = false;
	}
}

void updateBranches(Side branchPositions[], int seed) {
	for (int j = NUM_BRANCHES - 1; j > 0; j--)
		branchPositions[j] = branchPositions[j - 1];
	srand((int)time(0) + seed);
	int r = rand() % 5;
	switch (r) {
	case 0:
		branchPositions[0] = Side::LEFT;
		break;
	case 1:
		branchPositions[0] = Side::RIGHT;
		break;
	default:
		branchPositions[0] = Side::NONE;
		break;
	}
}

void updateBranchSprites(Side branchPositions[], Sprite branches[]) {
	for (int i = 0; i < NUM_BRANCHES; i++) {
		float height = i * 150;
		if (branchPositions[i] == Side::LEFT) {
			branches[i].setPosition({ 650, height });
			branches[i].setRotation(degrees(180));
			branches[i].setScale({ 1, -1 });
		}
		else if (branchPositions[i] == Side::RIGHT) {
			branches[i].setPosition({ 1250, height });
			branches[i].setRotation(degrees(0));
		}
		else {
			branches[i].setPosition({ 3000, height });
		}
	}
}