#pragma once
#include <queue>
#include <bitset>

class Keyboard
{
	friend class Window; // let window access the private stuff of keyboard
public:
	class Event
	{
	public:
		enum class Type
		{
			Press, Release, Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event() noexcept
			: type( Type::Invalid), code(0u)
		{}
		Event(Type type, unsigned char code) noexcept
			: type(type), code(code)
		{}
		bool IsPress() const noexcept
		{
			return type == Type::Press;
		}
		bool IsRelease() const noexcept
		{
			return type == Type::Release;
		}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
	};
public:
	Keyboard() = default; // explicitly defaulted, calls A::A()
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator = (const Keyboard&) = delete;
	// key event
	bool KeyIsPressed(unsigned char keycode) const noexcept;
	Event ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void ClearKey() noexcept;
	// char event
	char ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void ClearChar() noexcept;
	// key, char event
	void Clear() noexcept;
	// autorepeat constrol
	void EnableAutorep() noexcept;
	void DisableAutoRep() noexcept;
	bool AutoRepIsEnabled() const noexcept;
// Window Use:	
private:
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T> static void Trimbuffer(std::queue<T>& buffer) noexcept;	// reduce buffer to the max size if exceed
private:
	static constexpr unsigned int nKeys = 265u;	// maximum amount of virtual key code see WM_KEYDOWN range 01 ~ FE eg 1Byte
	static constexpr unsigned int maxBufferSize = 16u;	// max size of the buffer
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keystates;
	std::queue<Event> keybuffer;
	std::queue<char> charbuffer;
};
