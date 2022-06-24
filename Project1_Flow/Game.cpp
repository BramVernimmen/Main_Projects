#include "pch.h"
#include "Game.h"
#include <iostream>

//Basic game functions
#pragma region gameFunctions											
void Start()
{
	int NrGridCells{ g_NrRows * g_NrColumns };
	g_LevelNumbers = new Texture[NrGridCells];
	g_IsLevelCompleted = new bool[NrGridCells];
	for (int index{}; index < NrGridCells; ++index)
	{
		g_IsLevelCompleted[index] = false;
	}


	g_GridColorDynArr = new GridCellColor[NrGridCells]{};
	g_GridStateDynArr = new GridCellState[NrGridCells]{};
	g_GridValueDynArr = new int[NrGridCells] {};
	g_MaxValueDynArr = new int[g_AmountOfColors];
	g_IsColorConnectedDynArr = new bool[g_AmountOfColors];

	//InitializeLevel();
	for (int index{}; index < NrGridCells; ++index)
	{
		if (index < 9)
		{
			TextureFromString("0" + std::to_string(index + 1), g_FontName, g_TextSize, g_TextColor, g_LevelNumbers[index]);
		}
		else
		{
			TextureFromString(std::to_string(index + 1), g_FontName, g_TextSize, g_TextColor, g_LevelNumbers[index]);
		}

	}
	TextureFromString(g_TextForLevelComplete, g_FontName, g_TextSize, g_TextColor, g_LevelComplete);
	TextureFromString(g_TextForAllLevelsComplete, g_FontName, g_TextSize, g_TextColor, g_AllLevelsComplete);
}

void Draw()
{
	ClearBackground(0, 0, 0);
	if (g_CurrentGameState == GameState::mainMenu && CheckAllLevelsCompleted() == false)
	{
		DrawGrid();
		DrawLevels();
	}
	else if (g_CurrentGameState == GameState::inGame)
	{
		DrawGrid();
		DrawCircles();
		DrawConnections();
		if (CheckEndCondition())
		{
			float rectangleWidth{ 450 }, rectangleHeight{ 100 };
			SetColor(0.2f, 0.2f, 0.2f);
			FillRect(g_WindowWidth / 2 - rectangleWidth / 2, g_WindowHeight / 2 - rectangleHeight / 2, rectangleWidth, rectangleHeight);
			DrawTexture(g_LevelComplete, Point2f{ 150, g_WindowHeight / 2 - g_TextSize / 2 });
			g_StartCountDown = true;
			g_CurrentInputState = InputState::disabled;
			g_HasCurrentLevelStarted = false;
		}
	}
	else if (g_CurrentGameState == GameState::mainMenu && CheckAllLevelsCompleted() == true)
	{
		DrawGrid();
		DrawLevels();
		float rectangleWidth{ 550 }, rectangleHeight{ 100 };
		SetColor(0.2f, 0.2f, 0.2f);
		FillRect(g_WindowWidth / 2 - rectangleWidth / 2, g_WindowHeight / 2 - rectangleHeight / 2, rectangleWidth, rectangleHeight);
		DrawTexture(g_AllLevelsComplete, Point2f{ 100, g_WindowHeight / 2 - g_TextSize / 2 });
		g_CurrentInputState = InputState::disabled;
	}

}

void Update(float elapsedSec)
{
	CheckCountDown(elapsedSec);
}

void End()
{
	delete[] g_GridColorDynArr;
	delete[] g_GridStateDynArr;
	delete[] g_GridValueDynArr;
	delete[] g_MaxValueDynArr;
	delete[] g_IsColorConnectedDynArr;
	DeleteTexture(g_LevelComplete);
	for (int index{}; index < g_NrRows * g_NrColumns; ++index)
	{
		DeleteTexture(g_LevelNumbers[index]);
	}
	delete[] g_IsLevelCompleted;
}
#pragma endregion gameFunctions

//Keyboard and mouse input handling
#pragma region inputHandling											
void OnKeyDownEvent(SDL_Keycode key)
{

}

void OnKeyUpEvent(SDL_Keycode key)
{
	switch (key)
	{
	case SDLK_i:
		PrintInfo();
		break;
	}
}

void OnMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	if (g_CurrentMouseClickState == MouseClickState::down)
	{
		CheckMouseGridPosition(float(e.x), float(e.y));
	}
}

void OnMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	switch (e.button)
	{
	case SDL_BUTTON_LEFT:
		if (CheckClickStart(float(e.x), float(e.y)) && g_CurrentGameState == GameState::inGame && g_CurrentInputState == InputState::enabled)
		{
			g_CurrentMouseClickState = MouseClickState::down;
		}
		else if (g_CurrentInputState == InputState::enabled && g_HasCurrentLevelStarted == false)
		{
			InitializeLevel(float(e.x), float(e.y));
		}
		break;
	}
}

void OnMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	switch (e.button)
	{
	case SDL_BUTTON_LEFT:
		g_CurrentMouseClickState = MouseClickState::up;
		g_CurrentMouseNumber = 1;
		break;
	}
}
#pragma endregion inputHandling

#pragma region ownDefinitions
void DrawGrid()
{
	SetColor(0.5f, 0.5f, 0.5f);
	for (int i{}; i < g_NrColumns - 1; ++i)
	{
		Point2f bottom{ g_CellSize * (i + 1), 0 }, top{ g_CellSize * (i + 1), g_WindowHeight };
		DrawLine(bottom, top);
	}
	for (int i{}; i < g_NrRows - 1; ++i)
	{
		Point2f left{ 0, g_CellSize * (i + 1) }, right{ g_WindowWidth, g_CellSize * (i + 1) };
		DrawLine(left, right);
	}
	SetColor(1, 1, 1);
	DrawLine(0, 0, g_WindowWidth, 0);
	DrawLine(1, 0, 1, g_WindowHeight);
	DrawLine(g_WindowWidth, g_WindowHeight - 1, 0, g_WindowHeight - 1);
	DrawLine(g_WindowWidth, g_WindowHeight, g_WindowWidth, 0);
}
void InitializeLevel(float mouseX, float mouseY)
{
	if (g_CurrentGameState == GameState::mainMenu)
	{
		for (int index{}; index < g_NrRows * g_NrColumns; ++index)
		{
			float	left{ g_CellSize * (index % g_NrColumns) },
				right{ left + g_CellSize },
				top{ g_WindowHeight - g_CellSize * (index / g_NrColumns) },
				bottom{ top - g_CellSize };
			if (mouseX < right && mouseX > left && mouseY < top && mouseY > bottom && g_IsLevelCompleted[index] == false)
			{
				g_ClickedLevelNumber = index;
				g_CurrentGameState = GameState::inGame;
			}
		}
	}

	if (g_HasCurrentLevelStarted == false) // load up the selected level
	{
		LoadLevel();
	}

	for (int index{}; index < g_NrRows * g_NrColumns; ++index)
	{
		if (g_GridColorDynArr[index] != GridCellColor::none)
		{
			g_GridStateDynArr[index] = GridCellState::circle;
		}
	}
}
void DrawCircles()
{
	float circleRadius{ 50 };
	for (int index{}; index < g_NrRows * g_NrColumns; ++index)
	{
		if (g_GridStateDynArr[index] == GridCellState::circle)
		{
			switch (g_GridColorDynArr[index])
			{
			case GridCellColor::blue:
				SetColor(0, 0, 1);
				break;
			case GridCellColor::red:
				SetColor(1, 0, 0);
				break;
			case GridCellColor::orange:
				SetColor(1, 0.5f, 0);
				break;
			case GridCellColor::yellow:
				SetColor(0.8f, 0.8f, 0);
				break;
			case GridCellColor::green:
				SetColor(0, 0.6f, 0);
				break;
			}
			int row{ index / g_NrColumns }, column{ index % g_NrColumns };
			Point2f center{ g_CellSize / 2 + column * g_CellSize, g_WindowHeight - g_CellSize / 2 - row * g_CellSize };
			FillEllipse(center, circleRadius, circleRadius);
		}
	}
}
bool CheckClickStart(float mouseX, float mouseY)
{
	for (int index{}; index < g_NrRows * g_NrColumns; ++index)
	{
		if (g_GridStateDynArr[index] == GridCellState::circle && g_CurrentInputState == InputState::enabled)
		{
			float	left{ g_CellSize * (index % g_NrColumns) },
				right{ left + g_CellSize },
				top{ g_WindowHeight - g_CellSize * (index / g_NrColumns) },
				bottom{ top - g_CellSize };

			if (mouseX < right && mouseX > left && mouseY < top && mouseY > bottom)
			{
				g_CurrentMouseColor = g_GridColorDynArr[index];
				g_CurrentMouseIndex = index;
				ResetColor(g_CurrentMouseColor);
				g_GridValueDynArr[index] = g_CurrentMouseNumber;
				++g_CurrentMouseNumber;
				return true;
			}
		}
	}
	return false;

}
void CheckMouseGridPosition(float mouseX, float mouseY)
{
	for (int index{}; index < g_NrRows * g_NrColumns; ++index)
	{
		float	left{ g_CellSize * (index % g_NrColumns) },
			right{ left + g_CellSize },
			top{ g_WindowHeight - g_CellSize * (index / g_NrColumns) },
			bottom{ top - g_CellSize };

		if (mouseX < right && mouseX > left && mouseY < top && mouseY > bottom)
		{
			if (g_CurrentMouseIndex != index && g_GridStateDynArr[index] == GridCellState::none &&
				(index == g_CurrentMouseIndex + 1 || index == g_CurrentMouseIndex - 1
					|| index == g_CurrentMouseIndex + g_NrColumns || index == g_CurrentMouseIndex - g_NrColumns))
			{
				g_CurrentMouseIndex = index;
				g_GridColorDynArr[index] = g_CurrentMouseColor;
				g_GridStateDynArr[index] = GridCellState::line;
				g_GridValueDynArr[index] = g_CurrentMouseNumber;
				switch (g_CurrentMouseColor)
				{
				case GridCellColor::blue:
					g_MaxValueDynArr[0] = g_CurrentMouseNumber;
					break;
				case GridCellColor::red:
					g_MaxValueDynArr[1] = g_CurrentMouseNumber;
					break;
				case GridCellColor::orange:
					g_MaxValueDynArr[2] = g_CurrentMouseNumber;
					break;
				case GridCellColor::yellow:
					g_MaxValueDynArr[3] = g_CurrentMouseNumber;
					break;
				case GridCellColor::green:
					g_MaxValueDynArr[4] = g_CurrentMouseNumber;
					break;
				}
				++g_CurrentMouseNumber;
			}
			else if (g_GridStateDynArr[index] == GridCellState::circle &&
				g_GridColorDynArr[index] == g_CurrentMouseColor &&
				g_GridValueDynArr[index] == 0 &&
				(index == g_CurrentMouseIndex + 1 || index == g_CurrentMouseIndex - 1
					|| index == g_CurrentMouseIndex + g_NrColumns || index == g_CurrentMouseIndex - g_NrColumns))
			{
				g_CurrentMouseIndex = index;
				g_GridValueDynArr[index] = g_CurrentMouseNumber;
				switch (g_CurrentMouseColor)
				{
				case GridCellColor::blue:
					g_MaxValueDynArr[0] = g_CurrentMouseNumber;
					g_IsColorConnectedDynArr[0] = true;
					break;
				case GridCellColor::red:
					g_MaxValueDynArr[1] = g_CurrentMouseNumber;
					g_IsColorConnectedDynArr[1] = true;
					break;
				case GridCellColor::orange:
					g_MaxValueDynArr[2] = g_CurrentMouseNumber;
					g_IsColorConnectedDynArr[2] = true;
					break;
				case GridCellColor::yellow:
					g_MaxValueDynArr[3] = g_CurrentMouseNumber;
					g_IsColorConnectedDynArr[3] = true;
					break;
				case GridCellColor::green:
					g_MaxValueDynArr[4] = g_CurrentMouseNumber;
					g_IsColorConnectedDynArr[4] = true;
					break;
				}
				++g_CurrentMouseNumber;
				g_CurrentMouseClickState = MouseClickState::up;

			}
		}
	}
}
void DrawConnections()
{
	float lineWidth{ 50 }, radius{ lineWidth / 2 };
	for (int index{}; index < g_AmountOfColors; ++index)
	{
		switch (index)
		{
		case 0:
			SetColor(0, 0, 1); // blue
			break;
		case 1:
			SetColor(1, 0, 0); // red
			break;
		case 2:
			SetColor(1, 0.5f, 0); // orange
			break;
		case 3:
			SetColor(0.8f, 0.8f, 0); // yellow
			break;
		case 4:
			SetColor(0, 0.6f, 0); // green
			break;
		}
		for (int i{}; i < g_MaxValueDynArr[index] - 1; ++i)
		{
			GridCellColor currentColor{ GridCellColor(index + 1) };
			int indexPoint1{ SearchNextConnection(currentColor, i + 1) };
			int indexPoint2{ SearchNextConnection(currentColor, i + 2) };
			Point2f point1{ g_CellSize / 2 + (indexPoint1 % g_NrColumns) * g_CellSize ,
							g_WindowHeight - g_CellSize / 2 - (indexPoint1 / g_NrColumns) * g_CellSize };
			Point2f point2{ g_CellSize / 2 + (indexPoint2 % g_NrColumns) * g_CellSize ,
							g_WindowHeight - g_CellSize / 2 - (indexPoint2 / g_NrColumns) * g_CellSize };


			if (point1.x == point2.x && point1.y - point2.y > 0)
			{
				FillRect(point1.x - lineWidth / 2, point1.y, lineWidth, -g_CellSize);
			}
			else if (point1.x == point2.x && point1.y - point2.y < 0)
			{
				FillRect(point1.x - lineWidth / 2, point1.y, lineWidth, g_CellSize);
			}
			else if (point1.y == point2.y && point1.x - point2.x > 0)
			{
				FillRect(point1.x, point1.y - lineWidth / 2, -g_CellSize, lineWidth);
			}
			else if (point1.y == point2.y && point1.x - point2.x < 0)
			{
				FillRect(point1.x, point1.y - lineWidth / 2, g_CellSize, lineWidth);
			}

			//DrawLine(point1, point2, lineWidth);  // for some reason broken for me

			FillEllipse(point2, radius, radius);
		}
	}
}
int SearchNextConnection(GridCellColor color, int value)
{
	for (int index{}; index < g_NrColumns * g_NrRows; ++index)
	{
		if (g_GridColorDynArr[index] == color && g_GridValueDynArr[index] == value)
		{
			return index;
		}
	}
	std::cout << "ERROR!!!! In SearchNextConnection()" << std::endl;
	return 0;
}
void ResetColor(GridCellColor color)
{
	switch (color)
	{
	case GridCellColor::blue:
		g_MaxValueDynArr[0] = 1;
		g_IsColorConnectedDynArr[0] = false;
		break;
	case GridCellColor::red:
		g_MaxValueDynArr[1] = 1;
		g_IsColorConnectedDynArr[1] = false;
		break;
	case GridCellColor::orange:
		g_MaxValueDynArr[2] = 1;
		g_IsColorConnectedDynArr[2] = false;
		break;
	case GridCellColor::yellow:
		g_MaxValueDynArr[3] = 1;
		g_IsColorConnectedDynArr[3] = false;
		break;
	case GridCellColor::green:
		g_MaxValueDynArr[4] = 1;
		g_IsColorConnectedDynArr[4] = false;
		break;
	}
	for (int index{}; index < g_NrRows * g_NrColumns; ++index)
	{
		if (g_GridColorDynArr[index] == color && g_CurrentInputState == InputState::enabled)
		{
			g_GridValueDynArr[index] = 0;
			if (g_GridStateDynArr[index] != GridCellState::circle)
			{
				g_GridStateDynArr[index] = GridCellState::none;
				g_GridColorDynArr[index] = GridCellColor::none;
			}
		}
	}
}
bool CheckEndCondition()
{
	for (int index{}; index < g_NrRows * g_NrColumns; ++index)
	{
		if (g_GridValueDynArr[index] == 0)
		{
			return false;
		}
	}
	for (int index{}; index < g_AmountOfColors; ++index)
	{
		if (g_IsColorConnectedDynArr[index] == false)
		{
			return false;
		}
	}
	return true;
}
void PrintInfo()
{
	std::cout << "-- Flow --\n\n"
		<< "Goal: connect the circles of the same color, utilising every cell in the grid.\n"
		<< "To connect click a circle and hold the left mouse button while dragging.\n"
		<< "Click a circle again to reset its line.\n"
		<< "3 seconds after completing a level, you will be sent back to the level selection screen.\n"
		<< "Try to complete all 25 levels!\n";
}

