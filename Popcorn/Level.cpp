#include "Level.h"



char AsLevel::Level_01[AsConfig::Level_Height][AsConfig::Level_Width] =
{ 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


char AsLevel::Test_Level[AsConfig::Level_Height][AsConfig::Level_Width] =
{ 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


// AsLevel
AsLevel::~AsLevel()
{
   Cancel_All_Activity();

   delete[] Teleport_Bricks_Position;
}


AsLevel::AsLevel()
   : Level_Rect{}, Need_To_Cancel_All(false), Active_Bricks_Count(0), Falling_Letters_Count(0), Teleport_Bricks_Count(0), Teleport_Bricks_Position(0),
   Parachute_Color(AsConfig::Red_Color, AsConfig::Blue_Color, AsConfig::Global_Scale), Advertisement(0)
{
}

bool AsLevel::Check_Hit(double next_x_pos, double next_y_pos, ABall *ball)
{
   //Корректируем позицию при отражение от платформы
   int i, j;
   double direction;
   double min_ball_x, max_ball_x;
   double min_ball_y, max_ball_y;
   int min_level_x, max_level_x;
   int min_level_y, max_level_y;
   bool got_horizontal_hit, got_vertical_hit;
   double horizontal_reflection_pos, vertical_reflection_pos;

   if (next_y_pos + ball->Radius > AsConfig::Level_Y_Offset + (AsConfig::Level_Height - 1) * AsConfig::Cell_Height + AsConfig::Brick_Height)
      return false;

   direction = ball->Get_Direction();

   min_ball_x = next_x_pos - ball->Radius;
   max_ball_x = next_x_pos + ball->Radius;
   min_ball_y = next_y_pos - ball->Radius;
   max_ball_y = next_y_pos + ball->Radius;

   min_level_x = (int)( (min_ball_x - AsConfig::Level_X_Offset) / (double)AsConfig::Cell_Width);
   if (min_level_x < 0)
      min_level_x = 0;


   max_level_x = (int)( (max_ball_x - AsConfig::Level_X_Offset) / (double)AsConfig::Cell_Width);
   if (max_level_x >= AsConfig::Level_Width - 1)
      max_level_x = AsConfig::Level_Width - 1;

   min_level_y = (int)( (min_ball_y - AsConfig::Level_Y_Offset) / (double)AsConfig::Cell_Height);
   if (min_level_y < 0)
      min_level_y = 0;

   max_level_y = (int)( (max_ball_y - AsConfig::Level_Y_Offset) / (double)AsConfig::Cell_Height);
   if (max_level_y >= AsConfig::Level_Height - 1)
      max_level_y = AsConfig::Level_Height - 1;


   for (i = max_level_y; i >= min_level_y ; i--)
   {
      Current_Brick_Top_Y = AsConfig::Level_Y_Offset + i * AsConfig::Cell_Height;
      Current_Brick_Low_Y = Current_Brick_Top_Y + AsConfig::Brick_Height;
         

      for (j = min_level_x; j <= max_level_x; j++)
      {
         if (Current_Level[i][j] == 0)
            continue;

         // x*x + y*y = R*R

         Current_Brick_Left_X = AsConfig::Level_X_Offset + j * AsConfig::Cell_Width; //получили левую грань кирпича  
         Current_Brick_Right_X = Current_Brick_Left_X + AsConfig::Brick_Width;
         
         //reflection_pos - vertical and horizontal переменные принимают значения насколько углубился мячик в кирпич при верт/гориз ударе
         got_horizontal_hit = Check_Horizontal_Hit(next_x_pos, next_y_pos, j, i, ball, horizontal_reflection_pos);

         got_vertical_hit = Check_Vertical_Hit(next_x_pos, next_y_pos, j, i, ball, vertical_reflection_pos);

         if (got_horizontal_hit && got_vertical_hit) // Если отражение и от гориз и от верт
         {            
            if (On_Hit(j, i, ball, true))
            {
               if (vertical_reflection_pos < horizontal_reflection_pos) // Если верт позиция меньше или равна горизонт поз 
                  ball->Reflect(true); // То мы отражаем от верт
               else
                  ball->Reflect(false); // А иначе от горизонтальной
            }
            
            return true;   
         }
         else
            if (got_horizontal_hit) // Если отражение от гориз 
            {
               if (On_Hit(j, i, ball, false) )
               ball->Reflect(false);

               return true;
            }
            else 
               if (got_vertical_hit) // Если отражение от верт
               {
                  if (On_Hit(j, i, ball, true) )
                  ball->Reflect(true);
                  return true;
               }
      }
   }
   return false;
}
//------------------------------------------------------------------------------------------
void AsLevel::Act()
{
   Act_Objects( (AGraphics_Object **)&Active_Bricks, Active_Bricks_Count, AsConfig::Max_Active_Bricks_Count);
   Act_Objects( (AGraphics_Object **)&Falling_Letters, Falling_Letters_Count, AsConfig::Max_Falling_Letters_Count);

   if (Advertisement != 0)
      Advertisement->Act();
}
//------------------------------------------------------------------------------------------
void AsLevel::Clear(HDC hdc, RECT& paint_area)
{// 1. Стираем движущиеся обьекты
   Clear_Objects(hdc, paint_area, (AGraphics_Object **)&Falling_Letters, AsConfig::Max_Falling_Letters_Count);

   if (Advertisement != 0)
      Advertisement->Clear(hdc, paint_area);

   if (Need_To_Cancel_All)
   {
      Cancel_All_Activity();
      Need_To_Cancel_All = false;
   }

}
//------------------------------------------------------------------------------------------
void AsLevel::Draw(HDC hdc, RECT &paint_area)
{ // Вывод всех обьектов уровня

   int i, j;
   RECT intersection_rect, brick_rect;

   if (Advertisement != 0)
      Advertisement->Draw(hdc, paint_area);

   if (IntersectRect(&intersection_rect, &paint_area, &Level_Rect))
   {
      for (i = 0; i < AsConfig::Level_Height; i++)
         for (j = 0; j < AsConfig::Level_Width; j++)
         {
            brick_rect.left = (AsConfig::Level_X_Offset + j * AsConfig::Cell_Width) * AsConfig::Global_Scale;
            brick_rect.top = (AsConfig::Level_Y_Offset + i * AsConfig::Cell_Height) * AsConfig::Global_Scale;
            brick_rect.right = brick_rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
            brick_rect.bottom = brick_rect.top + AsConfig::Brick_Height * AsConfig::Global_Scale;

            if (IntersectRect(&intersection_rect, &paint_area, &brick_rect))
               Draw_Brick(hdc, brick_rect, j, i);


         }
      Draw_Objects(hdc, paint_area, (AGraphics_Object **) &Active_Bricks, AsConfig::Max_Active_Bricks_Count);
   }

   Draw_Objects(hdc, paint_area, (AGraphics_Object **) &Falling_Letters, AsConfig::Max_Falling_Letters_Count);



}
//------------------------------------------------------------------------------------------
bool AsLevel::Is_Finished()
{
   return false;
}
//------------------------------------------------------------------------------------------
void AsLevel::Init()
{


   Level_Rect.left = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
   Level_Rect.top = AsConfig::Level_Y_Offset * AsConfig::Global_Scale;
   Level_Rect.right = Level_Rect.left + AsConfig::Cell_Width * AsConfig::Level_Width * AsConfig::Global_Scale;
   Level_Rect.bottom = Level_Rect.top + AsConfig::Cell_Height * AsConfig::Level_Height * AsConfig::Global_Scale;

   memset(Current_Level, 0, sizeof(Current_Level));
      // 1. DST - Надо написать указатель типа void той области памяти, которая должна быть заполнена какими-то значениями. В моём случае указатель это имя массива.
      // 2. Указать значение, которое будет записано в каждой 4 байтах этого массива.
      // 3. Нужно указать количество байт в нашем массиве. С помощью сайзофф указываем.
   memset(Active_Bricks, 0, sizeof(Active_Bricks));
   memset(Falling_Letters, 0, sizeof(Falling_Letters));

}
//------------------------------------------------------------------------------------------
void AsLevel::Set_Current_Level(char level[AsConfig::Level_Height][AsConfig::Level_Width])
{
   int i, j;
   int index;
   EBrick_Type brick_type;

   memcpy(Current_Level, level, sizeof(Current_Level));
   // 1. DST - Надо написать указатель типа void той области памяти, которая должна быть заполнена какими-то значениями. В моём случае указатель это имя массива.
   // 2. SRC - источник.
   // 3. Количество байт, которые надо передать (снова применяем сайзофф)

   // 1. Считаем телепорты
   Teleport_Bricks_Count = 0;

   for (i = 0; i < AsConfig::Level_Height; i++)
   {
      for (j = 0; j < AsConfig::Level_Width; j++)
      {
         brick_type = (EBrick_Type)Current_Level[i][j]; // в каждой иттерации цикла берем элементы из индексов: i - номер строки, j - номер элемента в строке. Этот элемент сохранен как тип Кирпича    

         if (brick_type == EBT_Teleport)
            ++Teleport_Bricks_Count;
      }
   }

   delete[] Teleport_Bricks_Position;
   Teleport_Bricks_Position = 0;

   // 2. Сохраняем координаты телепортов
   if (Teleport_Bricks_Count != 0)
   {
      if (Teleport_Bricks_Count == 1)
         AsConfig::Throw(); // Телепортов должно быть больше 1;

      Teleport_Bricks_Position = new SPoint[Teleport_Bricks_Count];
      index = 0;

      for (i = 0; i < AsConfig::Level_Height; i++)
      {
         for (j = 0; j < AsConfig::Level_Width; j++)
         {
            brick_type = (EBrick_Type)Current_Level[i][j]; 

            if (brick_type == EBT_Teleport)
            {
               Teleport_Bricks_Position[index].X = j;
               Teleport_Bricks_Position[index].Y = i;
               ++index;
            }
         }
      }
   }
   Advertisement = new AAdvertisement(9, 6, 2, 3);
}
//------------------------------------------------------------------------------------------
bool AsLevel::Get_Next_Falling_Letter(int &index, AFalling_Letter **falling_letter)
{
   AFalling_Letter *current_letter;

   if (Falling_Letters_Count == 0)
      return false;

   if (index < 0 || index >= AsConfig::Max_Falling_Letters_Count)
      return false;

   while (index < AsConfig::Max_Falling_Letters_Count)
   {
      current_letter = Falling_Letters[index++];

      if (current_letter != 0)
      {
         *falling_letter = current_letter;
         return true;
      }
   }
   return false;


}
//------------------------------------------------------------------------------------------
void AsLevel::Stop()
{
   Need_To_Cancel_All = true;
}
//------------------------------------------------------------------------------------------
bool AsLevel::On_Hit(int brick_x, int brick_y, ABall *ball, bool vertical_hit)
{
   EBrick_Type brick_type;
   bool can_reflect = true;
   brick_type = (EBrick_Type)Current_Level[brick_y][brick_x];   


   if (brick_type == EBT_Parachute)
   {
      ball->Set_On_Parachute(brick_x, brick_y);
      Current_Level[brick_y][brick_x] = EBT_None;
   }
   else if (Add_Falling_Letter(brick_x, brick_y, brick_type) )
      Current_Level[brick_y][brick_x] = EBT_None;   // y - ряди, x - конкретний елемент цього ряда
   else
      can_reflect = Create_Active_Brick(brick_x, brick_y, brick_type, ball, vertical_hit);

   Redraw_Brick(brick_x, brick_y);

   return can_reflect;
}
//------------------------------------------------------------------------------------------
void AsLevel::Redraw_Brick(int brick_x, int brick_y)
{
   RECT brick_rect;

   brick_rect.left = (AsConfig::Level_X_Offset + brick_x * AsConfig::Cell_Width) * AsConfig::Global_Scale;
   brick_rect.top = (AsConfig::Level_Y_Offset + brick_y * AsConfig::Cell_Height) * AsConfig::Global_Scale;
   brick_rect.right = brick_rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
   brick_rect.bottom  = brick_rect.top + AsConfig::Brick_Height * AsConfig::Global_Scale;

   AsConfig::Invalidate_Rect(brick_rect);
}
//------------------------------------------------------------------------------------------
bool AsLevel::Add_Falling_Letter(int brick_x, int brick_y, EBrick_Type brick_type)
{ // Создаём падающую букву, если можем 
   int i;
   int letter_x, letter_y;
   ELetter_Type letter_type;
   AFalling_Letter* falling_letter;
   

   if (! (brick_type == EBT_Red || brick_type == EBT_Blue) )
   return false;


   if (AsConfig::Rand(AsConfig::Hits_Per_Letter) != 0) // если рандомное число из функции ранд (шанс 50/50) будет 1 (есть только 0 и 1) 
   return false;
    

   if (Falling_Letters_Count >= AsConfig::Max_Falling_Letters_Count) //если количество букв меньше максимального
     return false;

      for (i = 0; i < AsConfig::Max_Falling_Letters_Count; i++)
      {
         if (Falling_Letters[i] == 0)
         {
            letter_x = (brick_x * AsConfig::Cell_Width + AsConfig::Level_X_Offset) * AsConfig::Global_Scale;
            letter_y = (brick_y * AsConfig::Cell_Height + AsConfig::Level_Y_Offset) * AsConfig::Global_Scale;

            //letter_type = (ELetter_Type)AsConfig::Rand(ELT_Max - 1);
            //letter_type = AFalling_Letter::Get_Random_Letter_Type();
            //if (AsConfig::Rand(2) == 0)
               letter_type = ELT_L;
            //else
            //   letter_type = ELT_K;

            falling_letter = new AFalling_Letter(brick_type, letter_type, letter_x, letter_y); //создаём новую букву
            Falling_Letters[i] = falling_letter;
            ++Falling_Letters_Count;
            return true;
         }
      }

      return false;
}
//------------------------------------------------------------------------------------------
bool AsLevel::Create_Active_Brick(int brick_x, int brick_y, EBrick_Type brick_type, ABall *ball, bool vertical_hit)
{ // Создаём активный кирпич, если можем 
   // создал переменную типа указатель на обьект типа AActive_Brick_Red_Blue в которую присваиваю новый обьект класса AActive_Brick_Red_Blue
   // этот обьект динамический. в нашей памяти ему уделена особая область - называется куча

   AActive_Brick *active_brick = 0;
 

   if (Active_Bricks_Count >= AsConfig::Max_Active_Bricks_Count)
      return true; // Активных кирпичей слишком много!   

   //brick_type = (EBrick_Type)Current_Level[brick_y][brick_x];   
   //        // y - ряди, x - конкретний елемент цього ряда

   switch (brick_type)
   {
   case EBT_None:
      return true;

   case EBT_Red:
   case EBT_Blue:
      active_brick = new AActive_Brick_Red_Blue(brick_type, brick_x, brick_y); 
      Current_Level[brick_y][brick_x] = EBT_None;
      break;

   case EBT_Unbreakable:
      active_brick = new AActive_Brick_Unbreakable(brick_x, brick_y); 
      break;

   case EBT_Multihit_1:
      active_brick = new AActive_Brick_Multihit(brick_x, brick_y); 
      Current_Level[brick_y][brick_x] = EBT_None;
      break;

   case EBT_Multihit_2:
   case EBT_Multihit_3:
   case EBT_Multihit_4:
      Current_Level[brick_y][brick_x] = brick_type - 1;
      break;

   case EBT_Parachute:
      AsConfig::Throw(); // Для парашюта активный кирпич не создаётся   
      break;

   case EBT_Teleport:
      Add_Active_Brick_Teleport(brick_x, brick_y, ball, vertical_hit);
      return false;

   case EBT_Ad:
      active_brick = new AActive_Brick_Ad(brick_x, brick_y, Advertisement); 
      Current_Level[brick_y][brick_x] = EBT_Invisible;
      break;

   case EBT_Invisible:
      return true;

   default:
      AsConfig::Throw();
   }

   if (active_brick != 0)
      Add_New_Active_Brick(active_brick);

   return true;
}
//------------------------------------------------------------------------------------------
void AsLevel::Add_Active_Brick_Teleport(int brick_x, int brick_y, ABall* ball, bool vertical_hit)
{   
   int i;
   bool got_direction;
   EDirection_Type direction;
   int dest_brick_x, dest_brick_y;
   double pre_teleport_x_pos, pre_teleport_y_pos;
   double curr_ball_x_pos, curr_ball_y_pos;
   AActive_Brick_Teleport *source_teleport, *destination_teleport;


   ball->Get_Center(pre_teleport_x_pos, pre_teleport_y_pos); // Позиция мячика перед входом в телепорт

   destination_teleport = Select_Destination_Teleport(brick_x, brick_y);
   source_teleport = new AActive_Brick_Teleport(brick_x, brick_y, ball, destination_teleport);

   destination_teleport->Get_Level_Pos(dest_brick_x, dest_brick_y);


   // После создания телепорта мячик стал в его центре 
   ball->Get_Center(curr_ball_x_pos, curr_ball_y_pos);

   // Определяем направление откуда прилетел мячик 
   if (vertical_hit)
   {
      if (pre_teleport_y_pos < curr_ball_y_pos)
         direction = EDT_Down;
      else
         direction = EDT_Up;
   }
   else
   {
      if (pre_teleport_x_pos < curr_ball_x_pos)
         direction = EDT_Right;
      else
         direction = EDT_Left;

   }

   //Перебираем всё направление в поисках свободного
   got_direction = false;
   for (i = 0; i < 4; i++)
   {
      //Проверяем свободное направление для выхода мячика
      switch (direction) 
      {
      case EDT_Left:
         if (dest_brick_x > 0 && Current_Level[dest_brick_y][dest_brick_x - 1] == EBT_None)  // Если кирпич не крайний (-1) и при этом у нас в текущем уровне позиции brick_x brick_у ничего нету то только тогда можно испол такое направление
            got_direction = true;
            break;

      case EDT_Up:
         if (dest_brick_y > 0 && Current_Level[dest_brick_y - 1][dest_brick_x] == EBT_None)
            got_direction = true;
            break;

      case EDT_Right:
         if (dest_brick_x < AsConfig::Level_Width - 1 && Current_Level[dest_brick_y][dest_brick_x + 1] == EBT_None)
            got_direction = true;
            break;

      case EDT_Down:
         if (dest_brick_y < AsConfig::Level_Height - 1 && Current_Level[dest_brick_y + 1][dest_brick_x] == EBT_None)
            got_direction = true;
         break;

      default:
         AsConfig::Throw();
      }

      if (got_direction) 
         break;

      direction = (EDirection_Type)(direction - 1);   // Иначе надо изменить направление движения
      ball->Set_Direction(ball->Get_Direction() + M_PI_2);

      if (direction < 0) // Если мы вышли за диапазон
         direction = EDT_Down;
   }

   destination_teleport->Release_Direction = direction;

   Add_New_Active_Brick(source_teleport); //исходный телепорт
   Add_New_Active_Brick(destination_teleport); // телепорт назначения

}
//------------------------------------------------------------------------------------------
void AsLevel::Add_New_Active_Brick(AActive_Brick *active_brick)
{
   int i;
   // Добавляем новый активный кирпич на первое свободное место
   for (i = 0; i < AsConfig::Max_Active_Bricks_Count; i++)
   {
      if (Active_Bricks[i] == 0)
      {
         Active_Bricks[i] = active_brick;
         ++Active_Bricks_Count;
         break;
      }
   }

}
//------------------------------------------------------------------------------------------
AActive_Brick_Teleport *AsLevel::Select_Destination_Teleport(int source_x, int source_y)
{
   int dest_index;

   AActive_Brick_Teleport *destination_teleport;

   // Выбираем рандомный телепот
   if (Teleport_Bricks_Count < 2) // Если количество меньше 2 - Исключение 
   {
      AsConfig::Throw();
      return 0;
   }

   dest_index = AsConfig::Rand(Teleport_Bricks_Count); // Рандомный телепорт в диапазоне количества телепортов (2)

   if (Teleport_Bricks_Position[dest_index].X == source_x && Teleport_Bricks_Position[dest_index].Y == source_y)
   { // Если случайно выбрали текущий телепорт - переходим к следующему

      ++dest_index; // Чтоб выбрать след. увеличиваем индекс на единицу
      if (dest_index >= Teleport_Bricks_Count)     // Проверяем не вышли ли за границы массива 
         dest_index = 0; // Переходим на начало массива если вышли за его пределы
   }
         

   destination_teleport = new AActive_Brick_Teleport(Teleport_Bricks_Position[dest_index].X, Teleport_Bricks_Position[dest_index].Y, 0, 0); // другой телепорт
   
   return destination_teleport;
}
//------------------------------------------------------------------------------------------
bool AsLevel::Check_Vertical_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall *ball, double &reflection_pos)
{ 
   double direction = ball->Get_Direction();


   if (ball->Is_Moving_Up())
   {  // Проверяем попадание в нижнюю грань 
      if (Hit_Circle_On_Line(next_y_pos - Current_Brick_Low_Y, next_x_pos, Current_Brick_Left_X, Current_Brick_Right_X, ball->Radius, reflection_pos))
      {
         // Проверяем возможность отскока вниз
         if (level_y < AsConfig::Level_Height - 1 && Current_Level[level_y + 1][level_x] == 0)
            // Если в массиве для этого кирпича ничего нет (то есть 0) только тогда код выполнит отражение,а иначе мы запретим отражение 
            // Если этот Y меньше, чем предпоследний, то тогда будем смотреть на кипич ниже
            return true;
         else
            return false;
      }
   }
   else
   { // Проверяем попадание в верхнюю грань 

      if (Hit_Circle_On_Line(next_y_pos - Current_Brick_Top_Y, next_x_pos, Current_Brick_Left_X, Current_Brick_Right_X, ball->Radius, reflection_pos))
      {
         // Проверяем возможность отскока вверх
         // Проверяем, чтобы этот ряд кирпичей был не самый верхний (то есть больше 0)
         if (level_y > 0 && Current_Level[level_y - 1][level_x] == 0)
            return true;
         else
            return false;
      }
   }
   return false;
}
//------------------------------------------------------------------------------------------
bool AsLevel::Check_Horizontal_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall* ball, double &reflection_pos)
{
   double direction = ball->Get_Direction();

   // Проверяем попадание в левую грань 
   if (!ball->Is_Moving_Left())
   {
      if (Hit_Circle_On_Line(Current_Brick_Left_X - next_x_pos, next_y_pos, Current_Brick_Top_Y, Current_Brick_Low_Y, ball->Radius, reflection_pos))
      {
         // Проверяем возможность отскока влево 
         // При отскоке вправо мы будем проверять ячейку, которая меньше, значит Х - 1 (не нулевым) 
         // Значение по Х должно быть меньше чем максимальное значение для уровня 
         if (level_x > 0 && Current_Level[level_y][level_x - 1] == 0)
            return true;
         else
            return false;
      }
   }
   else
   {  // Проверяем попадание в правую грань 
      // Когда мячик будет подлетать справа к этой грани, нам нужно из меньше значение (то есть из правого края по х)
      // вычесть центр мячика (большее значение) тогда мы получим отрицательный х,
      //  который и будет соответствовать отрицательному занчению, если система координат находится в центре мячика 
      if (Hit_Circle_On_Line(Current_Brick_Right_X - next_x_pos, next_y_pos, Current_Brick_Top_Y, Current_Brick_Low_Y, ball->Radius, reflection_pos))
      {
         // Проверяем возможность отскока вправо
         // Так как он может отскочить вправо мы должны смотреть на его след позицию (левел х + 1) 
         if (level_x < AsConfig::Level_Width - 1 && Current_Level[level_y][level_x + 1] == 0)
            return true;
         else
            return false;
      }
   }
      return false;
}
//------------------------------------------------------------------------------------------
void AsLevel::Draw_Brick(HDC hdc, RECT &brick_rect, int level_x, int level_y)
{ //Вывод кирпича


   const AColor *color = 0;
   EBrick_Type brick_type = (EBrick_Type)Current_Level[level_y][level_x];

   switch (brick_type)
   {
   case EBT_None: 
      if (Advertisement != 0 && Advertisement->Has_Brick_At(level_x, level_y) )
         break;
         // else - No break!

   case EBT_Red: 
   case EBT_Blue:
      AActive_Brick_Red_Blue::Draw_In_Level(hdc, brick_rect, brick_type);
      break;

   case EBT_Unbreakable:
      AActive_Brick_Unbreakable::Draw_In_Level(hdc, brick_rect);
      break;

   case EBT_Multihit_1:
   case EBT_Multihit_2:
   case EBT_Multihit_3:
   case EBT_Multihit_4:
      AActive_Brick_Multihit::Draw_In_Level(hdc, brick_rect, brick_type);
      break;

   case EBT_Parachute:
      Draw_Parachute_In_Level(hdc, brick_rect);
      break;

   case EBT_Teleport:
      AActive_Brick_Teleport::Draw_In_Level(hdc, brick_rect);
      break;

   case EBT_Ad:
      AActive_Brick_Ad::Draw_In_Level(hdc, brick_rect);
      break;

   case EBT_Invisible:
      break;

   default:
      AsConfig::Throw();
   }

}
//------------------------------------------------------------------------------------------
void AsLevel::Draw_Parachute_In_Level(HDC hdc, RECT& brick_rect)
{

   Draw_Parachute_Part(hdc, brick_rect, 0, 4);
   Draw_Parachute_Part(hdc, brick_rect, 4, 6); // Через переменные задаём смещение относительно первой координаты и ширину 
   Draw_Parachute_Part(hdc, brick_rect, 10, 4);
}
//------------------------------------------------------------------------------------------
void AsLevel::Draw_Parachute_Part(HDC hdc, RECT& brick_rect, int offset, int width)
{
   const int scale = AsConfig::Global_Scale;
   RECT rect; 

   // Верхний сегмент
   rect.left = brick_rect.left + offset * scale + 1;
   rect.top = brick_rect.top + 1;
   rect.right = rect.left + width * scale + 1; 
   rect.bottom = rect.top + 3 * scale + 1;

   Parachute_Color.Select(hdc);
   AsConfig::Round_Rect(hdc, rect );

   // Нижний сегмент
   rect.top += 3 * scale;
   rect.bottom += 3 * scale;


   //Parachute_Color.Select(hdc);
   AsConfig::Round_Rect(hdc, rect );


}
//------------------------------------------------------------------------------------------
void AsLevel::Clear_Objects(HDC hdc, RECT &paint_area, AGraphics_Object**objects_array , int objects_max_count)
{
   int i; 

   for (i = 0; i < objects_max_count; i++)
   {
      if (objects_array[i] != 0)
         objects_array[i]->Clear(hdc, paint_area);
   }
}
//------------------------------------------------------------------------------------------
void AsLevel::Delete_Objects(AGraphics_Object**objects_array , int &objects_count , int objects_max_count)
{
   int i; 

   for (i = 0; i < objects_max_count; i++)
   {
      if (objects_array[i] != 0)
      {
         delete objects_array[i];
         objects_array[i] = 0;
      }
   }

   objects_count = 0;
}
//------------------------------------------------------------------------------------------
void AsLevel::Draw_Objects(HDC hdc, RECT &paint_area, AGraphics_Object**objects_array , int objects_max_count)
{
   int i; 

   for (i = 0; i < objects_max_count; i++)
   {
      if (objects_array[i] != 0)
         objects_array[i]->Draw(hdc, paint_area);
   }
}
//------------------------------------------------------------------------------------------
void AsLevel::Act_Objects(AGraphics_Object**objects_array , int &objects_count , const int objects_max_count)
{
   int i;

   for (i = 0; i < objects_max_count; i++)
   {
      if (objects_array[i] != 0)
      {
         objects_array[i]->Act();

         if (objects_array[i]->Is_Finished())
         {
            delete objects_array[i];
            objects_array[i] = 0;
            --objects_count;
         }
      }
   }
}
//------------------------------------------------------------------------------------------
void AsLevel::Cancel_All_Activity()
{
   Delete_Objects( (AGraphics_Object **)&Active_Bricks, Active_Bricks_Count, AsConfig::Max_Active_Bricks_Count);
   Delete_Objects( (AGraphics_Object **)&Falling_Letters, Falling_Letters_Count, AsConfig::Max_Falling_Letters_Count);
 
}
//------------------------------------------------------------------------------------------