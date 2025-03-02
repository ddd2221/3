#include "Ball_Set.h"


// AsBall_Set
//------------------------------------------------------------------------------------------
void AsBall_Set::Begin_Movement()
{
   int i; 
   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
      Balls[i].Begin_Movement();

}
//------------------------------------------------------------------------------------------
void AsBall_Set::Finish_Movement()
{
   int i; 
   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
      Balls[i].Finish_Movement();

}
//------------------------------------------------------------------------------------------
void AsBall_Set::Advance(double max_speed)
{
   int i; 
   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
      Balls[i].Advance(max_speed);

}
//------------------------------------------------------------------------------------------
double AsBall_Set::Get_Speed()
{ 
   int i; 
   double curr_speed, max_speed = 0.0;


   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
   {
      curr_speed = Balls[i].Get_Speed();
      if (curr_speed > max_speed)
         max_speed = curr_speed;
   }
   return max_speed;
}
//--------------------------------------------------------------------------------------------
void AsBall_Set::Act()
{ 
   int i;
   ABall *curr_ball;

   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
   {
      curr_ball = &Balls[i];
      if (curr_ball->Get_State() == EBS_On_Platform)
         if (curr_ball->Release_Timer_Tick != 0 && AsConfig::Current_Timer_Tick >= curr_ball->Release_Timer_Tick)
            curr_ball->Release();
   }
}
//--------------------------------------------------------------------------------------------
void AsBall_Set::Clear(HDC hdc, RECT& paint_area)
{
   int i; 

   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
      Balls[i].Clear(hdc, paint_area);
}
//--------------------------------------------------------------------------------------------
void AsBall_Set::Draw(HDC hdc, RECT& paint_area)
{
   int i; 

   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
      Balls[i].Draw(hdc, paint_area);

}
//--------------------------------------------------------------------------------------------
bool  AsBall_Set::Is_Finished()
{ //  Заглушка. Не используется

      return false;
}
//--------------------------------------------------------------------------------------------
void AsBall_Set::Release_From_Platform(double platform_x_pos)
{
   int i;

   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
      if (Balls[i].Get_State() == EBS_On_Platform)
         Balls[i].Set_State(EBS_Normal, platform_x_pos, AsConfig::Start_Ball_Y_Pos);

}
//------------------------------------------------------------------------------------------
bool AsBall_Set::Release_Next_Ball()
{
   int i;
   ABall *curr_ball;

   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
   {
      curr_ball = &Balls[i];

      if (curr_ball->Get_State() == EBS_On_Platform)
      {
         curr_ball->Release();
         return true;
         
      }
   }
   return false;
}
//------------------------------------------------------------------------------------------
void AsBall_Set::Set_On_Platform(double platform_x_pos)
{
   int i;
   // 1. Ставит несколько мячиков на платформу
   for (i = 0; i < 1; i++)
   {
      Balls[i].Set_State(EBS_Normal);
      Balls[i].Set_State(EBS_On_Platform, platform_x_pos, AsConfig::Start_Ball_Y_Pos);
   }

   // 2. Обьявляет все оставшиеся отключенными
   for (; i < AsConfig::Max_Balls_Count; i++)
      Balls[i].Set_State(EBS_Disabled);

}
//------------------------------------------------------------------------------------------
bool AsBall_Set::All_Balls_Are_Lost()
{
   int i;
   int active_balls_count = 0;
   int lost_balls_count = 0;

   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
   {

      if (Balls[i].Get_State() == EBS_Disabled)
         continue;

      ++active_balls_count;

      if (Balls[i].Get_State() == EBS_Lost)
      {
         ++lost_balls_count;
         continue;
      }
   }
   if (active_balls_count == lost_balls_count)
      return true;
   else
      return false;

}
//------------------------------------------------------------------------------------------
void AsBall_Set::Set_For_Test()
{
   Balls[0].Set_For_Test(); // В повторяющихся тестах участвует только 0-й мячик
}
//------------------------------------------------------------------------------------------
bool AsBall_Set::Is_Test_Finished()
{
   return Balls[0].Is_Test_Finished();// В повторяющихся тестах участвует только 0-й мячик 

}
//------------------------------------------------------------------------------------------
void AsBall_Set::Triple_Balls()
{ // "Растроить" самый дальний летящий от платформы мячик
   int i;
   ABall* curr_ball;
   ABall* further_ball = 0;
   ABall* left_candidate = 0, *right_candidate = 0;
   double curr_ball_x, curr_ball_y;
   double further_ball_x, further_ball_y;
   // 1. Выбираем самый дальний по Y мячик 
   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
   {
      curr_ball = &Balls[i];

      if (curr_ball->Get_State() != EBS_Normal)
         continue;

      if (further_ball == 0)
         further_ball = curr_ball;
      else
      {
         curr_ball->Get_Center(curr_ball_x, curr_ball_y);
         further_ball->Get_Center(further_ball_x, further_ball_y);

         if (curr_ball_y < further_ball_y)
            further_ball = curr_ball;
      }
   }
   // 2. Если есть "нормальный" мячик, то размножаем его
   if (further_ball == 0)
      return;
   // В массиве ищем кандидата на размножение для левого и правого шарика который будет растраиваться в три пон
   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
   {
      curr_ball = &Balls[i];

      switch (curr_ball->Get_State() ) 
      {
      case EBS_Disabled:
      case EBS_Lost:
         if (left_candidate == 0)
            left_candidate = curr_ball;
         else
            if (right_candidate == 0)
            {
               right_candidate = curr_ball;
               break; // Оба кандидата найдены
            }
      }
   }
   // 3. Разводим боковые мячики в стороны
   if (left_candidate != 0)
   {
      *left_candidate = *further_ball;
      left_candidate->Set_Direction(left_candidate->Get_Direction() + AsConfig::Min_Ball_Angle);

   }

   if (right_candidate != 0)
   {
      *right_candidate = *further_ball;
      right_candidate->Set_Direction(right_candidate->Get_Direction() - AsConfig::Min_Ball_Angle);

   }

}
//------------------------------------------------------------------------------------------
void AsBall_Set::Inverse_Balls()
{ // Меняем направление на обратное у всех мячиков
   int i;
   ABall *curr_ball;

   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
   {
      curr_ball = &Balls[i];
      // Если состояние текущего мячика - нормальное, то мы берем его направление и изменяем (добавляем к нему 180 град)
      if (curr_ball->Get_State() == EBS_Normal)
         curr_ball->Set_Direction(curr_ball->Get_Direction() + M_PI);
   }
}
//------------------------------------------------------------------------------------------
void AsBall_Set::Accelerate()
{ // Меняем скорость на быструю у всех мячиков 
   int i;
   ABall *curr_ball;

   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
   {
      curr_ball = &Balls[i];

      if (curr_ball->Get_State() == EBS_Normal)
         curr_ball->Set_Speed(curr_ball->Get_Speed() * AsConfig::Ball_Acceleration);
   }
}
//------------------------------------------------------------------------------------------
void AsBall_Set::Reset_Speed()
{
   int i;
   ABall* curr_ball;

   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
   {
      curr_ball = &Balls[i];

      if (curr_ball->Get_State() == EBS_Normal)
         curr_ball->Set_Speed(AsConfig::Normal_Ball_Speed);
   }
}
//------------------------------------------------------------------------------------------
void AsBall_Set::On_Platform_Advance(double direction, double platform_speed, double max_speed)
{
   int i;
   ABall *curr_ball;

   for (i = 0; i < AsConfig::Max_Balls_Count; i++)
   {
      curr_ball = &Balls[i];

      if (curr_ball->Get_State() == EBS_On_Platform)  
         curr_ball->Forced_Advance(direction, platform_speed   , max_speed);
   }

}
//------------------------------------------------------------------------------------------