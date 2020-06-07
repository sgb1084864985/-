#include "graphics.h"
#include "extgraph.h"
#include "genlib.h"
#include "simpio.h"
#include "conio.h"
#include "draw.h"
#include "edit.h"
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
#include <assert.h>

/*全局变量*/
static double ButtonInterval = 1;// 主界面按钮间距
static char* Title = "小型算法流程图绘制工具";// 主界面标题
static char* Author = " This Program Is Written By Zhu Lizhen, Yan Tianming and Bao Dezheng. ZJU 2020 Summer";// 作者信息字符串，互评阶段部分屏蔽
enum __page { StartPage_, SavePage_, DrawPage_, HelpPage_, ExamplePage_, LoadPage_, LastPage_ };// 枚举类型定义所有界面

/*
* 函数名：Start
* 接口：Start(double winwidth, double winheight)
* ———————————————————————
* 这个函数用于初始化窗口标题与调用整体的界面绘图
* 函数。
*/
void Start(double winwidth, double winheight)
{
	// 初始化窗口和图形系统
	Display_Start(winwidth, winheight);
}

/*
* 函数名：DrawStartMenu
* 接口：DrawStartMenu(double winwidth, double winheight)
* ———————————————————————————
* 这个函数用于绘制开始界面的五个菜单按钮，并实时监测，以
* result记录点击结果并返回。实现页面转换。
*/
int DrawStartMenu(double winwidth, double winheight)
{
	// 对菜单内容进行定义
	static char* menuListFile[] = { "文件",
		"新建  | Ctrl-N", // 快捷键必须采用[Ctrl-X]格式，放在字符串的结尾
		"开始  | Ctrl-R" };
	static char* menuListExample[] = { "演示",
		"进入演示" };
	static char* menuListHelp[] = { "帮助",
		"使用帮助",
		"快捷热键",
		"关于程序" };
	static char* menuListExit[] = { "退出",
		"确认退出  | Ctrl-Q" };
	static char* menuListMusic[] = { "音乐",
		"浙江大学校歌",
		"The Congress Reel",
		"Yellow",
		"停止播放" };

	double h; // 控件高度
	double w = TextStringWidth(menuListFile[0]) * 1.5; // 控件宽度 
	double x = winwidth / 2 - (5 * w + 4 * ButtonInterval) / 2;// 控件位置x坐标
	double y = winheight / 4;// 控件位置y坐标
	int    FileSelection, ExampleSelection, HelpSelection, ExitSelection, MusicSelection, Result = StartPage_;// 记录用户选择与对应产生的结果

	// 自定义按钮与菜单颜色
	DefineColor("ButtonFrameCommon", 0.07, 0.72, 0.96);
	DefineColor("ButtonFrameHot", 0.20, 0.60, 1.00);
	setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	// 设置字体与字号
	SetFont("微软雅黑");
	SetPointSize(18);

	h = GetFontHeight() * 1.2;// 按钮高度赋值

	// 绘制窗口标题
	SetWindowTitle("小型算法流程图绘制工具-开始界面");

	// File 菜单
	FileSelection = menuList(GenUIID(0), x, y - h, w, TextStringWidth(menuListFile[1]) * 1.2, h,
		menuListFile, sizeof(menuListFile) / sizeof(menuListFile[0]));
	switch (FileSelection) {
	case 1: Result = DrawPage_;// 转至绘制界面
		break;
	case 2: Result = LoadPage_;// 转至加载界面
		break;
	default:
		break;
	}

	// Example 菜单
	ExampleSelection = menuList(GenUIID(0), x + w + ButtonInterval, y - h, w, TextStringWidth(menuListExample[1]) * 1.2, h,
		menuListExample, sizeof(menuListExample) / sizeof(menuListExample[0]));
	if (ExampleSelection == 1) Result = ExamplePage_;// 转至演示界面

	// Help 菜单
	HelpSelection = menuList(GenUIID(0), x + 2 * w + 2 * ButtonInterval, y - h, w, TextStringWidth(menuListHelp[1]) * 1.2, h,
		menuListHelp, sizeof(menuListHelp) / sizeof(menuListHelp[0]));
	switch (HelpSelection) {
	case 1:
		SetHelpMode(1);
		Result = HelpPage_;// 转至帮助界面-使用帮助
		break;
	case 2:
		SetHelpMode(2);
		Result = HelpPage_;// 转至帮助界面-快捷热键
		break;
	case 3:
		SetHelpMode(3);
		Result = HelpPage_;// 转至帮助界面-关于程序
		break;

	}

	// Music 菜单
	MusicSelection = menuList(GenUIID(0), x + 3 * w + 3 * ButtonInterval, y - h, w, TextStringWidth(menuListMusic[2]) * 1.2, h,
		menuListMusic, sizeof(menuListMusic) / sizeof(menuListMusic[0]));
	if (MusicSelection > 0) {
		Music(MusicSelection);// 播放或定制播放音乐
	}


	// Exit 菜单
	ExitSelection = menuList(GenUIID(0), x + 4 * w + 4 * ButtonInterval, y - h, w, TextStringWidth(menuListExit[1]) * 1.2, h,
		menuListExit, sizeof(menuListExit) / sizeof(menuListExit[0]));
	if (ExitSelection == 1) exit(-1);// 退出程序


	return Result;
}

