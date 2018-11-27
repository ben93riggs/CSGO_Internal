#include "CRender.h"
#include "Globals.h"

#undef DrawText

void CRender::SetupFonts()
{
	MATSURFACE->SetFontGlyphSet(font = MATSURFACE->CreateFont(), XorStr("Verdana"), 12, FW_BOLD, NULL, NULL, FontFlags::FONTFLAG_DROPSHADOW);
	MATSURFACE->SetFontGlyphSet(fontMedium = MATSURFACE->CreateFont(), XorStr("Verdana"), 18, FW_BOLD, NULL, NULL, FontFlags::FONTFLAG_DROPSHADOW);
	MATSURFACE->SetFontGlyphSet(fontLarge = MATSURFACE->CreateFont(), XorStr("Verdana"), 22, FW_BOLD, NULL, NULL, FontFlags::FONTFLAG_DROPSHADOW);

	g::screenx = 0;
	g::screeny = 0;
	g::centerx = 0;
	g::centery = 0;
}

void CRender::DrawText(int X, int Y, Color Color, bool Center, const char* _Input, ...)
{
	if (!font)
		MATSURFACE->SetFontGlyphSet(font = MATSURFACE->CreateFont(), XorStr("Verdana"), 12, FW_BOLD, NULL, NULL, FontFlags::FONTFLAG_DROPSHADOW); //outline + antialias

	/* set up buffer */
	char Buffer[1024] = {'\0'};

	/* set up varargs*/
	va_list Args;

	va_start(Args, _Input);
	vsprintf_s(Buffer, _Input, Args);
	va_end(Args);

	size_t Size = strlen(Buffer) + 1;

	/* set up widebuffer*/
	wchar_t* WideBuffer = new wchar_t[Size];

	/* char -> wchar */
	mbstowcs_s(nullptr, WideBuffer, Size, Buffer, Size - 1);

	/* check center */
	int Width = 0, Height = 0;

	if (Center)
	{
		MATSURFACE->GetTextSize(font, WideBuffer, Width, Height);
	}

	/* call and draw*/
	MATSURFACE->DrawSetTextColor(Color);
	MATSURFACE->DrawSetTextFont(font);
	MATSURFACE->DrawSetTextPos(X - (Width * .5), Y);
	MATSURFACE->DrawPrintText(WideBuffer, wcslen(WideBuffer), FontDrawType::FONT_DRAW_DEFAULT);
}

void CRender::DrawTextMedium(int X, int Y, Color Color, bool Center, const char* _Input, ...)
{
	if (!fontMedium)
		MATSURFACE->SetFontGlyphSet(fontMedium = MATSURFACE->CreateFont(), XorStr("Consolas"), 18, FW_HEAVY, NULL, NULL, FontFlags::FONTFLAG_OUTLINE, 12, 18);

	/* set up buffer */
	char Buffer[1024] = { '\0' };

	/* set up varargs*/
	va_list Args;

	va_start(Args, _Input);
	vsprintf_s(Buffer, _Input, Args);
	va_end(Args);

	size_t Size = strlen(Buffer) + 1;

	/* set up widebuffer*/
	wchar_t* WideBuffer = new wchar_t[Size];

	/* char -> wchar */
	mbstowcs_s(nullptr, WideBuffer, Size, Buffer, Size - 1);

	/* check center */
	int Width = 0, Height = 0;

	if (Center)
		MATSURFACE->GetTextSize(fontMedium, WideBuffer, Width, Height);

	/* call and draw*/
	MATSURFACE->DrawSetTextColor(Color);
	MATSURFACE->DrawSetTextFont(fontMedium);
	MATSURFACE->DrawSetTextPos(X - (Width * .5), Y);
	MATSURFACE->DrawPrintText(WideBuffer, wcslen(WideBuffer), FontDrawType::FONT_DRAW_DEFAULT);
}

