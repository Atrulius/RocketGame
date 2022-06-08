#include <random>
#include <iostream>
#include "Game.h"
#include <Windows.h>
#include <mmsystem.h>
#include <fstream>
#include <stdio.h>

int game_Width			= 500;
int game_Height			= 500;
int screen_Min			= 500;
int iFrames_per_Second  = 60;
float screen_X_Offset	= 0;
float screen_Y_Offset	= 0;

int block_Number;

sf::Texture blocks_Block[5][13];
sf::Texture blocks_Mixed[5][13];
sf::Texture blocks_Spike[5][13];
sf::Texture blocks_Top[5][3];

sf::Texture decoration_Air[5][3];

sf::Texture number[10];

sf::Texture ship;
sf::Texture ufo;

sf::Texture icon;

sf::Texture menu;
sf::Texture back;

sf::Texture glow;

sf::Texture txExit;
sf::Texture txPlay;

sf::Texture confirm;
sf::Texture no;
sf::Texture yes;
sf::Texture btnEditor;
sf::Texture chapter;
sf::Texture editor;

const bool Game::running() const
{
	return this->window->isOpen();
}

Game::Game()
{
	int input = 0;
	int num = 0;

	for (int c = 0; c < 5; c++) {
		for (int l = 0; l < 5; l++) {

			input = 0;
			num = 0;

			std::ifstream main_levels_file("saves/levels/" + std::to_string(c) + "/" + std::to_string(l) + ".txt");

			std::vector<int> level_inputs;

			while (main_levels_file >> input) {
				level_inputs.push_back(input);
			}

			for (int block : level_inputs) {
				if (num < 80 * 20) {
					int x = num % 80;
					int y = int(float(num) / 80);
					map[c][l][y][x] = block;
				}
				num++;
			}
		}
	}

	input = 0;
	num = 0;

	std::ifstream editor_levels_file("saves/editor_levels.txt");

	while (editor_levels_file >> input) {
		if (num == 0) {
			iEditor_Levels = input;
		}
		else {
			iCurrent_Editor_Level = input;
		}
		num++;
	}
	if (num == 0) {
		std::ofstream createFile("saves/editor_levels.txt");
		iEditor_Levels = 1;
		iCurrent_Editor_Level = 0;
		createFile << iEditor_Levels << " ";
		createFile << iCurrent_Editor_Level;
		createFile.close();
	}

	std::ifstream editor_file("saves/editor" + std::to_string(iCurrent_Editor_Level) + ".txt");

	std::vector<int> editor_inputs;

	input = 0;
	num = 0;

	while (editor_file >> input) {
		editor_inputs.push_back(input);
	}

	for (int block : editor_inputs) {
		if (num < 80 * 20) {
			int x = num % 80;
			int y = int(float(num) / 80);
			editorMap[y][x] = block;
		}
		else {
			if (num == 80 * 20) {
				iLevel = block;
			}
		}
		num++;
	}
	if (num == 0) {
		std::ofstream createFile("saves/editor" + std::to_string(iCurrent_Editor_Level) + ".txt");
		for (int y = 0; y < 20; y++) {
			for (int x = 0; x < 80; x++) {
				int block;
				if (x == 0 || x == 79 || y == 0 || y == 19) {
					block = 1;
				}
				else {
					block = 0;
				}
				editorMap[y][x] = block;
				createFile << block << " ";
			}
			createFile << "\n";
		}
		createFile << 0;
		createFile.close();
	}

	std::ifstream sidebar_width_file("saves/sidebar_width.txt");

	input = 0;
	num = 0;

	while (sidebar_width_file >> input) {
		iEditor_Sidebar_Width = input;
		num++;
	}
	if (num == 0) {
		std::ofstream createFile("saves/sidebar_width.txt");
		createFile << iEditor_Sidebar_Width;
		createFile.close();
	}

	ShowWindow(GetConsoleWindow(), SW_HIDE);

	for (int world = 0; world < 5; world++) {
		for (int type = 0; type < 13; type++) {
			blocks_Block[world][type].loadFromFile("images/blocks/" + std::to_string(world) + "/block" + std::to_string(type) + ".png");
			blocks_Spike[world][type].loadFromFile("images/blocks/" + std::to_string(world) + "/spike" + std::to_string(type) + ".png");
			blocks_Mixed[world][type].loadFromFile("images/blocks/" + std::to_string(world) + "/mixed" + std::to_string(type) + ".png");
			if (type < 3) {
				blocks_Top[world][type].loadFromFile("images/blocks/" + std::to_string(world) + "/top" + std::to_string(type) + ".png");
			}
		}
	}
	for (int i = 0; i < 10; i++) {
		number[i].loadFromFile("images/extra/" + std::to_string(i) + ".png");
	}

	ship.loadFromFile("images/extra/ship.png");
	ufo.loadFromFile("images/extra/ufo.png");

	icon.loadFromFile("images/extra/icon.png");

	menu.loadFromFile("images/extra/menu.png");

	back.loadFromFile("images/extra/back.png");

	glow.loadFromFile("images/decoration/glow.png");

	txExit.loadFromFile("images/extra/exit.png");
	txPlay.loadFromFile("images/extra/play.png");

	confirm.loadFromFile("images/extra/confirm.png");

	no.loadFromFile("images/extra/no.png");
	yes.loadFromFile("images/extra/yes.png");

	btnEditor.loadFromFile("images/extra/btnEditor.png");
	editor.loadFromFile("images/extra/editor.png");
	chapter.loadFromFile("images/extra/chapter.png");

	this->player.setTexture(ship);

	arrow_Velocity = arrow_Start_Velocity;
	arrow_Acceleration = -(2 * arrow_Start_Velocity / arrow_Acceleration);

	this->Initialize_Variables();
	this->Initialize_Glow();
	this->Initialize_Decoration();
	this->Initialize_Window();
	this->Initialize_Mixed();
}

Game::~Game()
{
	delete this->window;
}

void Game::Initialize_Variables()
{
	this->window = nullptr;
}

void Game::Initialize_Glow() {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist4000(1, 4000);
	std::uniform_int_distribution<std::mt19937::result_type> dist1000(1, 1000);
	std::uniform_int_distribution<std::mt19937::result_type> dist500(1, 500);
	std::uniform_int_distribution<std::mt19937::result_type> dist255(1, 255);
	for (int i = 0; i < iGlow_Total; i++) {
		arrGlow_Position_X[i] = dist4000(rng) - 750;
		arrGlow_Position_Y[i] = dist1000(rng) - 750;
		arrGlow_Scale[i] = dist500(rng) + 1000;
		arrGlow_Brightness[i] = float(dist255(rng)) / 255;
		//std::cout << arrGlow_Position_X[i] << ", " << arrGlow_Position_Y[i] << ", " << arrGlow_Scale[i] << ", " << arrGlow_Brightness[i] << "\n";
	}
}

void Game::Initialize_Decoration() {
	for (int i = 0; i < 64; i++) {
		arrDecoration_Size[i] = iDecoration_Min_Size + rand()%(iDecoration_Max_Size - iDecoration_Min_Size);
		arrDecoration_Sprite[i] = rand()%3;
		arrDecoration_Main_Velocity[i] = iDecoration_Min_Main_Velocity + rand()%(iDecoration_Max_Main_Velocity - iDecoration_Min_Main_Velocity);
		arrDecoration_Rotation[i] = rand()%90;
		arrDecoration_Offset_X[i] = 0;
		arrDecoration_Offset_Y[i] = 0;
		arrDecoration_Velocity_X[i] = 0;
		arrDecoration_Velocity_Y[i] = 0;
		arrDecoration_Position_X[i] = (rand()%int(80 - 80 * (arrDecoration_Size[i] / iDecoration_Max_Size) * fDecoration_Z_Sensitivity)) * fGrid_Cell_Size;
		arrDecoration_Position_Y[i] = (rand()%20) * fGrid_Cell_Size;
		arrDecoration_Acceleration_X[i] = (rand()%2) * 2 - 1; 
		arrDecoration_Acceleration_Y[i] = (rand()%2) * 2 - 1;
		arrDecoration_Color_Value[i] = float(rand()%100) / 99;
		if (arrDecoration_Color_Value[i] > 0.5) {
			arrDecoration_Color_Velocity[i] = -1;
		}
		else {
			arrDecoration_Color_Velocity[i] = 1;
		}
		if (!bEditor && iCurrent_Chapter == 0 || bEditor && iLevel == 0) {
			arrDecoration_Velocity_Y[i] = 50;
		}
		if (!bEditor && iCurrent_Chapter == 1 || bEditor && iLevel == 1) {
			arrDecoration_Velocity_X[i] = rand() % 100 - 50;
			arrDecoration_Velocity_Y[i] = rand() % 100 - 50;
		}
		if (!bEditor && iCurrent_Chapter == 2 || bEditor && iLevel == 2) {
			arrDecoration_Velocity_X[i] = 100;
		}
		if (!bEditor && iCurrent_Chapter == 3 || bEditor && iLevel == 3) {
			arrDecoration_Velocity_Y[i] = 50;
		}
		if (!bEditor && iCurrent_Chapter == 4 || bEditor && iLevel == 4) {
			arrDecoration_Velocity_Y[i] = 50;
		}
	}
}

void Game::Initialize_Window()
{
	this->videoMode.height = game_Height;
	this->videoMode.width = game_Width;
	this->window = new sf::RenderWindow(this->videoMode, "Rocket Game", sf::Style::Default);

	sf::Image icon;
	icon.loadFromFile("images/extra/icon.png");
	this->window->setIcon(32, 32, icon.getPixelsPtr());

	this->window->setFramerateLimit(iFrames_per_Second);
}

void Game::Initialize_Mixed() {
	for (int column = 0; column < 20; column++) {
		for (int row = 0; row < 80; row++) {
			int mixed = rand() % 20;
			if (mixed == 0) {
				mixedMap[column][row] = 1;
			}
			else {
				mixedMap[column][row] = 0;
			}
		}
	}
}

void Game::pollEvents()
{
	while (this->window->pollEvent(this->event)) {
		if (event.type == sf::Event::Closed) {
			Save();
			this->window->close();
			break;
		}
		if (event.type == sf::Event::KeyPressed) {
			if (this->event.key.code == sf::Keyboard::Enter) {
				bButton_Held_Enter = true;
			}
			if (this->event.key.code == sf::Keyboard::Escape) {
				bButton_Held_ESC = true;
			}
			if (this->event.key.code == sf::Keyboard::Up || this->event.key.code == sf::Keyboard::W) {
				bButton_Held_Up = true;
			}
			if (this->event.key.code == sf::Keyboard::Right || this->event.key.code == sf::Keyboard::D) {
				bButton_Held_Right = true;
			}
			if (this->event.key.code == sf::Keyboard::Left || this->event.key.code == sf::Keyboard::A) {
				bButton_Held_Left = true;
			}
			if (this->event.key.code == sf::Keyboard::Down || this->event.key.code == sf::Keyboard::S) {
				bButton_Held_Down = true;
			}
			if (this->event.key.code == sf::Keyboard::C) {
				bButton_Held_C = true;
			}
			if (this->event.key.code == sf::Keyboard::LShift || this->event.key.code == sf::Keyboard::RShift) {
				bButton_Held_Shift = true;
			}
			if (bEditor || bDeveloper) {
				if (bEditing || bDeveloper && !bDeveloper_Playtesting) {
					if (this->event.key.code == sf::Keyboard::Num1) {
						iLevel = 0;
					}
					if (this->event.key.code == sf::Keyboard::Num2) {
						iLevel = 1;
					}
					if (this->event.key.code == sf::Keyboard::Num3) {
						iLevel = 2;
					}
					if (this->event.key.code == sf::Keyboard::Num4) {
						iLevel = 3;
					}
					if (this->event.key.code == sf::Keyboard::Num5) {
						iLevel = 4;
					}
					if (this->event.key.code == sf::Keyboard::Q) {
						iBlock = 1;
					}
					if (this->event.key.code == sf::Keyboard::W) {
						iBlock = 2;
					}
					if (!bDeveloper) {
						if (this->event.key.code == sf::Keyboard::Left) {
							if (iCurrent_Editor_Level != 0) {
								Save_Editor();
								iCurrent_Editor_Level--;
								std::ifstream editor_file("saves/editor" + std::to_string(iCurrent_Editor_Level) + ".txt");

								std::vector<int> editor_inputs;

								int input = 0;
								int num = 0;

								while (editor_file >> input) {
									editor_inputs.push_back(input);
								}

								for (int block : editor_inputs) {
									if (num < 80 * 20) {
										int x = num % 80;
										int y = int(float(num) / 80);
										editorMap[y][x] = block;
									}
									else {
										if (num == 80 * 20) {
											iLevel = block;
										}
									}
									num++;
								}
							}
						}
						if (this->event.key.code == sf::Keyboard::Right) {
							if (iCurrent_Editor_Level != iEditor_Levels - 1) {
								Save_Editor();
								iCurrent_Editor_Level++;
								std::ifstream editor_file("saves/editor" + std::to_string(iCurrent_Editor_Level) + ".txt");

								std::vector<int> editor_inputs;

								int input = 0;
								int num = 0;

								while (editor_file >> input) {
									editor_inputs.push_back(input);
								}

								for (int block : editor_inputs) {
									if (num < 80 * 20) {
										int x = num % 80;
										int y = int(float(num) / 80);
										editorMap[y][x] = block;
									}
									else {
										if (num == 80 * 20) {
											iLevel = block;
										}
									}
									num++;
								}
							}
						}
						if (this->event.key.code == sf::Keyboard::X) {
							Save_Editor();
							iEditor_Levels++;
							iCurrent_Editor_Level = iEditor_Levels - 1;
							iLevel = 0;

							int input = 0;
							int num = 0;
							std::ofstream createFile("saves/editor" + std::to_string(iCurrent_Editor_Level) + ".txt");
							for (int y = 0; y < 20; y++) {
								for (int x = 0; x < 80; x++) {
									int block;
									if (x == 0 || x == 79 || y == 0 || y == 19) {
										block = 1;
									}
									else {
										block = 0;
									}
									editorMap[y][x] = block;
									createFile << block << " ";
								}
								createFile << "\n";
							}
							createFile << iLevel;
							createFile.close();
						}
						if (this->event.key.code == sf::Keyboard::Z) {
							if (iEditor_Levels != 1) {
								remove(("saves/editor" + std::to_string(iCurrent_Editor_Level) + ".txt").c_str());
								iEditor_Levels--;
								iCurrent_Editor_Level = iEditor_Levels - 1;

								std::ifstream editor_file("saves/editor" + std::to_string(iCurrent_Editor_Level) + ".txt");

								std::vector<int> editor_inputs;

								int input = 0;
								int num = 0;

								while (editor_file >> input) {
									editor_inputs.push_back(input);
								}

								for (int block : editor_inputs) {
									if (num < 80 * 20) {
										int x = num % 80;
										int y = int(float(num) / 80);
										editorMap[y][x] = block;
									}
									else {
										if (num == 80 * 20) {
											iLevel = block;
										}
									}
									num++;
								}
							}
						}
						if (this->event.key.code == sf::Keyboard::Backspace) {
							for (int y = 0; y < 20; y++) {
								for (int x = 0; x < 80; x++) {
									editorMap[y][x] = 1;
									if (y > 0 && y < 19 && x > 0 && x < 79) {
										editorMap[y][x] = 0;
									}
								}
							}
						}
					}
					else {
						if (this->event.key.code == sf::Keyboard::Left) {
							if (iCurrent_Level != 0) {
								iCurrent_Level--;
								fCamera_Position_X = 60 * fGrid_Cell_Size + iEditor_Sidebar_Width;
							}
							else {
								fCamera_Position_X = 0;
							}
						}
						if (this->event.key.code == sf::Keyboard::Right) {
							if (iCurrent_Level != 4) {
								iCurrent_Level++;
								fCamera_Position_X = 0;
							}
							else {
								fCamera_Position_X = 60 * fGrid_Cell_Size + iEditor_Sidebar_Width;
							}
						}
						if (this->event.key.code == sf::Keyboard::Backspace) {
							for (int y = 0; y < 20; y++) {
								for (int x = 0; x < 80; x++) {
									map[iCurrent_Chapter][iCurrent_Level][y][x] = 1;
									if (y > 0 && y < 19 && x > 0 && x < 79) {
										map[iCurrent_Chapter][iCurrent_Level][y][x] = 0;
									}
								}
							}
						}
					}
				}
				if (this->event.key.code == sf::Keyboard::Space && bPlaying) {
					if (bEditor) {
						bEditing = !bEditing;
						if (bEditing) {
							fPosition_Y = -50;
							if (fRespawning_Timer < fRespawning_Time / 2) {
								fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
								fRespawning_Last_Timer = fRespawning_Timer;
							}
						}
						else {
							if (fRespawning_Timer < fRespawning_Time) {
								if (fRespawning_Timer > fRespawning_Time / 2) {
									fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
								}
							}
							else {
								fRespawning_Timer = 0;
							}
						}
					}
					else {
						bDeveloper_Playtesting = !bDeveloper_Playtesting;
						if (bDeveloper_Playtesting) {
							fPosition_Y = -50;
							if (fRespawning_Timer < fRespawning_Time / 2) {
								fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
								fRespawning_Last_Timer = fRespawning_Timer;
							}
						}
						else {
							if (fRespawning_Timer < fRespawning_Time) {
								if (fRespawning_Timer > fRespawning_Time / 2) {
									fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
								}
							}
							else {
								fRespawning_Timer = 0;
							}
						}
					}
				}
			}
			break;
		}

		if (event.type == sf::Event::GainedFocus) {
			window_Is_Active = true;
		}
		if (event.type == sf::Event::LostFocus) {
			window_Is_Active = false;
		}
		if (event.type == sf::Event::MouseWheelMoved && (bEditor && bEditing || bDeveloper && !bDeveloper_Playtesting)) {
			fCamera_Position_X -= event.mouseWheel.delta * fGrid_Cell_Size;
			if (fCamera_Position_X < 0 * fGrid_Cell_Size) {
				fCamera_Position_X = 0;
			}
			if (fCamera_Position_X - iEditor_Sidebar_Width > 60 * fGrid_Cell_Size) {
				fCamera_Position_X = iEditor_Sidebar_Width + 60 * fGrid_Cell_Size;
			}
		}
		if (event.type == sf::Event::KeyReleased) {
			if (this->event.key.code	== sf::Keyboard::Enter) {
				bButton_Held_Enter					= false;
			}
			if (this->event.key.code	== sf::Keyboard::Escape) {
				bButton_Held_ESC					= false;
			}
			if (this->event.key.code	== sf::Keyboard::Up || this->event.key.code == sf::Keyboard::W) {
				bButton_Held_Up						= false;
			}
			if (this->event.key.code	== sf::Keyboard::Right || this->event.key.code == sf::Keyboard::D) {
				bButton_Held_Right					= false;
			}
			if (this->event.key.code	== sf::Keyboard::Left || this->event.key.code == sf::Keyboard::A) {
				bButton_Held_Left					= false;
			}
			if (this->event.key.code	== sf::Keyboard::Down || this->event.key.code == sf::Keyboard::S) {
				bButton_Held_Down					= false;
			}
			if (this->event.key.code	== sf::Keyboard::C) {
				bButton_Held_C						= false;
			}
			if (this->event.key.code	== sf::Keyboard::LShift) {
				bButton_Held_Shift					= false;
			}
			break;
		}
		if (event.type == sf::Event::Resized) {
			if (this->window->getSize().x > float(this->window->getSize().y) * (float(game_Width) / float(game_Height))) {
				sf::FloatRect visibleArea(game_Width / 2 - (game_Width * (float(this->window->getSize().x) / float(this->window->getSize().y) * (float(game_Height) / float(game_Width)))) / 2, 0, game_Width * (float(this->window->getSize().x) / float(this->window->getSize().y) * (float(game_Height) / float(game_Width))), game_Height);
				this->window->setView(sf::View(visibleArea));
				screen_Min = this->window->getSize().y;
				screen_X_Offset = float(this->window->getSize().x - screen_Min) / 2;
				screen_Y_Offset = 0;
			}
			else {
				sf::FloatRect visibleArea(0, game_Height / 2 - (game_Height * (float(this->window->getSize().y) / float(this->window->getSize().x) * (float(game_Width) / float(game_Height)))) / 2, game_Width, game_Height * (float(this->window->getSize().y) / float(this->window->getSize().x) * (float(game_Width) / float(game_Height))));
				this->window->setView(sf::View(visibleArea));
				screen_Min = this->window->getSize().x;
				screen_X_Offset = 0;
				screen_Y_Offset = float(this->window->getSize().y - screen_Min) / 2;
			}
		}
	}
}

