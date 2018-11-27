#pragma once
#include "Singleton.h"
#include <string>
#include <vector>
#include "SourceEngine/Color.hpp"

class DevMsg : public singleton<DevMsg>
{
	float max_time = 6;
	float fade_time = 0.5f;
	struct MessageData {
		MessageData(float m_Time, char* m_Message, Color m_Color) : m_Time(m_Time), m_Message(m_Message), m_Color(m_Color) {}
		float m_Time;
		char *m_Message;
		Color m_Color;
	};
	std::vector<MessageData> m_Messages;
public:
	void OutputMessage(Color const &color, const char* buf, ...);
	void Draw();
};
