#include "Active_Brick.h"


// Active_Break

void AActive_Brick::Get_Level_Pos(int &dest_brick_x, int &dest_brick_y)
{
   dest_brick_x = Level_X;
   dest_brick_y = Level_Y;
}

AActive_Brick::~AActive_Brick()
{
}

AActive_Brick::AActive_Brick(EBrick_Type brick_type, int level_x, int level_y)
   : Brick_Type(brick_type), Level_X(level_x), Level_Y(level_y), Brick_Rect{}
{
   Brick_Rect.left = (AsConfig::Level_X_Offset + level_x * AsConfig::Cell_Width) * AsConfig::Global_Scale;
   Brick_Rect.top = (AsConfig::Level_Y_Offset + level_y * AsConfig::Cell_Height) * AsConfig::Global_Scale;
   Brick_Rect.right = Brick_Rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
   Brick_Rect.bottom = Brick_Rect.top + AsConfig::Brick_Height * AsConfig::Global_Scale;


}
void AActive_Brick::Clear(HDC hdc, RECT& paint_area)
{
}


double AActive_Brick::Get_Brick_X_Pos(bool of_center)
{
   double pos = (double)(AsConfig::Level_X_Offset + Level_X * AsConfig::Cell_Width);

   if (of_center)
      pos += (double)AsConfig::Brick_Width / 2.0;

   return pos;
}

double AActive_Brick::Get_Brick_Y_Pos(bool of_center)
{
   double pos = (double)(AsConfig::Level_Y_Offset + Level_Y * AsConfig::Cell_Height);

   if (of_center)
      pos += (double)AsConfig::Brick_Height / 2.0;

   return pos;
}


// AActive_Brick_Red_Blue
AColor AActive_Brick_Red_Blue::Fading_Red_Brick_Colors[Max_Fade_Step];
AColor AActive_Brick_Red_Blue::Fading_Blue_Brick_Colors[Max_Fade_Step];

AActive_Brick_Red_Blue::~AActive_Brick_Red_Blue()
{
}


AActive_Brick_Red_Blue::AActive_Brick_Red_Blue(EBrick_Type brick_type, int level_x, int level_y)
: AActive_Brick(brick_type, level_x, level_y), Fade_Step(0)
{
   if (! (brick_type == EBT_Red || brick_type == EBT_Blue) )
      AsConfig::Throw();
}



void AActive_Brick_Red_Blue::Act()
{
   if (Fade_Step < Max_Fade_Step - 1)
   {
      ++Fade_Step;
      AsConfig::Invalidate_Rect(Brick_Rect);
   }
}


void AActive_Brick_Red_Blue::Draw(HDC hdc, RECT &paint_area)
{
   AColor *color = 0;

   switch (Brick_Type)
   {
   case EBT_Red:
      color = &Fading_Red_Brick_Colors[Fade_Step];

      break;
   case EBT_Blue:
      color = &Fading_Blue_Brick_Colors[Fade_Step];

      break;
   }

   if (color != 0)
      color->Select(hdc);

   AsConfig::Round_Rect(hdc, Brick_Rect);
   
}



bool AActive_Brick_Red_Blue::Is_Finished()
{
   if (Fade_Step >= Max_Fade_Step - 1)
      return true;
   else
      return false;
}


void AActive_Brick_Red_Blue::Setup_Colors()
{
    int i;

    for (i = 0; i < Max_Fade_Step; i++)
    {
       Get_Fading_Color(AsConfig::Red_Color, i, Fading_Red_Brick_Colors[i]);
       Get_Fading_Color(AsConfig::Blue_Color, i, Fading_Blue_Brick_Colors[i]);
    }

}


void AActive_Brick_Red_Blue::Draw_In_Level(HDC hdc, RECT &brick_rect, EBrick_Type brick_type)
{ // Вывод неактивного кирпича на уровне
   const AColor *color = 0;

   switch (brick_type)
   {
   case EBT_None: 
      color = &AsConfig::BG_Color;
      break;

   case EBT_Red: 
      color = &AsConfig::Red_Color;
      break;

   case EBT_Blue:
      color = &AsConfig::Blue_Color;
      break;

   default:
      AsConfig::Throw();   
   }

   if (color !=0)
      color->Select(hdc);

   AsConfig::Round_Rect(hdc, brick_rect);
}