void Game::Update()
{
	if (bDeveloper && !bDeveloper_Playtesting) {
		fPosition_Y = -50;
	}
	int iLast_Level = 0;

	if (bEditor) {
		iLast_Level = iLevel;
	}
	else {
		iLast_Level = iCurrent_Chapter;
	}

	this->pollEvents();

	int iLevel_Sound;
	if (bEditor) {
		iLevel_Sound = iLevel;
	}
	else {
		iLevel_Sound = iCurrent_Chapter;
	}
	if (bEditor && bEditing || bDeveloper && !bDeveloper_Playtesting) {
		if (!bUsed_Editor) {
			PlaySound(TEXT("sounds/editor.wav"), NULL, SND_ASYNC | SND_LOOP);
		}
		bUsed_Editor = true;
	}
	else {
		if (iLevel_Sound == 0 && (iLast_Level_Sound != 0 || bUsed_Editor)) {
			PlaySound(TEXT("sounds/0.wav"), NULL, SND_ASYNC | SND_LOOP);
		}
		if (iLevel_Sound == 1 && (iLast_Level_Sound != 1 || bUsed_Editor)) {
			PlaySound(TEXT("sounds/1.wav"), NULL, SND_ASYNC | SND_LOOP);
		}
		if (iLevel_Sound == 2 && (iLast_Level_Sound != 2 || bUsed_Editor)) {
			PlaySound(TEXT("sounds/2.wav"), NULL, SND_ASYNC | SND_LOOP);
		}
		if (iLevel_Sound == 3 && (iLast_Level_Sound != 3 || bUsed_Editor)) {
			PlaySound(TEXT("sounds/3.wav"), NULL, SND_ASYNC | SND_LOOP);
		}
		if (iLevel_Sound == 4 && (iLast_Level_Sound != 4 || bUsed_Editor)) {
			PlaySound(TEXT("sounds/4.wav"), NULL, SND_ASYNC | SND_LOOP);
		}
		bUsed_Editor = false;
	}
	if (iLevel_Sound != iLast_Level_Sound) {
		Initialize_Decoration();
	}
	iLast_Level_Sound = iLevel_Sound;
	if (bEditing && bEditor || bDeveloper && !bDeveloper_Playtesting) {
		fPosition_X = fCamera_Position_X + game_Width / 2;
	}
	arrow_Velocity += arrow_Acceleration * (1.f / iFrames_per_Second);
	arrow_Distance += arrow_Velocity * (1.f / iFrames_per_Second);
	if (arrow_Distance < 0) {
		arrow_Velocity = arrow_Start_Velocity;
		arrow_Distance = arrow_Velocity * (1.f / iFrames_per_Second);
	}

	if (bButton_Held_Up || bButton_Held_Down || bButton_Held_Left || bButton_Held_Right) {
		bFlying = true;
	}
	else {
		bFlying = false;
	}
	if (bFlying) {
		bStarted = true;
	}

	bWas_Flying = bFlying;

	mouseX = float(sf::Mouse::getPosition(*this->window).x - float(screen_X_Offset)) * float(game_Width) / float(screen_Min);
	mouseY = float(sf::Mouse::getPosition(*this->window).y - float(screen_Y_Offset)) * float(game_Height) / float(screen_Min);

	bButton_Held_Mouse1 = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	bButton_Held_Mouse2 = sf::Mouse::isButtonPressed(sf::Mouse::Right);

	if (bButton_Pressed_Mouse1 && !bButton_Held_Mouse1) {
		bButton_Released_Mouse1 = true;
	}
	if (bButton_Pressed_Mouse2 && !bButton_Held_Mouse2) {
		bButton_Released_Mouse2 = true;
	}
	if (bButton_Pressed_Shift && !bButton_Held_Shift) {
		bButton_Released_Shift = true;
	}
	bButton_Clicked_Mouse1 = false;
	if (bButton_Held_Mouse1 && !bButton_Pressed_Mouse1) {
		bButton_Clicked_Mouse1 = true;
	}
	bButton_Clicked_Mouse2 = false;
	if (bButton_Held_Mouse2 && !bButton_Pressed_Mouse2) {
		bButton_Clicked_Mouse2 = true;
	}

	bButton_Pressed_Mouse1 = bButton_Held_Mouse1;
	bButton_Pressed_Mouse2 = bButton_Held_Mouse2;
	bButton_Pressed_Shift = bButton_Held_Shift;

	if (bButton_Held_ESC && !bButton_Pressed_ESC && !bStart_Menu) {
		bButton_Pressed_ESC = true;
		if (bPlaying) {
			right_Side = true;
			if (died_Next_Level != -1) {
				iSelected_Difficulty = died_Next_Chapter;
			}
			else {
				iSelected_Difficulty = iCurrent_Chapter;
			}
			if (bEditor) {
				iSelected_Difficulty = 5;
			}
			bPlaying = false;
			arrow_Velocity = arrow_Start_Velocity;
			arrow_Distance = arrow_Velocity * (1.f / iFrames_per_Second);
		}
		else {
			if (iCurrent_Chapter >= 0 && iCurrent_Chapter <= 4 || bEditor) {
				bPlaying = true;
			}
		}
	}
	else if (bButton_Held_ESC && !bButton_Pressed_ESC && bStart_Menu) {
		bButton_Pressed_ESC = true;
		bExit_Warning = !bExit_Warning;
	}
	else if (!bButton_Held_ESC) {
		bButton_Pressed_ESC = false;
	}

	Update_Simulation_Speed();

	if (bPlaying && bStarted && iCurrent_Chapter >= 0 && iCurrent_Chapter <= 4 && !bEditor && (!bDeveloper || bDeveloper_Playtesting) || bEditor && bPlaying && !bEditing && bStarted) {
		Update_Segment();
		Update_Hard_Fall();
		Update_Vertical_Movement();
		Update_Horizontal_Movement();
		Update_Camera_Position();

		if (iCurrent_Level >= 0 && iCurrent_Level <= 9 && (!bDeveloper || bDeveloper_Playtesting) || bEditor && bPlaying && (!bDeveloper || bDeveloper_Playtesting)) {
			Update_Collision();
		}

		fPosition_X += fVelocity_X * fSpeed_of_Simulation;
		fPosition_Y += fVelocity_Y * fSpeed_of_Simulation;

		fVelocity_Forward = sqrt(fVelocity_X * fVelocity_X + fVelocity_Y * fVelocity_Y);

		fRocket_Rotation = atan2(fVelocity_Y, fVelocity_X) * 180 / 3.141;
	}
	if (bPlaying) {
		Update_Decoration();
		Update_Glow();
		Update_Particles();
	}
	if (fRespawning_Timer < fRespawning_Time && bPlaying) {
		Update_Dead();
	}
	if (bEditing && bEditor && bPlaying || bDeveloper && !bDeveloper_Playtesting && bPlaying) {
		Update_Editor();
	}
	if (iLast_Level != iLevel && bEditor || iCurrent_Chapter != iLast_Level && !bEditor) {
		Initialize_Decoration();
	}
	fSimulation_Speed_Overlay_Visibility = (1 - fSpeed_of_Simulation) * (1.f / fMin_Simulation_Speed) * fSimulation_Speed_Overlay_Max_Visibility;
}

void Game::Update_Simulation_Speed() {
	if (bButton_Held_C) {
		fSpeed_of_Simulation -= fTime_before_simulation_Speed_is_done_Changing / iFrames_per_Second;
		if (fSpeed_of_Simulation < fMin_Simulation_Speed) {
			fSpeed_of_Simulation = fMin_Simulation_Speed;
		}
	}
	else {
		fSpeed_of_Simulation += fTime_before_simulation_Speed_is_done_Changing / iFrames_per_Second;
		if (fSpeed_of_Simulation > 1) {
			fSpeed_of_Simulation = 1;
		}
	}
}

void Game::Update_Segment() {
	if (fPosition_X > 80 * fGrid_Cell_Size) {
		fPosition_X = fGrid_Cell_Size;
		fCamera_Position_X = 0;
		if (!bEditor) {
			iCurrent_Level++;
			if (iCurrent_Level > 4) {
				iCurrent_Chapter++;
				iCurrent_Level = 0;
			}
		}
	}
	if (fPosition_X < 0 * fGrid_Cell_Size) {
		fPosition_X = 79 * fGrid_Cell_Size;
		fCamera_Position_X = 80 * fGrid_Cell_Size;
		if (!bEditor) {
			iCurrent_Level--;
			if (iCurrent_Level < 0) {
				iCurrent_Chapter--;
				iCurrent_Level = 4;
			}
		}
	}
}

void Game::Update_Hard_Fall() {
	if (bButton_Held_Down && !bButton_Held_Up) {
		fVelocity_Y			+= fRocket_Gravity / iFrames_per_Second		* fRocket_Gravity_Multiplier_when_Down_is_Held * fSpeed_of_Simulation;
		if (fVelocity_Y		> fRocket_Max_Fall_Speed					* fRocket_Max_Fall_Speed_Multiplier_when_Down_is_Held) {
			fVelocity_Y		= fRocket_Max_Fall_Speed					* fRocket_Max_Fall_Speed_Multiplier_when_Down_is_Held;
		}
	}
	else {
		fVelocity_Y			+= fRocket_Gravity / iFrames_per_Second		* fSpeed_of_Simulation;
		if (fVelocity_Y		> fRocket_Max_Fall_Speed) {
			fVelocity_Y		-= 2 * fRocket_Gravity / iFrames_per_Second	* fSpeed_of_Simulation;
			if (fVelocity_Y	< fRocket_Max_Fall_Speed) {
				fVelocity_Y	= fRocket_Max_Fall_Speed;
			}
		}
	}
}

void Game::Update_Vertical_Movement() {
	if (bButton_Held_Up && !bButton_Held_Down) {
		if (fVelocity_Y	> 0) {
			fVelocity_Y	-= 2.5	* fRocket_Gravity / iFrames_per_Second * fSpeed_of_Simulation;
		}
		else {
			fVelocity_Y	-= 2		* fRocket_Gravity / iFrames_per_Second * fSpeed_of_Simulation;
		}
	}
	if (fVelocity_Y		< -fRocket_Max_Fall_Speed) {
		fVelocity_Y		= -fRocket_Max_Fall_Speed;
	}
}

