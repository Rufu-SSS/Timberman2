//=====================================================================
// INCLUDES I NAMESPACES
//=====================================================================
// Afegim les llibreries que necessitem i els namespaces per abreviar 
// les comandes que farem servir per no haver d'escriure cada vegada 
// "sf::" en el nostre cas.
//=====================================================================

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>
#include <ctime>
#include <iostream>
using namespace sf;
//=====================================================================
//Graphics: ens deixa crear una finestra SFML a partir del terminal.
//Audio: ens deixa fer sortir sons dins la finestra SFML
//sstream: ens deixa treballar amb fils.
//ctime: ens ajuda a crear i treballar amb rellotges que compten el 
// temps entre accions.
//iostream: ens deixa capturar entrades per la consola, però no el fem 
// servir dins el programa.
//=====================================================================


//=====================================================================
// ENUMERACIONS I CONSTANTS GLOBALS
//=====================================================================
// Inicialitzem vàries variables constants (int, float) per a gestionar 
// més endavant esdeveniments. També hi afegim variables globals perquè
// no es puguin canviar al llarg del programa.
//=====================================================================
const int NUM_CLOUDS = 3;
const float timeBarStartWidth = 400;
const float timeBarHeight = 80;
const int NUM_BRANCHES = 6;
enum class Side { LEFT, RIGHT, NONE };
//=====================================================================
//NUM_CLOUDS: el nombre total de núvols que sortiran per pantalla SFML.
//timeBarStartWidth: el gruix inicial de la barra de temps
//timeBarHeight: l'altura de la barra de temps, aquesta no canvia.
//NUM_BRANCHES: el nombre total de branques que sortiran per pantalla.
//Side: definim els possibles costats on apareixen les branques
//=====================================================================


//=====================================================================
// DEFINICIÓ DE L'ESTRUCTURA NPC
//=====================================================================
// Aquesta estructura serveix com una mena de "carpeta" per a gestionar
// els objectes que es mouen per la pantalla.
//=====================================================================
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
//=====================================================================
//sprite: definim un sprite buit.
//active: diem si es troba actiu o no la nova estructura.
//speed: li donem una velocitat per a poder moure's i no ser estàtic.
//maxHeight: perquè el nou NPC no surti dels límits li posem un màxim.
//maxSpeed: li donem un límit de velocitat perquè no el sobrepassi.
//sentit: li diem en quin sentit es mourà [LEFT, RIGHT].
//posicioInicialX: li donem un punt inicial pel qual començar a moure's.
//=====================================================================


//=====================================================================
// PROTOTIPS DE FUNCIONS
//=====================================================================
// Definim les funcions abans de l'int main per tal de poder recòrrer a 
// elles quan faci falta.
//=====================================================================
void updateNPC(NPC&, float);
void updateBranchSprites(Side[], Sprite[]);
void updateBranches(Side branchPositions[], int seed);
//=====================================================================
//updateNPC(...): actualitzem l'estructura NPC.
//updateBranchSprites(...): actualitzem els sprites de les branques.
//updateBranches(...): actualitzem les branques de posicions i llavors.
//=====================================================================


