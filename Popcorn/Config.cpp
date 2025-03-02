#include "Config.h"

//AColor

AColor::AColor()
   : R(0), G(0), B(0), Pen(0), Brush(0)
{
}

AColor::AColor(unsigned char r, unsigned char g, unsigned char b)
   : R(r), G(g), B(b), Pen(0), Brush(0)
{
   Pen = CreatePen(PS_SOLID, 0, RGB(r, g, b) );
   Brush = CreateSolidBrush(RGB(r, g, b) );
}

AColor::AColor(const AColor &color, int pen_size)
   : R(color.R), G(color.G), B(color.B), Pen(0), Brush(0)
{
   Pen = CreatePen(PS_SOLID, pen_size, color.Get_RGB() );
}


AColor::AColor(unsigned char r, unsigned char g, unsigned char b, int pen_size)
   : R(r), G(g), B(b), Pen(0), Brush(0)
{
   Pen = CreatePen(PS_SOLID, pen_size, RGB(r, g, b) );
}

AColor::AColor(const AColor &pen_color, const AColor &brush_color, int pen_size)
   : R(0), G(0), B(0), Pen(0), Brush(0)
{
   Pen = CreatePen(PS_SOLID, pen_size, pen_color.Get_RGB() );
   Brush = CreateSolidBrush(brush_color.Get_RGB() );
}

int AColor::Get_RGB() const
{
   return RGB (R, G, B);
}

void AColor::Select(HDC hdc) const
{
   SelectObject(hdc, Pen);
   SelectObject(hdc, Brush);
}

void AColor::Select_Pen(HDC hdc) const
{
   SelectObject(hdc, Pen);
}

HBRUSH AColor::Get_Brush() const
{
   return Brush;
}





// AsConfig
bool AsConfig::Level_Has_Floor = true;
int  AsConfig::Current_Timer_Tick = 0;

const AColor AsConfig::BG_Color(15, 63, 31);
const AColor AsConfig::Red_Color(255, 85, 85);
const AColor AsConfig::Blue_Color(85, 255, 255);
const AColor AsConfig::White_Color(255, 255, 255);
const AColor AsConfig::Letter_Color(AsConfig::White_Color, AsConfig::Global_Scale); // Сделали жирный белый цвет для букв и цифр 
// Назначили просто белый цвет, и ширину указали как Глобальный масштаб (то есть 3 пикселя)
const AColor AsConfig::Unbreakable_Blue_Highlight(AsConfig::Blue_Color, AsConfig::Global_Scale);
const AColor AsConfig::Unbreakable_Red_Highlight(AsConfig::Red_Color, 3 * AsConfig::Global_Scale); 
const AColor AsConfig::Teleport_Portal_Color(AsConfig::Blue_Color, AsConfig::BG_Color, AsConfig::Global_Scale);
const AColor AsConfig::Advertisement_Blue_Table(0, 159, 159, AsConfig::Global_Scale); 
const AColor AsConfig::Advertisement_Red_Table(AsConfig::Red_Color, 2 * AsConfig::Global_Scale); 

HWND AsConfig::Hwnd;
const double AsConfig::D_Global_Scale = (double)Global_Scale;
const double AsConfig::Moving_Step_Size = 1.0 / AsConfig::Global_Scale; // делим 1 пиксель игровой на наш глобальный масштаб (который в 3 раза больше) и получаем треть пиксель, 
// именно настолько и происходит шаг перемещение на который мы смещаем наш шарик

const double AsConfig::Start_Ball_Y_Pos = 184.0;
const double AsConfig::Ball_Acceleration = 1.001;
const double AsConfig::Normal_Ball_Speed = 3.0;
const double AsConfig::Min_Ball_Angle = M_PI / 8.0;


//------------------------------------------------------------------------------------------
int AsConfig::Rand(int range)
{ // Вычисляем псевдослучайное число в диапазоне [0 , ... range -1]
   return rand() * range / RAND_MAX;
}

void AsConfig::Round_Rect(HDC hdc, RECT& rect, int corner_radius)
{
   int radius = corner_radius * AsConfig::Global_Scale;

   RoundRect(hdc, rect.left , rect.top,  rect.right - 1, rect.bottom - 1, radius, radius);

}

void AsConfig::Invalidate_Rect(RECT& rect)
{
   InvalidateRect(Hwnd, &rect, FALSE);
}

void AsConfig::Throw()
{
   throw 13;
}