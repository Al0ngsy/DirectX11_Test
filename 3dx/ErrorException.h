#pragma once
#include <exception>
#include <string>

class ErrorException : public std::exception
{
public:
	ErrorException(int line, const char* file) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;
private:
	// Capture and return the following:
	int line;
	std::string file;
protected:
	mutable std::string whatbuffer;
};