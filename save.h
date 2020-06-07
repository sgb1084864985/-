#pragma once

#include "file.h"

int Display_Load(double wholeW, double wholeH, Link ptrh, text* ptrth, cline* HeadLine, char* Name);
int Display_Save(double wholeW, double wholeH, char* filename, Link head, text* thead, cline* HeadLine);

void besaved();