void DrawLevels()
{
	for (int index{}; index < g_NrRows * g_NrColumns; ++index)
	{
		if (g_CurrentGameState == GameState::mainMenu)
		{
			for (int index{}; index < g_NrRows * g_NrColumns; ++index)
			{
				float	left{ g_CellSize * (index % g_NrColumns) },
					right{ left + g_CellSize },
					top{ g_WindowHeight - g_CellSize * (index / g_NrColumns) },
					bottom{ top - g_CellSize };
				if (g_IsLevelCompleted[index] == false) // if level is not completed
				{
					DrawTexture(g_LevelNumbers[index], Point2f{ left + (g_CellSize / 3.5f),top -  g_CellSize + (g_CellSize / 4 )});
				}
				else // if level is completed
				{
					SetColor(0, 0.8f, 0);
					float lineWidth{ 10.f }, radius{ 4 };
					Point2f point1{ left + (right - left) / 8 , bottom + (top - bottom) / 2 };
					Point2f point2{ left + (right - left) / 3 , bottom + (top - bottom) / 4 };
					Point2f point3{ right - (right - left) / 8 , top - (top - bottom) / 4 };
					FillEllipse(point1, radius, radius);
					FillEllipse(point2, radius, radius);
					FillEllipse(point3, radius, radius);

					DrawLine(point1, point2, lineWidth);
					DrawLine(point2, point3, lineWidth);
				}

			}
		}
	}
}
void CheckCountDown(float elapsedSec)
{
	if (g_StartCountDown == true)
	{
		g_CountDown += elapsedSec;
	}

	if (g_CountDown >= 3.f)
	{
		g_StartCountDown = false;
		g_CountDown = 0;
		g_IsLevelCompleted[g_ClickedLevelNumber] = true;
		g_CurrentGameState = GameState::mainMenu;
		g_CurrentInputState = InputState::enabled;
		for (int index{}; index < g_NrRows * g_NrColumns; ++index)
		{
			g_GridColorDynArr[index] = GridCellColor::none;
			g_GridStateDynArr[index] = GridCellState::none;
			g_GridValueDynArr[index] = 0;
		}
		for (int index{}; index < g_AmountOfColors; ++index)
		{
			g_MaxValueDynArr[index] = 0;
			g_IsColorConnectedDynArr[index] = false;
		}
	}
}
bool CheckAllLevelsCompleted()
{
	for (int index{}; index < g_NrRows * g_NrColumns; ++index)
	{
		if (g_IsLevelCompleted[index] == false)
		{
			return false;
		}
	}
	return true;
}
void LoadLevel()
{
	// could also use a switch
	if (g_ClickedLevelNumber == 0)
	{
		g_GridColorDynArr[0] = GridCellColor::red;
		g_GridColorDynArr[2] = GridCellColor::red;
		g_GridColorDynArr[3] = GridCellColor::green;
		g_GridColorDynArr[5] = GridCellColor::blue;
		g_GridColorDynArr[6] = GridCellColor::orange;
		g_GridColorDynArr[11] = GridCellColor::green;
		g_GridColorDynArr[12] = GridCellColor::yellow;
		g_GridColorDynArr[13] = GridCellColor::orange;
		g_GridColorDynArr[18] = GridCellColor::yellow;
		g_GridColorDynArr[20] = GridCellColor::blue;
	}
	else if (g_ClickedLevelNumber == 1)
	{
		g_GridColorDynArr[0] = GridCellColor::red;
		g_GridColorDynArr[2] = GridCellColor::green;
		g_GridColorDynArr[4] = GridCellColor::yellow;
		g_GridColorDynArr[7] = GridCellColor::blue;
		g_GridColorDynArr[9] = GridCellColor::orange;
		g_GridColorDynArr[16] = GridCellColor::green;
		g_GridColorDynArr[18] = GridCellColor::yellow;
		g_GridColorDynArr[21] = GridCellColor::red;
		g_GridColorDynArr[22] = GridCellColor::blue;
		g_GridColorDynArr[23] = GridCellColor::orange;
	}
	else if (g_ClickedLevelNumber == 2)
	{
		g_GridColorDynArr[1] = GridCellColor::yellow;
		g_GridColorDynArr[2] = GridCellColor::blue;
		g_GridColorDynArr[3] = GridCellColor::green;
		g_GridColorDynArr[8] = GridCellColor::red;
		g_GridColorDynArr[12] = GridCellColor::red;
		g_GridColorDynArr[15] = GridCellColor::yellow;
		g_GridColorDynArr[18] = GridCellColor::orange;
		g_GridColorDynArr[20] = GridCellColor::blue;
		g_GridColorDynArr[22] = GridCellColor::orange;
		g_GridColorDynArr[23] = GridCellColor::green;
	}
	else if (g_ClickedLevelNumber == 3)
	{
		g_GridColorDynArr[0] = GridCellColor::blue;
		g_GridColorDynArr[3] = GridCellColor::red;
		g_GridColorDynArr[4] = GridCellColor::orange;
		g_GridColorDynArr[8] = GridCellColor::yellow;
		g_GridColorDynArr[12] = GridCellColor::yellow;
		g_GridColorDynArr[16] = GridCellColor::red;
		g_GridColorDynArr[17] = GridCellColor::orange;
		g_GridColorDynArr[19] = GridCellColor::green;
		g_GridColorDynArr[21] = GridCellColor::blue;
		g_GridColorDynArr[22] = GridCellColor::green;

	}
	else if (g_ClickedLevelNumber == 4)
	{
		g_GridColorDynArr[3] = GridCellColor::red;
		g_GridColorDynArr[4] = GridCellColor::green;
		g_GridColorDynArr[7] = GridCellColor::blue;
		g_GridColorDynArr[8] = GridCellColor::green;
		g_GridColorDynArr[10] = GridCellColor::red;
		g_GridColorDynArr[15] = GridCellColor::orange;
		g_GridColorDynArr[16] = GridCellColor::blue;
		g_GridColorDynArr[18] = GridCellColor::yellow;
		g_GridColorDynArr[19] = GridCellColor::orange;
		g_GridColorDynArr[24] = GridCellColor::yellow;
	}
	else if (g_ClickedLevelNumber == 5)
	{
		g_GridColorDynArr[0] = GridCellColor::green;
		g_GridColorDynArr[1] = GridCellColor::yellow;
		g_GridColorDynArr[3] = GridCellColor::yellow;
		g_GridColorDynArr[4] = GridCellColor::orange;
		g_GridColorDynArr[10] = GridCellColor::red;
		g_GridColorDynArr[12] = GridCellColor::orange;
		g_GridColorDynArr[13] = GridCellColor::blue;
		g_GridColorDynArr[18] = GridCellColor::green;
		g_GridColorDynArr[23] = GridCellColor::red;
		g_GridColorDynArr[24] = GridCellColor::blue;
	}
	else if (g_ClickedLevelNumber == 6)
	{
		g_GridColorDynArr[2] = GridCellColor::green;
		g_GridColorDynArr[3] = GridCellColor::yellow;
		g_GridColorDynArr[4] = GridCellColor::blue;
		g_GridColorDynArr[10] = GridCellColor::green;
		g_GridColorDynArr[13] = GridCellColor::orange;
		g_GridColorDynArr[18] = GridCellColor::red;
		g_GridColorDynArr[19] = GridCellColor::blue;
		g_GridColorDynArr[20] = GridCellColor::yellow;
		g_GridColorDynArr[21] = GridCellColor::orange;
		g_GridColorDynArr[24] = GridCellColor::red;

	}
	else if (g_ClickedLevelNumber == 7)
	{
		g_GridColorDynArr[6] = GridCellColor::green;
		g_GridColorDynArr[7] = GridCellColor::red;
		g_GridColorDynArr[9] = GridCellColor::yellow;
		g_GridColorDynArr[14] = GridCellColor::orange;
		g_GridColorDynArr[15] = GridCellColor::yellow;
		g_GridColorDynArr[16] = GridCellColor::blue;
		g_GridColorDynArr[18] = GridCellColor::red;
		g_GridColorDynArr[20] = GridCellColor::blue;
		g_GridColorDynArr[22] = GridCellColor::green;
		g_GridColorDynArr[23] = GridCellColor::orange;
	}
	else if (g_ClickedLevelNumber == 8)
	{
		g_GridColorDynArr[1] = GridCellColor::red;
		g_GridColorDynArr[6] = GridCellColor::orange;
		g_GridColorDynArr[8] = GridCellColor::blue;
		g_GridColorDynArr[10] = GridCellColor::red;
		g_GridColorDynArr[11] = GridCellColor::yellow;
		g_GridColorDynArr[12] = GridCellColor::blue;
		g_GridColorDynArr[17] = GridCellColor::orange;
		g_GridColorDynArr[20] = GridCellColor::yellow;
		g_GridColorDynArr[21] = GridCellColor::green;
		g_GridColorDynArr[24] = GridCellColor::green;
	}
	else if (g_ClickedLevelNumber == 9)
	{
		g_GridColorDynArr[0] = GridCellColor::green;
		g_GridColorDynArr[1] = GridCellColor::red;
		g_GridColorDynArr[4] = GridCellColor::red;
		g_GridColorDynArr[13] = GridCellColor::orange;
		g_GridColorDynArr[14] = GridCellColor::green;
		g_GridColorDynArr[15] = GridCellColor::orange;
		g_GridColorDynArr[18] = GridCellColor::blue;
		g_GridColorDynArr[19] = GridCellColor::yellow;
		g_GridColorDynArr[20] = GridCellColor::blue;
		g_GridColorDynArr[22] = GridCellColor::yellow;
	}
	else if (g_ClickedLevelNumber == 10)
	{
		g_GridColorDynArr[5] = GridCellColor::orange;
		g_GridColorDynArr[6] = GridCellColor::red;
		g_GridColorDynArr[8] = GridCellColor::red;
		g_GridColorDynArr[9] = GridCellColor::orange;
		g_GridColorDynArr[10] = GridCellColor::yellow;
		g_GridColorDynArr[16] = GridCellColor::blue;
		g_GridColorDynArr[18] = GridCellColor::green;
		g_GridColorDynArr[20] = GridCellColor::yellow;
		g_GridColorDynArr[21] = GridCellColor::green;
		g_GridColorDynArr[23] = GridCellColor::blue;
	}
	else if (g_ClickedLevelNumber == 11)
	{
		g_GridColorDynArr[0] = GridCellColor::green;
		g_GridColorDynArr[5] = GridCellColor::orange;
		g_GridColorDynArr[6] = GridCellColor::red;
		g_GridColorDynArr[8] = GridCellColor::red;
		g_GridColorDynArr[13] = GridCellColor::orange;
		g_GridColorDynArr[14] = GridCellColor::green;
		g_GridColorDynArr[15] = GridCellColor::blue;
		g_GridColorDynArr[18] = GridCellColor::blue;
		g_GridColorDynArr[19] = GridCellColor::yellow;
		g_GridColorDynArr[20] = GridCellColor::yellow;
	}
	else if (g_ClickedLevelNumber == 12)
	{
		g_GridColorDynArr[0] = GridCellColor::red;
		g_GridColorDynArr[2] = GridCellColor::red;
		g_GridColorDynArr[3] = GridCellColor::orange;
		g_GridColorDynArr[4] = GridCellColor::blue;
		g_GridColorDynArr[11] = GridCellColor::green;
		g_GridColorDynArr[15] = GridCellColor::orange;
		g_GridColorDynArr[17] = GridCellColor::yellow;
		g_GridColorDynArr[18] = GridCellColor::green;
		g_GridColorDynArr[20] = GridCellColor::yellow;
		g_GridColorDynArr[22] = GridCellColor::blue;
	}
	else if (g_ClickedLevelNumber == 13)
	{
		g_GridColorDynArr[4] = GridCellColor::yellow;
		g_GridColorDynArr[7] = GridCellColor::red;
		g_GridColorDynArr[8] = GridCellColor::blue;
		g_GridColorDynArr[10] = GridCellColor::blue;
		g_GridColorDynArr[11] = GridCellColor::red;
		g_GridColorDynArr[15] = GridCellColor::yellow;
		g_GridColorDynArr[18] = GridCellColor::green;
		g_GridColorDynArr[20] = GridCellColor::orange;
		g_GridColorDynArr[23] = GridCellColor::orange;
		g_GridColorDynArr[24] = GridCellColor::green;
	}
	else if (g_ClickedLevelNumber == 14)
	{
		g_GridColorDynArr[0] = GridCellColor::blue;
		g_GridColorDynArr[1] = GridCellColor::green;
		g_GridColorDynArr[3] = GridCellColor::red;
		g_GridColorDynArr[10] = GridCellColor::yellow;
		g_GridColorDynArr[12] = GridCellColor::blue;
		g_GridColorDynArr[13] = GridCellColor::green;
		g_GridColorDynArr[16] = GridCellColor::orange;
		g_GridColorDynArr[18] = GridCellColor::orange;
		g_GridColorDynArr[20] = GridCellColor::yellow;
		g_GridColorDynArr[21] = GridCellColor::red;
	}
	else if (g_ClickedLevelNumber == 15)
	{
		g_GridColorDynArr[0] = GridCellColor::red;
		g_GridColorDynArr[3] = GridCellColor::red;
		g_GridColorDynArr[4] = GridCellColor::yellow;
		g_GridColorDynArr[7] = GridCellColor::blue;
		g_GridColorDynArr[8] = GridCellColor::yellow;
		g_GridColorDynArr[11] = GridCellColor::green;
		g_GridColorDynArr[12] = GridCellColor::orange;
		g_GridColorDynArr[18] = GridCellColor::green;
		g_GridColorDynArr[22] = GridCellColor::blue;
		g_GridColorDynArr[23] = GridCellColor::orange;
	}
	else if (g_ClickedLevelNumber == 16)
	{
		g_GridColorDynArr[4] = GridCellColor::orange;
		g_GridColorDynArr[5] = GridCellColor::yellow;
		g_GridColorDynArr[6] = GridCellColor::blue;
		g_GridColorDynArr[10] = GridCellColor::red;
		g_GridColorDynArr[11] = GridCellColor::green;
		g_GridColorDynArr[17] = GridCellColor::blue;
		g_GridColorDynArr[18] = GridCellColor::yellow;
		g_GridColorDynArr[20] = GridCellColor::red;
		g_GridColorDynArr[22] = GridCellColor::green;
		g_GridColorDynArr[23] = GridCellColor::orange;
	}
	else if (g_ClickedLevelNumber == 17)
	{
		g_GridColorDynArr[4] = GridCellColor::blue;
		g_GridColorDynArr[6] = GridCellColor::orange;
		g_GridColorDynArr[7] = GridCellColor::green;
		g_GridColorDynArr[9] = GridCellColor::yellow;
		g_GridColorDynArr[12] = GridCellColor::blue;
		g_GridColorDynArr[16] = GridCellColor::orange;
		g_GridColorDynArr[17] = GridCellColor::green;
		g_GridColorDynArr[18] = GridCellColor::red;
		g_GridColorDynArr[19] = GridCellColor::yellow;
		g_GridColorDynArr[24] = GridCellColor::red;
	}
	else if (g_ClickedLevelNumber == 18)
	{
		g_GridColorDynArr[1] = GridCellColor::yellow;
		g_GridColorDynArr[2] = GridCellColor::blue;
		g_GridColorDynArr[3] = GridCellColor::orange;
		g_GridColorDynArr[6] = GridCellColor::green;
		g_GridColorDynArr[8] = GridCellColor::red;
		g_GridColorDynArr[17] = GridCellColor::blue;
		g_GridColorDynArr[19] = GridCellColor::orange;
		g_GridColorDynArr[20] = GridCellColor::yellow;
		g_GridColorDynArr[22] = GridCellColor::green;
		g_GridColorDynArr[24] = GridCellColor::red;
	}
	else if (g_ClickedLevelNumber == 19)
	{
		g_GridColorDynArr[0] = GridCellColor::blue;
		g_GridColorDynArr[2] = GridCellColor::orange;
		g_GridColorDynArr[3] = GridCellColor::green;
		g_GridColorDynArr[5] = GridCellColor::red;
		g_GridColorDynArr[8] = GridCellColor::orange;
		g_GridColorDynArr[12] = GridCellColor::yellow;
		g_GridColorDynArr[18] = GridCellColor::yellow;
		g_GridColorDynArr[21] = GridCellColor::red;
		g_GridColorDynArr[22] = GridCellColor::blue;
		g_GridColorDynArr[23] = GridCellColor::green;
	}
	else if (g_ClickedLevelNumber == 20)
	{
		g_GridColorDynArr[1] = GridCellColor::blue;
		g_GridColorDynArr[2] = GridCellColor::green;
		g_GridColorDynArr[6] = GridCellColor::yellow;
		g_GridColorDynArr[7] = GridCellColor::orange;
		g_GridColorDynArr[9] = GridCellColor::green;
		g_GridColorDynArr[12] = GridCellColor::red;
		g_GridColorDynArr[18] = GridCellColor::red;
		g_GridColorDynArr[20] = GridCellColor::blue;
		g_GridColorDynArr[23] = GridCellColor::yellow;
		g_GridColorDynArr[24] = GridCellColor::orange;
	}
	else if (g_ClickedLevelNumber == 21)
	{
		g_GridColorDynArr[4] = GridCellColor::red;
		g_GridColorDynArr[5] = GridCellColor::orange;
		g_GridColorDynArr[6] = GridCellColor::blue;
		g_GridColorDynArr[8] = GridCellColor::red;
		g_GridColorDynArr[9] = GridCellColor::orange;
		g_GridColorDynArr[10] = GridCellColor::yellow;
		g_GridColorDynArr[17] = GridCellColor::blue;
		g_GridColorDynArr[18] = GridCellColor::green;
		g_GridColorDynArr[23] = GridCellColor::yellow;
		g_GridColorDynArr[24] = GridCellColor::green;
	}
	else if (g_ClickedLevelNumber == 22)
	{
		g_GridColorDynArr[3] = GridCellColor::yellow;
		g_GridColorDynArr[4] = GridCellColor::blue;
		g_GridColorDynArr[8] = GridCellColor::green;
		g_GridColorDynArr[10] = GridCellColor::yellow;
		g_GridColorDynArr[12] = GridCellColor::orange;
		g_GridColorDynArr[13] = GridCellColor::blue;
		g_GridColorDynArr[20] = GridCellColor::green;
		g_GridColorDynArr[21] = GridCellColor::red;
		g_GridColorDynArr[23] = GridCellColor::red;
		g_GridColorDynArr[24] = GridCellColor::orange;
	}
	else if (g_ClickedLevelNumber == 23)
	{
		g_GridColorDynArr[0] = GridCellColor::red;
		g_GridColorDynArr[1] = GridCellColor::orange;
		g_GridColorDynArr[4] = GridCellColor::yellow;
		g_GridColorDynArr[8] = GridCellColor::yellow;
		g_GridColorDynArr[9] = GridCellColor::blue;
		g_GridColorDynArr[10] = GridCellColor::green;
		g_GridColorDynArr[13] = GridCellColor::orange;
		g_GridColorDynArr[22] = GridCellColor::green;
		g_GridColorDynArr[23] = GridCellColor::red;
		g_GridColorDynArr[24] = GridCellColor::blue;
	}
	else if (g_ClickedLevelNumber == 24)
	{
		g_GridColorDynArr[0] = GridCellColor::blue;
		g_GridColorDynArr[2] = GridCellColor::green;
		g_GridColorDynArr[3] = GridCellColor::orange;
		g_GridColorDynArr[5] = GridCellColor::red;
		g_GridColorDynArr[6] = GridCellColor::blue;
		g_GridColorDynArr[8] = GridCellColor::yellow;
		g_GridColorDynArr[14] = GridCellColor::orange;
		g_GridColorDynArr[16] = GridCellColor::green;
		g_GridColorDynArr[17] = GridCellColor::yellow;
		g_GridColorDynArr[19] = GridCellColor::red;
	}
	g_HasCurrentLevelStarted = true;
}
#pragma endregion ownDefinitions