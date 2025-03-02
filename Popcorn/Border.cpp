#include "Border.h"

// Config

//------------------------------------------------------------------------------------------
// AsBorder
AsBorder::AsBorder()
{
   Floor_Rect.left = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
   Floor_Rect.top = AsConfig::Floor_Y_Pos * AsConfig::Global_Scale;
   Floor_Rect.right = (AsConfig::Max_X_Pos - 1) * AsConfig::Global_Scale; 
   Floor_Rect.bottom = AsConfig::Max_Y_Pos * AsConfig::Global_Scale;
}
//------------------------------------------------------------------------------------------
void AsBorder::Redraw_Floor()
{
   AsConfig::Invalidate_Rect(Floor_Rect);
}
//------------------------------------------------------------------------------------------
bool AsBorder::Check_Hit(double next_x_pos, double next_y_pos, ABall *ball )
{   //  Корректируем позицию при отражении от рамки    

   bool got_hit = false;
   // Левый край
   if (next_x_pos - ball->Radius < AsConfig::Border_X_Offset) //Проверяем значение центра - радиус
   {
      //next_x_pos = AsConfig::Level_X_Offset - (next_x_pos - AsConfig::Level_X_Offset);
      got_hit = true;
      ball->Reflect(false);
   }
   // Верхний край
   if (next_y_pos - ball->Radius < AsConfig::Border_Y_Offset) //При проверке столкновение с верхней гранью надо проверить не сталкивается ли она с пределом 
   {
      //next_y_pos = AsConfig::Border_Y_Offset - (next_y_pos - AsConfig::Border_Y_Offset);
      got_hit = true;
      ball->Reflect(true);
   }
   // Правый край
   if (next_x_pos + ball->Radius > AsConfig::Max_X_Pos + 1) //Проверяем столкновения с правыми краями
   {  
      got_hit = true;
      ball->Reflect(false);
   }

   // 4. Нижний край
   if (AsConfig::Level_Has_Floor && next_y_pos + ball->Radius > AsConfig::Floor_Y_Pos) //Проверяем столкновения с нижними краями
   {
      got_hit = true;
      ball->Reflect(true);
   }
   // Чтобы шарик смог улететь ниже пола, проверяем его max_y_pos ниже видимой границы  
      if (next_y_pos + ball->Radius > (double)AsConfig::Max_Y_Pos + ball->Radius * 4.0) 
         ball->Set_State(EBS_Lost);
   

   return got_hit;
}
//------------------------------------------------------------------------------------------
void AsBorder::Act()
{ // Заглушка. Не используется, так как мячик сам ничего не делает (не анимируется)

/*   if (! AsConfig::Level_Has_Floor)
      return; */ 

}
//------------------------------------------------------------------------------------------
void AsBorder::Clear(HDC hdc, RECT& paint_area)
{
   RECT intersection_rect;

   if (! AsConfig::Level_Has_Floor)
         return;

   if (! IntersectRect(&intersection_rect, &paint_area, &Floor_Rect))
      return;

   AsConfig::BG_Color.Select(hdc);

   Rectangle(hdc, Floor_Rect.left, Floor_Rect.top, Floor_Rect.right - 1, Floor_Rect.bottom - 1);
}
//------------------------------------------------------------------------------------------
void AsBorder::Draw(HDC hdc, RECT &paint_area)
{ // Рисуется рамка уровня
   int i;


   // 1.Рисуем рамку слева
   for(i = 0; i < 50; i++)
      Draw_Element(hdc, paint_area, 2, 1 + i * 4, false);

   // 2.Рисуем рамку справа
   for(i = 0; i < 50; i++)
      Draw_Element(hdc, paint_area, AsConfig::Max_X_Pos + 1, 1 + i * 4, false);

   // 3.Рисуем рамку сверху
   for(i = 0; i < 50; i++)
      Draw_Element(hdc, paint_area, 3 + i * 4, 0, true);

   // 4. Пол (если есть)
   if (AsConfig::Level_Has_Floor)
      Draw_Floor(hdc, paint_area);
}

//------------------------------------------------------------------------------------------
bool AsBorder::Is_Finished()
{
   return false;
}
//------------------------------------------------------------------------------------------
void AsBorder::Draw_Element(HDC hdc, RECT &paint_area, int x, int y, bool top_border)
{ //рисуем элемент рамки

   RECT intersection_rect, rect;

   // Прямоугольник, который ограничивает элемент который мы нарисуем
   rect.left = x * AsConfig::Global_Scale;
   rect.top = y * AsConfig::Global_Scale;
   rect.right = (x + 4) * AsConfig::Global_Scale;
   rect.bottom = (y + 4) * AsConfig::Global_Scale;

   if (! IntersectRect(&intersection_rect, &paint_area, &rect) )
                  return;

  //Выводится основная линия
   AsConfig::Blue_Color.Select(hdc);

   if (top_border)
      Rectangle(hdc, x * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale - 1, (y + 4) * AsConfig::Global_Scale - 1);
   else
      Rectangle(hdc, (x + 1) * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale - 1, (y + 4) * AsConfig::Global_Scale - 1);

   //Белая кайма
   AsConfig::White_Color.Select(hdc);

   if (top_border)
      Rectangle(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale - 1, (y + 1) * AsConfig::Global_Scale - 1);
   else
      Rectangle(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale , (x + 1) * AsConfig::Global_Scale - 1, (y + 4) * AsConfig::Global_Scale - 1);


   //Перфорация

   AsConfig::BG_Color.Select(hdc);

   if (top_border)
      Rectangle(hdc, (x + 2) * AsConfig::Global_Scale, (y + 2) * AsConfig::Global_Scale, (x + 3) * AsConfig::Global_Scale - 1, (y + 3) * AsConfig::Global_Scale - 1);
   else
      Rectangle(hdc, (x + 2) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 3) * AsConfig::Global_Scale - 1, (y + 2) * AsConfig::Global_Scale - 1);
}
//------------------------------------------------------------------------------------------
void AsBorder::Draw_Floor(HDC hdc, RECT& paint_area)
{
   int i, strokes_count;
   int x_pos, y_pos;
   int line_len = 4 * AsConfig::Global_Scale;
   int gap_len = 2 * AsConfig::Global_Scale;
   int stroke_len = line_len + gap_len;
   RECT intersection_rect;

   if (! IntersectRect(&intersection_rect, &paint_area, &Floor_Rect))
         return;
      
   strokes_count = (Floor_Rect.right - Floor_Rect.left + AsConfig::Global_Scale) / stroke_len;
   x_pos = Floor_Rect.left + 1;
   y_pos = Floor_Rect.top + (Floor_Rect.bottom - Floor_Rect.top) / 2;

   AsConfig::Letter_Color.Select(hdc);

   for (i = 0; i < strokes_count; i++)
   {
      MoveToEx(hdc, x_pos, y_pos, 0);
      LineTo(hdc, x_pos + line_len, y_pos);

      x_pos += stroke_len;
   }
   

}
//------------------------------------------------------------------------------------------


