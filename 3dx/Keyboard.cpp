#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
	return keystates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (keybuffer.size() > 0u)
	{
		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	else
	{
		return Keyboard::Event();
	}
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return keybuffer.empty();
}

void Keyboard::ClearKey() noexcept
{
	keybuffer = std::queue<Event>();
}

char Keyboard::ReadChar() noexcept
{
	if (charbuffer.size() > 0u)
	{
		unsigned char charcode = charbuffer.front();
		charbuffer.pop();
		return charcode;
	}
	else
	{
		return 0;
	}
}

bool Keyboard::CharIsEmpty() const noexcept
{
	return charbuffer.empty();
}

void Keyboard::ClearChar() noexcept
{
	charbuffer = std::queue<char>();
}

void Keyboard::Clear() noexcept
{
	ClearKey();
	ClearChar();
}

void Keyboard::EnableAutorep() noexcept
{
	autorepeatEnabled = true;
}

void Keyboard::DisableAutoRep() noexcept
{
	autorepeatEnabled = false;
}

bool Keyboard::AutoRepIsEnabled() const noexcept
{
	return autorepeatEnabled;
}

// private - Window use

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	keystates[keycode] = true;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	Trimbuffer(keybuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	keystates[keycode] = false;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	Trimbuffer(keybuffer);
}

void Keyboard::OnChar(char character) noexcept
{
	charbuffer.push(character);
	Trimbuffer(charbuffer);
}

void Keyboard::ClearState() noexcept
{
	keystates.reset();
}


template<typename T>
void Keyboard::Trimbuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > maxBufferSize) {
		buffer.pop();
	}
}