void CRender::DrawTextLarge(int X, int Y, Color Color, bool Center, const char* _Input, ...)
{
	if (!fontLarge)
		MATSURFACE->SetFontGlyphSet(fontLarge = MATSURFACE->CreateFont(), XorStr("Consolas"), fontLarge, FW_HEAVY, NULL, NULL, FontFlags::FONTFLAG_OUTLINE);

	/* set up buffer */
	char Buffer[1024] = {'\0'};

	/* set up varargs*/
	va_list Args;

	va_start(Args, _Input);
	vsprintf_s(Buffer, _Input, Args);
	va_end(Args);

	size_t Size = strlen(Buffer) + 1;

	/* set up widebuffer*/
	wchar_t* WideBuffer = new wchar_t[Size];

	/* char -> wchar */
	mbstowcs_s(nullptr, WideBuffer, Size, Buffer, Size - 1);

	/* check center */
	int Width = 0, Height = 0;
	if (Center)
		MATSURFACE->GetTextSize(fontLarge, WideBuffer, Width, Height);

	/* call and draw*/
	MATSURFACE->DrawSetTextColor(Color);
	MATSURFACE->DrawSetTextFont(fontLarge);
	MATSURFACE->DrawSetTextPos(X - (Width * .5), Y);
	MATSURFACE->DrawPrintText(WideBuffer, wcslen(WideBuffer), FontDrawType::FONT_DRAW_DEFAULT);
}

void CRender::DrawIconTest(unsigned font, int X, int Y, Color color, bool Center, const char* _Input, ...) const
{
	//54: HFont:0x000000a1, IconsSmall, IconsSmallClientScheme-no, font:Counter-Strike, tall:20
	//55: HFont:0x000000a2, IconsSmall, IconsSmallClientScheme-p, font:Counter-Strike, tall:31

	/* set up buffer */
	char Buffer[1024] = {'\0'};

	/* set up varargs*/
	va_list Args;

	va_start(Args, _Input);
	vsprintf_s(Buffer, _Input, Args);
	va_end(Args);

	size_t Size = strlen(Buffer) + 1;

	/* set up widebuffer*/
	wchar_t* WideBuffer = new wchar_t[Size];

	/* char -> wchar */
	mbstowcs_s(nullptr, WideBuffer, Size, Buffer, Size - 1);

	/* check center */
	int Width = 0, Height = 0;

	if (Center)
		MATSURFACE->GetTextSize(font, WideBuffer, Width, Height);

	/* call and draw*/
	MATSURFACE->DrawSetTextColor(color);
	MATSURFACE->DrawSetTextFont(font);
	MATSURFACE->DrawSetTextPos(X - (Width * .5), Y);
	MATSURFACE->DrawPrintText(WideBuffer, wcslen(WideBuffer), FontDrawType::FONT_DRAW_DEFAULT);
}

void CRender::DrawIcon(int X, int Y, Color Color, bool Center, const char* _Input, ...) const
{
	//50: HFont:0x0000009d, Icons, IconsClientScheme-no, font:Counter-Strike, tall:28
	//52: HFont:0x0000009f, IconsBig, IconsBigClientScheme - no, font : Counter - Strike, tall : 47

	//54: HFont:0x000000a1, IconsSmall, IconsSmallClientScheme-no, font:Counter-Strike, tall:20
	//55: HFont:0x000000a2, IconsSmall, IconsSmallClientScheme-p, font:Counter-Strike, tall:60

	/* set up buffer */
	char Buffer[1024] = {'\0'};

	/* set up varargs*/
	va_list Args;

	va_start(Args, _Input);
	vsprintf_s(Buffer, _Input, Args);
	va_end(Args);

	size_t Size = strlen(Buffer) + 1;

	/* set up widebuffer*/
	wchar_t* WideBuffer = new wchar_t[Size];

	/* char -> wchar */
	mbstowcs_s(nullptr, WideBuffer, Size, Buffer, Size - 1);

	/* check center */
	int Width = 0, Height = 0;

	if (Center)
		MATSURFACE->GetTextSize(fontIcon, WideBuffer, Width, Height);

	/* call and draw*/
	MATSURFACE->DrawSetTextColor(Color);
	MATSURFACE->DrawSetTextFont(fontIcon);
	MATSURFACE->DrawSetTextPos(X - (Width * .5), Y);
	MATSURFACE->DrawPrintText(WideBuffer, wcslen(WideBuffer), FontDrawType::FONT_DRAW_DEFAULT);
}

int CRender::GetStringWidth(HFont font, const char* msg, ...) const
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int iWidth, iHeight;

	MATSURFACE->GetTextSize(font, wbuf, iWidth, iHeight);

	return iWidth;
}