unsigned char AActive_Brick_Red_Blue::Get_Fading_Channel(unsigned char color, unsigned char bg_color, int step)
{
   return color - step * (color - bg_color) / (Max_Fade_Step - 1);
}

void AActive_Brick_Red_Blue::Get_Fading_Color(const AColor &origin_color, int step, AColor &result_color)
{
      unsigned char r, g, b;

      r = Get_Fading_Channel(origin_color.R, AsConfig::BG_Color.R, step);
      g = Get_Fading_Channel(origin_color.G, AsConfig::BG_Color.G, step);
      b = Get_Fading_Channel(origin_color.B,  AsConfig::BG_Color.B,step);

      result_color = AColor(r, g, b);

}








// AActive_Brick_Unbreakable


AActive_Brick_Unbreakable::~AActive_Brick_Unbreakable()
{
   DeleteObject(Region);
}


AActive_Brick_Unbreakable::AActive_Brick_Unbreakable(int level_x, int level_y)
   : AActive_Brick(EBT_Unbreakable, level_x, level_y), Animation_Step(0), Region(0)
{
   Region = CreateRoundRectRgn(Brick_Rect.left, Brick_Rect.top, Brick_Rect.right + 1, Brick_Rect.bottom + 1, 2 * AsConfig::Global_Scale - 1, 2 * AsConfig::Global_Scale - 1);


}


void AActive_Brick_Unbreakable::Act()
{
   if (Animation_Step <= Max_Animation_Step)
   {
      ++Animation_Step;
      AsConfig::Invalidate_Rect(Brick_Rect);
   }
}


void AActive_Brick_Unbreakable::Draw(HDC hdc, RECT &paint_area)
{
   int offset;
   const int scale = AsConfig::Global_Scale;


   Draw_In_Level(hdc, Brick_Rect);


   SelectClipRgn(hdc, Region); // Весь последующий код внизу теперь будет описан только в рамках описанного прямоугольника с помощью данной функции

   offset = 2 * Animation_Step * scale - AsConfig::Brick_Width * scale;

   AsConfig::Unbreakable_Blue_Highlight.Select_Pen(hdc);
   MoveToEx(hdc, Brick_Rect.left + 4 * scale + offset, Brick_Rect.bottom + scale, 0);
   LineTo(hdc, Brick_Rect.left + 13 * scale + offset - 1, Brick_Rect.top - 1 * scale);

   AsConfig::Unbreakable_Red_Highlight.Select_Pen(hdc);
   MoveToEx(hdc, Brick_Rect.left + 6 * scale + offset, Brick_Rect.bottom + scale, 0);
   LineTo(hdc, Brick_Rect.left + 15 * scale + offset - 1, Brick_Rect.top - 1 * scale);

   SelectClipRgn(hdc, 0); // Передали вместо региона 0 тем самым сообщив что обрезка закончена

}



bool AActive_Brick_Unbreakable::Is_Finished()
{
   if (Animation_Step >= Max_Animation_Step)
      return true;
   else
      return false;
}

void AActive_Brick_Unbreakable::Draw_In_Level(HDC hdc, RECT &brick_rect)
{ // Вывод неактивного кирпича на уровне
   AsConfig::White_Color.Select(hdc);
   AsConfig::Round_Rect(hdc, brick_rect);
}












// AActive_Brick_Multihit
//AColor AActive_Brick_Multihit::Blue_Highlight(AsConfig::Blue_Color, AsConfig::Global_Scale);
//AColor AActive_Brick_Multihit::Red_Highlight(AsConfig::Red_Color, 3 * AsConfig::Global_Scale);


AActive_Brick_Multihit::~AActive_Brick_Multihit()
{
}


AActive_Brick_Multihit::AActive_Brick_Multihit(int level_x, int level_y)
   : AActive_Brick(EBT_Multihit_1, level_x, level_y), Rotation_Step(0)
{
}


void AActive_Brick_Multihit::Act()
{
   if (Rotation_Step <= Max_Rotation_Step)
   {
      ++Rotation_Step;
      AsConfig::Invalidate_Rect(Brick_Rect);
   }
}


