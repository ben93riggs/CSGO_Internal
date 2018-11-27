#pragma once
#include "Singleton.h"
#include <Windows.h>

struct Vertex_t;
class Vector;
class Vector2D;
class Color;

typedef ULONG HFont;

#undef DrawText

class CRender : public singleton<CRender>
{

	void DrawIconTest(unsigned font, int X, int Y, Color color, bool Center, const char* _Input, ...) const;
	int GetStringWidth(HFont font, const char* msg, ...) const;
public:
	HFont font = 0;
	HFont fontMedium = 0;
	HFont fontLarge = 0;
	HFont fontIcon = 0xA1;

	void SetupFonts();
	void DrawText(int X, int Y, Color Color, bool Center, const char* _Input, ...);
	void DrawTextLarge(int X, int Y, Color Color, bool Center, const char* _Input, ...);
	void DrawTextMedium(int X, int Y, Color Color, bool Center, const char* _Input, ...);
	void DrawIcon(int X, int Y, Color Color, bool Center, const char* _Input, ...) const;
	
	static void DrawLine(int x0, int y0, int x1, int y1, const Color& color);
	static void DrawLine(Vector& start, Vector& end, const Color& color);
	static void DrawLineWithOutline(Vector start, Vector end, const Color& color);
	static void DrawLineWithOutline(int x0, int y0, int x1, int y1, Color& color);

	static void DrawRect(int X, int Y, int W, int H, const Color& color);
	static void DrawFilledRect(int X, int Y, int W, int H, const Color& color);
	static void FillRGBA(int x, int y, int w, int h, const Color& color);
	static void DrawCornerESP(float left, float right, float top, float bottom, Color& color);

	static void DrawCircle(int x, int y, int r, int res, const Color& color);
	static void DrawFilledCircle(Vector2D position, float points, float radius, Color color);

	static void DrawPolygon(int count, Vertex_t* Vertexs, const Color& color);
	static void DrawFilledPolygon(int count, Vertex_t* Vertexs, const Color& color);
};
