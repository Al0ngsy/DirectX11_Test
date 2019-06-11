#pragma once
#include <queue>

class Mouse
{
	friend class Window; // let window access the private stuff of Mouse
public:
	class Event
	{
	public:
		enum class Type
		{
			LPress, RPress, MPress,
			LRelease, RRelease, MRelease,
			WheelUp, WheelDown,
			Move, Enter, Leave,
			Invalid
		};
	private:
		Type type;
		bool leftIsPressed, rightIsPressed, middleIsPressed;
		int x, y;
	public:
		Event() noexcept
			:	type(Type::Invalid),
				leftIsPressed(false),
				rightIsPressed(false),
				middleIsPressed(false),
				x(0), y(0)
		{}
		Event(Type type, const Mouse& parent) noexcept
			:	type(type),
				leftIsPressed(parent.leftIsPressed),
				rightIsPressed(parent.rightIsPressed),
				middleIsPressed(parent.middleIsPressed),
				x(parent.x), y(parent.y)
		{}
		Type GetType() const noexcept
		{
			return type;
		}
		std::pair<int, int> GetPos() const noexcept
		{
			return { x, y };
		}
		int GetPosX() const noexcept
		{
			return x;
		}
		int GetPosY() const noexcept
		{
			return y;
		}
		bool LeftIsPress() const noexcept
		{
			return leftIsPressed;
		}
		bool RightIsPress() const noexcept
		{
			return rightIsPressed;
		}
		bool MiddleIsPress() const noexcept
		{
			return middleIsPressed;
		}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
	};
public:
	Mouse() = default; // explicitly defaulted, calls A::A()
	Mouse(const Mouse&) = delete;
	Mouse& operator = (const Mouse&) = delete;
	// mouse event
	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	bool IsInWindow() const noexcept;
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	bool MiddleIsPressed() const noexcept;
	Mouse::Event Read() noexcept;
	bool IsEmpty() const noexcept
	{
		return buffer.empty();
	}
	void Clear() noexcept;
	// Window Use:	
private:
	void OnMouseMove(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnMiddlePressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnMiddleReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void Trimbuffer() noexcept;	// reduce buffer to the max size if exceed
	void OnWheelDelta(int x, int y, int delta) noexcept;
private:
	static constexpr unsigned int maxBufferSize = 16u;	// max size of the buffer
	int x, y;
	int wheelDeltaCarry = 0;
	bool leftIsPressed, rightIsPressed, middleIsPressed = false;
	bool isInWindow = false;
	std::queue<Event> buffer;
};