void Game::Update_Horizontal_Movement() {
	if         (bButton_Held_Left == bButton_Held_Right) {
		if     (fVelocity_X	> 0) {
				fVelocity_X	-= ((1.f / fTime_before_Rocket_X_Velocity_is_Done_Changing) / iFrames_per_Second) * fRocket_Max_Velocity_X * fSpeed_of_Simulation;
			if (fVelocity_X	< 0) {
				fVelocity_X	= 0;
			}
		}
		if	   (fVelocity_X	< 0) {
				fVelocity_X	+= ((1.f / fTime_before_Rocket_X_Velocity_is_Done_Changing) / iFrames_per_Second) * fRocket_Max_Velocity_X * fSpeed_of_Simulation;
			if (fVelocity_X	> 0) {
				fVelocity_X	= 0;
			}
		}
	}
	else if (bButton_Held_Left) {
		if (fVelocity_X		> 0) {
			fVelocity_X		-= ((1.f / fTime_before_Rocket_X_Velocity_is_Done_Changing) / iFrames_per_Second) * fRocket_Max_Velocity_X * fSpeed_of_Simulation;
		}
		fVelocity_X			-= ((1.f / fTime_before_Rocket_X_Velocity_is_Done_Changing) / iFrames_per_Second) * fRocket_Max_Velocity_X * fSpeed_of_Simulation;
		if (fVelocity_X		< -fRocket_Max_Velocity_X) {
			fVelocity_X		= -fRocket_Max_Velocity_X;
		}
	}
	else {
		if (fVelocity_X		< 0) {
			fVelocity_X		+= ((1.f / fTime_before_Rocket_X_Velocity_is_Done_Changing) / iFrames_per_Second) * fRocket_Max_Velocity_X * fSpeed_of_Simulation;
		}
		fVelocity_X			+= ((1.f / fTime_before_Rocket_X_Velocity_is_Done_Changing) / iFrames_per_Second) * fRocket_Max_Velocity_X * fSpeed_of_Simulation;
		if (fVelocity_X		> fRocket_Max_Velocity_X) {
			fVelocity_X		= fRocket_Max_Velocity_X;
		}
	}
}

void Game::Update_Camera_Position() {
	if (fPosition_X - fCamera_Position_X > 500 - fCamera_Offset_from_Screen_Edge_X) {
		fCamera_Position_X += float(fPosition_X - 500 + fCamera_Offset_from_Screen_Edge_X - fCamera_Position_X) * fCamera_Follow_Speed;
	}
	if (fPosition_X - fCamera_Position_X < fCamera_Offset_from_Screen_Edge_X) {
		fCamera_Position_X += float(fPosition_X - fCamera_Offset_from_Screen_Edge_X - fCamera_Position_X) * fCamera_Follow_Speed;
	}
	if (fCamera_Position_X < 0) {
		fCamera_Position_X = 0;
	}
	if (!(bEditing && bEditor || bDeveloper && !bDeveloper_Playtesting)) {
		if (fCamera_Position_X > 80 * fGrid_Cell_Size - 500) {
			fCamera_Position_X = 80 * fGrid_Cell_Size - 500;
		}
	}
}

void Game::Update_Collision() {
	for (int column = floor(fPosition_X / fGrid_Cell_Size + 0.5) - 1; column < floor(fPosition_X / fGrid_Cell_Size + 0.5) + 1; column++) {
		for (int row = floor(fPosition_Y / fGrid_Cell_Size + 0.5) - 1; row < floor(fPosition_Y / fGrid_Cell_Size + 0.5) + 1; row++) {
			if (!bEditor) {
				if (map[iCurrent_Chapter][iCurrent_Level][row][column] != 0) {
					if (fPosition_X - 3 * fGrid_Cell_Size / 16 - fVelocity_X * 2 < (column + 1) * fGrid_Cell_Size && fPosition_X + 3 * fGrid_Cell_Size / 16 - fVelocity_X * 2 > column * fGrid_Cell_Size && fPosition_Y - 3 * fGrid_Cell_Size / 16 - fVelocity_Y * 2 < (row + 1) * fGrid_Cell_Size && fPosition_Y + 3 * fGrid_Cell_Size / 16 - fVelocity_Y * 2 > row * fGrid_Cell_Size && fRespawning_Timer >= fRespawning_Time / 2) {
						if (fRespawning_Timer < fRespawning_Time) {
							if (fRespawning_Timer > fRespawning_Time / 2) {
								fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
							}
						}
						else {
							fRespawning_Timer = 0;
						}
					}
				}
			}
			else {
				if (editorMap[row][column] != 0) {
					if (fPosition_X - 3 * fGrid_Cell_Size / 16 - fVelocity_X * 2 < (column + 1) * fGrid_Cell_Size && fPosition_X + 3 * fGrid_Cell_Size / 16 - fVelocity_X * 2 > column * fGrid_Cell_Size && fPosition_Y - 3 * fGrid_Cell_Size / 16 - fVelocity_Y * 2 < (row + 1) * fGrid_Cell_Size && fPosition_Y + 3 * fGrid_Cell_Size / 16 - fVelocity_Y * 2 > row * fGrid_Cell_Size && fRespawning_Timer >= fRespawning_Time / 2) {
						if (fRespawning_Timer < fRespawning_Time) {
							if (fRespawning_Timer > fRespawning_Time / 2) {
								fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
							}
						}
						else {
							fRespawning_Timer = 0;
						}
					}
				}
			}
		}
	}
}

void Game::Update_Glow() {
	fGlow_Timer += 1.f / iFrames_per_Second * fSpeed_of_Simulation;
	if (fGlow_Timer > fGlow_Time) {
		fGlow_Timer -= fGlow_Time;
		for (int i = 0; i < iGlow_Total; i++) {
			arrGlow_Velocity_Increase_X[i] = rand() % 2 == 1;
			arrGlow_Velocity_Increase_Y[i] = rand() % 2 == 1;
		}
	}
	for (int i = 0; i < iGlow_Total; i++) {
		if (arrGlow_Offset_X[i] < -0.5 && arrGlow_Velocity_X[i] < 0) {
			arrGlow_Velocity_Increase_X[i] = true;
		}
		else if (arrGlow_Offset_X[i] > 0.5 && arrGlow_Velocity_X[i] > 0) {
			arrGlow_Velocity_Increase_X[i] = false;
		}
		if (arrGlow_Velocity_Increase_X[i]) {
			arrGlow_Velocity_X[i] += 1.f / iFrames_per_Second * fGlow_Velocity_Speed * fSpeed_of_Simulation;
		}
		else {
			arrGlow_Velocity_X[i] -= 1.f / iFrames_per_Second * fGlow_Velocity_Speed * fSpeed_of_Simulation;
		}
		if (arrGlow_Offset_Y[i] < -0.5 && arrGlow_Velocity_Y[i] < 0) {
			arrGlow_Velocity_Increase_Y[i] = true;
		}
		else if (arrGlow_Offset_Y[i] > 0.5 && arrGlow_Velocity_Y[i] > 0) {
			arrGlow_Velocity_Increase_Y[i] = false;
		}
		if (arrGlow_Velocity_Increase_Y[i]) {
			arrGlow_Velocity_Y[i] += 1.f / iFrames_per_Second * fGlow_Velocity_Speed * fSpeed_of_Simulation;
		}
		else {
			arrGlow_Velocity_Y[i] -= 1.f / iFrames_per_Second * fGlow_Velocity_Speed * fSpeed_of_Simulation;
		}
		arrGlow_Offset_X[i] += arrGlow_Velocity_X[i] * fGlow_Velocity_Speed * fSpeed_of_Simulation;
		arrGlow_Offset_Y[i] += arrGlow_Velocity_Y[i] * fGlow_Velocity_Speed * fSpeed_of_Simulation;
	}
}

void Game::Update_Editor() {
	Update_Editor_Sidebar();
	if (mouseX > 0 && mouseX < 500 - iEditor_Sidebar_Width && mouseY > 0 && mouseY < 500 && window_Is_Active && !bEditor_Resizing) {
		if (bButton_Held_Mouse1 || bButton_Released_Mouse1) {
			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 80; x++) {
					fillMap[y][x] = 0;
				}
			}
			if (bButton_Held_Shift && bButton_Clicked_Mouse1) {
				dragX = (mouseX + fCamera_Position_X) / fGrid_Cell_Size;
				dragY = mouseY / fGrid_Cell_Size;
			}
			if (bButton_Held_Shift && bButton_Held_Mouse1) {
				float dragX2 = (mouseX + fCamera_Position_X) / fGrid_Cell_Size;
				float dragY2 = mouseY / fGrid_Cell_Size;
				if (dragY < dragY2) {
					for (int y = int(dragY); y < ceil(dragY2); y++) {
						if (dragX < dragX2) {
							for (int x = int(dragX); x < ceil(dragX2); x++) {
								fillMap[y][x] = 1;
							}
						}
						else {
							for (int x = int(dragX2); x < ceil(dragX); x++) {
								fillMap[y][x] = 1;
							}
						}
					}
				}
				else {
					for (int y = int(dragY2); y < ceil(dragY); y++) {
						if (dragX < dragX2) {
							for (int x = int(dragX); x < ceil(dragX2); x++) {
								fillMap[y][x] = 1;
							}
						}
						else {
							for (int x = int(dragX2); x < ceil(dragX); x++) {
								fillMap[y][x] = 1;
							}
						}
					}
				}
			}
			if (bButton_Held_Shift && bButton_Released_Mouse1) {
				float dragX2 = (mouseX + fCamera_Position_X) / fGrid_Cell_Size;
				float dragY2 = mouseY / fGrid_Cell_Size;
				if (dragX > dragX2) {
					float save_dragX = dragX;
					dragX = int(dragX2);
					dragX2 = ceil(save_dragX);
				}
				else {
					dragX = int(dragX);
					dragX2 = ceil(dragX2);
				}
				if (dragY > dragY2) {
					float save_dragY = dragY;
					dragY = int(dragY2);
					dragY2 = ceil(save_dragY);
				}
				else {
					dragY = int(dragY);
					dragY2 = ceil(dragY2);
				}
				for (int y = dragY; y < dragY2; y++) {
					for (int x = dragX; x < dragX2; x++) {
						(bDeveloper) ? map[iCurrent_Chapter][iCurrent_Level][y][x] = iBlock : editorMap[y][x] = iBlock;
					}
				}
			}
		}
		else {
			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 80; x++) {
					fillMap[y][x] = 0;
				}
			}
			if (bButton_Held_Shift && bButton_Clicked_Mouse2) {
				dragX = (mouseX + fCamera_Position_X) / fGrid_Cell_Size;
				dragY = mouseY / fGrid_Cell_Size;
			}
			if (bButton_Held_Shift && bButton_Held_Mouse2) {
				float dragX2 = (mouseX + fCamera_Position_X) / fGrid_Cell_Size;
				float dragY2 = mouseY / fGrid_Cell_Size;
				if (dragY < dragY2) {
					for (int y = int(dragY); y < ceil(dragY2); y++) {
						if (dragX < dragX2) {
							for (int x = int(dragX); x < ceil(dragX2); x++) {
								fillMap[y][x] = 1;
							}
						}
						else {
							for (int x = int(dragX2); x < ceil(dragX); x++) {
								fillMap[y][x] = 1;
							}
						}
					}
				}
				else {
					for (int y = int(dragY2); y < ceil(dragY); y++) {
						if (dragX < dragX2) {
							for (int x = int(dragX); x < ceil(dragX2); x++) {
								fillMap[y][x] = 1;
							}
						}
						else {
							for (int x = int(dragX2); x < ceil(dragX); x++) {
								fillMap[y][x] = 1;
							}
						}
					}
				}
			}
			if (bButton_Held_Shift && bButton_Released_Mouse2) {
				float dragX2 = (mouseX + fCamera_Position_X) / fGrid_Cell_Size;
				float dragY2 = mouseY / fGrid_Cell_Size;
				if (dragX > dragX2) {
					float save_dragX = dragX;
					dragX = int(dragX2);
					dragX2 = ceil(save_dragX);
				}
				else {
					dragX = int(dragX);
					dragX2 = ceil(dragX2);
				}
				if (dragY > dragY2) {
					float save_dragY = dragY;
					dragY = int(dragY2);
					dragY2 = ceil(save_dragY);
				}
				else {
					dragY = int(dragY);
					dragY2 = ceil(dragY2);
				}
				for (int y = dragY; y < dragY2; y++) {
					for (int x = dragX; x < dragX2; x++) {
						(bDeveloper) ? map[iCurrent_Chapter][iCurrent_Level][y][x] = 0 : editorMap[y][x] = 0;
					}
				}
			}
		}

		if (!bButton_Held_Shift) {
			if (bButton_Held_Mouse1) {
				(bDeveloper) ? map[iCurrent_Chapter][iCurrent_Level][int(mouseY / fGrid_Cell_Size)][int((mouseX + fCamera_Position_X) / fGrid_Cell_Size)] = iBlock : editorMap[int(mouseY / fGrid_Cell_Size)][int((mouseX + fCamera_Position_X) / fGrid_Cell_Size)] = iBlock;
			}
			if (bButton_Held_Mouse2) {
				(bDeveloper) ? map[iCurrent_Chapter][iCurrent_Level][int(mouseY / fGrid_Cell_Size)][int((mouseX + fCamera_Position_X) / fGrid_Cell_Size)] = 0 : editorMap[int(mouseY / fGrid_Cell_Size)][int((mouseX + fCamera_Position_X) / fGrid_Cell_Size)] = 0;
			}
		}
	}
}

void Game::Update_Editor_Sidebar() {
	if (mouseX > 500 - iEditor_Sidebar_Width - 4 && mouseX < 500 - iEditor_Sidebar_Width + 4 && mouseY > 0 && mouseY < 500) {
		SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
		if (bButton_Clicked_Mouse1) {
			bEditor_Resizing = true;
			bResizing_On_Edge = false;
			if (fCamera_Position_X - iEditor_Sidebar_Width >= 60 * fGrid_Cell_Size) {
				fCamera_Position_X = iEditor_Sidebar_Width + 60 * fGrid_Cell_Size;
				bResizing_On_Edge = true;
			}
		}
	}
	if (bEditor_Resizing) {
		if (bButton_Released_Mouse1) {
			bEditor_Resizing = false;
			return;
		}
		iEditor_Sidebar_Width = game_Width - mouseX;
		if (iEditor_Sidebar_Width > iEditor_Sidebar_Max_Width) {
			iEditor_Sidebar_Width = iEditor_Sidebar_Max_Width;
		}
		if (iEditor_Sidebar_Width < iEditor_Sidebar_Min_Width) {
			iEditor_Sidebar_Width = iEditor_Sidebar_Min_Width;
		}
		if (fCamera_Position_X - iEditor_Sidebar_Width > 60 * fGrid_Cell_Size) {
			fCamera_Position_X = iEditor_Sidebar_Width + 60 * fGrid_Cell_Size;
		}
		if (bResizing_On_Edge) {
			fCamera_Position_X = iEditor_Sidebar_Width + 60 * fGrid_Cell_Size;
		}
	}
}