void AActive_Brick_Multihit::Draw(HDC hdc, RECT &paint_area)
{
   int step;
   const int scale = AsConfig::Global_Scale;
   double rotation_angle, x_ratio;
   RECT zero_rect;
   XFORM xform, old_xform;

   // 1. Очищаем фон
   AsConfig::BG_Color.Select(hdc);
   AsConfig::Round_Rect(hdc, Brick_Rect);



   // Поворот на 30 градусов
   // 2. Настраиваем матрицу "переворота" буквы
   step = Rotation_Step % Steps_Per_Turn;
   rotation_angle = M_PI_4 / 2.0 * (double)step;
   x_ratio = cos(rotation_angle);

   xform.eM11 = (float)x_ratio;
   xform.eM12 = 0.0f;
   xform.eM21 = 0.0f;
   xform.eM22 = 1.0f;
   xform.eDx = (float)Brick_Rect.left + (float)(1.0 - x_ratio) * (float)(AsConfig::Brick_Width * AsConfig::Global_Scale) / 2.0f;
   xform.eDy = (float)Brick_Rect.top;
   GetWorldTransform(hdc, &old_xform); //Сохранил старое преобразование
   SetWorldTransform(hdc, &xform); //Сделал новое преобразование (поворот на 30 градусов)

   // 3. Рисуем "100"
   AsConfig::Letter_Color.Select_Pen(hdc);
   MoveToEx(hdc, 0 + 1 * scale + 1, 0 + 3 * scale, 0);
   LineTo(hdc, 0 + 3 * scale + 1, 0 + 1 * scale);
   LineTo(hdc, 0 + 3 * scale + 1, 0 + 6 * scale - 1);

   zero_rect.left = 0 + 5 * scale + 1;
   zero_rect.top = 0 + 1 * scale;
   zero_rect.right = zero_rect.left + 3 * scale + 1;
   zero_rect.bottom = zero_rect.top + 5 * scale;
   AsConfig::Round_Rect(hdc, zero_rect);

   zero_rect.left += 5 * scale;
   zero_rect.right += 5 * scale; 
   AsConfig::Round_Rect(hdc, zero_rect);

   SetWorldTransform(hdc, &old_xform); //Сделал новое преобразование (поворот на 30 градусов)

}



bool AActive_Brick_Multihit::Is_Finished()
{
   if (Rotation_Step >= Max_Rotation_Step)
      return true;
   else
      return false;
}

void AActive_Brick_Multihit::Draw_In_Level(HDC hdc, RECT &brick_rect, EBrick_Type brick_type)
{ // Вывод неактивного кирпича на уровне

   const int scale = AsConfig::Global_Scale;

   // 1. Рисуем фон
   AsConfig::White_Color.Select(hdc);
   AsConfig::Round_Rect(hdc, brick_rect);

   AsConfig::Red_Color.Select(hdc);
   Rectangle(hdc, brick_rect.left + scale, brick_rect.top + scale, brick_rect.right - scale - 1, brick_rect.bottom - scale - 1);

   // 2. Рисуем внутренние прямоугольники
   switch(brick_type)
   {
      case EBT_Multihit_1:
         Draw_Stage(hdc, brick_rect, 2, 10);
         break;

      case EBT_Multihit_2:
         Draw_Stage(hdc, brick_rect, 2, 4);
         Draw_Stage(hdc, brick_rect, 8, 4);
         break;

      case EBT_Multihit_3:
         Draw_Stage(hdc, brick_rect, 2, 2);
         Draw_Stage(hdc, brick_rect, 6, 2);
         Draw_Stage(hdc, brick_rect, 10, 2);
         break;

      case EBT_Multihit_4:
         Draw_Stage(hdc, brick_rect, 2, 2);
         Draw_Stage(hdc, brick_rect, 5, 2);
         Draw_Stage(hdc, brick_rect, 8, 2);
         Draw_Stage(hdc, brick_rect, 11, 2);
         break;

      default:
         AsConfig::Throw(); 
   }




}


