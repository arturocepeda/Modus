#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <fstream>
#include "modus.h"

enum ScriptType
{
    Score,
    Harmony,
    Scales,
    Song
};

bool generateScore(char* sScriptFile, char* sBinaryFile);
bool generateHarmonyPattern(char* sScriptFile, char* sBinaryFile, char* sDefinitionFile);
bool generateScalePattern(char* sScriptFile, char* sBinaryFile, char* sDefinitionFile);
bool generateSongStructure(char* sScriptFile, char* sBinaryFile);

void writeHeader(std::ofstream& fStream, ScriptType iScriptType);
void writeTimePosition(std::ofstream& fStream, const MSTimePosition& mTimePosition);
void writeNoteList(std::ofstream& fStream, const std::vector<MTNote>& mNoteList);