void Game::Update_Particles() {
	if (fParticle_Timer >= fParticle_Respawn_Rate / iParticle_Total_at_Once) {
		if (!bFlying || !bStarted || fRespawning_Timer < fRespawning_Time) {
			arr_Particle_Active[iParticles_Init_Num] = false;
		}
		else {
			arr_Particle_Active[iParticles_Init_Num] = true;
		}
		arr_Particle_Velocity_X[iParticles_Init_Num] = 0;
		arr_Particle_Velocity_Y[iParticles_Init_Num] = 0;
		arr_Particle_Position_X[iParticles_Init_Num] = fPosition_X - fVelocity_X * 4;
		arr_Particle_Position_Y[iParticles_Init_Num] = fPosition_Y - fVelocity_Y * 4;
		arr_Particle_Start_Size[iParticles_Init_Num] = rand() % 3 + 2;
		arr_Particle_Size[iParticles_Init_Num] = 0;
		arr_Particle_Rotation_Speed[iParticles_Init_Num] = rand() % 360;
		arr_Particle_Rotation[iParticles_Init_Num] = rand() % 360;
		arr_Particle_Timer[iParticles_Init_Num] = 0;
		arr_Particle_Points[iParticles_Init_Num] = iParticle_Min_Points + rand()%(iParticle_Max_Points - iParticle_Min_Points + 1);
		iParticles_Init_Num++;
		fParticle_Timer -= fParticle_Respawn_Rate / iParticle_Total_at_Once;
	}
	if (iParticles_Init_Num >= iParticle_Total_at_Once) {
		iParticles_Init_Num = 0;
	}
	fParticle_Timer += 1.f / iFrames_per_Second * fSpeed_of_Simulation;

	for (int i = 0; i < iParticle_Total_at_Once; i++) {
		arr_Particle_Timer[i] += 1.f / iFrames_per_Second * fSpeed_of_Simulation;
		arr_Particle_Velocity_Y[i] += fParticle_Gravity * (1.f / iFrames_per_Second) * fSpeed_of_Simulation;
		arr_Particle_Position_X[i] += arr_Particle_Velocity_X[i] * fSpeed_of_Simulation;
		arr_Particle_Position_Y[i] += arr_Particle_Velocity_Y[i] * fSpeed_of_Simulation;
		arr_Particle_Size[i] = arr_Particle_Start_Size[i] * (1 + ((arr_Particle_Timer[i] / fParticle_Respawn_Rate) * (arr_Particle_Timer[i] / fParticle_Respawn_Rate)));
		arr_Particle_Rotation[i] += arr_Particle_Rotation_Speed[i] * (1.f / iFrames_per_Second) * fSpeed_of_Simulation;
		if (arr_Particle_Size[i] < 0) {
			arr_Particle_Size[i] = 0;
		}
	}
}

void Game::Update_Dead() {
	if (fRespawning_Timer == 0) {
		fRespawning_Size = 0;
		for (int x = 0; x < 5; x++) {
			for (int y = 0; y < 5; y++) {
				arrRespawning_Offset_X[x][y] = rand() % (iRespawning_Max_Offset * 2) + iRespawning_Max_Offset;
				arrRespawning_Offset_Y[x][y] = rand() % (iRespawning_Max_Offset * 2) + iRespawning_Max_Offset;
				arrRespawning_Acceleration[x][y] = rand() % (iRespawning_Max_Acceleration - iRespawning_Min_Acceleration) + iRespawning_Min_Acceleration;
				arrRespawning_Rotation[x][y] = rand() % 360;
				arrRespawning_Points[x][y] = rand() % (iRespawning_Max_Points - (iRespawning_Min_Points - 1)) + iRespawning_Min_Points;
				arrRespawning_Rotating_Left[x][y] = rand() % 2;
			}
		}
	}
	fRespawning_Timer += 1.f / iFrames_per_Second * fSpeed_of_Simulation;
	if (fRespawning_Timer > fRespawning_Time / 2 && fRespawning_Last_Timer < fRespawning_Time / 2) {
		fRespawning_Size = 450;
		for (int x = 0; x < 5; x++) {
			for (int y = 0; y < 5; y++) {
				arrRespawning_Offset_X[x][y] = rand() % (iRespawning_Max_Offset * 2) + iRespawning_Max_Offset;
				arrRespawning_Offset_Y[x][y] = rand() % (iRespawning_Max_Offset * 2) + iRespawning_Max_Offset;
				arrRespawning_Acceleration[x][y] = rand() % (iRespawning_Max_Acceleration - iRespawning_Min_Acceleration) + iRespawning_Min_Acceleration;
				arrRespawning_Rotation[x][y] = rand() % 360;
				arrRespawning_Points[x][y] = rand() % (iRespawning_Max_Points - (iRespawning_Min_Points - 1)) + iRespawning_Min_Points;
				arrRespawning_Rotating_Left[x][y] = rand() % 2;
			}
		}
		if (died_Next_Chapter != -1) {
			if (died_Next_Chapter == 5) {
				bEditor = true;
			}
			else {
				iCurrent_Chapter = died_Next_Chapter;
				iCurrent_Level = died_Next_Level;
				bEditor = false;
			}
		}
		died_Next_Chapter = -1;
		died_Next_Level = -1;
	}
	if (fRespawning_Timer <= fRespawning_Time / 2) {
		fRespawning_Size = (fRespawning_Timer / (fRespawning_Time / 2)) * (fRespawning_Timer / (fRespawning_Time / 2)) * 450;
	}
	if (fRespawning_Timer > fRespawning_Time / 2) {
		fRespawning_Size = ((fRespawning_Time - fRespawning_Timer) / (fRespawning_Time / 2)) * ((fRespawning_Time - fRespawning_Timer) / (fRespawning_Time / 2)) * 450;
	}
	if (fRespawning_Timer >= fRespawning_Time) {
		fRespawning_Size = 0;
	}
	if (fRespawning_Timer > fRespawning_Time / 2 && fRespawning_Last_Timer < fRespawning_Time / 2) {
		Respawn();
	}
	else if (fRespawning_Timer < fRespawning_Time / 2) {
		bStarted = false;
		fVelocity_X = 0;
		fVelocity_Y = 0;
		fVelocity_Forward = 0;
		fPosition_Y = -100;
	}
	fRespawning_Last_Timer = fRespawning_Timer;
}

void Game::Update_Decoration() {
	fDecoration_Randomness_Timer += 1.f / iFrames_per_Second * fSpeed_of_Simulation;
	if (fDecoration_Randomness_Timer > fDecoration_Randomness_Time) {
		fDecoration_Randomness_Timer -= fDecoration_Randomness_Time;
		for (int i = 0; i < 64; i++) {
			arrDecoration_Acceleration_X[i] = (rand() % 2) * 2 - 1;
			arrDecoration_Acceleration_Y[i] = (rand() % 2) * 2 - 1;
			arrDecoration_Color_Acceleration[i] = (rand() % 2) * 2 - 1;
		}
	}
	for (int i = 0; i < 64; i++) {
		arrDecoration_Color_Velocity[i] += arrDecoration_Color_Acceleration[i] / iFrames_per_Second * fSpeed_of_Simulation;
		arrDecoration_Color_Value[i] += arrDecoration_Color_Velocity[i] / iFrames_per_Second * fSpeed_of_Simulation;
		if (arrDecoration_Color_Value[i] < 0) {
			arrDecoration_Color_Value[i] *= -1;
			arrDecoration_Color_Velocity[i] = abs(arrDecoration_Color_Velocity[i]) / 2;
		}
		if (arrDecoration_Color_Value[i] > 1) {
			arrDecoration_Color_Value[i] = 1 - (arrDecoration_Color_Value[i] - 1);
			arrDecoration_Color_Velocity[i] = 0 - abs(arrDecoration_Color_Velocity[i]) / 2;
		}
		arrDecoration_Velocity_X[i] += (arrDecoration_Acceleration_X[i] * arrDecoration_Main_Velocity[i]) * 1.f / iFrames_per_Second * fSpeed_of_Simulation;
		arrDecoration_Velocity_Y[i] += (arrDecoration_Acceleration_Y[i] * arrDecoration_Main_Velocity[i]) * 1.f / iFrames_per_Second * fSpeed_of_Simulation;
		if (!bEditor && iCurrent_Chapter == 0 || bEditor && iLevel == 0) {
			if (arrDecoration_Offset_X[i] < 0 - iDecoration_Max_Offset && arrDecoration_Velocity_X[i] < 0) {
				arrDecoration_Acceleration_X[i] = 1;
			}
			if (arrDecoration_Offset_X[i] > iDecoration_Max_Offset && arrDecoration_Velocity_X[i] > 0) {
				arrDecoration_Acceleration_X[i] = -1;
			}
			if (arrDecoration_Velocity_Y[i] < arrDecoration_Main_Velocity[i] * (float(iDecoration_Min_Main_Velocity) / float(iDecoration_Max_Main_Velocity))) {
				arrDecoration_Acceleration_Y[i] = 1;
			}
			if (arrDecoration_Velocity_Y[i] > arrDecoration_Main_Velocity[i]) {
				arrDecoration_Acceleration_Y[i] = -1;
			}
			arrDecoration_Offset_X[i] += arrDecoration_Velocity_X[i] * 1.f / iFrames_per_Second * fSpeed_of_Simulation;
			arrDecoration_Offset_Y[i] = 0;
			arrDecoration_Position_Y[i] += arrDecoration_Velocity_Y[i] * 1.f / iFrames_per_Second * fSpeed_of_Simulation;
			if (arrDecoration_Position_Y[i] > game_Height + 50) {
				arrDecoration_Position_Y[i] = -50;
			}
			arrDecoration_Rotation[i] = atan2(arrDecoration_Velocity_Y[i] * 4, arrDecoration_Velocity_X[i]) * 180 / 3.141;
		}
		if (!bEditor && iCurrent_Chapter == 1 || bEditor && iLevel == 1) {
			if (arrDecoration_Offset_X[i] < 0 - iDecoration_Max_Offset && arrDecoration_Velocity_X[i] < 0) {
				arrDecoration_Acceleration_X[i] = 1;
			}
			if (arrDecoration_Offset_X[i] > iDecoration_Max_Offset && arrDecoration_Velocity_X[i] > 0) {
				arrDecoration_Acceleration_X[i] = -1;
			}
			if (arrDecoration_Offset_Y[i] < 0 - iDecoration_Max_Offset && arrDecoration_Velocity_Y[i] < 0) {
				arrDecoration_Acceleration_Y[i] = 1;
			}
			if (arrDecoration_Offset_Y[i] > iDecoration_Max_Offset && arrDecoration_Velocity_Y[i] > 0) {
				arrDecoration_Acceleration_Y[i] = -1;
			}
			arrDecoration_Offset_X[i] += arrDecoration_Velocity_X[i] * 1.f / iFrames_per_Second * fSpeed_of_Simulation;
			arrDecoration_Offset_Y[i] += arrDecoration_Velocity_Y[i] * 1.f / iFrames_per_Second * fSpeed_of_Simulation;
			arrDecoration_Rotation[i] = atan2(arrDecoration_Velocity_Y[i], arrDecoration_Velocity_X[i]) * 180 / 3.141;
		}
		if (!bEditor && iCurrent_Chapter == 2 || bEditor && iLevel == 2) {
			if (arrDecoration_Velocity_X[i] < arrDecoration_Main_Velocity[i]) {
				arrDecoration_Acceleration_X[i] = 1;
			}
			if (arrDecoration_Velocity_X[i] > arrDecoration_Main_Velocity[i] * (float(iDecoration_Min_Main_Velocity) / float(iDecoration_Max_Main_Velocity))) {
				arrDecoration_Acceleration_X[i] = -1;
			}
			if (arrDecoration_Offset_Y[i] < 0 - iDecoration_Max_Offset && arrDecoration_Velocity_Y[i] < 0) {
				arrDecoration_Acceleration_Y[i] = 1;
			}
			if (arrDecoration_Offset_Y[i] > iDecoration_Max_Offset && arrDecoration_Velocity_Y[i] > 0) {
				arrDecoration_Acceleration_Y[i] = -1;
			}
			arrDecoration_Offset_X[i] = 0;
			arrDecoration_Offset_Y[i] += arrDecoration_Velocity_Y[i] * 1.f / iFrames_per_Second * fSpeed_of_Simulation;
			arrDecoration_Position_X[i] -= arrDecoration_Velocity_X[i] * 7 * 1.f / iFrames_per_Second * fSpeed_of_Simulation;
			if (arrDecoration_Position_X[i] < -50) {
				arrDecoration_Position_X[i] = 79 * fGrid_Cell_Size + 50;
			}
			arrDecoration_Rotation[i] = atan2(0 - arrDecoration_Velocity_Y[i], arrDecoration_Velocity_X[i] * 7) * 180 / 3.141;
		}
		if (!bEditor && iCurrent_Chapter == 3 || bEditor && iLevel == 3) {
			if (arrDecoration_Offset_X[i] < 0 - iDecoration_Max_Offset && arrDecoration_Velocity_X[i] < 0) {
				arrDecoration_Acceleration_X[i] = 1;
			}
			if (arrDecoration_Offset_X[i] > iDecoration_Max_Offset && arrDecoration_Velocity_X[i] > 0) {
				arrDecoration_Acceleration_X[i] = -1;
			}
			if (arrDecoration_Velocity_Y[i] < arrDecoration_Main_Velocity[i]) {
				arrDecoration_Acceleration_Y[i] = 1;
			}
			if (arrDecoration_Velocity_Y[i] > arrDecoration_Main_Velocity[i] * (float(iDecoration_Min_Main_Velocity) / float(iDecoration_Max_Main_Velocity))) {
				arrDecoration_Acceleration_Y[i] = -1;
			}
			arrDecoration_Offset_X[i] += arrDecoration_Velocity_X[i] * 1.f / iFrames_per_Second * fSpeed_of_Simulation;
			arrDecoration_Offset_Y[i] = 0;
			arrDecoration_Position_Y[i] -= arrDecoration_Velocity_Y[i] * 1.f / iFrames_per_Second * fSpeed_of_Simulation;
			if (arrDecoration_Position_Y[i] < -50) {
				arrDecoration_Position_Y[i] = game_Height + 50;
			}
			arrDecoration_Rotation[i] = atan2(0 - arrDecoration_Velocity_Y[i], arrDecoration_Velocity_X[i]) * 180 / 3.141;
		}
		if (!bEditor && iCurrent_Chapter == 4 || bEditor && iLevel == 4) {
			if (arrDecoration_Offset_X[i] < 0 - iDecoration_Max_Offset && arrDecoration_Velocity_X[i] < 0) {
				arrDecoration_Acceleration_X[i] = 1;
			}
			if (arrDecoration_Offset_X[i] > iDecoration_Max_Offset && arrDecoration_Velocity_X[i] > 0) {
				arrDecoration_Acceleration_X[i] = -1;
			}
			if (arrDecoration_Velocity_Y[i] < arrDecoration_Main_Velocity[i]) {
				arrDecoration_Acceleration_Y[i] = 1;
			}
			if (arrDecoration_Velocity_Y[i] > arrDecoration_Main_Velocity[i] * (float(iDecoration_Min_Main_Velocity) / float(iDecoration_Max_Main_Velocity))) {
				arrDecoration_Acceleration_Y[i] = -1;
			}
			arrDecoration_Offset_X[i] += arrDecoration_Velocity_X[i] * 1.f / iFrames_per_Second * fSpeed_of_Simulation;
			arrDecoration_Offset_Y[i] = 0;
			arrDecoration_Position_Y[i] -= arrDecoration_Velocity_Y[i] * 1.f / iFrames_per_Second * fSpeed_of_Simulation;
			if (arrDecoration_Position_Y[i] < -50) {
				arrDecoration_Position_Y[i] = game_Height + 50;
			}
			arrDecoration_Rotation[i] = atan2(0 - arrDecoration_Velocity_Y[i], arrDecoration_Velocity_X[i]) * 180 / 3.141;
		}
		//if (!bEditor && (iCurrent_Level == 8 || iCurrent_Level == 9) || bEditor && iLevel == 4) {
		//	arrDecoration_Rotation[i] = atan2(arrDecoration_Velocity_Y[i], arrDecoration_Velocity_X[i]) * 180 / 3.141;
		//	if (arrDecoration_Offset_X[i] < 0 - iDecoration_Max_Offset && arrDecoration_Velocity_X[i] < 0) {
		//		arrDecoration_Acceleration_X[i] = 1;
		//	}
		//	if (arrDecoration_Offset_X[i] > iDecoration_Max_Offset && arrDecoration_Velocity_X[i] > 0) {
		//		arrDecoration_Acceleration_X[i] = -1;
		//	}
		//	if (arrDecoration_Offset_Y[i] < 0 - iDecoration_Max_Offset && arrDecoration_Velocity_Y[i] < 0) {
		//		arrDecoration_Acceleration_Y[i] = 1;
		//	}
		//	if (arrDecoration_Offset_Y[i] > iDecoration_Max_Offset && arrDecoration_Velocity_Y[i] > 0) {
		//		arrDecoration_Acceleration_Y[i] = -1;
		//	}
		//	arrDecoration_Velocity_X[i] += (arrDecoration_Acceleration_X[i] * arrDecoration_Main_Velocity[i]) * 1.f / iFrames_per_Second;
		//	arrDecoration_Velocity_Y[i] += (arrDecoration_Acceleration_Y[i] * arrDecoration_Main_Velocity[i]) * 1.f / iFrames_per_Second;
		//	arrDecoration_Offset_X[i] += arrDecoration_Velocity_X[i] * 1.f / iFrames_per_Second;
		//	arrDecoration_Offset_Y[i] += arrDecoration_Velocity_Y[i] * 1.f / iFrames_per_Second;
		//}
	}
}

