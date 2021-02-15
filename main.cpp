#include <iostream>
#include <algorithm>
#include <string>

std::string title;
std::string composer;

std::string ltrim(const std::string &line) {
	auto end { line.end() };
	auto cur { line.begin() };
	while (cur != end && *cur <= ' ') { ++cur; }
	return std::string { cur, end };
}

std::string rtrim(const std::string &line) {
	auto begin { line.begin() };
	auto cur { line.end() };
	if (begin != cur) {
		for (;;) {
			--cur;
			if (cur == begin || *cur > ' ') { break; }
		}
		return std::string { begin, cur + 1 };
	} else { return line; }
}

std::string trim(const std::string &line) {
	return rtrim(ltrim(line));
}

std::string condense(const std::string &line) {
	std::string result;
	auto end { line.end() };
	auto cur { line.begin() };
	while (cur < end) {
		result += *cur;
		if (*cur != ' ' && cur + 1 < end && *(cur + 1) == ' ') {
			cur += 2;
		} else {
			++cur;
		}
	}
	return result;
}

void read_title() {
	std::string line;
	std::getline(std::cin, line);
	title = condense(trim(line));
	std::getline(std::cin, line);
}

void read_composer() {
	std::string line;
	std::getline(std::cin, line);
	composer = trim(line);
}

struct Chord {
	Chord(const std::string &chord, int pos): chord { chord }, pos { pos } { }

	std::string chord;
	int pos;
};

void write_chord(std::string chord, std::string classname) {
	if (islower(chord[0])) {
		classname += " repeated";
		chord[0] = toupper(chord[0]);
		auto slash { chord.find('/') };
		if (slash != std::string::npos) {
			chord[slash + 1] = toupper(chord[slash + 1]);
		}
	}
	std::cout << "<span class=\"chord " << classname << "\">" << chord[0];
	bool sup { false };
	int i { 1 };
	if (chord.size() > i && (chord[i] == '#' || chord[i] == 'b')) {
		std::cout << "<sup>"; sup = true;
		std::cout << (chord[i] == '#' ? "♯" : "♭");
		++i;
	}
	if (chord.size() > i && chord[i] == 'm' && (chord.size() <= i + 1 || chord[i + 1] != 'a')) {
		if (sup) { std::cout << "</sup>"; sup = false; }
		std::cout << chord[i];
		++i;
	}
	for (; i < chord.size() && chord[i] != '/'; ++i) {
		if (! sup) { std::cout << "<sup>"; sup = true; }
		if (chord[i] == '#') {
			std::cout << "♯";
		} else if (chord[i] == 'b') {
			std::cout << "♭";
		} else { std::cout << chord[i]; }
	}
	if (sup) { std::cout << "</sup>"; }
	for (; i < chord.size(); ++i) {
		if (chord[i] == '#') {
			std::cout << "♯";
		} else if (chord[i] == 'b') {
			std::cout << "♭";
		} else { std::cout << chord[i]; }
	}
	std::cout << "</span>";
}

void write_bars(const std::string &line) {
	if (line.empty() || line[0] != '|' || line[line.length() - 1] != '|') {
		std::cerr << "invalid [" << line << "]\n";
		return;
	}
	auto cur { ++line.begin() };
	auto end { line.end() };
	std::cout << "\t\t<div class=\"bars\">|";
	for (;;) {
		std::cout << "<span class=\"bar\">";

		auto begin { cur };
		std::vector<Chord> chords;

		while (*cur != '|') {
			if (*cur == ' ') { ++cur; continue; }
			std::string chord;
			int pos = cur - begin;
			while (*cur != ' ' && *cur != '|') {
				chord += *cur++;
			}
			chords.emplace_back(chord, pos);

			std::cout << *cur;
			++cur;
		}

		switch (chords.size()) {
		case 0: break;
		case 1:
			write_chord(chords[0].chord, "full");
			break;
		case 2:
			write_chord(chords[0].chord, "half");
			write_chord(chords[1].chord, "half");
			break;
		default:
			std::cerr << "too many chords in " << line << "\n";
		}
		std::cout << "</span>|";
		++cur;
		if (cur == end) { break; }
	}
	std::cout << "</div>\n";
}

int main() {
	read_title();
	read_composer();

	std::cout << "<!doctype html>\n"
		"<html>\n"
		"\t<head>\n"
		"\t\t<meta charset=\"utf-8\">\n"
		"\t\t<title>" << title << "</title>\n"
		"\t\t<link rel=\"stylesheet\" href=\"musx.css\">\n"
		"\t</head>\n"
		"\t<body>\n"
		"\t\t<h1>" << title << "</h1>\n"
		"\t\t<div class=\"composer\">" << composer << "</div>\n";

	std::string line;
	while (std::getline(std::cin, line)) {
		if (line.empty()) {
			std::cout << "\t\t<br />\n";
		} else if (line[0] <= ' ') {
			write_bars(trim(line));
		} else if (isupper(line[0])) {
			std::cout << "\t\t<div class=\"segment\">" << line << "</div>\n";
		} else {
			std::transform(line.begin(), line.end(), line.begin(), ::toupper);
			std::cout << "\t\t<div class=\"segment repeated\">" << line << "</div>\n";
		}

	}
	std::cout << "\t</body>\n"
		"</html>\n";
	return 0;
}
