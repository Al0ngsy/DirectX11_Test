#include "Mouse.h"
#include <Windows.h>

std::pair<int, int> Mouse::GetPos() const noexcept
{
	return std::pair<int, int>(x,y);
}

int Mouse::GetPosX() const noexcept
{
	return x;
}

int Mouse::GetPosY() const noexcept
{
	return y;
}

bool Mouse::IsInWindow() const noexcept
{
	return isInWindow;
}

bool Mouse::LeftIsPressed() const noexcept
{
	return leftIsPressed;
}

bool Mouse::RightIsPressed() const noexcept
{
	return rightIsPressed;
}

bool Mouse::MiddleIsPressed() const noexcept
{
	return middleIsPressed;
}

Mouse::Event Mouse::Read() noexcept
{
	if (buffer.size() > 0u)
	{
		Mouse::Event e = buffer.front();
		buffer.pop();
		return e;
	}
	else
	{
		return Mouse::Event();
	}
}

void Mouse::Clear() noexcept
{
	buffer = std::queue<Event>();
}

void Mouse::OnMouseMove(int xNew, int yNew) noexcept
{
	x = xNew;
	y = yNew;

	buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
	Trimbuffer();
}

void Mouse::OnMouseLeave() noexcept
{
	isInWindow = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
	Trimbuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	isInWindow = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
	Trimbuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
	leftIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
	Trimbuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
	rightIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
	Trimbuffer();
}

void Mouse::OnMiddlePressed(int x, int y) noexcept
{
	middleIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::MPress, *this));
	Trimbuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
	leftIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
	Trimbuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
	rightIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
	Trimbuffer();
}

void Mouse::OnMiddleReleased(int x, int y) noexcept
{
	middleIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::MPress, *this));
	Trimbuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	Trimbuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	Trimbuffer();
}

void Mouse::Trimbuffer() noexcept
{
	if (buffer.size() > maxBufferSize)
	{
		buffer.pop();
	}
}

void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
	wheelDeltaCarry += delta;
	// generate Event for every threshold of 120
	while (wheelDeltaCarry >= WHEEL_DELTA)
	{
		wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}
	while (wheelDeltaCarry <= -WHEEL_DELTA)
	{
		wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
	
}

