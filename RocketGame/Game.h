#pragma once

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

class Game {

private:


	sf::RenderWindow* window;
	sf::VideoMode		videoMode;
	sf::Event			event;

	sf::Sprite			player;

	void	Initialize_Variables();
	void	Initialize_Window();
	void	Initialize_Glow();
	void	Initialize_Decoration();
	void	Initialize_Mixed();

public:

	float	fPosition_X = 0;
	float	fPosition_Y = 0;
	int		iRocket_Width = 25;
	int		iRocket_Height = 25;

	float	fRocket_Rotation = 0;
	float	fVelocity_X = 0;
	float	fVelocity_Y = 0;
	float	fVelocity_Forward = 0;
	float	fRocket_Gravity = 11;
	float	fRocket_Width_based_on_Velocity_Sensitivity = 0.1;
	float	fRocket_Height_based_on_Velocity_Sensitivity = 10;
	float	fRocket_Gravity_Multiplier_when_Down_is_Held = 2;
	float	fRocket_Max_Fall_Speed_Multiplier_when_Down_is_Held = 2;
	float	fRocket_Max_Fall_Speed = 5;

	float	fTime_before_Rocket_X_Velocity_is_Done_Changing = 0.25;
	float	fRocket_Max_Velocity_X = 5;

	float	fCamera_Position_X = 0;
	float	fCamera_Position_Y = 0;
	float	fCamera_Offset_from_Screen_Edge_X = 200;
	float	fCamera_Offset_From_Screen_Edge_Y = 175;
	float	fCamera_Follow_Speed = 0.2;
 
	float	fSpeed_of_Simulation = 1;
	float	fMin_Simulation_Speed = 0.5;
	float	fTime_before_simulation_Speed_is_done_Changing = 1.4;
	float	fZoom_based_on_Simulation_Speed_Sensitivity = 0.3;

	float	fParticle_Angle_Facing = 0;
	float	fParticle_Angle_Width = 90;

	float	fParticle_Position_X = 200;
	float	fParticle_Position_Y = 200;

	float	fParticle_Timer = 0;

	float	fParticle_Start_Size_Max = 100;
	float	fParticle_Start_Size_Min = 50;
	float	fParticle_Start_Velocity_Max = 4;
	float	fParticle_Start_Velocity_Min = 3;
	float	fParticle_Start_Rotation_Max = 360;
	float	fParticle_Start_Rotation_Min = -360;
	int		iParticle_Max_Points = 5;
	int		iParticle_Min_Points = 3;

	const int iParticle_Total_at_Once = 20;
	float	fParticle_Respawn_Rate = 0.3;

	float	fParticle_Gravity = 4;

	float	arr_Particle_Points[20];
	float	arr_Particle_Velocity_X[20];
	float	arr_Particle_Velocity_Y[20];
	float	arr_Particle_Position_X[20];
	float	arr_Particle_Position_Y[20];
	float	arr_Particle_Start_Size[20];
	float	arr_Particle_Size[20];
	float	arr_Particle_Rotation_Speed[20];
	float	arr_Particle_Rotation[20];
	float	arr_Particle_Timer[20];
	bool	arr_Particle_Active[20];

	int		iParticles_Init_Num = 0;

	bool	bButton_Held_Up = false;
	bool	bButton_Held_Left = false;
	bool	bButton_Held_Right = false;
	bool	bButton_Held_Down = false;

	bool	bButton_Pressed_Up = false;
	bool	bButton_Pressed_Left = false;
	bool	bButton_Pressed_Right = false;
	bool	bButton_Pressed_Down = false;

	bool	bButton_Held_C = false;

	bool	bButton_Held_ESC = false;
	bool	bButton_Pressed_ESC = false;

	bool	bButton_Held_Enter = false;
	bool	bButton_Pressed_Enter = false;

	bool	bButton_Held_Shift = false;
	bool	bButton_Pressed_Shift = false;
	bool	bButton_Released_Shift = false;

