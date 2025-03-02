#pragma once


#include "Active_Brick.h"

enum ELetter_Type
{
   ELT_O, // "Отмена"
   ELT_I, // "Инверсия"
   ELT_C, // "Скорость"
   ELT_M, // "Монстры"
   ELT_G, // "Жизнь"
   ELT_K, // "Клей"
   ELT_W, // "Шире"

   ELT_P, // "Пол"
   ELT_L, // "Лазер"
   ELT_T, // "Три"

   ELT_Plus, // "Переход на следующий уровень"
   ELT_Max 
};



enum EFalling_Letter_State
{
   EFLS_Normal,
   EFLS_Finalizing, // Начинаем удалять обьект 
   EFLS_Finished // Когда обьект можно удалять

};

class AFalling_Letter: public AGraphics_Object
{
public:
   AFalling_Letter(EBrick_Type brick_type, ELetter_Type letter_type, int x, int y);

   virtual void Act();
   virtual void Clear(HDC hdc, RECT &paint_area);
   virtual void Draw(HDC hdc, RECT &paint_area);
   virtual  bool Is_Finished();

   void Get_Letter_Cell(RECT &rect);
   void Finalize();
   void Test_Draw_All_Steps(HDC hdc);

   static void Init();
   static ELetter_Type Get_Random_Letter_Type();

   const EBrick_Type Brick_Type;
   const ELetter_Type Letter_Type;

private:

   void Set_Brick_Letter_Colors(bool is_switch_color, const AColor **front_color, const AColor **back_color);
   void Draw_Brick_Letter(HDC hdc);
   void Draw_C(HDC hdc);
   void Draw_Line(HDC hdc, int x_1, int y_1, int x_2, int y_2);
   void Draw_Line_To(HDC hdc, int x, int y);


   EFalling_Letter_State Falling_Letter_State;
   int X, Y;
   int Rotation_Step;
   int Next_Rotation_Tick;
   RECT Letter_Cell, Prev_Letter_Cell;

   static const int Ticks_Per_Step = 4;
   static const int Brick_Half_Height = AsConfig::Brick_Height * AsConfig::Global_Scale / 2;
   static const int Max_Rotation_Step = 16;
   static int All_Letters_Popularity;
   static int Letters_Popularity[ELT_Max]; // Вес (вероятность) каждой буквы: первые 7 букв - 7, 3 буквы - 3 и 1 буква - 1
};

