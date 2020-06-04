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


/*全局变量*/
static int	  HelpMode = 2;			// 说明模式，1为使用帮助，2为快捷热键，3为关于程序
enum __page { StartPage_, SavePage_, DrawPage_, HelpPage_, ExamplePage_, LoadPage_, LastPage_ };

/*
* 函数名：drawButtons
* 接口：drawButtons(double winwidth, double winheight)
* ——————————————————————————
* 这个函数用于绘制窗口右下角的返回按钮，用于返回上一级
* 界面。
*/
int drawButtons(double winwidth, double winheight)
{
	double h, w;  // 控件高度宽度
	double x = winwidth * 5 / 6;
	double y = winheight / 6;// 控件位置

	// 定义字体、字号
	SetFont("微软雅黑");
	SetPointSize(18);

	// 为控件高度宽度、y坐标赋值
	h = GetFontHeight() * 1.3;
	y += h;
	w = TextStringWidth("返回") * 1.5;

	// 定义颜色
	DefineColor("ButtonFrameCommon", 0.07, 0.72, 0.96);
	DefineColor("ButtonFrameHot", 0.20, 0.60, 1.00);
	setButtonColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	// 绘制按钮
	if (button(GenUIID(0), x, y - 2 * h, w, h, "返回"))
		return LastPage_;
	else
		return HelpPage_;
}


/*
* 函数名：ProgramHelp
* 接口：ProgramHelp(double winwidth, double winheight)
* ——————————————————————————
* 这个函数用于绘制帮助界面-使用帮助界面，给予用户关于
* 程序的使用帮助。
*/
void ProgramHelp(double winwidth, double winheight) {
	// 标题与正文。正文中\n表示一行的结束，end\n表示一段的结束，END\n表示全文的结束
	char* ProgramHelpTitle[] = { "·程序说明\n","·绘制模块\n","·演示模块\n","·特色功能\n","·感谢\n" };
	char* ProgramHelpText[] = {
		"本程序用于绘制算法流程图，主要分为绘制与演示两个模块。在开始界面您可以选择",
		"\n",
		"新建文件开始绘制，也可以选择读取已储存好的后缀为DAT的文件，进行修改与进一",
		"\n",
		"步绘制。",
		"end\n",
		"本模块可以实现的功能有：①插入文本框、图形（包括矩形、圆角矩形、菱形与圆形）",
		"\n",
		"、连接线；②对图形进行伸缩、旋转的变换；③对图形、连接线进行颜色、粗细和虚",
		"\n",
		"实的样式调整；④对文本框、图形进行复制、粘贴、剪切与删除的操作。",
		"\n",
		"需要注意的是本模块分为自由模式、文本模式、图形模式与连线模式四个模式，相应",
		"\n",
		"的功能需要在所处模式下才可以使用。（不能使用的功能按钮显示为灰色）您可以在",
		"\n",
		"【模式】菜单下切换模式。左下角的模式球会提示您当前所处模式。",
		"\n",
		"最下方的信息栏提供当前文件保存状态与选中元素的相关信息。",
		"end\n",
		"本模块可以实现的功能：①为您提供典型的算法演示图解，通过点击演示进行切换；",
		"\n",
		"②通过上一步与下一步，可以进行逐步观看与回看；③提供代码的展示与隐藏功能。",
		"end\n",
		"在本程序的各个界面均有音乐播放的功能，预制3首音乐，默认循环播放，您随时都",
		"\n",
		"可以点击停止播放。",
		"end\n",
		"感谢您试用本程序，并感谢您提供的宝贵意见与建议！",
		"END\n" };
	double Interval[2], TextX, TextY;// 数组中定义了标题与正文、正文与正文之间的间隔；TextX与TextY用于表示画笔位置
	int i;// 计数

	// 定义标题与正文字体、字号，为两个间隔赋值
	SetFont("微软雅黑");
	SetPointSize(14);
	Interval[0] = GetFontHeight() / 2;
	SetFont("黑体");
	SetPointSize(18);
	Interval[1] = GetFontHeight() / 2;

	// 绘制窗口的名字
	SetWindowTitle("小型算法流程图绘制工具-帮助界面-使用帮助");

	// 绘制标题
	TextX = TextStringWidth("空格");
	TextY = winheight - Interval[1] * 3;// 起始位置
	MovePen(TextX, TextY);
	SetPenColor("Black");// 标题颜色为黑色
	for (i = 0; i < 5; i++) {
		DrawTextString(ProgramHelpTitle[i]);
		switch (i) {// 一共有5个标题，根据每个标题下的正文内容行数测算得出所需间隔数量
		case 0:TextY -= (10 * Interval[0] + 3 * Interval[1]); break;
		case 1:TextY -= (22 * Interval[0] + 3 * Interval[1]); break;
		case 2:case 3:TextY -= (7 * Interval[0] + 3 * Interval[1]); break;
		case 4:TextY += (46 * Interval[0] + 10 * Interval[1]); TextX += TextStringWidth("·"); break;// 绘制最后一个标题后，返回正文行首
		}
		MovePen(TextX, TextY);
	}

	// 绘制正文
	SetFont("微软雅黑");
	SetPointSize(18);
	DefineColor("TextBlue", 0.15, 0.25, 0.63);// 自定义墨水蓝
	SetPenColor("TextBlue");
	for (i = 0; ProgramHelpText[i] != "END\n"; i++) {
		if (ProgramHelpText[i] == "\n") {// 行末换行
			TextY -= Interval[0];
		}
		else if (ProgramHelpText[i] == "end\n") {// 段末换行
			TextY -= (2 * Interval[0] + 3 * Interval[1]);
		}
		else {
			DrawTextString(ProgramHelpText[i]);// 正文内容绘制
			TextY -= 2 * Interval[0];
		}
		MovePen(TextX, TextY);
	}
}

