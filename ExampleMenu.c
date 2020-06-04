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
#include "menu.h"

// 全局变量
static int  CodeShowMode = 0;		// 代码展示状态
static int  ExampleFlagSelection = -1;// 应用于记录用户点击菜单后的返回值


void DrawMenu_(double winwidth, double winheight)
{
	// 以下是依simpleGUI规定，预先定义的5个菜单的内容
	static char* menuListFile[] = { "返回菜单",
		"确认返回" };
	static char* menuListShow[] = { "演示选择",
		"Program 1" };
	static char* menuListCode[] = { "代码",
		"展示代码" };
	static char* menuListMusic[] = { "音乐",
		"浙江大学校歌",
		"The Congress Reel",
		"Yellow",
		"停止播放" };
	static char* menuListProcessure[] = { "动画",
		"开始  | Ctrl-Z", };


	double h; // 控件高度
	double w = TextStringWidth(menuListFile[0]) * 1.5; // 控件宽度
	double x = 0;// 各个按钮的x位置，逐渐递增
	double y = winheight;// 按钮的y位置，不变
	double ButtonInterval = 1;// 按钮之间的间隔
	int    FileSelection, ShowSelection, CodeSelection, MusicSelection, ProcessureSelection;// 分别记录每一个按钮的返回值
	int    ExampleReturnFlag = 6;// 用于确定最终将哪个按钮的返回值传入FlagSelection

	// 自定义颜色
	DefineColor("ButtonFrameCommon", 0.07, 0.72, 0.96);
	DefineColor("ButtonFrameHot", 0.20, 0.60, 1.00);
	setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	// 定义字体与大小
	SetFont("微软雅黑");
	SetPointSize(18);

	//在规定好字体与大小后，对控件高度赋值
	h = GetFontHeight() * 1.2;

	// 绘制窗口标题
	SetWindowTitle("小型算法流程图绘制工具-演示界面");

	// 依次绘制菜单栏与各个菜单按钮
	drawMenuBar(0, y - h, winwidth, h);

	// File 菜单
	FileSelection = menuList(GenUIID(0), x, y - h, w, TextStringWidth(menuListFile[1]) * 1.2, h,
		menuListFile, sizeof(menuListFile) / sizeof(menuListFile[0])) + 10;
	if (FileSelection > 10) {
		ExampleReturnFlag = 1;
	}

	// Show 菜单
	ShowSelection = menuList(GenUIID(0), x + w + ButtonInterval, y - h, w, TextStringWidth(menuListShow[1]) * 1.2, h,
		menuListShow, sizeof(menuListShow) / sizeof(menuListShow[0])) + 20;
	if (ShowSelection > 20) {
		ExampleReturnFlag = 2;
	}

	// Code 菜单
	menuListCode[1] = CodeShowMode ? "隐藏代码" : "展示代码";
	CodeSelection = menuList(GenUIID(0), x + 2 * (w + ButtonInterval), y - h, w, TextStringWidth(menuListCode[1]) * 1.2, h,
		menuListCode, sizeof(menuListCode) / sizeof(menuListCode[0])) + 30;
	if (CodeSelection > 30) {
		ExampleReturnFlag = 3;
		if (CodeSelection == 31) CodeShowMode = !CodeShowMode;
	}

	// Music 菜单
	MusicSelection = menuList(GenUIID(0), x + 3 * (w + ButtonInterval), y - h, w, TextStringWidth(menuListMusic[2]) * 1.2, h,
		menuListMusic, sizeof(menuListMusic) / sizeof(menuListMusic[0])) + 40;
	if (MusicSelection > 40) {
		Music(MusicSelection);
		ExampleReturnFlag = 4;
	}

	// Processure 菜单
	ProcessureSelection = menuList(GenUIID(0), x + 5 * (w + ButtonInterval), y - h, w, TextStringWidth(menuListProcessure[1]) * 1.2, h,
		menuListProcessure, sizeof(menuListProcessure) / sizeof(menuListProcessure[0])) + 50;
	if (ProcessureSelection > 50) {
		ExampleReturnFlag = 5;
	}

	// 通过对ExampleReturnFlag进行switch-case，记录恰当的返回值
	switch (ExampleReturnFlag) {
	case 0: ExampleFlagSelection = 0; break;
	case 1: ExampleFlagSelection = FileSelection; break;
	case 2: ExampleFlagSelection = ShowSelection; break;
	case 3: ExampleFlagSelection = CodeSelection; break;
	case 4: ExampleFlagSelection = MusicSelection; break;
	case 5: ExampleFlagSelection = ProcessureSelection; break;
	default: ExampleFlagSelection = -1; break;
	}
	// 重置ReturnFlag
	ExampleReturnFlag = 0;
}

int GetExampleSelection() {
	int PresentExampleFlagSelection = ExampleFlagSelection;
	ExampleFlagSelection = -1;
	return PresentExampleFlagSelection;
}