void Game::Respawn() {
	Initialize_Glow();
	bStarted = false;
	fPosition_X = 200;
	fPosition_Y = 200;
	fVelocity_X = 0;
	fVelocity_Y = 0;
	fVelocity_Forward = 0;
	fCamera_Position_X = 0;
	fCamera_Position_Y = 0;
	fRocket_Rotation = atan2(0, 1) * 180 / 3.141;
	this->Initialize_Mixed();
}

void Game::Draw()
{
	if (!bEditor) {
		if (iCurrent_Chapter >= 0 && iCurrent_Chapter <= 4) {
			this->window->clear(sf::Color(arrColor[iCurrent_Chapter][0][0], arrColor[iCurrent_Chapter][0][1], arrColor[iCurrent_Chapter][0][2], 255));
		}
		else {
			this->window->clear(sf::Color(0, 0, 0, 255));
		}
	}
	else {
		this->window->clear(sf::Color(arrColor[iLevel][0][0], arrColor[iLevel][0][1], arrColor[iLevel][0][2], 255));
	}

	if (iCurrent_Chapter >= 0 && iCurrent_Chapter <= 4 || bEditor) {
		Draw_Game();
	}

	//sf::RectangleShape hitBox;
	//hitBox.setPosition(Get_Position_with_Zoom("x", fPosition_X - 3 * fGrid_Cell_Size / 16 - fVelocity_X * 2), Get_Position_with_Zoom("y", fPosition_Y - 3 * fGrid_Cell_Size / 16 - fVelocity_Y * 2));
	//hitBox.setSize(sf::Vector2f(Get_Size_with_Zoom() * 6 * fGrid_Cell_Size / 16, Get_Size_with_Zoom() * 6 * fGrid_Cell_Size / 16));
	//hitBox.setFillColor(sf::Color(255, 0, 0, 255));
	//this->window->draw(hitBox);

	if (!bPlaying && !bStart_Menu) {
		sf::RectangleShape rect;
		rect.setPosition(0, 0);
		rect.setSize(sf::Vector2f(500, 500));
		rect.setFillColor(sf::Color(0, 0, 0, 127));
		this->window->draw(rect);

		Draw_Menu();
	}
	else if (bStart_Menu) {
		sf::Sprite sprite;
		sprite.setPosition(0, 0);
		sprite.setScale(500.f / 48, 500.f / 48);
		sprite.setTexture(menu);
		this->window->draw(sprite);

		sf::RectangleShape rect;
		rect.setPosition(0, 0);
		rect.setSize(sf::Vector2f(500, 500));
		rect.setFillColor(sf::Color(0, 0, 0, 127));
		this->window->draw(rect);

		Draw_Start_Menu();
	}
	
	sf::RectangleShape rect;
	rect.setFillColor(sf::Color(0, 0, 0, 255));
	rect.setPosition(-10000, 0);
	rect.setSize(sf::Vector2f(10000, game_Height));
	this->window->draw(rect);
	rect.setPosition(game_Width, 0);
	rect.setSize(sf::Vector2f(10000, game_Height));
	this->window->draw(rect);
	rect.setPosition(0, -10000);
	rect.setSize(sf::Vector2f(game_Width, 10000));
	this->window->draw(rect);
	rect.setPosition(0, game_Height);
	rect.setSize(sf::Vector2f(game_Width, 10000));
	this->window->draw(rect);

	this->window->display();

	bButton_Released_Mouse1 = false;
	bButton_Released_Mouse2 = false;
}

void Game::Draw_Menu() {
	if (bButton_Held_Down && !bButton_Pressed_Down) {
		bButton_Pressed_Down = true;
		if (iSelected_Difficulty < 5) {
			iSelected_Difficulty++;
		}
		else {
			iSelected_Difficulty = 0;
		}
	}
	if (!bButton_Held_Down) {
		bButton_Pressed_Down = false;
	}
	if (bButton_Held_Up && !bButton_Pressed_Up) {
		bButton_Pressed_Up = true;
		if (iSelected_Difficulty > 0) {
			iSelected_Difficulty--;
		}
		else {
			iSelected_Difficulty = 5;
		}
	}
	if (!bButton_Held_Up) {
		bButton_Pressed_Up = false;
	}
	if (bButton_Held_Left && !bButton_Pressed_Left) {
		bButton_Pressed_Left = true;
		right_Side = false;
	}
	if (!bButton_Held_Left) {
		bButton_Pressed_Left = false;
	}
	if (bButton_Held_Right && !bButton_Pressed_Right) {
		bButton_Pressed_Right = true;
		right_Side = true;
	}
	if (!bButton_Held_Right) {
		bButton_Pressed_Right = false;
	}
	if (bButton_Held_Enter && !bButton_Pressed_Enter) {
		bButton_Pressed_Enter = true;
		if (!right_Side) {
			bStart_Menu = true;
			selected_Option = 1;
			arrow_Velocity = arrow_Start_Velocity;
			arrow_Distance = arrow_Velocity * (1.f / iFrames_per_Second);
			return;
		}
		if (iSelected_Difficulty == 5) {
			died_Next_Chapter = 5;
			if (fRespawning_Timer < fRespawning_Time) {
				if (fRespawning_Timer > fRespawning_Time / 2) {
					fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
				}
			}
			else {
				fRespawning_Timer = 0;
			}
			bPlaying = true;
			bEditing = false;
			return;
		}
		died_Next_Chapter = iSelected_Difficulty;
		died_Next_Level = 0;
		if (fRespawning_Timer < fRespawning_Time) {
			if (fRespawning_Timer > fRespawning_Time / 2) {
				fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
			}
		}
		else {
			fRespawning_Timer = 0;
		}
		bPlaying = true;
	}
	if (!bButton_Held_Enter) {
		bButton_Pressed_Enter = false;
	}

	if (right_Side) {
		sf::CircleShape triangle(25, 3);
		triangle.setPosition(172.5 - arrow_Distance * 10, 500.f / 12 * (((iSelected_Difficulty) * 2) + 0.5f));
		triangle.setRotation(90);
		this->window->draw(triangle);
	}
	else {
		sf::CircleShape triangle(25, 3);
		triangle.setPosition(40 + arrow_Distance * 10 + 50, 75);
		triangle.setRotation(-90);
		this->window->draw(triangle);
	}

	for (int i = 0; i < 5; i++) {
		sf::RectangleShape rect;
		rect.setPosition(187.5, 500.f / 12 * ((i * 2) + 0.5f));
		rect.setSize(sf::Vector2f(300, 50));
		if (mouseY > 500.f / 12 * ((i * 2) + 0.5f) && mouseY < 500.f / 12 * ((i * 2) + 0.5f) + 50 && mouseX > 187.5 && mouseX < 487.5) {
			if (bButton_Released_Mouse1) {
				died_Next_Chapter = i;
				died_Next_Level = 0;
				if (fRespawning_Timer < fRespawning_Time) {
					if (fRespawning_Timer > fRespawning_Time / 2) {
						fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
					}
				}
				else {
					fRespawning_Timer = 0;
				}
				bPlaying = true;
			}
			else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				rect.setFillColor(sf::Color(127, 127, 127, 255));
			}
			else {
				rect.setFillColor(sf::Color(191, 191, 191, 255));
			}
		}
		else {
			rect.setFillColor(sf::Color(255, 255, 255, 255));
		}
		sf::Sprite sprite;
		sprite.setPosition(437.5 + 50.f * 0.15, 500.f / 12 * ((i * 2) + 0.5f) + 50.f * 0.15);
		sprite.setScale(sf::Vector2f(50.f / 8 * 0.7, 50.f / 8 * 0.7));
		sprite.setTexture(number[i + 1]);
		this->window->draw(rect);
		sprite.setColor(sf::Color(0, 0, 0, 255));
		this->window->draw(sprite);
		sprite.setPosition(sprite.getPosition().x + 24.f / 8, sprite.getPosition().y + 24.f / 8);
		sprite.setColor(sf::Color(0, 0, 0, 127));
		this->window->draw(sprite);

		sf::Sprite sprt;
		sprt.setPosition(187.5 + 50.f * 0.15, 500.f / 12 * ((i * 2) + 0.5f) + 50.f * 0.15);
		sprt.setScale(sf::Vector2f(50.f / 56 * 0.7 * 7, 50.f / 8 * 0.7));
		sprt.setTexture(chapter);
		sprt.setColor(sf::Color(0, 0, 0, 255));
		this->window->draw(sprt);
		sprt.setPosition(sprt.getPosition().x + 24.f / 8, sprt.getPosition().y + 24.f / 8);
		sprt.setColor(sf::Color(0, 0, 0, 127));
		this->window->draw(sprt);
	}
	sf::RectangleShape rect;
	rect.setPosition(187.5, 500.f / 12 * ((5 * 2) + 0.5f));
	rect.setSize(sf::Vector2f(300, 50));
	if (mouseX > 187.5 && mouseX < 487.5 && mouseY > 500.f / 12 * ((5 * 2) + 0.5f) && mouseY < 500.f / 12 * ((5 * 2) + 0.5f) + 50) {
		if (bButton_Released_Mouse1) {
			died_Next_Chapter = 5;
			if (fRespawning_Timer < fRespawning_Time) {
				if (fRespawning_Timer > fRespawning_Time / 2) {
					fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
				}
			}
			else {
				fRespawning_Timer = 0;
			}
			bPlaying = true;
			bEditing = false;
			return;
		}
		else if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(127, 127, 127, 255));
		}
		else {
			rect.setFillColor(sf::Color(191, 191, 191, 255));
		}
	}
	else {
		rect.setFillColor(sf::Color(255, 255, 255, 255));
	}
	sf::Sprite sprite;
	sprite.setPosition(437.5 + 50.f * 0.15, 500.f / 12 * ((5 * 2) + 0.5f) + 50.f * 0.15);
	sprite.setScale(sf::Vector2f(50.f / 8 * 0.7, 50.f / 8 * 0.7));
	sprite.setTexture(btnEditor);
	this->window->draw(rect);
	sprite.setColor(sf::Color(0, 0, 0, 255));
	this->window->draw(sprite);
	sprite.setPosition(sprite.getPosition().x + 24.f / 8, sprite.getPosition().y + 24.f / 8);
	sprite.setColor(sf::Color(0, 0, 0, 127));
	this->window->draw(sprite);

	sf::Sprite sprt;
	sprt.setPosition(187.5 + 50.f * 0.15, 500.f / 12 * ((5 * 2) + 0.5f) + 50.f * 0.15);
	sprt.setScale(sf::Vector2f(50.f / 56 * 0.7 * 7, 50.f / 8 * 0.7));
	sprt.setTexture(editor);
	sprt.setColor(sf::Color(0, 0, 0, 255));
	this->window->draw(sprt);
	sprt.setPosition(sprt.getPosition().x + 24.f / 8, sprt.getPosition().y + 24.f / 8);
	sprt.setColor(sf::Color(0, 0, 0, 127));
	this->window->draw(sprt);

	rect.setPosition(25, 25);
	rect.setSize(sf::Vector2f(50, 50));
	rect.setFillColor(sf::Color(255, 255, 255, 255));
	if (mouseX > 25 && mouseX < 25 + 50 && mouseY > 25 && mouseY < 25 + 50) {
		if (!bDeveloper) {
			if (bButton_Released_Mouse1) {
				bStart_Menu = true;
				selected_Option = 1;
				arrow_Velocity = arrow_Start_Velocity;
				arrow_Distance = arrow_Velocity * (1.f / iFrames_per_Second);
				return;
			}
			else if (bButton_Held_Mouse1) {
				rect.setFillColor(sf::Color(127, 127, 127, 255));
			}
			else {
				rect.setFillColor(sf::Color(191, 191, 191, 255));
			}
		}
	}
	sprt.setTexture(back);
	sprt.setPosition(25 + 50.f * 0.15, 25 + 50.f * 0.15);
	sprt.setScale(50.f / 8 * 0.7, 50.f / 8 * 0.7);
	this->window->draw(rect);
	sprt.setColor(sf::Color(0, 0, 0, 255));
	this->window->draw(sprt);
	sprt.setPosition(sprt.getPosition().x + 24.f / 8, sprt.getPosition().y + 24.f / 8);
	sprt.setColor(sf::Color(0, 0, 0, 127));
	this->window->draw(sprt);
}