void AActive_Brick_Multihit::Draw_Stage(HDC hdc, RECT& brick_rect, int x, int width)
{ // Рисуем внутренний прямоугольник

   const int scale = AsConfig::Global_Scale;
   RECT stage_rect;

   stage_rect.left = brick_rect.left + x * scale;
   stage_rect.top = brick_rect.top + 2 * scale;
   stage_rect.right = stage_rect.left + width * scale;
   stage_rect.bottom = stage_rect.top + 3 * scale;

   AsConfig::BG_Color.Select(hdc);
   Rectangle(hdc, stage_rect.left + scale, stage_rect.top + scale, stage_rect.right + scale - 1, stage_rect.bottom + scale - 1);

   AsConfig::Blue_Color.Select(hdc);
   Rectangle(hdc, stage_rect.left, stage_rect.top, stage_rect.right - 1, stage_rect.bottom - 1);
}





// AActive_Brick_Teleport

AActive_Brick_Teleport::~AActive_Brick_Teleport()
{
}


AActive_Brick_Teleport::AActive_Brick_Teleport(int level_x, int level_y, ABall *ball, AActive_Brick_Teleport *destination_teleport)
   : AActive_Brick(EBT_Teleport, level_x, level_y), Teleport_State(ETS_Starting), Animation_Step(0), Ball(0), Destination_Teleport(destination_teleport)
{
   Set_Ball(ball);
}


void AActive_Brick_Teleport::Act()
{
   double ball_x = 0, ball_y = 0;
   double direction;

   if (Animation_Step <= Max_Animation_Step)
   {
      ++Animation_Step;
      AsConfig::Invalidate_Rect(Brick_Rect);   
   }
   else
   {
      switch (Teleport_State)
      {
      case ETS_Starting:
         Animation_Step = 0;
         Teleport_State = ETS_Finishing;

         if (Destination_Teleport != 0)
         {
            Destination_Teleport->Set_Ball(Ball);
            Ball = 0;
         }
         break;

      case ETS_Finishing:
         Teleport_State = ETS_Done;

         if (Ball != 0)
         {
            switch (Release_Direction)
            { // Выпускаем мячик из телепорта в зависимости куда он попал, выпускаем в том направлении
            case EDT_Left:
               ball_x = Get_Brick_X_Pos(false) - ABall::Radius; // Середина кирпича - если тру, если фолс - грань 
               ball_y = Get_Brick_Y_Pos(true); 
                  break;

            case EDT_Up:
               ball_x = Get_Brick_X_Pos(true);
               ball_y = Get_Brick_Y_Pos(false) - ABall::Radius;
               break;

            case EDT_Right:
               ball_x = Get_Brick_X_Pos(false) + (double)AsConfig::Brick_Width + ABall::Radius; // Середина кирпича - если тру, если фолс - грань 
               ball_y = Get_Brick_Y_Pos(true); 
               break;
            case EDT_Down:  
               ball_x = Get_Brick_X_Pos(true); // Середина кирпича - если тру, если фолс - грань 
               ball_y = Get_Brick_Y_Pos(false) + (double)AsConfig::Brick_Height + ABall::Radius; 
               break;

            default:
               AsConfig::Throw();
            }
            direction = Ball->Get_Direction();
            Ball->Set_State(EBS_Normal, ball_x, ball_y);
            Ball->Set_Direction(direction);

            Ball = 0; // Отмечваем мячик как отсутсвующий в телепорте
            AsConfig::Invalidate_Rect(Brick_Rect);         }
         break;
      }
   }
}


void AActive_Brick_Teleport::Draw(HDC hdc, RECT &paint_area)
{

   int step;


   switch (Teleport_State)
   {
   case ETS_Starting:
      step = Animation_Step;
      break;


   case ETS_Finishing:
      step = Max_Animation_Step - Animation_Step;
      break;

   default:
      step = 0;
   }

   Draw_In_Level(hdc, Brick_Rect, step);

   if (Ball != 0)
      Ball->Draw_Teleporting(hdc, step);
}

bool AActive_Brick_Teleport::Is_Finished()
{
   if (Teleport_State == ETS_Done)
      return true;
   else
      return false;
}



void AActive_Brick_Teleport::Draw_In_Level(HDC hdc, RECT& brick_rect, int step)
{
   const int scale = AsConfig::Global_Scale;
   int top_y = brick_rect.top + step / 2 + 1;
   int low_y = brick_rect.top + 6 * scale - step / 2 + 1;

   // Фон
   AsConfig::Red_Color.Select(hdc);
   AsConfig::Round_Rect(hdc, brick_rect);

   // Портал 
   AsConfig::Teleport_Portal_Color.Select(hdc);
   Ellipse(hdc, brick_rect.left + 3 * scale + 1, top_y, brick_rect.left + 11 * scale + 1, low_y);
}


