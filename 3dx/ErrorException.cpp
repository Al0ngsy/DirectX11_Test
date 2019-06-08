#include "ErrorException.h"
#include <sstream>

ErrorException::ErrorException(int line, const char* file) noexcept
	: line(line), file(file)
{
}
// return Type and OriginString of the Error
const char* ErrorException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	whatbuffer = oss.str();
	return whatbuffer.c_str();
}
// return Type of the error
const char* ErrorException::GetType() const noexcept
{
	return "Error Exception";
}
// return line where the error was thrown
int ErrorException::GetLine() const noexcept
{
	return line;
}
// return filename where the error was thrown
const std::string& ErrorException::GetFile() const noexcept
{
	return file;
}
// return formated string with file and line where the error was thrown
std::string ErrorException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "File: " << GetFile() << std::endl
		<< "Line: " << GetFile() << std::endl;
	return oss.str();
}
