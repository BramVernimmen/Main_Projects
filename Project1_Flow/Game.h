#pragma once
using namespace utils;
#pragma region gameInformation
// Set your name and group in the title here
std::string g_WindowTitle{ "Flow - Soubry, Hanne - Vernimmen, Bram - 1DAE23" };

// Change the window dimensions here
float g_WindowWidth{ 720 };
float g_WindowHeight{ 720 };
#pragma endregion gameInformation



#pragma region ownDeclarations
// Declare your own global variables here
enum class GameState { mainMenu, inGame };
GameState g_CurrentGameState{ GameState::mainMenu };

enum class InputState { disabled, enabled };
InputState g_CurrentInputState{ InputState::enabled };

bool g_StartCountDown{ false };
float g_CountDown{};

int g_ClickedLevelNumber{ 1 };

int g_NrRows{ 5 }, g_NrColumns{ 5 };

int g_TotalAmountOfLevels{ g_NrRows * g_NrColumns };

enum class GridCellColor { none, blue, red, orange, yellow, green };
enum class GridCellState { none, circle, line };

bool* g_IsLevelCompleted;


GridCellColor* g_GridColorDynArr;
GridCellState* g_GridStateDynArr;
int* g_GridValueDynArr;
bool* g_IsColorConnectedDynArr;

float g_CellSize{ g_WindowWidth / g_NrColumns };

enum class MouseClickState { up, down };
MouseClickState g_CurrentMouseClickState{ MouseClickState::up };

GridCellColor g_CurrentMouseColor{ GridCellColor::none };
int g_CurrentMouseNumber{ 1 };
int g_CurrentMouseIndex{};

int* g_MaxValueDynArr;
int g_AmountOfColors{ 5 };

Texture* g_LevelNumbers;


Texture g_LevelComplete{};
const std::string g_TextForLevelComplete{ "Level complete!" };

Texture g_AllLevelsComplete{};
const std::string g_TextForAllLevelsComplete{ "All levels complete!" };


const std::string g_FontName{ "Resources/DIN-Light.otf" };
const int g_TextSize{ 60 };
const Color4f g_TextColor{ 1,1,1,1 };

bool g_HasCurrentLevelStarted{false};
// Declare your own functions here
void DrawGrid();
void InitializeLevel(float mouseX, float mouseY);
void DrawCircles();
bool CheckClickStart(float mouseX, float mouseY);
void CheckMouseGridPosition(float mouseX, float mouseY);
void DrawConnections();
int SearchNextConnection(GridCellColor color, int value);
void ResetColor(GridCellColor color);
bool CheckEndCondition();
void PrintInfo();

void DrawLevels();
void CheckCountDown(float elapsedSec);
bool CheckAllLevelsCompleted();
void LoadLevel();

#pragma endregion ownDeclarations

#pragma region gameFunctions											
void Start();
void Draw();
void Update(float elapsedSec);
void End();
#pragma endregion gameFunctions

#pragma region inputHandling											
void OnKeyDownEvent(SDL_Keycode key);
void OnKeyUpEvent(SDL_Keycode key);
void OnMouseMotionEvent(const SDL_MouseMotionEvent& e);
void OnMouseDownEvent(const SDL_MouseButtonEvent& e);
void OnMouseUpEvent(const SDL_MouseButtonEvent& e);
#pragma endregion inputHandling