/*
* 函数名：DrawTitle
* 接口：DrawTitle(double winwidth, double winheight)
* —————————————————————————
* 这个函数用于绘制开始界面中央的标题。
*/
void DrawTitle(double winwidth, double winheight)
{
	int OriginSize = GetPointSize();// 保留原始字号
	char* OriginFont = GetFont(), * OriginColor = GetPenColor();// 保留原始字体与文字颜色
	double TitleX, TitleY;// 标题的位置坐标

	// 设置字号、字体与颜色
	SetPointSize(40);
	SetFont("楷体");
	SetPenColor("black");

	// 为标题位置赋值
	TitleX = (winwidth - TextStringWidth(Title)) / 2;
	TitleY = winheight * 2 / 3;

	// 绘制标题
	MovePen(TitleX, TitleY);
	DrawTextString(Title);

	// 恢复原始字体
	SetPointSize(OriginSize);
	SetFont(OriginFont);
	SetPenColor(OriginColor);
}

/*
* 函数名：DrawAuthor
* 接口：DrawAuthor()
* —————————
* 这个函数用于绘制开
* 始界面左下角的作者
* 信息（互评阶段不显
* 示身份信息）。
*/
void DrawAuthor() {
	int OriginSize = GetPointSize();// 保留原始字号
	char* OriginFont = GetFont(), * OriginColor = GetPenColor();// 保留原始字体与文字颜色
	double AuthorX = 0, AuthorY;// 作者信息的位置坐标

	// 设置字号、字体与颜色
	SetPointSize(12);
	SetFont("仿宋");
	SetPenColor("gray");

	// 为作者信息赋值
	AuthorY = GetFontHeight();

	// 绘制作者信息
	MovePen(AuthorX, AuthorY);
	DrawTextString(Author);

	// 恢复原始字号、字体与颜色
	SetPointSize(OriginSize);
	SetFont(OriginFont);
	SetPenColor(OriginColor);
}

/*
* 函数名：Display_Start
* 接口：Display_Start(double winwidth, double winheight)
* ———————————————————————————
* 这个函数调用上述各个绘制函数，按顺序绘制开始界面的各个
* 元素。并返回菜单按钮点击后对应的转换目标页面信息。
*/
int Display_Start(double winwidth, double winheight)
{
	// 绘制标题与作者
	DrawTitle(winwidth, winheight);
	DrawAuthor();

	// 绘制和处理菜单
	return DrawStartMenu(winwidth, winheight);
}