void Game::Draw_Start_Menu() {
	if (!bExit_Warning) {
		if (bButton_Held_Down && !bButton_Pressed_Down && !bExit_Warning) {
			bButton_Pressed_Down = true;
			selected_Option = 0;
		}
		if (!bButton_Held_Down) {
			bButton_Pressed_Down = false;
		}
		if (bButton_Held_Up && !bButton_Pressed_Up && !bExit_Warning) {
			bButton_Pressed_Up = true;
			selected_Option = 1;
		}
		if (!bButton_Held_Up) {
			bButton_Pressed_Up = false;
		}

		if (bButton_Held_Enter && !bButton_Pressed_Enter) {
			bButton_Pressed_Enter = true;
			if (selected_Option == 0) {
				no_Selected = false;
				bExit_Warning = true;
			}
			if (selected_Option == 1) {
				bStart_Menu = false;
				arrow_Velocity = arrow_Start_Velocity;
				arrow_Distance = arrow_Velocity * (1.f / iFrames_per_Second);
				right_Side = true;
			}
		}
		if (!bButton_Held_Enter) {
			bButton_Pressed_Enter = false;
		}
	}

	sf::CircleShape triangle(25, 3);
	triangle.setPosition(260 - arrow_Distance * 10, 425 - selected_Option * 75);
	triangle.setRotation(90);
	this->window->draw(triangle);

	sf::RectangleShape rect;
	rect.setPosition(275, 425);
	rect.setSize(sf::Vector2f(200, 50));
	rect.setFillColor(sf::Color(255, 255, 255, 255));
	if (mouseX > 275 && mouseX < 275 + 200 && mouseY > 425 && mouseY < 425 + 50 && !bExit_Warning) {
		rect.setFillColor(sf::Color(191, 191, 191, 255));
		if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(127, 127, 127, 255));
		}
		if (bButton_Released_Mouse1) {
			no_Selected = false;
			bExit_Warning = true;
		}
	}
	this->window->draw(rect);

	sf::Sprite sprt;
	sprt.setTexture(txExit);
	sprt.setPosition(275 + 200.f * 0.15, 425 + 50.f * 0.15);
	sprt.setScale(200.f / 32 * 0.7, 50.f / 8 * 0.7);
	sprt.setColor(sf::Color(0, 0, 0, 255));
	this->window->draw(sprt);
	sprt.setPosition(sprt.getPosition().x + 24.f / 8, sprt.getPosition().y + 24.f / 8);
	sprt.setColor(sf::Color(0, 0, 0, 127));
	this->window->draw(sprt);

	rect.setPosition(275, 350);
	rect.setSize(sf::Vector2f(200, 50));
	rect.setFillColor(sf::Color(255, 255, 255, 255));
	if (mouseX > 275 && mouseX < 275 + 200 && mouseY > 350 && mouseY < 350 + 50 && !bExit_Warning) {
		rect.setFillColor(sf::Color(191, 191, 191, 255));
		if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(127, 127, 127, 255));
		}
		if (bButton_Released_Mouse1) {
			bStart_Menu = false;
			arrow_Velocity = arrow_Start_Velocity;
			arrow_Distance = arrow_Velocity * (1.f / iFrames_per_Second);
			right_Side = true;
		}
	}
	this->window->draw(rect);

	sprt.setTexture(txPlay);
	sprt.setPosition(275 + 200.f * 0.15, 350 + 50.f * 0.15);
	sprt.setScale(200.f / 32 * 0.7, 50.f / 8 * 0.7);
	sprt.setColor(sf::Color(0, 0, 0, 255));
	this->window->draw(sprt);
	sprt.setPosition(sprt.getPosition().x + 24.f / 8, sprt.getPosition().y + 24.f / 8);
	sprt.setColor(sf::Color(0, 0, 0, 127));
	this->window->draw(sprt);

	if (bExit_Warning) {

		if (bButton_Held_Right && !bButton_Pressed_Right) {
			bButton_Pressed_Right = true;
			no_Selected = true;
		}
		if (!bButton_Held_Right) {
			bButton_Pressed_Right = false;
		}
		if (bButton_Held_Left && !bButton_Pressed_Left) {
			bButton_Pressed_Left = true;
			no_Selected = false;
		}
		if (!bButton_Held_Left) {
			bButton_Pressed_Left = false;
		}

		if (bButton_Held_Enter && !bButton_Pressed_Enter) {
			bButton_Pressed_Enter = true;
			if (!no_Selected) {
				Save();
				this->window->close();
			}
			if (no_Selected) {
				bExit_Warning = false;
			}
		}
		if (!bButton_Held_Enter) {
			bButton_Pressed_Enter = false;
		}

		sf::Sprite sprConfirm;
		sf::Sprite sprite;
		sf::RectangleShape rect;
		rect.setPosition(75, 125);
		rect.setSize(sf::Vector2f(350, 250));
		rect.setFillColor(sf::Color(0, 0, 0, 225));
		this->window->draw(rect);
		sprConfirm.setPosition(75, 125);
		sprConfirm.setScale(350.f / 76, 100.f / 20);
		sprConfirm.setTexture(confirm);
		this->window->draw(sprConfirm);
		rect.setPosition(100, 300);
		rect.setSize(sf::Vector2f(137.5f, 50));
		if (mouseX > 100 && mouseX < 100 + 137.5f && mouseY > 300 && mouseY < 300 + 50) {
			rect.setFillColor(sf::Color(191, 191, 191, 255));
			if (bButton_Held_Mouse1) {
				rect.setFillColor(sf::Color(127, 127, 127, 255));
			}
			if (bButton_Released_Mouse1) {
				Save();
				this->window->close();
			}
		}
		else {
			rect.setFillColor(sf::Color(255, 255, 255, 255));
		}
		this->window->draw(rect);
		sprite.setPosition(100 + 137.5f * 0.15, 300 + 50.f * 0.15);
		sprite.setScale(137.5f / 24 * 0.7, 50.f / 8 * 0.7);
		sprite.setTexture(yes);
		sprite.setColor(sf::Color(0, 0, 0, 255));
		this->window->draw(sprite);
		sprite.setPosition(sprite.getPosition().x + 24.f / 8, sprite.getPosition().y + 24.f / 8);
		sprite.setColor(sf::Color(0, 0, 0, 127));
		this->window->draw(sprite);
		rect.setPosition(262.5f, 300);
		rect.setSize(sf::Vector2f(137.5f, 50));
		if (mouseX > 262.5f && mouseX < 262.5f + 137.5f && mouseY > 300 && mouseY < 300 + 50) {
			rect.setFillColor(sf::Color(191, 191, 191, 255));
			if (bButton_Held_Mouse1) {
				rect.setFillColor(sf::Color(127, 127, 127, 255));
			}
			if (bButton_Released_Mouse1) {
				bExit_Warning = false;
			}
		}
		else {
			rect.setFillColor(sf::Color(255, 255, 255, 255));
		}
		this->window->draw(rect);
		sprite.setPosition(262.5f + 137.5f * 0.25, 300 + 50.f * 0.15);
		sprite.setScale(137.5f / 16 * 0.5, 50.f / 8 * 0.7);
		sprite.setTexture(no);
		sprite.setColor(sf::Color(0, 0, 0, 255));
		this->window->draw(sprite);
		sprite.setPosition(sprite.getPosition().x + 24.f / 8, sprite.getPosition().y + 24.f / 8);
		sprite.setColor(sf::Color(0, 0, 0, 127));
		this->window->draw(sprite);

		sf::CircleShape triangle(25, 3);
		triangle.setPosition(125 + 162.5f * no_Selected + 137.5f / 2, 285 - arrow_Distance * 10);
		triangle.setRotation(180);
		this->window->draw(triangle);
	}
}

void Game::Draw_Game() {
	Draw_Glow();

	Draw_Particles();

	sf::Sprite sprite;
	sprite.setTexture(ship);
	sprite.setColor(sf::Color(0, 0, 0, 127));
	sprite.setPosition(Get_Position_with_Zoom("x", fPosition_X + 30.f / 8), Get_Position_with_Zoom("y", fPosition_Y + 30.f / 8));
	sprite.setOrigin(Get_Origin_with_Zoom("x") / 3, Get_Origin_with_Zoom("y") / 3);
	sprite.setRotation(fRocket_Rotation);
	sprite.setScale(sf::Vector2f((Get_Size_with_Zoom() * (iRocket_Width + iRocket_Width * fVelocity_Forward * fRocket_Width_based_on_Velocity_Sensitivity)) / 8, (Get_Size_with_Zoom() * (iRocket_Height - fRocket_Height_based_on_Velocity_Sensitivity * ((iRocket_Height * fVelocity_Forward * fRocket_Width_based_on_Velocity_Sensitivity) / iRocket_Height))) / 8));

	this->player.setOrigin(Get_Origin_with_Zoom("x") / 3, Get_Origin_with_Zoom("y") / 3);
	this->player.setPosition(Get_Position_with_Zoom("x", fPosition_X), Get_Position_with_Zoom("y", fPosition_Y));
	this->player.setRotation(fRocket_Rotation);
	this->player.setScale(sf::Vector2f((Get_Size_with_Zoom() * (iRocket_Width + iRocket_Width * fVelocity_Forward * fRocket_Width_based_on_Velocity_Sensitivity)) / 8, (Get_Size_with_Zoom() * (iRocket_Height - fRocket_Height_based_on_Velocity_Sensitivity * ((iRocket_Height * fVelocity_Forward * fRocket_Width_based_on_Velocity_Sensitivity) / iRocket_Height))) / 8));

	this->window->draw(sprite);
	this->window->draw(this->player);

	Draw_Gameplay();

	Draw_Decoration();

	Draw_Overlay();

	Draw_Respawn();

	if (bEditor && bEditing || bDeveloper && !bDeveloper_Playtesting) {
		Draw_Editor_Sidebar();
	}
}

void Game::Draw_Respawn() {
	if (iRespawning_Animation == 0) {
		sf::CircleShape respawning_Shape;
		respawning_Shape.setFillColor(sf::Color(255, 255, 255, 255));
		float size;
		float multi = 1.05;
		for (int i = 0; i < 2; i++) {
			for (int x = 0; x < 5; x++) {
				for (int y = 0; y < 5; y++) {
					size = multi * (fRespawning_Size * (float(arrRespawning_Acceleration[x][y]) / 100)) / 2;
					respawning_Shape.setPointCount(arrRespawning_Points[x][y]);
					respawning_Shape.setOrigin(size, size);
					if (arrRespawning_Rotating_Left[x][y]) {
						respawning_Shape.setRotation(-arrRespawning_Acceleration[x][y] / multi * size / 200 + arrRespawning_Rotation[x][y]);
					}
					else {
						respawning_Shape.setRotation(arrRespawning_Acceleration[x][y] / multi * size / 200 + arrRespawning_Rotation[x][y]);
					}
					respawning_Shape.setRadius(size);
					respawning_Shape.setPosition(500.f * (float(float(x) - 0.5) / 4) + arrRespawning_Offset_X[x][y], 500.f * (float(float(y) - 0.5) / 4) + arrRespawning_Offset_Y[x][y]);
					this->window->draw(respawning_Shape);
				}
			}
			multi = 1;
			respawning_Shape.setFillColor(sf::Color(0, 0, 0, 255));
		}
	}
	if (iRespawning_Animation == 1) {
		sf::RectangleShape respawning_Shape;
		respawning_Shape.setFillColor(sf::Color(0, 0, 0, 255));
		for (int x = 0; x < 5; x++) {
			float size = (fRespawning_Size * (float(arrRespawning_Acceleration[x][0] * 1.5 - 87.5f) / 100));
			for (int y = 0; y < 2; y++) {
				int top = 0;
				if (y == 1) { top = 1; size = (fRespawning_Size * (350.f / 100)) / 2 - size; }
				respawning_Shape.setSize(sf::Vector2f(500.f / 5, size));
				respawning_Shape.setPosition((500.f / 5) * x, y * 500 - size * top);
				this->window->draw(respawning_Shape);
			}
		}
	}
}

void Game::Draw_Glow() {
	sf::Sprite sprite;
	sprite.setTexture(glow);
	for (int i = 0; i < iGlow_Total; i++) {
		sprite.setPosition(Get_Position_with_Zoom("x", arrGlow_Position_X[i] + arrGlow_Offset_X[i] * arrGlow_Scale[i] * fGlow_Offset_based_on_Size_Multiplier), Get_Position_with_Zoom("y", arrGlow_Position_Y[i] + arrGlow_Offset_Y[i] * arrGlow_Scale[i] * fGlow_Offset_based_on_Size_Multiplier));
		sprite.setScale(sf::Vector2f(Get_Size_with_Zoom() * arrGlow_Scale[i] / 256, Get_Size_with_Zoom() * arrGlow_Scale[i] / 256));
		if (!bEditor) {
			sprite.setColor(sf::Color(arrColor[iCurrent_Chapter][1][0] + arrGlow_Brightness[i] * (arrColor[iCurrent_Chapter][2][0] - arrColor[iCurrent_Chapter][1][0]), arrColor[iCurrent_Chapter][1][1] + arrGlow_Brightness[i] * (arrColor[iCurrent_Chapter][2][1] - arrColor[iCurrent_Chapter][1][1]), arrColor[iCurrent_Chapter][1][2] + arrGlow_Brightness[i] * (arrColor[iCurrent_Chapter][2][2] - arrColor[iCurrent_Chapter][1][2])));
		}
		else {
			sprite.setColor(sf::Color(arrColor[iLevel][1][0] + arrGlow_Brightness[i] * (arrColor[iLevel][2][0] - arrColor[iLevel][1][0]), arrColor[iLevel][1][1] + arrGlow_Brightness[i] * (arrColor[iLevel][2][1] - arrColor[iLevel][1][1]), arrColor[iLevel][1][2] + arrGlow_Brightness[i] * (arrColor[iLevel][2][2] - arrColor[iLevel][1][2])));
		}
		this->window->draw(sprite);
	}
}

void Game::Draw_Particles() {
	for (int i = 0; i < iParticle_Total_at_Once; i++) {
		float fAge;
		float fColor;
		float fAlpha;
		if (arr_Particle_Active[i]) {
			fAge = (1 - (arr_Particle_Timer[i] / fParticle_Respawn_Rate));
			fColor = (1 - 0.2 * (arr_Particle_Timer[i] / fParticle_Respawn_Rate));
			fAlpha = (1 - 0.4 * (arr_Particle_Timer[i] / fParticle_Respawn_Rate));
			if (fAge < 0.45) {
				fColor = (1 - 1 * (arr_Particle_Timer[i] / fParticle_Respawn_Rate));
			}
			if (fColor < 0) {
				fColor = 0;
			}
			if (fAge < 0.45) {
				fAlpha = (1.5 - 1.75 * (arr_Particle_Timer[i] / fParticle_Respawn_Rate));
			}
			if (fAlpha < 0) {
				fAlpha = 0;
			}
			sf::CircleShape particle(0, arr_Particle_Points[i]);
			particle.setPosition(Get_Position_with_Zoom("x", arr_Particle_Position_X[i]), Get_Position_with_Zoom("y", arr_Particle_Position_Y[i]));
			particle.setRadius(arr_Particle_Size[i] * Get_Size_with_Zoom());
			particle.setRotation(arr_Particle_Rotation[i]);
			particle.setFillColor(sf::Color(255 * fColor, 255 * fColor, 255 * fColor, 255 * fAlpha));
			this->window->draw(particle);
		}
	}
}