	bool	bButton_Held_Mouse1 = false;
	bool	bButton_Pressed_Mouse1 = false;
	bool	bButton_Released_Mouse1 = false;
	bool	bButton_Clicked_Mouse1 = false;
	bool	bButton_Held_Mouse2 = false;
	bool	bButton_Pressed_Mouse2 = false;
	bool	bButton_Released_Mouse2 = false;
	bool	bButton_Clicked_Mouse2 = false;

	float	fGrid_Cell_Size = 25;

	int		iCurrent_Level = -1;
	int		iCurrent_Chapter = -1;

	int		iSelected_Difficulty = 0;
	int		selected_Option = 1;

	bool	bExit_Warning = false;

	bool	bPlaying = false;
	bool	bStart_Menu = true;

	float	mouseX = 0;
	float	mouseY = 0;

	int		iGlow_Total = 40;

	int		arrGlow_Position_X[40];
	int		arrGlow_Position_Y[40];
	int 	arrGlow_Scale[40];
	float	arrGlow_Brightness[40];
	float	arrGlow_Offset_X[40];
	float	arrGlow_Offset_Y[40];
	float	arrGlow_Velocity_X[40];
	float	arrGlow_Velocity_Y[40];

	bool	arrGlow_Velocity_Increase_X[40];
	bool	arrGlow_Velocity_Increase_Y[40];

	float	fGlow_Timer = 0;
	float	fGlow_Time = 0.5;

	float	fGlow_Velocity_Speed = 0.1;

	float	fGlow_Offset_based_on_Size_Multiplier = 0.1;

	bool	bStarted = false;

	bool	bFlying = false;
	bool	bWas_Flying = false;

	float	fRespawning_Last_Timer = 0.f;
	float	fRespawning_Timer = 0.5f;
	float	fRespawning_Time = 1.f;

	int		iRespawning_Max_Offset = 50;
	int		iRespawning_Max_Acceleration = 150;
	int		iRespawning_Min_Acceleration = 75;
	int		iRespawning_Max_Points = 6;
	int		iRespawning_Min_Points = 4;

	float	arrRespawning_Offset_X[5][5];
	float	arrRespawning_Offset_Y[5][5];
	float	arrRespawning_Acceleration[5][5];
	float	arrRespawning_Rotation[5][5];
	int		arrRespawning_Points[5][5];
	bool	arrRespawning_Rotating_Left[5][5];

	bool	bRespawning_Growing;
	bool	bRespawning_Shrinking;
	float	fRespawning_Size;

	int		iRespawning_Animation = 0;

	int		iDecoration_Max_Size = 10;
	int		iDecoration_Min_Size = 5;
	int		iDecoration_Max_Offset = 50;
	int		iDecoration_Max_Main_Velocity = 100;
	int		iDecoration_Min_Main_Velocity = 75;
	float	fDecoration_Randomness_Timer = 0;
	float	fDecoration_Randomness_Time = 0.5;
	float	fDecoration_Z_Sensitivity = 0;

	int		arrDecoration_Sprite[64];
	float	arrDecoration_Size[64];
	float	arrDecoration_Rotation[64];
	float	arrDecoration_Offset_X[64];
	float	arrDecoration_Offset_Y[64];
	float	arrDecoration_Position_X[64];
	float	arrDecoration_Position_Y[64];
	float	arrDecoration_Velocity_X[64];
	float	arrDecoration_Velocity_Y[64];
	float	arrDecoration_Main_Velocity[64];
	float	arrDecoration_Acceleration_X[64];
	float	arrDecoration_Acceleration_Y[64];
	float	arrDecoration_Color_Value[64];
	float	arrDecoration_Color_Velocity[64];
	float	arrDecoration_Color_Acceleration[64];

