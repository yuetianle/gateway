#pragma once
#define RESET       "\033[0m"
#define BLACK       "\033[30m"      /* Black */

#define RED         "\033[31m"      /* Red */
#define LRED        "\033[01;31m"   /* Light Red */
#define GREEN       "\033[32m"      /* Green */
#define LGREEN      "\033[01;32m"   /* Light Green */
#define YELLOW      "\033[33m"      /* Yellow */
#define LYELLOW     "\033[01;33m"   /* Light Yellow */
#define BLUE        "\033[34m"      /* Blue */
#define LBLUE       "\033[01;34m"   /* Light Blue */
#define MAGENTA     "\033[35m"      /* Magenta */
#define LMAGENTA    "\033[01;35m"   /* Light Magenta */
#define CYAN        "\033[36m"      /* Cyan */
#define LCYAN       "\033[01;36m"   /* Light Cyan */
#define WHITE       "\033[37m"      /* White */
#define LWHITE      "\033[01;37m"   /* Light White */


#define BOLD        "\033[1m"               /* Bold */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#define ENDLINE "\r\n"
inline std::ostream& red(std::ostream& s)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, FOREGROUND_RED);
	return s;
}
inline std::ostream& green(std::ostream& s)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, FOREGROUND_GREEN);
	return s;
}
inline std::ostream& blue(std::ostream& s)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, FOREGROUND_BLUE);
	return s;
}
inline std::ostream& yellow(std::ostream& s)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN);
	return s;
}
inline std::ostream& white(std::ostream& s)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	return s;
}
#else
#define ENDLINE "\n"
inline std::ostream& red(std::ostream& s)
{
	s << BOLD << RED;
	return s;
}
inline std::ostream& green(std::ostream& s)
{
	s << BOLD << GREEN;
	return s;
}
inline std::ostream& blue(std::ostream& s)
{
	s << BOLD << BLUE;
	return s;
}
inline std::ostream& yellow(std::ostream& s)
{
	s << BOLD << YELLOW;
	return s;
}
inline std::ostream& white(std::ostream& s)
{
	s << WHITE;
	return s;
}

inline std::ostream& reset(std::ostream& s)
{
	s << RESET;
	return s;
}
#endif




struct log_color
{
	int16_t m_color;
	log_color(int16_t attr)
		: m_color(attr)
	{

	}
};
template<class _Elem, class _Traits>
inline std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream < _Elem, _Traits>& i, const log_color& c)
{
#ifdef WIN32
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, c.m_color);
#endif
	return i;
}