void CRender::DrawCornerESP(float left, float right, float top, float bottom, Color& color)
{
	int w = (right - left) * .10f;
	int h = (bottom - top) * .10f;

	DrawLineWithOutline(left, bottom, left + w, bottom, color);
	DrawLineWithOutline(left, bottom, left, bottom - h, color);


	DrawLineWithOutline(right, bottom, right - w, bottom, color);
	DrawLineWithOutline(right, bottom, right, bottom - h, color);

	DrawLineWithOutline(left, top, left + w, top, color);
	DrawLineWithOutline(left, top, left, top + h, color);

	DrawLineWithOutline(right, top, right - w, top, color);
	DrawLineWithOutline(right, top, right, top + h, color);
}

void CRender::DrawRect(int X, int Y, int W, int H, const Color& color)
{
	MATSURFACE->DrawSetColor(color);
	MATSURFACE->DrawOutlinedRect(X, Y, X + W, Y + H);
}

void CRender::DrawFilledRect(int X, int Y, int W, int H, const Color& color)
{
	MATSURFACE->DrawSetColor(color);
	MATSURFACE->DrawFilledRect(X, Y, X + W, Y + H);
}

void CRender::DrawCircle(int x, int y, int r, int _res, const Color& color)
{
	float res = min((float)r * M_PI, _res);

	MATSURFACE->DrawSetColor(color);
	MATSURFACE->DrawOutlinedCircle(x, y, r, (int)res);
}

void CRender::DrawFilledCircle(Vector2D position, float _points, float radius, Color color)
{
	float points = min(radius * M_PI, _points);

	std::vector<Vertex_t> vertices;
	float step = (float)M_PI * 2.0f / points;

	for (float a = 0; a < (M_PI * 2.0f); a += step)
		vertices.push_back(Vertex_t(Vector2D(radius * cosf(a) + position.x, radius * sinf(a) + position.y)));

	DrawFilledPolygon((int)points, vertices.data(), color);
}

void CRender::DrawLine(int x0, int y0, int x1, int y1, const Color& color)
{
	

	MATSURFACE->DrawSetColor(color);
	MATSURFACE->DrawLine(x0, y0, x1, y1);
}

void CRender::DrawLine(Vector& start, Vector& end, const Color& color)
{
	

	MATSURFACE->DrawSetColor(color);
	MATSURFACE->DrawLine(start.x, start.y, end.x, end.y);
}

void CRender::DrawLineWithOutline(Vector start, Vector end, const Color& color)
{
	DrawLine(start, end, color);
	return;


	MATSURFACE->DrawSetColor(color);
	MATSURFACE->DrawLine(start.x, start.y, end.x, end.y);
	MATSURFACE->DrawSetColor(Color::Black());
	MATSURFACE->DrawLine(start.x + 1, start.y + 1, end.x + 1, end.y + 1);
}

void CRender::DrawLineWithOutline(int x0, int y0, int x1, int y1, Color& color)
{
	DrawLine(x0, y0, x1, y1, color);
	return;


	MATSURFACE->DrawSetColor(color);
	MATSURFACE->DrawLine(x0, y0, x1, y1);

	Color outlinecolor = Color::Black();
	outlinecolor.SetA(color.GetA());

	MATSURFACE->DrawSetColor(outlinecolor);
	MATSURFACE->DrawLine(x0 + 1, y0 + 1, x1 + 1, y1 + 1);
}

void CRender::FillRGBA(int x, int y, int w, int h, const Color& color)
{
	MATSURFACE->DrawSetColor(color);
	MATSURFACE->DrawFilledRect(x, y, x + w, y + h);
}

void CRender::DrawPolygon(int count, Vertex_t* Vertexs, const Color& color)
{
	MATSURFACE->DrawSetColor(color);
	MATSURFACE->DrawTexturedPolyLine(Vertexs, count);
}

void CRender::DrawFilledPolygon(int count, Vertex_t* Vertexs, const Color& color)
{
	static int Texture = MATSURFACE->CreateNewTextureID(true);
	unsigned char buffer[4] = {255, 255, 255, 255};

	MATSURFACE->DrawSetTextureRGBA(Texture, buffer, 1, 1);
	MATSURFACE->DrawSetColor(color);
	MATSURFACE->DrawSetTexture(Texture);

	MATSURFACE->DrawTexturedPolygon(count, Vertexs);
}
