#ifndef _menu_h
#define _menu_h

#include "graphics.h"
#include "extgraph.h"
#include "genlib.h"
#include "simpio.h"
#include "conio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <windows.h>
#include <olectl.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>

#include "imgui.h"
#include "draw.h"

typedef struct tag_StatusInfo {
	char* filename;
	node* PresentObject;
	text* PresentText;
	int type;	// 0代表图形，1代表连接线，2代表文本框，3代表未选中
	double mouseX, mouseY;
}StatusInfo, * LinkStatusInfo;

//music
void Music(int Choice);


//menu
void DrawMenu(double winwidth, double winheigh);

void DrawMode(double winwidth, double winheigh);

int GetSelection();

void DrawStatusBar(LinkStatusInfo PresentLinkStatusInfo, double winwidth, double winheight);

void DrawIconButton(double winwidth, double winheight);

void ChangeMode(int a);


//help
int Display_Help(double winwidth, double winheight);

int Display_Start(double winwidth, double winheight);

void SetHelpMode(int mode);

#endif