void Game::Draw_Gameplay() {
	for (int column = 0; column < 20; column++) {
		for (int row = int(fCamera_Position_X / fGrid_Cell_Size) - 1; row < int((game_Width + fCamera_Position_X) / fGrid_Cell_Size) + 1; row++) {
			if (row >= 0 && row <= 79) {
				if (!bEditor) {
					if (map[iCurrent_Chapter][iCurrent_Level][column][row] != 0) {
						sf::Sprite shadow;
						shadow.setPosition(Get_Position_with_Zoom("x", row * fGrid_Cell_Size + 40 / 8), Get_Position_with_Zoom("y", column * fGrid_Cell_Size + 40 / 8));
						shadow.setScale((fGrid_Cell_Size * Get_Size_with_Zoom()) / 8, (fGrid_Cell_Size * Get_Size_with_Zoom()) / 8);
						if (row == 0) {
							shadow.setPosition(Get_Position_with_Zoom("x", (row - 1) * fGrid_Cell_Size + 40 / 8), Get_Position_with_Zoom("y", column * fGrid_Cell_Size + 40 / 8));
							shadow.setScale((fGrid_Cell_Size * Get_Size_with_Zoom() * 2) / 8, (fGrid_Cell_Size * Get_Size_with_Zoom()) / 8);
						}
						shadow.setColor(sf::Color(0, 0, 0, 127));
						sf::Sprite sprite;
						sprite.setPosition(Get_Position_with_Zoom("x", row * fGrid_Cell_Size), Get_Position_with_Zoom("y", column * fGrid_Cell_Size));
						sprite.setScale((fGrid_Cell_Size * Get_Size_with_Zoom()) / 8, (fGrid_Cell_Size * Get_Size_with_Zoom()) / 8);
						if (map[iCurrent_Chapter][iCurrent_Level][column][row] == 1) {
							int sprite_Number = Get_Sprite_Number("block", 1, column, row, true);
							sprite.setTexture(blocks_Block[iCurrent_Chapter][sprite_Number]);
							shadow.setTexture(blocks_Block[iCurrent_Chapter][sprite_Number]);
							this->window->draw(shadow);
							this->window->draw(sprite);
							if (mixedMap[column][row] == 1) {
								sprite.setTexture(blocks_Mixed[iCurrent_Chapter][sprite_Number]);
								this->window->draw(sprite);
							}
							if (sprite_Number >= 0 && sprite_Number <= 2) {
								sf::Sprite top;
								top.setPosition(Get_Position_with_Zoom("x", row * fGrid_Cell_Size), Get_Position_with_Zoom("y", (column - 1) * fGrid_Cell_Size));
								top.setScale((fGrid_Cell_Size * Get_Size_with_Zoom()) / 8, (fGrid_Cell_Size * Get_Size_with_Zoom()) / 8);
								top.setTexture(blocks_Top[iCurrent_Chapter][sprite_Number]);
								this->window->draw(top);
							}
						}
						if (map[iCurrent_Chapter][iCurrent_Level][column][row] == 2) {
							int sprite_Number = Get_Sprite_Number("block", 2, column, row, false);
							sprite.setTexture(blocks_Spike[iCurrent_Chapter][sprite_Number]);
							shadow.setTexture(blocks_Spike[iCurrent_Chapter][sprite_Number]);
							this->window->draw(shadow);
							this->window->draw(sprite);
						}
					}
				}
				else {
					if (editorMap[column][row] != 0) {
						sf::Sprite shadow;
						shadow.setPosition(Get_Position_with_Zoom("x", row * fGrid_Cell_Size + 40 / 8), Get_Position_with_Zoom("y", column * fGrid_Cell_Size + 40 / 8));
						shadow.setScale(Get_Size_with_Zoom() * fGrid_Cell_Size / 8, Get_Size_with_Zoom() * fGrid_Cell_Size / 8);
						if (row == 0) {
							shadow.setPosition(Get_Position_with_Zoom("x", (row - 1) * fGrid_Cell_Size + 40 / 8), Get_Position_with_Zoom("y", column * fGrid_Cell_Size + 40 / 8));
							shadow.setScale((fGrid_Cell_Size * Get_Size_with_Zoom() * 2) / 8, (fGrid_Cell_Size * Get_Size_with_Zoom()) / 8);
						}
						shadow.setColor(sf::Color(0, 0, 0, 127));
						sf::Sprite sprite;
						sprite.setPosition(Get_Position_with_Zoom("x", row * fGrid_Cell_Size), Get_Position_with_Zoom("y", column * fGrid_Cell_Size));
						sprite.setScale(Get_Size_with_Zoom() * fGrid_Cell_Size / 8, Get_Size_with_Zoom() * fGrid_Cell_Size / 8);
						if (editorMap[column][row] == 1) {
							int sprite_Number = Get_Sprite_Number("block", 1, column, row, true);
							sprite.setTexture(blocks_Block[iLevel][sprite_Number]);
							shadow.setTexture(blocks_Block[iLevel][sprite_Number]);
							this->window->draw(shadow);
							this->window->draw(sprite);
							if (mixedMap[column][row] == 1) {
								sprite.setTexture(blocks_Mixed[iLevel][sprite_Number]);
								this->window->draw(sprite);
							}
							if (sprite_Number >= 0 && sprite_Number <= 2) {
								sf::Sprite top;
								top.setPosition(Get_Position_with_Zoom("x", row * fGrid_Cell_Size), Get_Position_with_Zoom("y", (column - 1) * fGrid_Cell_Size));
								top.setScale(Get_Size_with_Zoom() * fGrid_Cell_Size / 8, Get_Size_with_Zoom() * fGrid_Cell_Size / 8);
								top.setTexture(blocks_Top[iLevel][sprite_Number]);
								this->window->draw(top);
							}
						}
						if (editorMap[column][row] == 2) {
							int sprite_Number = Get_Sprite_Number("block", 2, column, row, false);
							sprite.setTexture(blocks_Spike[iLevel][sprite_Number]);
							shadow.setTexture(blocks_Spike[iLevel][sprite_Number]);
							this->window->draw(shadow);
							this->window->draw(sprite);
						}
					}
				}
			}
		}
	}
	for (int column = 0; column < 20; column++) {
		for (int row = int(fCamera_Position_X / fGrid_Cell_Size); row < int((game_Width + fCamera_Position_X) / fGrid_Cell_Size) + 1; row++) {
			if (bEditor || bDeveloper) {
				if (fillMap[column][row] == 1) {
					sf::RectangleShape rect;
					rect.setPosition(Get_Position_with_Zoom("x", row * fGrid_Cell_Size), Get_Position_with_Zoom("y", (column)*fGrid_Cell_Size));
					rect.setSize(sf::Vector2f(fGrid_Cell_Size, fGrid_Cell_Size));
					rect.setFillColor(sf::Color(0, 0, 0, 127));
					this->window->draw(rect);
				}
			}
		}
	}
}

void Game::Draw_Decoration() {
	sf::RectangleShape rect;
	for (int i = 0; i < 64; i++) {
		float posX = arrDecoration_Position_X[i] + arrDecoration_Offset_X[i] + (arrDecoration_Size[i] / iDecoration_Max_Size) * fDecoration_Z_Sensitivity * fCamera_Position_X;
		float posY = arrDecoration_Position_Y[i] + arrDecoration_Offset_Y[i];
		/*if (arrDecoration_Position_X[i] + arrDecoration_Offset_X[i] > fCamera_Position_X - fGrid_Cell_Size && arrDecoration_Position_X[i] + arrDecoration_Offset_X[i] < fCamera_Position_X + fGrid_Cell_Size + game_Width) {*/
		rect.setOrigin(arrDecoration_Size[i] / 4, arrDecoration_Size[i] / 4);
		rect.setRotation(arrDecoration_Rotation[i]);
		rect.setSize(sf::Vector2f(Get_Size_with_Zoom() * arrDecoration_Size[i] / 2, Get_Size_with_Zoom() * arrDecoration_Size[i] / 2));
		rect.setPosition(Get_Position_with_Zoom("x", posX + 2), Get_Position_with_Zoom("y", posY + 2));
		if (!bEditor) {
			rect.setFillColor(sf::Color(0, 0, 0, ((arrDecoration_Color[iCurrent_Chapter][0][3] + 1) / 4 - 1) + arrDecoration_Color_Value[i] * (((arrDecoration_Color[iCurrent_Chapter][1][3] + 1) / 4 - 1) - ((arrDecoration_Color[iCurrent_Chapter][0][3] + 1) / 4 - 1))));
		}
		else {
			rect.setFillColor(sf::Color(0, 0, 0, ((arrDecoration_Color[iLevel][0][3] + 1) / 4 - 1) + arrDecoration_Color_Value[i] * (((arrDecoration_Color[iLevel][1][3] + 1) / 4 - 1) - ((arrDecoration_Color[iLevel][0][3] + 1) / 4 - 1))));
		}
		this->window->draw(rect);
		if (!bEditor) {
			rect.setFillColor(sf::Color(arrDecoration_Color[iCurrent_Chapter][0][0] + arrDecoration_Color_Value[i] * (arrDecoration_Color[iCurrent_Chapter][1][0] - arrDecoration_Color[iCurrent_Chapter][0][0]), arrDecoration_Color[iCurrent_Chapter][0][1] + arrDecoration_Color_Value[i] * (arrDecoration_Color[iCurrent_Chapter][1][1] - arrDecoration_Color[iCurrent_Chapter][0][1]), arrDecoration_Color[iCurrent_Chapter][0][2] + arrDecoration_Color_Value[i] * (arrDecoration_Color[iCurrent_Chapter][1][2] - arrDecoration_Color[iCurrent_Chapter][0][2]), arrDecoration_Color[iCurrent_Chapter][0][3] + arrDecoration_Color_Value[i] * (arrDecoration_Color[iCurrent_Chapter][1][3] - arrDecoration_Color[iCurrent_Chapter][0][3])));
		}
		else {
			rect.setFillColor(sf::Color(arrDecoration_Color[iLevel][0][0] + arrDecoration_Color_Value[i] * (arrDecoration_Color[iLevel][1][0] - arrDecoration_Color[iLevel][0][0]), arrDecoration_Color[iLevel][0][1] + arrDecoration_Color_Value[i] * (arrDecoration_Color[iLevel][1][1] - arrDecoration_Color[iLevel][0][1]), arrDecoration_Color[iLevel][0][2] + arrDecoration_Color_Value[i] * (arrDecoration_Color[iLevel][1][2] - arrDecoration_Color[iLevel][0][2]), arrDecoration_Color[iLevel][0][3] + arrDecoration_Color_Value[i] * (arrDecoration_Color[iLevel][1][3] - arrDecoration_Color[iLevel][0][3])));
		}
		rect.setPosition(Get_Position_with_Zoom("x", posX), Get_Position_with_Zoom("y", posY));
		this->window->draw(rect);
		/*}*/
	}
}

void Game::Draw_Overlay() {
	sf::RectangleShape rect;
	rect.setPosition(0, 0);
	rect.setSize(sf::Vector2f(game_Width, game_Height));
	rect.setFillColor(sf::Color(0, 0, 0, fSimulation_Speed_Overlay_Visibility));
	this->window->draw(rect);
}

void Game::Draw_Editor_Sidebar() {
	sf::RectangleShape rect;
	rect.setFillColor(sf::Color(31, 31, 31, 255));
	rect.setPosition(game_Width - iEditor_Sidebar_Width, 0);
	rect.setSize(sf::Vector2f(iEditor_Sidebar_Width, game_Height));
	this->window->draw(rect);
	Draw_Editor_Items();
}

void Game::Draw_Editor_Items(){
	sf::Sprite sprt;
	sf::Sprite shadow;
	for (int i = 0; i < 2; i++) {
		if (iBlock - 1 != i) {
			shadow.setColor(sf::Color(0, 0, 0, 127));
		}
		else {
			shadow.setColor(sf::Color(0, 0, 0, 0));
		}
		if (mouseX > 500 - iEditor_Sidebar_Width + float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2 && mouseX < 500 - float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2 && mouseY > float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2 + i * iEditor_Sidebar_Width && mouseY < float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2 + i * iEditor_Sidebar_Width + float(iEditor_Sidebar_Width) * fEditor_Items_Width) {
			shadow.setColor(sf::Color(0, 0, 0, 95));
			if (bButton_Held_Mouse1) {
				shadow.setColor(sf::Color(0, 0, 0, 127));
			}
			else if (bButton_Released_Mouse1) {
				iBlock = i + 1;
			}
		}
		sprt.setScale((float(iEditor_Sidebar_Width)* fEditor_Items_Width) / (8 * 3), (float(iEditor_Sidebar_Width)* fEditor_Items_Width) / (8 * 3));
		shadow.setScale((float(iEditor_Sidebar_Width) * fEditor_Items_Width) / (8 * 3), (float(iEditor_Sidebar_Width) * fEditor_Items_Width) / (8 * 3));
		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 3; x++) {
				sprt.setPosition(500 - iEditor_Sidebar_Width + float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2 + ((float(iEditor_Sidebar_Width) * fEditor_Items_Width) / 3) * x, float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2 + i * iEditor_Sidebar_Width + ((float(iEditor_Sidebar_Width) * fEditor_Items_Width) / 3) * y);
				shadow.setPosition(500 - iEditor_Sidebar_Width + float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2 + ((float(iEditor_Sidebar_Width) * fEditor_Items_Width) / 3) * x, float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2 + i * iEditor_Sidebar_Width + ((float(iEditor_Sidebar_Width) * fEditor_Items_Width) / 3) * y);
				if (i == 0) {
					if (bEditor) {
						sprt.setTexture(blocks_Block[iLevel][y * 3 + x]);
						shadow.setTexture(blocks_Block[iLevel][y * 3 + x]);
					}
					else {
						sprt.setTexture(blocks_Block[iCurrent_Chapter][y * 3 + x]);
						shadow.setTexture(blocks_Block[iCurrent_Chapter][y * 3 + x]);
					}
				}
				else {
					if (bEditor) {
						sprt.setTexture(blocks_Spike[iLevel][y * 3 + x]);
						shadow.setTexture(blocks_Spike[iLevel][y * 3 + x]);
					}
					else {
						sprt.setTexture(blocks_Spike[iCurrent_Chapter][y * 3 + x]);
						shadow.setTexture(blocks_Spike[iCurrent_Chapter][y * 3 + x]);
					}
				}
				this->window->draw(sprt);
				this->window->draw(shadow);
			}
		}
	}
}

void Game::Save() {
	Save_Editor();
	std::ofstream createFile("saves/editor_levels.txt");
	createFile << iEditor_Levels << " " << iCurrent_Editor_Level;
	createFile.close();
	Save_Sidebar_Width();
	if (bDeveloper) {
		Save_Main_Levels();
	}
}

void Game::Save_Editor() {
	std::ofstream createFile("saves/editor" + std::to_string(iCurrent_Editor_Level) + ".txt");
	for (int y = 0; y < 20; y++) {
		//std::cout << "{";
		for (int x = 0; x < 80; x++) {
			createFile << editorMap[y][x] << " ";
			//std::cout << editorMap[y][x];
			//if (x != 79) {
				//std::cout << ",";
			//}
		}
		createFile << "\n";
		//std::cout << "}";
		//if (y != 19) {
			//std::cout << ",";
		//}
		//std::cout << "\n";
	}
	createFile << iLevel;
	createFile.close();
}

void Game::Save_Sidebar_Width() {
	std::ofstream createFile("saves/sidebar_width.txt");
	createFile << iEditor_Sidebar_Width;
	createFile.close();
}

void Game::Save_Main_Levels() {
	for (int c = 0; c < 5; c++) {
		for (int l = 0; l < 5; l++) {
			std::ofstream createFile("saves/levels/" + std::to_string(c) + "/" + std::to_string(l) + ".txt");
			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 80; x++) {
					createFile << map[c][l][y][x] << " ";
				}
				createFile << "\n";
			}
			createFile.close();
		}
	}
}

float Game::Get_Size_with_Zoom() {
	return (1 + (1 - fSpeed_of_Simulation) * fZoom_based_on_Simulation_Speed_Sensitivity) / (500.f / game_Height);
};

float Game::Get_Origin_with_Zoom(const char* The_Axis_Id) {
	if (The_Axis_Id == "x") {
		return ((1 + (1 - fSpeed_of_Simulation) * fZoom_based_on_Simulation_Speed_Sensitivity) * (iRocket_Width + iRocket_Width * fVelocity_Forward * fRocket_Width_based_on_Velocity_Sensitivity)) / 2;
	}
	if (The_Axis_Id == "y") {
		return ((1 + (1 - fSpeed_of_Simulation) * fZoom_based_on_Simulation_Speed_Sensitivity) * (iRocket_Height - fRocket_Height_based_on_Velocity_Sensitivity * ((iRocket_Height * fVelocity_Forward * fRocket_Width_based_on_Velocity_Sensitivity) / iRocket_Height))) / 2;
	}
}