//=====================================================================
// FUNCIÓ PRINCIPAL DEL JOC
//=====================================================================
// Aqui dins hi ha tot el codi que realment importa després de les 
// definicions i tot. 
// Consta d'un bucle de joc (Inputs/Actualització/Render) i funciona 
// tot mentre la finestra SFML es troba oberta, en cas contrari no.
//=====================================================================
int main()
{
	bool paused = true;
	Clock clock;
	int score = 0;
	float timeRemaining = 6.0f;
	RectangleShape timeBar(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition({ 1920 / 2 - timeBarStartWidth / 2, 980 });
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;
	Side playerSide = Side::LEFT;

	//============================//
	// CREACIÓ I ELEMENTS VISUALS
	//============================//
	VideoMode vm({ 1920, 1080 });
	RenderWindow window(vm, "Timber!!!", State::Fullscreen);
	Texture textureBackground("graphics/background.png");
	Sprite spriteBackground(textureBackground);
	Texture texturePlayer("graphics/player.png");
	Sprite spritePlayer(texturePlayer);
	spritePlayer.setPosition({ 580, 720 });

	Texture textureGravestone("graphics/rip.png");
	Sprite spriteRIP(textureGravestone);
	spriteRIP.setPosition({ 1000, 900 });

	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;
	Texture textureAxe("graphics/axe.png");
	Sprite spriteAxe(textureAxe);
	spriteAxe.setPosition({ AXE_POSITION_LEFT, 500 });

	Texture textureTree1("graphics/tree.png");
	Sprite spriteTree1(textureTree1);
	spriteTree1.setPosition({ 810, 0 });

	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	Texture textureLog("graphics/log.png");
	Sprite spriteLog(textureLog);


	//============================//
	// TEXTOS I MARCADOR
	//============================//
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

	Texture textureTreeBackground("graphics/tree-background.png");
	Sprite treeBackground1(textureTreeBackground);
	Sprite treeBackground2(textureTreeBackground);
	Sprite treeBackground3(textureTreeBackground);
	treeBackground1.setPosition({ 1200, 550 });
	treeBackground2.setPosition({ 400, 700 });
	treeBackground3.setPosition({ 1600, 600 });

	Texture  textureBee("graphics/bee.png");
	NPC bee(textureBee, 500, 400, -1, 2000);
	
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
		branches[i].setOrigin({ 220, 20 });
		branchPositions[i] = Side::LEFT;
	}

	Text fpsText(font);
	fpsText.setCharacterSize(50);
	fpsText.setFillColor(Color::Yellow);
	fpsText.setPosition({ 1600, 20 });

	Clock fpsClock;
	int frameCount = 0;
	float fpsTime = 0.0f;
	float fpsValue = 0.0f;

	//============================//
	// BUCLE PRINCIPAL DEL JOC
	//============================//
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
						playerSide = Side::LEFT;
						score++;
						timeRemaining += (2 / score) + .15;
						spriteAxe.setPosition({ AXE_POSITION_RIGHT,
						spriteAxe.getPosition().y });
						spritePlayer.setPosition({ 600, 720 });
						updateBranches(branchPositions, score);
						spriteLog.setPosition({ 810, 720 });
						logSpeedX = -5000;
						logActive = true;
					}

				}
			}
		//============================//
		// UPDATE
		//============================//

		if(!paused){
			Time dt = clock.restart();
			timeRemaining -= dt.asSeconds();

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

			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());
		}
		//============================//
		// RENDER
		//============================//
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
//=====================================================================
//CREACIÓ D'ELEMENTS VISUALS
// Creem la finestra i les mides.
// Creem i assignem textures al fons i el jugador, aquest últim el 
// situem dins la finestra.
// Creem i assignem textures a l'arbre, destral, tronc volador i tomba.
//=====================================================================
//TEXTOS I MARCADOR
// Creem un HUD senzill i tot seguit afegim més elements visuals per la
// pantalla, l'abella, arbres de fons, núvols i les branques que cauen.
// Afegim rellotges per comptar el temps, els fps i actualitzem els 
// valors de score.
//=====================================================================
//BUCLE PRINCIPAL DEL JOC
// Mentre la finestra estigui oberta gestiona esdeveniments, actualitza
// la pantalla i renderitza els sprites/textures.
//=====================================================================


//=====================================================================
//FUNCIÓ updateNPC(...)
//=====================================================================
// Mentre les estructures NPC estiguin actives les generarà cada vegada
// amb velocitats/altures/posicions aleatòries, quan surti de la 
// pantalla deixarà d'estar actiu, per això farem que torni a començar 
// a moure's de nou del costat inicial.
//=====================================================================
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
//=====================================================================
//if(...): Mentre estiguin actius els nostres npc's els generarem amb 
// valors aleatòris per a donar-hi més naturalesa al moviment d'aquests.
//else{...}: Inicialitzem de nou la posició inicial i trajectòria dels 
// nostres npc.
//if2(...): Quan els nostres npc's surtin de pantalla els desactivem.
//=====================================================================


//=====================================================================
//FUNCIÓ updateBranches(...)
//=====================================================================
// Tot el que fa es generar aleatòriament la posició de les branques.
//=====================================================================
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
//=====================================================================
//for(...): obliga a les posicions de branques a ser diferents cada 
// vegada restant 1 al seu valor anterior.
//switch(r): segons el valor aleatori de r tria entre els tres casos 
// [LEFT, RIGHT, NONE].
//=====================================================================


//=====================================================================
//FUNCIÓ updateBranchSprites(...)
//=====================================================================
//Segons l'altura en la qual es vagin situant les branques, se les 
// actualitzarà el seu costat, posició i altura.
//=====================================================================
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
//=====================================================================
//for(...): per cada branca, a tots dos costats fes que vagi caiguent 
// cada frame en que moguem el timberman.
//=====================================================================