/*
* 函数名：ShortcutHelp
* 接口：ShortcutHelp(double winwidth, double winheight)
* ——————————————————————————
* 这个函数用于绘制帮助界面-快捷热键界面，给予用户关于
* 各个界面的快捷键汇总提示。
*/
void ShortcutHelp(double winwidth, double winheight) {
	// 标题与正文。正文中\n表示一行的结束，end\n表示一段的结束，END\n表示全文的结束
	char* ShortcutTitle[] = { "·开始界面\n","·绘制界面\n" };
	char* ShortcutText[] = {
		"【文件】新建： Ctrl + N  |  打开：Ctrl + O",
		"\n",
		"【退出】退出： Ctrl + Q",
		"end\n",
		"【文件】保存： Ctrl + S  |  退出： Ctrl + Q  |  打开：Ctrl + O  |  保存：Ctrl + S",
		"\n",
		"【编辑】复制： Ctrl + C  |  粘贴： Ctrl + V  |  剪切：Ctrl + X  |  删除：Ctrl + D",
		"END\n" };
	double Interval[2], TextX, TextY;// 数组中定义了标题与正文、正文与正文之间的间隔；TextX与TextY用于表示画笔位置
	int i;// 计数

	// 定义标题与正文字体、字号，为两个间隔赋值
	SetFont("微软雅黑");
	SetPointSize(18);
	Interval[0] = GetFontHeight() / 2;
	SetFont("黑体");
	SetPointSize(22);
	Interval[1] = GetFontHeight() / 2;

	// 绘制窗口的名字
	SetWindowTitle("小型算法流程图绘制工具-帮助界面-快捷热键");

	// 绘制标题
	TextX = TextStringWidth("空格");
	TextY = winheight - Interval[1] * 3;
	MovePen(TextX, TextY);
	SetPenColor("Black");
	for (i = 0; i < 2; i++) {
		DrawTextString(ShortcutTitle[i]);
		switch (i) {// 一共有3个标题，根据每个标题下的正文内容行数测算得出所需间隔数量
		case 0:TextY -= (7 * Interval[0] + 3 * Interval[1]); break;
		case 1:TextY += (7 * Interval[0] + 1 * Interval[1]); TextX += TextStringWidth("·"); break;
		}
		MovePen(TextX, TextY);
	}

	// 绘制正文
	SetFont("微软雅黑");
	SetPointSize(18);
	DefineColor("TextBlue", 0.15, 0.25, 0.63);
	SetPenColor("TextBlue");
	for (i = 0; ShortcutText[i] != "END\n"; i++) {
		if (ShortcutText[i] == "\n") {// 行末换行
			TextY -= Interval[0];
		}
		else if (ShortcutText[i] == "end\n") {// 段末换行
			TextY -= (2 * Interval[0] + 3 * Interval[1]);
		}
		else {
			DrawTextString(ShortcutText[i]);// 绘制正文
			TextY -= 2 * Interval[0];
		}
		MovePen(TextX, TextY);
	}
}