void AActive_Brick_Teleport::Set_Ball(ABall *ball)
{
   double ball_x, ball_y;

   // Ставим мячик в центр кирпича
   ball_x = Get_Brick_X_Pos(true);         
   ball_y = Get_Brick_Y_Pos(true);

   if(ball != 0)
      ball->Set_State(EBS_Teleporting, ball_x, ball_y);

   Ball = ball;
}







// AAdvertisement
AAdvertisement::~AAdvertisement()
{   
   int i, j;
   HRGN region;

   for (i = 0; i < Height; i++)
      for (j = 0; j < Width; j++)
      {
         region = Brick_Regions[i * Width + j];

         if (region != 0)
            DeleteObject(region);
      }

   delete[] Brick_Regions;
}


AAdvertisement::AAdvertisement(int level_x, int level_y, int width, int height)
   : Level_X(level_x), Level_Y(level_y), Width(width), Height(height), Empty_Region(0),  Ball_X(0), Ball_Y(0), 
   Ball_Width(Ball_Size * AsConfig::Global_Scale), Ball_Height(Ball_Size * AsConfig::Global_Scale), Ball_Y_Offset(0), 
   Falling_Speed(0.0), Acceleration_Step(0.2), Brick_Regions(0)
{
   //int i, j;

   const int scale = AsConfig::Global_Scale;


   Empty_Region = CreateRectRgn(0, 0, 0, 0);

   Brick_Regions = new HRGN[Width * Height];
   memset(Brick_Regions, 0, sizeof(HRGN) * Width * Height);

   Ad_Rect.left = (AsConfig::Level_X_Offset + Level_X * AsConfig::Cell_Width) * scale;
   Ad_Rect.top = (AsConfig::Level_Y_Offset + Level_Y * AsConfig::Cell_Height) * scale;
   Ad_Rect.right = Ad_Rect.left + ( (Width - 1) * AsConfig::Cell_Width + AsConfig::Brick_Width) * scale;
   Ad_Rect.bottom = Ad_Rect.top + ( (Height - 1) * AsConfig::Cell_Height + AsConfig::Brick_Height) * scale;

   Ball_X = Ad_Rect.left + 9 * scale + Ball_Width / 2 + 1;
   Ball_Y = Ad_Rect.top + 2 * scale + Ball_Height / 2;

   //for (i = 0; i < Height; i++)
   //   for (j = 0; j < Width; j++)
   //       Show_Under_Brick(Level_X + j, Level_Y + i);


}

void AAdvertisement::Act()
{
   //InvalidateRect(AsConfig::Hwnd, &Ad_Rect, FALSE);

   int i, j; 
   int cell_width = AsConfig::Cell_Width * AsConfig::Global_Scale;
   int cell_height = AsConfig::Cell_Height * AsConfig::Global_Scale;
   RECT rect;


   // 1. Заказываем перерисовку фрагментов, над которыми пропали кирпичи
   for (i = 0; i < Height; i++)
      for (j = 0; j < Width; j++)
         if (Brick_Regions[i * Width + j] != 0)
         {
            rect.left = Ad_Rect.left + j * cell_width;
            rect.top = Ad_Rect.top + i * cell_height;
            rect.right = rect.left + cell_width;
            rect.bottom = rect.top + cell_height;

            AsConfig::Invalidate_Rect(rect);
         }

   // 2. Смещаем шарик 
   Falling_Speed += Acceleration_Step;
   Ball_Y_Offset = High_Ball_Threshold - (int)(Falling_Speed * Falling_Speed);
   // формула У = Т2
   // Для того, чтобы смещать шарик вверх и вниз мы создаём переодичность. Мы ограничили движение сверхк верхним порогом, 
   // а снизу нижним порогом значения, то тогда мы написали код, который проверяет достижение этих порогов. 
   // Как только мы достигним вверхнего порога (при этом Y будет уменьшаться) мы поменяем знак в нашем сдвиге (Ball_Y_Shift) и шарик будет двигаться в другом направлении. 
   // А когда снова достигнет уже нижнего то соответственно будет наоборот и таким образом будет движения

   // Если смещение меньше либо равно нижнему порогу + высоте деформации = 
   // если шарик ниже двух игровых пикселей до нижнего порога, то тогда в коэф деформации задаем меньше 1(начинает плющить), а иначе 1(все норм). 
   // Если -1 значит он подлетает вниз и плющит, если просто летит то тогда просто 1 
   if (Ball_Y_Offset <= Low_Ball_Threshold + Deformation_Height)
      Deformation_Ratio = (double)(Ball_Y_Offset - Low_Ball_Threshold) / (double)Deformation_Height;
   else
      Deformation_Ratio = 1.0;


   if (Ball_Y_Offset > High_Ball_Threshold || Ball_Y_Offset < Low_Ball_Threshold )
      Acceleration_Step = -Acceleration_Step;
}

