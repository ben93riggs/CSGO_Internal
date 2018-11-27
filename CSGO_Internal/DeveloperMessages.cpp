#include "DeveloperMessages.h"
#include "SourceEngine/SDK.hpp"
#include "CRender.h"
#include "Utils.hpp"

void DevMsg::OutputMessage(Color const &color, const char* buf, ...)
{
	using con_col_fn = void(__stdcall*)(Color const &, const char*, ...);
	static auto con_col = reinterpret_cast<con_col_fn>(GetProcAddress(GetModuleHandleA(XorStr("tier0.dll")), XorStr("?ConColorMsg@@YAXABVColor@@PBDZZ")));
	con_col(Color::MediumPurple(), XorStr("[MSG] "));
	con_col(color, buf);
	con_col(color, XorStr("\n"));
	m_Messages.insert(m_Messages.begin(), MessageData(GLOBALS->curtime, _strdup(buf), color));
}

void DevMsg::Draw() {
	int y = 5;
	for(auto i = 0; i < m_Messages.size(); i++) {
		auto &message = m_Messages[i];
		float delta_time = GLOBALS->curtime - message.m_Time;
		if (delta_time == 0)
			continue;
		if(i > 7 && delta_time < max_time - fade_time) {
			if (delta_time < fade_time) {
				m_Messages.erase(m_Messages.begin() + i);
				continue;
			}
			if (delta_time < fade_time * (1 / fade_time)) {
				delta_time = (max_time - fade_time) + (fade_time * (1 / fade_time) - delta_time);
				message.m_Time = GLOBALS->curtime - delta_time;
			}
		}

		int alpha = 255;
		int x = 5;
		int local_y = y;
		std::string text = XorStr("[MSG] ") + std::string(message.m_Message);
		IntRect text_size_whole;
		MATSURFACE->GetTextSize(CRender::get().font, Utils::MultiByteToWideString(text.c_str()).c_str(), text_size_whole.w, text_size_whole.h);
		if(delta_time > (max_time - fade_time)) {
			if (delta_time >= max_time) {
				m_Messages.erase(m_Messages.begin() + i);
				continue;
			}

			delta_time -= (max_time - fade_time);
			delta_time *= 1/fade_time;
			alpha -= delta_time * 255;
			local_y += 10 * delta_time;
		}
		else if(delta_time < fade_time * 2) {
			if(delta_time <= fade_time) {
				delta_time *= 1 / fade_time;
				y += text_size_whole.h * delta_time;
				continue;
			}
			delta_time -= fade_time;
			delta_time *= 1 / fade_time;
			x -= 5 * (1 - delta_time);
			alpha *= delta_time;
		}
		CRender::get().DrawText(x, local_y, Color::MediumPurple(alpha), false, XorStr("[MSG] "));
		IntRect text_size;
		MATSURFACE->GetTextSize(CRender::get().font, Utils::MultiByteToWideString(XorStr("[MSG] ")).c_str(), text_size.w, text_size.h);
		Color color = message.m_Color;
		color.SetA(alpha);
		CRender::get().DrawText(x + text_size.w, local_y, color, false, _strdup(message.m_Message));
		y += text_size_whole.h + 3;
	}
}