/*
* 函数名：AboutHelp
* 接口：AboutHelp(double winwidth, double winheight)
* ——————————————————————————
* 这个函数用于绘制帮助界面-快捷热键界面，给予用户关于
* 各个界面的快捷键汇总提示。
*/
void AboutHelp(double winwidth, double winheight)
{
	// 标题与正文。正文中\n表示一行的结束，end\n表示一段的结束，END\n表示全文的结束
	char* AboutHelpTitle[] = { "·程序作者\n","·联系方式\n" };
	char* AboutHelpText[] = {
		"依互评规则，本段文字于互评阶段暂不予显示",
		"end\n",
		"依互评规则，本段文字于互评阶段暂不予显示",
		"END\n" };
	double Interval[2], TextX, TextY;// 数组中定义了标题与正文、正文与正文之间的间隔；TextX与TextY用于表示画笔位置
	int i;// 计数

	// 定义标题与正文字体、字号，为两个间隔赋值
	SetFont("微软雅黑");
	SetPointSize(18);
	Interval[0] = GetFontHeight() / 2;
	SetFont("黑体");
	SetPointSize(22);
	Interval[1] = GetFontHeight() / 2;

	// 绘制窗口的名字
	SetWindowTitle("小型算法流程图绘制工具-帮助界面-关于程序");

	// 绘制标题
	TextX = TextStringWidth("空格");
	TextY = winheight - Interval[1] * 3;
	MovePen(TextX, TextY);
	SetPenColor("Black");
	for (i = 0; i < 2; i++) {
		DrawTextString(AboutHelpTitle[i]);
		switch (i) {// 一共有2个标题，根据每个标题下的正文内容行数测算得出所需间隔数量
		case 0:TextY -= (4 * Interval[0] + 3 * Interval[1]); break;
		case 1:TextY += (4 * Interval[0] + Interval[1]); TextX += TextStringWidth("·"); break;
		}
		MovePen(TextX, TextY);
	}

	// 绘制正文
	SetFont("微软雅黑");
	SetPointSize(18);
	DefineColor("TextBlue", 0.15, 0.25, 0.63);
	SetPenColor("TextBlue");
	for (i = 0; AboutHelpText[i] != "END\n"; i++) {
		if (AboutHelpText[i] == "\n") {
			TextY -= Interval[0];
		}
		else if (AboutHelpText[i] == "end\n") {
			TextY -= (2 * Interval[0] + 3 * Interval[1]);
		}
		else {
			DrawTextString(AboutHelpText[i]);
			TextY -= 2 * Interval[0];
		}
		MovePen(TextX, TextY);
	}
}

/*
* 函数名：Display_Help
* 接口：Display_Help(double winwidth, double winheight)
* ——————————————————————————
* 这个函数用于根据当前的HelpMode进行对应帮助界面的函数
* 调用。
*/
int Display_Help(double winwidth, double winheight)
{
	// 按钮
	int result = drawButtons(winwidth, winheight);

	// 根据当前HelpMode进行帮助界面的选择绘制
	switch (HelpMode) {
	case 1:ProgramHelp(winwidth, winheight); break;
	case 2:ShortcutHelp(winwidth, winheight); break;
	case 3:AboutHelp(winwidth, winheight); break;
	}

	return result;
}

/*
* 函数名：SetHelpMode
* 接口：SetHelpMode(int mode)
* ——————————————
* 这个函数用于根据传入的值，对
* HelpMode值进行修改。起到对帮
* 助界面展示的选择作用。
*/
void SetHelpMode(int mode) {
	HelpMode = mode;
}