void AAdvertisement::Clear(HDC hdc, RECT& paint_area)
{
}

void AAdvertisement::Draw(HDC hdc, RECT& paint_area)
{ 
   int i, j;
   int x, y;
   int ball_width, ball_height;
   int shadow_width, shadow_height;
   int deformation;
   const int scale = AsConfig::Global_Scale;
   HRGN region;
   RECT intersection_rect;
   POINT table_points[4] = 
   {
      { Ad_Rect.left + 1, Ad_Rect.top + 15 * scale },
      { Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 10 * scale },
      { Ad_Rect.left + 30 * scale + 1, Ad_Rect.top + 15 * scale }, 
      { Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 20 * scale }
   };

   if (! IntersectRect(&intersection_rect, &paint_area, &Ad_Rect) )
      return;
   SelectClipRgn(hdc, Empty_Region);

   for (i = 0; i < Height; i++)
      for (j = 0; j < Width; j++)
      {
         region = Brick_Regions[i * Width + j];
         if (region != 0)
            ExtSelectClipRgn(hdc, region, RGN_OR); // Выбираем регион 
      }


   // 4. Рамка 
   // 4.1 Тонкая синяя рамка с скругленными краями
   AsConfig::BG_Color.Select(hdc);
   AsConfig::Blue_Color.Select_Pen(hdc);
   AsConfig::Round_Rect(hdc, Ad_Rect);


   // 1. Стол
   // 1.1 Белая поверхность
   AsConfig::White_Color.Select(hdc);
   Polygon(hdc, table_points, 4);
   // 2) имя массива для рисовки 
   // 3) количество точек в этом массиве

   //FloodFill(hdc, Ad_Rect.left + 15 * scale, Ad_Rect.top + 15 * scale, AsConfig::White_Color.Get_RGB() );


   //FloodFill(hdc, Ad_Rect.left + 15 * scale, Ad_Rect.top + 15 * scale, AsConfig::White_Color.Get_RGB() );

   // 2. Тень под шариком
   // 2.1 Синий эллипс 8х6, пока шарик полностью над "столом"
   // 2.2 Уезжает вниз, когда шарик в верхней точке
   // 2.3 Увеличивается, когда шарик плющится

   AsConfig::Blue_Color.Select(hdc);


   shadow_width = Ball_Width - 4 * scale;
   shadow_height = 4 * scale;

   deformation = (int)( (1.0 - Deformation_Ratio) * (double)scale * 2.0);

   ball_width = shadow_width + deformation;
   ball_height = shadow_height - deformation;

   x = Ball_X - ball_width / 2;
   y = Ball_Y - ball_height / 2 + Ball_Y_Offset / 6 + 9 * scale;

   Ellipse(hdc, x, y, x + ball_width, y + ball_height);





   // 3.2 Синяя кайма толщиной в 1 игровой пиксель
   AsConfig::Advertisement_Blue_Table.Select(hdc);

   MoveToEx(hdc, Ad_Rect.left + scale - 1, Ad_Rect.top + 15 * scale, 0);
   LineTo(hdc, Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 10 * scale);
   LineTo(hdc, Ad_Rect.left + 30 * scale, Ad_Rect.top + 15 * scale);
   LineTo(hdc, Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 20 * scale);
   LineTo(hdc, Ad_Rect.left + scale - 1, Ad_Rect.top + 15 * scale);

   // 3.3 Красный борт толщиной в 1 игровой пиксель
   AsConfig::Advertisement_Red_Table.Select(hdc);

   MoveToEx(hdc, Ad_Rect.left + scale, Ad_Rect.top + 16 * scale, 0);
   LineTo(hdc, Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 21 * scale);
   LineTo(hdc, Ad_Rect.left + 30 * scale - 1, Ad_Rect.top + 16 * scale);

   // 4. Шарик
   // 4.1 Красный эллипс 12х12
   // 4.2 Блик сверху
   // 4.3 Летает вверх/вниз (по затухающей територии)
   // 4.4 Сплющивается внизу до 16х9

   // Все эти локальные переменные с учетом деформации
   ball_width = Ball_Width + deformation;
   ball_height = Ball_Height - deformation;

   x = Ball_X - ball_width / 2;
   y = Ball_Y - ball_height / 2 - Ball_Y_Offset;

   AsConfig::Red_Color.Select(hdc);
   Ellipse(hdc, x, y, x + ball_width, y + ball_height);

   // Блик
   AsConfig::Letter_Color.Select(hdc);
   Arc(hdc, x + scale + 1, y + scale + 1, x + ball_width - scale, y + ball_height - scale, x + 4 * scale, y + scale, x + scale, y + 3 * scale); // Координаты ограничивающего прямоугольника  

   SelectClipRgn(hdc, 0);
}