float Game::Get_Position_with_Zoom(const char* The_Axis_Id, float The_Axis_Pos) {
	if (The_Axis_Id == "x") {
		return (The_Axis_Pos - fCamera_Position_X + (The_Axis_Pos - fCamera_Position_X - game_Width / 2) * (1 - fSpeed_of_Simulation) * fZoom_based_on_Simulation_Speed_Sensitivity) / (500.f / game_Width);
	}
	if (The_Axis_Id == "y") {
		return (The_Axis_Pos - fCamera_Position_Y + (The_Axis_Pos - fCamera_Position_Y - game_Height / 2) * (1 - fSpeed_of_Simulation) * fZoom_based_on_Simulation_Speed_Sensitivity) / (500.f / game_Height);
	}
};

int Game::Get_Sprite_Number(const char* The_Sprite, int The_Block_Number, int The_Column, int The_Row, bool Is_Grouped) {
	int nw, n, ne, w, e, sw, s, se;
	if (!bEditor) {
		nw	= map[iCurrent_Chapter][iCurrent_Level][The_Column - 1][The_Row - 1];
		n	= map[iCurrent_Chapter][iCurrent_Level][The_Column - 1][The_Row    ];
		ne	= map[iCurrent_Chapter][iCurrent_Level][The_Column - 1][The_Row + 1];
		w	= map[iCurrent_Chapter][iCurrent_Level][The_Column    ][The_Row - 1];
		e	= map[iCurrent_Chapter][iCurrent_Level][The_Column	  ][The_Row + 1];
		sw	= map[iCurrent_Chapter][iCurrent_Level][The_Column + 1][The_Row - 1];
		s	= map[iCurrent_Chapter][iCurrent_Level][The_Column + 1][The_Row    ];
		se	= map[iCurrent_Chapter][iCurrent_Level][The_Column + 1][The_Row + 1];
		if (The_Sprite == "block") {
			if (Is_Grouped) {
				if (n != The_Block_Number && s == The_Block_Number && The_Column != 0 || n != The_Block_Number && The_Column == 19) {
					if (w != The_Block_Number && e == The_Block_Number && The_Row != 0 || w != The_Block_Number && The_Row == 79 && The_Row != 0) {
						return 0;
					}
					else if (w == The_Block_Number && e == The_Block_Number || w == The_Block_Number && The_Row == 79 || The_Row == 0 && e == The_Block_Number) {
						return 1;
					}
					else if (w == The_Block_Number && e != The_Block_Number && The_Row != 79 || The_Row == 0 && e != The_Block_Number && The_Row != 79) {
						return 2;
					}
				}
				else if (n == The_Block_Number && s == The_Block_Number || n == The_Block_Number && The_Column == 19 || The_Column == 0 && s == The_Block_Number) {
					if (w != The_Block_Number && e == The_Block_Number && The_Row != 0 || w != The_Block_Number && The_Row == 79 && The_Row != 0) {
						return 3;
					}
					else if (w == The_Block_Number && e == The_Block_Number || w == The_Block_Number && The_Row == 79 || The_Row == 0 && e == The_Block_Number) {
						if (nw != The_Block_Number && The_Column != 0 && The_Row != 0) {
							return 9;
						}
						else if (ne != The_Block_Number && The_Column != 0 && The_Row != 79) {
							return 10;
						}
						else if (se != The_Block_Number && The_Column != 19 && The_Row != 79) {
							return 11;
						}
						else if (sw != The_Block_Number && The_Column != 19 && The_Row != 0) {
							return 12;
						}
						return 4;
					}
					else if (w == The_Block_Number && e != The_Block_Number && The_Row != 79 || The_Row == 0 && e != The_Block_Number && The_Row != 79) {
						return 5;
					}
				}
				else if (n == The_Block_Number && s != The_Block_Number && The_Column != 19 || The_Column == 0 && s != The_Block_Number) {
					if (w != The_Block_Number && e == The_Block_Number && The_Row != 0 || w != The_Block_Number && The_Row == 79 && The_Row != 0) {
						return 6;
					}
					else if (w == The_Block_Number && e == The_Block_Number || w == The_Block_Number && The_Row == 79 || The_Row == 0 && e == The_Block_Number) {
						return 7;
					}
					else if (w == The_Block_Number && e != The_Block_Number && The_Row != 79 || The_Row == 0 && e != The_Block_Number && The_Row != 79) {
						return 8;
					}
				}
			}
			else {
				if (map[iCurrent_Chapter][iCurrent_Level][The_Column - 1][The_Row] == 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column + 1][The_Row] != 0 && The_Column != 0 || map[iCurrent_Chapter][iCurrent_Level][The_Column - 1][The_Row] == 0 && The_Column == 19) {
					if (map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] == 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] != 0 && The_Row != 0 || map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] == 0 && The_Row == 79 && The_Row != 0) {
						return 0;
					}
					else if (map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] != 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] != 0 || map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] != 0 && The_Row == 79 || The_Row == 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] != 0) {
						return 1;
					}
					else if (map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] != 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] == 0 && The_Row != 79 || The_Row == 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] == 0 && The_Row != 79) {
						return 2;
					}
				}
				else if (map[iCurrent_Chapter][iCurrent_Level][The_Column - 1][The_Row] != 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column + 1][The_Row] != 0 || map[iCurrent_Chapter][iCurrent_Level][The_Column - 1][The_Row] != 0 && The_Column == 19 || The_Column == 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column + 1][The_Row] != The_Block_Number) {
					if (map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] == 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] != 0 && The_Row != 0 || map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] == 0 && The_Row == 79 && The_Row != 0) {
						return 3;
					}
					else if (map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] != 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] != 0 || map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] != 0 && The_Row == 79 || The_Row == 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] != The_Block_Number) {
						if (map[iCurrent_Chapter][iCurrent_Level][The_Column - 1][The_Row - 1] == 0 && The_Column != 0 && The_Row != 0) {
							return 9;
						}
						else if (map[iCurrent_Chapter][iCurrent_Level][The_Column - 1][The_Row + 1] == 0 && The_Column != 0 && The_Row != 79) {
							return 10;
						}
						else if (map[iCurrent_Chapter][iCurrent_Level][The_Column + 1][The_Row + 1] == 0 && The_Column != 19 && The_Row != 79) {
							return 11;
						}
						else if (map[iCurrent_Chapter][iCurrent_Level][The_Column + 1][The_Row - 1] == 0 && The_Column != 19 && The_Row != 0) {
							return 12;
						}
						return 4;
					}
					else if (map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] != 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] == 0 && The_Row != 79 || The_Row == 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] == 0 && The_Row != 79) {
						return 5;
					}
				}
				else if (map[iCurrent_Chapter][iCurrent_Level][The_Column - 1][The_Row] != 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column + 1][The_Row] == 0 && The_Column != 19 || The_Column == 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column + 1][The_Row] == 0) {
					if (map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] == 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] != 0 && The_Row != 0 || map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] == 0 && The_Row == 79 && The_Row != 0) {
						return 6;
					}
					else if (map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] != 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] != 0 || map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] != 0 && The_Row == 79 || The_Row == 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] != The_Block_Number) {
						return 7;
					}
					else if (map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row - 1] != 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] == 0 && The_Row != 79 || The_Row == 0 && map[iCurrent_Chapter][iCurrent_Level][The_Column][The_Row + 1] == 0 && The_Row != 79) {
						return 8;
					}
				}
			}
		}
		return 4;
	}
	else {
		if (The_Sprite == "block") {
			if (Is_Grouped) {
				if (editorMap[The_Column - 1][The_Row] != The_Block_Number && editorMap[The_Column + 1][The_Row] == The_Block_Number && The_Column != 0 || editorMap[The_Column - 1][The_Row] != The_Block_Number && The_Column == 19) {
					if (editorMap[The_Column][The_Row - 1] != The_Block_Number && editorMap[The_Column][The_Row + 1] == The_Block_Number && The_Row != 0 || editorMap[The_Column][The_Row - 1] != The_Block_Number && The_Row == 79 && The_Row != 0) {
						return 0;
					}
					else if (editorMap[The_Column][The_Row - 1] == The_Block_Number && editorMap[The_Column][The_Row + 1] == The_Block_Number || editorMap[The_Column][The_Row - 1] == The_Block_Number && The_Row == 79 || The_Row == 0 && editorMap[The_Column][The_Row + 1] == The_Block_Number) {
						return 1;
					}
					else if (editorMap[The_Column][The_Row - 1] == The_Block_Number && editorMap[The_Column][The_Row + 1] != The_Block_Number && The_Row != 79 || The_Row == 0 && editorMap[The_Column][The_Row + 1] != The_Block_Number && The_Row != 79) {
						return 2;
					}
				}
				else if (editorMap[The_Column - 1][The_Row] == The_Block_Number && editorMap[The_Column + 1][The_Row] == The_Block_Number || editorMap[The_Column - 1][The_Row] == The_Block_Number && The_Column == 19 || The_Column == 0 && editorMap[The_Column + 1][The_Row] == The_Block_Number) {
					if (editorMap[The_Column][The_Row - 1] != The_Block_Number && editorMap[The_Column][The_Row + 1] == The_Block_Number && The_Row != 0 || editorMap[The_Column][The_Row - 1] != The_Block_Number && The_Row == 79 && The_Row != 0) {
						return 3;
					}
					else if (editorMap[The_Column][The_Row - 1] == The_Block_Number && editorMap[The_Column][The_Row + 1] == The_Block_Number || editorMap[The_Column][The_Row - 1] == The_Block_Number && The_Row == 79 || The_Row == 0 && editorMap[The_Column][The_Row + 1] == The_Block_Number) {
						if (editorMap[The_Column - 1][The_Row - 1] != The_Block_Number && The_Column != 0 && The_Row != 0) {
							return 9;
						}
						else if (editorMap[The_Column - 1][The_Row + 1] != The_Block_Number && The_Column != 0 && The_Row != 79) {
							return 10;
						}
						else if (editorMap[The_Column + 1][The_Row + 1] != The_Block_Number && The_Column != 19 && The_Row != 79) {
							return 11;
						}
						else if (editorMap[The_Column + 1][The_Row - 1] != The_Block_Number && The_Column != 19 && The_Row != 0) {
							return 12;
						}
						return 4;
					}
					else if (editorMap[The_Column][The_Row - 1] == The_Block_Number && editorMap[The_Column][The_Row + 1] != The_Block_Number && The_Row != 79 || The_Row == 0 && editorMap[The_Column][The_Row + 1] != The_Block_Number && The_Row != 79) {
						return 5;
					}
				}
				else if (editorMap[The_Column - 1][The_Row] == The_Block_Number && editorMap[The_Column + 1][The_Row] != The_Block_Number && The_Column != 19 || The_Column == 0 && editorMap[The_Column + 1][The_Row] != The_Block_Number) {
					if (editorMap[The_Column][The_Row - 1] != The_Block_Number && editorMap[The_Column][The_Row + 1] == The_Block_Number && The_Row != 0 || editorMap[The_Column][The_Row - 1] != The_Block_Number && The_Row == 79 && The_Row != 0) {
						return 6;
					}
					else if (editorMap[The_Column][The_Row - 1] == The_Block_Number && editorMap[The_Column][The_Row + 1] == The_Block_Number || editorMap[The_Column][The_Row - 1] == The_Block_Number && The_Row == 79 || The_Row == 0 && editorMap[The_Column][The_Row + 1] == The_Block_Number) {
						return 7;
					}
					else if (editorMap[The_Column][The_Row - 1] == The_Block_Number && editorMap[The_Column][The_Row + 1] != The_Block_Number && The_Row != 79 || The_Row == 0 && editorMap[The_Column][The_Row + 1] != The_Block_Number && The_Row != 79) {
						return 8;
					}
				}
			}
			else {
				if (editorMap[The_Column - 1][The_Row] == 0 && editorMap[The_Column + 1][The_Row] != 0 && The_Column != 0 || editorMap[The_Column - 1][The_Row] == 0 && The_Column == 19) {
					if (editorMap[The_Column][The_Row - 1] == 0 && editorMap[The_Column][The_Row + 1] != 0 && The_Row != 0 || editorMap[The_Column][The_Row - 1] == 0 && The_Row == 79 && The_Row != 0) {
						return 0;
					}
					else if (editorMap[The_Column][The_Row - 1] != 0 && editorMap[The_Column][The_Row + 1] != 0 || editorMap[The_Column][The_Row - 1] != 0 && The_Row == 79 || The_Row == 0 && editorMap[The_Column][The_Row + 1] != 0) {
						return 1;
					}
					else if (editorMap[The_Column][The_Row - 1] != 0 && editorMap[The_Column][The_Row + 1] == 0 && The_Row != 79 || The_Row == 0 && editorMap[The_Column][The_Row + 1] == 0 && The_Row != 79) {
						return 2;
					}
				}
				else if (editorMap[The_Column - 1][The_Row] != 0 && editorMap[The_Column + 1][The_Row] != 0 || editorMap[The_Column - 1][The_Row] != 0 && The_Column == 19 || The_Column == 0 && editorMap[The_Column + 1][The_Row] != The_Block_Number) {
					if (editorMap[The_Column][The_Row - 1] == 0 && editorMap[The_Column][The_Row + 1] != 0 && The_Row != 0 || editorMap[The_Column][The_Row - 1] == 0 && The_Row == 79 && The_Row != 0) {
						return 3;
					}
					else if (editorMap[The_Column][The_Row - 1] != 0 && editorMap[The_Column][The_Row + 1] != 0 || editorMap[The_Column][The_Row - 1] != 0 && The_Row == 79 || The_Row == 0 && editorMap[The_Column][The_Row + 1] != The_Block_Number) {
						if (editorMap[The_Column - 1][The_Row - 1] == 0 && The_Column != 0 && The_Row != 0) {
							return 9;
						}
						else if (editorMap[The_Column - 1][The_Row + 1] == 0 && The_Column != 0 && The_Row != 79) {
							return 10;
						}
						else if (editorMap[The_Column + 1][The_Row + 1] == 0 && The_Column != 19 && The_Row != 79) {
							return 11;
						}
						else if (editorMap[The_Column + 1][The_Row - 1] == 0 && The_Column != 19 && The_Row != 0) {
							return 12;
						}
						return 4;
					}
					else if (editorMap[The_Column][The_Row - 1] != 0 && editorMap[The_Column][The_Row + 1] == 0 && The_Row != 79 || The_Row == 0 && editorMap[The_Column][The_Row + 1] == 0 && The_Row != 79) {
						return 5;
					}
				}
				else if (editorMap[The_Column - 1][The_Row] != 0 && editorMap[The_Column + 1][The_Row] == 0 && The_Column != 19 || The_Column == 0 && editorMap[The_Column + 1][The_Row] == 0) {
					if (editorMap[The_Column][The_Row - 1] == 0 && editorMap[The_Column][The_Row + 1] != 0 && The_Row != 0 || editorMap[The_Column][The_Row - 1] == 0 && The_Row == 79 && The_Row != 0) {
						return 6;
					}
					else if (editorMap[The_Column][The_Row - 1] != 0 && editorMap[The_Column][The_Row + 1] != 0 || editorMap[The_Column][The_Row - 1] != 0 && The_Row == 79 || The_Row == 0 && editorMap[The_Column][The_Row + 1] != The_Block_Number) {
						return 7;
					}
					else if (editorMap[The_Column][The_Row - 1] != 0 && editorMap[The_Column][The_Row + 1] == 0 && The_Row != 79 || The_Row == 0 && editorMap[The_Column][The_Row + 1] == 0 && The_Row != 79) {
						return 8;
					}
				}
			}
		}
		return 4;
	}
}
