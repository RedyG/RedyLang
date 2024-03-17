#pragma once
#include <string>
#include <iostream>
#include "TextPos.h"
#include "Project.h"

struct UnderlinedText {
	std::string_view Text;
	TextPos Start;
	TextPos End;

	UnderlinedText(std::string_view text, TextPos start, TextPos end)
		: Text(text), Start(start), End(end) {}
};

struct Log {
	std::string_view Title;
	std::string_view File;
	UnderlinedText Cause;
	std::vector<UnderlinedText> Hints;

	Log(std::string_view title, std::string_view file, UnderlinedText cause, std::vector<UnderlinedText> hints)
		: Title(title), File(file), Cause(cause), Hints(std::move(hints)) {}
};

namespace Logger {
	extern Project* RedyProject;
	void Error(Log log);
	void Error(std::string_view error);
	void Warning(std::string_view warning);
}