bool AAdvertisement::Is_Finished()
{
   return false; // Реклама не заканчивается никогда!
}


void AAdvertisement::Show_Under_Brick(int level_x, int level_y)
{
   int x, y;
   int cell_width = AsConfig::Cell_Width * AsConfig::Global_Scale;
   int cell_height = AsConfig::Cell_Height * AsConfig::Global_Scale;
   RECT rect;
   x = level_x - Level_X;
   y = level_y - Level_Y;


   if (x < 0 || x >= Width)
      AsConfig::Throw();

   if (y < 0 || y >= Height)
      AsConfig::Throw();

   rect.left = Ad_Rect.left + x * cell_width;
   rect.top = Ad_Rect.top + y * cell_height;
   rect.right = rect.left + cell_width;
   rect.bottom = rect.top + cell_height;

   Brick_Regions[y * Width + x] = CreateRectRgnIndirect(&rect);
}


bool AAdvertisement::Has_Brick_At(int level_x, int level_y)
{
   if (level_x >= Level_X && level_x <= Level_X + Width)
      if (level_y >= Level_Y && level_y <= Level_Y + Height)
         return true;

   return false;
}







// AActive_Brick_Ad
AActive_Brick_Ad::~AActive_Brick_Ad()
{
   //DeleteObject(Region);
}


AActive_Brick_Ad::AActive_Brick_Ad(int level_x, int level_y, AAdvertisement *advertisement)
   : AActive_Brick(EBT_Unbreakable, level_x, level_y), Advertisement(advertisement)
{
   if (Advertisement != 0)
      Advertisement->Show_Under_Brick(Level_X, Level_Y);


}


void AActive_Brick_Ad::Act()
{
   AsConfig::Invalidate_Rect(Brick_Rect);  
}


void AActive_Brick_Ad::Draw(HDC hdc, RECT &paint_area)
{
}



bool AActive_Brick_Ad::Is_Finished()
{
   return false;
}

void AActive_Brick_Ad::Draw_In_Level(HDC hdc, RECT &brick_rect)
{ // Вывод неактивного кирпича на уровне
   int i;
   int x = brick_rect.left;
   int y = brick_rect.top;
   const int scale = AsConfig::Global_Scale;
   int size = (Circle_Size - 1) * scale - 1;

   // 1. Стираем предыдущие изображения
   AsConfig::BG_Color.Select(hdc);

   Rectangle(hdc, brick_rect.left, brick_rect.top, brick_rect.right + scale - 1, brick_rect.bottom + scale - 1);

   for (i = 0; i < 2; i++)
   {
      AsConfig::Red_Color.Select(hdc);
      Ellipse(hdc, x, y, x + 7 * scale - 1, brick_rect.bottom - 1);

      // Рисуем блик на шарике
      AsConfig::White_Color.Select(hdc);

      Arc(hdc, x + scale, y + scale, x + size, y + size, x + 2 * scale, y + scale, x + scale, y + 3 * scale);

      x += 8 * scale;
   }
}

