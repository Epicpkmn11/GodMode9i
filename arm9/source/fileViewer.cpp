#include "fileViewer.h"
#include <fstream>
#include <limits>
#include <nds.h>
#include <vector>

#define SCREEN_COLS 32
#define SCREEN_ROWS 22

struct Line {
	std::string string;
	uint pos;
};

std::ifstream& GotoLine(std::ifstream& file, unsigned int num) {
	file.seekg(std::ios::beg);
	for(uint i=0; i < num - 1; ++i){
		file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
	}
	return file;
}

void openFileText(std::string path) {
	std::ifstream stream(path);
	std::vector<Line> lines;
	uint hDown, lineOffset = 0, screenOffset = 0, totalLines = 0;

	std::string temp;
	while(std::getline(stream, temp))	totalLines++;

	for(int i=0;i<SCREEN_ROWS;i++) {
		Line line;
		line.pos = i;
		std::getline(stream, line.string);
		lines.push_back(line);
	}

	iprintf ("\x1b[2J");
		for(uint i=0;i<lines.size();i++) {
			if ((lineOffset - screenOffset) == i) {
				printf ("\x1B[47m");		// Print foreground white color
			} else {
				printf ("\x1B[40m");		// Print foreground black color
			}
			printf("%s\n", lines[i].string.substr(0, 20).c_str());
		}

	while(1) {
		do {
			swiWaitForVBlank();
			scanKeys();
			hDown = keysDownRepeat();
		} while(!hDown);

		if(hDown & KEY_UP) {
			lineOffset--;
		} else if(hDown & KEY_DOWN) {
			lineOffset++;
		} else if(hDown & KEY_LEFT) {
			lineOffset -= SCREEN_ROWS;
		} else if(hDown & KEY_RIGHT) {
			lineOffset += SCREEN_ROWS;
		}

		if (lineOffset < 0)	lineOffset = 0;
		else if (lineOffset > totalLines)	lineOffset = totalLines;

		// Scroll screen if needed
		if (lineOffset < screenOffset) 	{
			screenOffset = lineOffset;

			Line line;
			line.pos = lines[0].pos-1;
			GotoLine(stream, line.pos);
			std::getline(stream, line.string);
			lines.pop_back();
			lines.insert(lines.begin(), line);
		}
		if (lineOffset > screenOffset + SCREEN_ROWS - 1) {
			screenOffset = lineOffset - SCREEN_ROWS + 1;

			Line line;
			line.pos = lines[lines.size()-1].pos+1;
			GotoLine(stream, line.pos);
			std::getline(stream, line.string);
			lines.erase(lines.begin());
			lines.push_back(line);
		}

		iprintf ("\x1b[2J");
		for(uint i=0;i<lines.size();i++) {
			if ((lineOffset - screenOffset) == i) {
				printf ("\x1B[47m");		// Print foreground white color
			} else {
				printf ("\x1B[40m");		// Print foreground black color
			}
			printf("%s\n", lines[i].string.substr(0, 20).c_str());
		}
	}
}