	int	arrDecoration_Color[5][2][4] = { {
		{255, 255, 255, 255}, // STYLE 0
		{149, 252, 252, 255},
		} , {
		{225, 255, 47 , 255}, // STYLE 1
		{165, 191, 30 , 0  },
		} , {
		{252, 234, 182, 255}, // STYLE 2
		{247, 212, 129, 255},
		} , {
		{200, 75 , 0  , 255}, // STYLE 3
		{255, 222, 63 , 0  },
		} , {
		{255, 255, 31 , 255}, // STYLE 4
		{191, 0  , 0  , 255},
	} };

	float	arrow_Distance = 0;
	float	arrow_Velocity = 0;
	float	arrow_Start_Velocity = 5;
	float	arrow_Acceleration = 0.75; // TIME IN SECONDS

	int		died_Next_Level = -1;
	int		died_Next_Chapter = -1;

	float	dragX;
	float	dragY;

	bool	bEditor;

	int		iEditor_Levels = 0;

	int		iCurrent_Editor_Level = 0;

	int		iEditor_Sidebar_Width = 75;
	int		iEditor_Sidebar_Min_Width = 40;
	int		iEditor_Sidebar_Max_Width = 150;
	float	fEditor_Items_Width = 0.8;

	bool	bResizing_On_Edge = false;

	int	arrColor[5][3][3] = { {
		{187, 149, 245},
		{47 , 164, 255}, // STYLE 0
		{184, 228, 240},
		} , {
		{47 , 127, 255},
		{63 , 207, 255}, // STYLE 1
		{158, 143, 255},
		} , {
		{90 , 205, 237},
		{255, 244, 184}, // STYLE 2
		{167, 240, 250},
		} , {
		{23 , 16 , 16 },
		{66 , 63 , 62 }, // STYLE 3
		{43 , 43 , 43 },
		} , {
		{35 , 4  , 0  },
		{107, 16 , 0  }, // STYLE 4
		{65 , 0  , 0  },
	} };

	bool	right_Side;
	bool	no_Selected;

	int		iLevel;

	int		iBlock = 1;

	bool	bEditing;

	bool	window_Is_Active = true;

	int		iLast_Level_Sound;

	bool	bEditor_Resizing = false;

	bool	bDeveloper = false;
	bool	bDeveloper_Playtesting = false;

	int		mixedMap[20][80];

	int		fillMap[20][80];

	int		map[5][5][20][80];

	int		editorMap[20][80];

	float	fSimulation_Speed_Overlay_Max_Visibility = 95;
	float	fSimulation_Speed_Overlay_Visibility = 0;

	bool	bUsed_Editor = false;

	const bool running() const;

	Game();
	virtual ~Game();

	void pollEvents();

	void Update();
	void Update_Simulation_Speed();
	void Update_Segment();
	void Update_Hard_Fall();
	void Update_Vertical_Movement();
	void Update_Horizontal_Movement();
	void Update_Camera_Position();
	void Update_Collision();
	void Update_Glow();
	void Update_Editor();
	void Update_Editor_Sidebar();
	void Update_Particles();
	void Update_Dead();
	void Update_Decoration();
	void Respawn();

	void Draw();
	void Draw_Menu();
	void Draw_Start_Menu();
	void Draw_Game();
	void Draw_Respawn();
	void Draw_Glow();
	void Draw_Particles();
	void Draw_Gameplay();
	void Draw_Decoration();
	void Draw_Overlay();
	void Draw_Editor_Sidebar();
	void Draw_Editor_Items();

	void Save();
	void Save_Editor();
	void Save_Sidebar_Width();
	void Save_Main_Levels();

	float	Get_Size_with_Zoom();
	float	Get_Origin_with_Zoom(const char* The_Axis_Id);
	float	Get_Position_with_Zoom(const char* The_Axis_Id, float The_Axis_Pos);
	int		Get_Sprite_Number(const char* The_Sprite, int The_Block_Number, int The_Column, int The_Row, bool Is_Grouped);
};