#pragma once

#include <Windows.h>

#pragma warning(push)
#pragma warning(disable: 4101)


#include "Border.h"
#include "Level.h"
#include "Platform.h"
#include "Ball_Set.h"

enum EKey_Type 
{
   EKT_Left, 
   EKT_Right,
   EKT_Space
};

enum EGame_State
{
   EGS_Test_Ball,

   EGS_Play_Level,
   EGS_Lost_Ball,
   EGS_Restart_Level
};


const int Timer_ID = WM_USER + 1;

class AsEngine
{
public:
   AsEngine();

   void Init_Engine(HWND hwnd);
   void Draw_Frame(HDC hdc, RECT &paint_area);
   int On_Key(EKey_Type key_type, bool key_down);
   int On_Timer();

private:
   void Play_Level();
   void Advance_Movers();
   void Act();
   void On_Falling_Letter(AFalling_Letter* falling_letter);
   EGame_State Game_State;
   double Rest_Distance;
   int Life_Count;

   AsLevel Level;
   AsPlatform Platform;
   AsBorder Border;
   AsBall_Set Ball_Set;
  

   AMover *Movers[AsConfig::Max_Movers_Count]; // UNO; Движущиеся в данный момент объекты
   AGraphics_Object *Modules[AsConfig::Max_Modules_Count]; // UNO; Главные графические обьекты (модули) игры
};
#pragma warning(push)