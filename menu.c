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
#include "draw.h"

/*静态局部全局变量*/
static double StatusBarHeight;		// 状态栏高度
static int	  SaveMode;				// 保存状态
static int    CreateMode = 0;		// 编辑状态，0-自由模式，1-文本模式，2-图形模式，3-连线模式
static int	  FlagSelection = -1;	// 应用于记录用户点击菜单后的返回值

/*
* 函数名：DrawMode
* 接口：DrawMode(double winwidth,double winheight)
* ————————————————————————
* 这个函数用于绘制窗口左下角的状态球，用于提示用户
* 当前所处的状态。分为蓝色底色的自由模式、黄色底色
* 的文本模式、绿色底色的图形模式与红色底色的连线模
* 式总计四种模式。通过改变CreateMode的数值，改变状
* 态球样式。
*/
void DrawMode(double winwidth, double winheight) {
	double ModeCircleX = winwidth / 20,
		ModeCircleY = winheight / 20 + StatusBarHeight, r;	// 状态球的坐标与半径
	char* ModeInformation, * ModeColor;						// 状态球内容与颜色字符串

	DefineColor("FreeModeColor", 0.07, 0.72, 0.96);
	DefineColor("TextModeColor", 1.00, 0.80, 0.26);
	DefineColor("GraphModeColor", 0.10, 0.63, 0.10);
	DefineColor("LineModeColor", 0.87, 0.31, 0.27);

	SetEraseMode(0);

	// 根据当前CreateMode值选择模式，对内容与颜色字符串赋值
	switch (CreateMode) {
	case 0:default:ModeInformation = "自由模式"; ModeColor = "FreeModeColor"; break;
	case 1:ModeInformation = "文本模式"; ModeColor = "TextModeColor"; break;
	case 2:ModeInformation = "图形模式"; ModeColor = "GraphModeColor"; break;
	case 3:ModeInformation = "连线模式"; ModeColor = "LineModeColor"; break;
	}

	// 设定半径与坐标
	r = TextStringWidth(ModeInformation) * 0.6;
	ModeCircleX += r;
	ModeCircleY += GetFontHeight() / 2;

	// 设定颜色、字体，移动画笔进行球与字符串的绘画
	StartFilledRegion(0.5);
	SetPenColor(ModeColor);
	SetFont("黑体");
	MovePen(ModeCircleX, ModeCircleY);
	DrawArc(r, 0, 360);
	EndFilledRegion();
	SetPenColor("Black");
	MovePen(ModeCircleX - TextStringWidth(ModeInformation) * 1.1, ModeCircleY - GetFontHeight() / 5);
	DrawTextString(ModeInformation);
}

/*
* 函数名：GetSelection
* 接口：GetSelection()
* ——————————
* 用于将局部全局变量
* FlagSelection传递给
* 其他文件内的函数。
*/
int GetSelection() {
	int PresentSelection = FlagSelection;
	FlagSelection = -1;
	return PresentSelection;
}

/*
* 函数名：DrawMenu
* 接口：DrawMenu(double winwidth, double winheight)
* ————————————————————————
* 利用simpleGUI绘制界面上方的菜单栏，并对用户的点击
* 进行监测，实时记录按下按键对应的参数值，储存在
* FlagSelection中。同时改变CreateMode的值表示变更模
* 式，对于比较简单的函数（如Music）直接进行调用。
*/
void DrawMenu(double winwidth, double winheight)
{
	SetEraseMode(0);
	// 以下是依simpleGUI规定，预先定义的8个菜单的内容
	static char* menuListFile[] = { "文件",
		"新建  | Ctrl-N",
		"打开  | Ctrl-O",
		"保存  | Ctrl-S",
		"另存为",
		"返回",
		"关闭",
		"退出  | Ctrl-Q" };
	static char* menuListEdit[] = { "编辑",
		"复制  | Ctrl-C",
		"粘贴  | Ctrl-V",
		"剪切  | Ctrl-X",
		"删除  | Ctrl-D",
		"选择" };
	static char* menuListMode[] = { "模式",
		"文本模式",
		"图形模式",
		"连线模式" };
	static char* menuListText[] = { "文本",
		"插入文本",
		"文本选择",
		"编辑文本",
		"切换颜色"};
	static char* menuListGraphic[] = { "图形",
		"插入矩形",
		"插入菱形",
		"插入圆角",
		"切换颜色",
		"切换虚实",
		"切换粗细",
		"切换填充",
		/*"切换闪烁"*/ };
	static char* menuListLine[] = { "连线",
		"切换颜色",
		"切换虚实",
		"切换粗细",
		"插入连线" };
	static char* menuListMusic[] = { "音乐",
		"浙江大学校歌",
		"The Congress Reel",
		"Yellow",
		"停止播放" };
	static char* menuListHelp[] = { "帮助",
		"使用帮助",
		"快捷热键",
		"关于软件" };
	static char* menuListSetting[] = { "设置",
		"连线自动对齐",
		"退出时自动保存" };

	double h; // 控件高度
	double w; // 控件宽度
	double x = 0;// 各个按钮的x位置，逐渐递增
	double y = winheight;// 按钮的y位置，不变
	double MenuButtonInterval = 0.4;// 按钮之间的间隔
	int    ReturnFlag = 0;// 用于确定最终将哪个按钮的返回值传入FlagSelection
	int	   MenuLen;       // 代表某个菜单选项的个数
	int    FileSelection, EditSelection, ModeSelection, TextSelection, GraphicSelection,
		LineSelection, MusicSelection, HelpSelection, SettingSelection;// 分别记录每一个按钮的返回值
	static int IsInText = 0, set1 = 1, set2 = 1;// 标志是否处于文本模式，以变动文本菜单按钮中的内容

	// 自定义颜色
	DefineColor("ButtonFrameCommon", 0.07, 0.72, 0.96);
	DefineColor("ButtonFrameHot", 0.20, 0.60, 1.00);
	setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	// 定义字体与大小
	SetFont("微软雅黑");
	SetPointSize(18);

	//在规定好字体与大小后，对控件宽度、高度赋值
	w = TextStringWidth(menuListFile[0]) * 1.5;
	h = GetFontHeight() * 1.2;

	// 绘制窗口标题
	SetWindowTitle("小型算法流程图绘制工具-绘制界面");

	// 依次绘制菜单栏与各个菜单按钮
	drawMenuBar(0, y - h, winwidth, h);

	// File 菜单
	setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);
	FileSelection = menuList(GenUIID(0), x, y - h, w, TextStringWidth(menuListFile[1]) * 1.2, h,
		menuListFile, sizeof(menuListFile) / sizeof(menuListFile[0])) + 10;
	if (FileSelection > 10) {
		ReturnFlag = 1;
		CreateMode = 0;// 转为自由模式
		if (FileSelection == 13 || FileSelection == 14) {
			SaveMode = 1;// 转为已保存状态
		}
	}

	// Edit 菜单
	if (CreateMode != 0) {// 若处于自由模式则处于禁用的灰色状态
		setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);
		MenuLen = sizeof(menuListText) / sizeof(menuListText[0]);
	}
	else {
		setMenuColors("Gray", "White", "Gray", "White", 1);
		MenuLen = 1;
	}
	EditSelection = menuList(GenUIID(0), x + w + MenuButtonInterval, y - h, w, TextStringWidth(menuListEdit[1]) * 1.2, h,
		menuListEdit, sizeof(menuListEdit) / sizeof(menuListEdit[0])) + 20;
	if (EditSelection > 20 && CreateMode != 0) {
		ReturnFlag = 2;
		if (EditSelection == 25) {
			CreateMode = 0; // 转为自由模式
		}
	}
	setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	// Mode 菜单
	ModeSelection = menuList(GenUIID(0), x + 2 * (w + MenuButtonInterval), y - h, w, TextStringWidth(menuListMode[1]) * 1.2, h,
		menuListMode, sizeof(menuListMode) / sizeof(menuListMode[0])) + 30;
	if (ModeSelection > 30) {
		ReturnFlag = 3;
		switch (ModeSelection) {
		case 31: if (CreateMode != 1) IsInText = 0; CreateMode = 1; break;// 转为文本模式
		case 32: CreateMode = 2; break;// 转为图形模式
		case 33: CreateMode = 3; break;// 转为连线模式
		}
	}

	// Text 菜单
	if (CreateMode == 1) {// 若不处于文本模式则处于禁用的灰色状态
		setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);
		MenuLen = sizeof(menuListText) / sizeof(menuListText[0]);
	}
	else {
		setMenuColors("Gray", "White", "Gray", "White", 1);
		MenuLen = 1;
	}
	TextSelection = menuList(GenUIID(0), x + 3 * (w + MenuButtonInterval), y - h, w, TextStringWidth(menuListText[1]) * 1.2, h,
		menuListText, MenuLen) + 40;
	if (TextSelection > 40 && CreateMode == 1) {// 当且仅当处于文本模式，这个菜单栏下的按钮才会有响应
		switch (TextSelection) {
		case 41:case 42: case 43: case 44: case 45: case 46: ReturnFlag = 4; break;
		default:break;
		}
	}
	setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	// Graphic 菜单
	if (CreateMode == 2) {// 若不处于图象模式则处于禁用的灰色状态
		setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);
		MenuLen = sizeof(menuListGraphic) / sizeof(menuListGraphic[0]);
	}
	else {
		setMenuColors("Gray", "White", "Gray", "White", 1);
		MenuLen = 1;
	}
	GraphicSelection = menuList(GenUIID(0), x + 4 * (w + MenuButtonInterval), y - h, w, TextStringWidth(menuListGraphic[1]) * 1.2, h,
		menuListGraphic, MenuLen) + 50;
	if (GraphicSelection > 50 && CreateMode == 2) {// 当且仅当处于图形模式，这个菜单栏下的按钮才会有响应
		ReturnFlag = 5;
	}
	setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	// Line 菜单
	if (CreateMode == 3) {// 若不处于连线模式则处于禁用的灰色状态
		setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);
		MenuLen = sizeof(menuListLine) / sizeof(menuListLine[0]);
	}
	else {
		setMenuColors("Gray", "White", "Gray", "White", 1);
		MenuLen = 1;
	}
	LineSelection = menuList(GenUIID(0), x + 5 * (w + MenuButtonInterval), y - h, w, TextStringWidth(menuListLine[1]) * 1.2, h,
		menuListLine, MenuLen) + 60;// 当且仅当处于连线模式，这个菜单栏下的按钮才会有响应
	if (LineSelection > 60 && CreateMode == 3) {
		ReturnFlag = 6;
	}
	setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	// Music 菜单
	MusicSelection = menuList(GenUIID(0), x + 6 * (w + MenuButtonInterval), y - h, w, TextStringWidth(menuListMusic[2]) * 1.2, h,
		menuListMusic, sizeof(menuListMusic) / sizeof(menuListMusic[0])) + 70;
	if (MusicSelection > 70) {// 对音乐进行播放与停止操作
		Music(MusicSelection - 70);
		CreateMode = 0;// 转为自由模式
		ReturnFlag = 7;
	}

	// Help 菜单
	HelpSelection = menuList(GenUIID(0), x + 7 * (w + MenuButtonInterval), y - h, w, TextStringWidth(menuListHelp[1]) * 1.2, h,
		menuListHelp, sizeof(menuListHelp) / sizeof(menuListHelp[0])) + 80;
	if (HelpSelection > 80) {
		CreateMode = 0; // 转为自由模式
		ReturnFlag = 8;
	}

	// Setting 菜单

	if (set1 ) menuListSetting[1] = "取消自动对齐";
	else menuListSetting[1] = "连线自动对齐";

	if (set2 ) menuListSetting[2] = "取消自动保存";
	else menuListSetting[2] = "退出时自动保存";

	SettingSelection = menuList(GenUIID(0), x + 8 * (w + MenuButtonInterval), y - h, w, TextStringWidth(menuListSetting[1]) * 1.2, h,
		menuListSetting, sizeof(menuListSetting) / sizeof(menuListSetting[0])) + 90;
	if (SettingSelection > 90) {
		if (SettingSelection == 91) set1 = !set1;
		else set2 = !set2;
		ReturnFlag = 9;
	}
	// 通过对ReturnFlag进行switch-case，记录恰当的返回值，并在一些情况下置SaveMode为0
	switch (ReturnFlag) {
	case 0: FlagSelection = 0; break;
	case 1: FlagSelection = FileSelection; break;
	case 2: FlagSelection = EditSelection; SaveMode = 0; break;
	case 3: FlagSelection = ModeSelection; SaveMode = 0; break;
	case 4: FlagSelection = TextSelection; SaveMode = 0; break;
	case 5: FlagSelection = GraphicSelection; break;
	case 6: FlagSelection = LineSelection; break;
	case 7: FlagSelection = MusicSelection; break;
	case 8: FlagSelection = HelpSelection; break;
	case 9: FlagSelection = SettingSelection; break;
	default: FlagSelection = -1; break;
	}
	// 重置ReturnFlag
	ReturnFlag = 0;
}


/*
* 函数名：DrawStatusBar
* 接口：DrawStatusBar(LinkStatusInfo PresentLinkStatusInfo, double winwidth, double winheight)
* ——————————————————————————————————————————————
* 通过传递进来的含有信息的结构体指针，了解文件名、当前用户是否选中了元素，若选中，则进一步了解
* 选中元素的相关属性。将以上信息全部展现在底部的信息栏，以便用户操作。对于文件是否处于保存状态，
* 采用在文件名后加上*标志表示未保存，没有则表示已被保存。
*/
void DrawStatusBar(LinkStatusInfo PresentLinkStatusInfo, double winwidth, double winheight) {
	static char PositionX[100], PositionY[100];// 储存所选元素的位置坐标
	static char MouseX[100], MouseY[100];// 储存鼠标的位置坐标
	static char ItemWidth[100], ItemHeight[100];// 储存元素的宽度与高度
	static char LineBeginX[100], LineBeginY[100];// 储存连线起点的位置坐标
	static char LineEndX[100], LineEndY[100];// 储存连线终点的位置坐标
	static char PenSize[100];// 储存画笔大小
	static char* IfFrameOrFilled[] = { "否","是" };// 用于显示闪烁与填充的是和否
	static char* DottedOrFull[] = { "虚线", "实线" };// 用于显示线型的虚实
	static char* SelectMsg[] = { "| 当前选中元素为菱形，相关信息为：",
		"| 当前选中元素为矩形，相关信息为：",
		"| 当前选中元素为圆角矩形，相关信息为：",
		"| 当前选中元素为连接线，相关信息为：",
		"| 当前选中元素为文本框，相关信息为：" };// 用于显示提示信息

	// 自定义颜色、设定字体与字号、画笔颜色
	DefineColor("StatusBarBackground", 0.07, 0.72, 0.96);
	SetPenColor("StatusBarBackground");

	// 设定信息栏高度
	StatusBarHeight = GetFontHeight() * 1.3;

	// 绘制信息栏背景-矩形
	drawRectangle(0, 0, winwidth, StatusBarHeight, 1);

	SetFont("Arial");
	SetPointSize(12);
	SetPenColor("White");

	// 首先画出文件名与保存状态
	MovePen(TextStringWidth(" "), StatusBarHeight / 2);
	char* temp = PresentLinkStatusInfo->filename,*p=temp;
	for (; *temp; temp++) {
		if (*temp == '\\') p = temp+1;
	}
	DrawTextString(p);
	if (!SaveMode) {
		DrawTextString(" * ");
	}

	// 根据结构中的Type进行分情况处理
	switch (PresentLinkStatusInfo->type) {
	case 0://图形（矩形，菱形、圆角矩形），显示种类、位置坐标、宽度高度、颜色线型、画笔大小与是否填充
		sprintf(PositionX, "%.2f", PresentLinkStatusInfo->PresentObject->cx);
		sprintf(PositionY, "%.2f", PresentLinkStatusInfo->PresentObject->cy);
		sprintf(ItemWidth, "%.2f", PresentLinkStatusInfo->PresentObject->width);
		sprintf(ItemHeight, "%.2f", PresentLinkStatusInfo->PresentObject->height);
		sprintf(PenSize, "%d", PresentLinkStatusInfo->PresentObject->pensize);
		DrawTextString(SelectMsg[PresentLinkStatusInfo->PresentObject->n]);
		DrawTextString("位置坐标：( ");
		DrawTextString(PositionX);
		DrawTextString(",");
		DrawTextString(PositionY);
		DrawTextString(")  宽度：");
		DrawTextString(ItemWidth);
		DrawTextString("  高度：");
		DrawTextString(ItemHeight);
		DrawTextString("  颜色：");
		DrawTextString(scolor[PresentLinkStatusInfo->PresentObject->color]);
		DrawTextString("  线型：");
		DrawTextString(DottedOrFull[PresentLinkStatusInfo->PresentObject->frame]);
		DrawTextString("  画笔大小：");
		DrawTextString(PenSize);
		DrawTextString("  是否填充：");
		DrawTextString(IfFrameOrFilled[PresentLinkStatusInfo->PresentObject->filled]);
		break;
	case 1://连接线，显示起始坐标与终止坐标
		sprintf(LineBeginX, "%.2f", PresentLinkStatusInfo->PresentLine->begin->bx);
		sprintf(LineBeginY, "%.2f", PresentLinkStatusInfo->PresentLine->begin->by);
		sprintf(LineEndX, "%.2f", PresentLinkStatusInfo->PresentLine->end->ex);
		sprintf(LineEndY, "%.2f", PresentLinkStatusInfo->PresentLine->end->ey);
		DrawTextString(SelectMsg[3]);
		DrawTextString("起始坐标：( ");
		DrawTextString(LineBeginX);
		DrawTextString(",");
		DrawTextString(LineBeginY);
		DrawTextString(")  终止坐标：( ");
		DrawTextString(LineEndX);
		DrawTextString(",");
		DrawTextString(LineEndY);
		DrawTextString(")");
		break;
	case 2://文本框，显示位置坐标、宽度高度与颜色字体
		sprintf(PositionX, "%.2f", PresentLinkStatusInfo->PresentText->cx);
		sprintf(PositionY, "%.2f", PresentLinkStatusInfo->PresentText->cy);
		sprintf(ItemWidth, "%.2f", PresentLinkStatusInfo->PresentText->width);
		sprintf(ItemHeight, "%.2f", PresentLinkStatusInfo->PresentText->height);
		DrawTextString(SelectMsg[4]);
		DrawTextString("位置坐标：( ");
		DrawTextString(PositionX);
		DrawTextString(",");
		DrawTextString(PositionY);
		DrawTextString(")  宽度：");
		DrawTextString(ItemWidth);
		DrawTextString("  高度：");
		DrawTextString(ItemHeight);
		DrawTextString("  颜色：");
		DrawTextString(scolor[PresentLinkStatusInfo->PresentText->color]);
		/*DrawTextString("  字体：");
		DrawTextString(fontstyle[PresentLinkStatusInfo->PresentText->fontstyle]);*/
		break;
	case 3:// 未选中，显示鼠标位置
		sprintf(MouseX, "%.2f", PresentLinkStatusInfo->mouseX);
		sprintf(MouseY, "%.2f", PresentLinkStatusInfo->mouseY);
		DrawTextString("| 当前未选中任何元素");
		DrawTextString(" 当前鼠标位置：( ");
		DrawTextString(MouseX);
		DrawTextString(",");
		DrawTextString(MouseY);
		DrawTextString(")");
		break;
	default:// 参数错误，显示故障信息
		DrawTextString("| 信息栏故障");
		break;
	}
}

/*
* 函数名：DrawIconButton
* 接口：DrawIconButton(double winwidth, double winheight)
* ————————————————————————————
* 利用simpleGUI绘制界面右侧的图标工具栏，实现常用功能的快
* 捷使用。实现原理采取与DrawMenu相同的方法，都将参数传入
* FlagSelection，方便统一管理，统一对接。
*/
void DrawIconButton(double winwidth, double winheight) {
	static double IconX, IconY, IconH, IconW;// 图标按钮的位置、高度与宽度
	static double IconButtonInterval = 0.4;// 图标按钮的垂直距离
	static int	  ButtonSelection[7] = { 0,0,0,0,0,0,0 };// 记录选择
	static char* ButtonName[] = {// 图标按钮的名字
		"   ▄   ",
		"   ◆   ",
		" （〓） ",
		"   ×   ",
		"   √   ",
		"  —>  ",
		"  A  ",
	};

	// 设定字体与字号
	SetFont("微软雅黑");
	SetPointSize(16);

	// 确定图标按钮的参数
	IconX = winwidth * 8 / 9 - TextStringWidth(ButtonName[0]);
	IconY = winheight * 3 / 4;
	IconH = GetFontHeight() * 1.5;
	IconW = TextStringWidth(ButtonName[0]) * 1.2;

	// 自定义颜色
	DefineColor("ButtonFrameCommon", 0.07, 0.72, 0.96);
	DefineColor("ButtonFrameHot", 0.20, 0.60, 1.00);
	setButtonColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	// 绘制七个按钮并且记录返回值
	if (CreateMode == 2) {// 若不处于图象模式则处于禁用的灰色状态
		setButtonColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);
	}
	else {
		setButtonColors("Gray", "White", "Gray", "White", 1);
	}
	ButtonSelection[0] = button(GenUIID(0), IconX, IconY, IconW, IconH, ButtonName[0]);
	ButtonSelection[1] = button(GenUIID(0), IconX, IconY - (IconH + IconButtonInterval), IconW, IconH, ButtonName[1]);
	ButtonSelection[2] = button(GenUIID(0), IconX, IconY - 2 * (IconH + IconButtonInterval), IconW, IconH, ButtonName[2]);
	setButtonColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);
	ButtonSelection[3] = button(GenUIID(0), IconX, IconY - 3 * (IconH + IconButtonInterval), IconW, IconH, ButtonName[3]);
	ButtonSelection[4] = button(GenUIID(0), IconX, IconY - 4 * (IconH + IconButtonInterval), IconW, IconH, ButtonName[4]);
	if (CreateMode == 3) {// 若不处于连线模式则处于禁用的灰色状态
		setButtonColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);
	}
	else {
		setButtonColors("Gray", "White", "Gray", "White", 1);
	}
	ButtonSelection[5] = button(GenUIID(0), IconX, IconY - 5 * (IconH + IconButtonInterval), IconW, IconH, ButtonName[5]);
	setButtonColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);
	if (CreateMode == 1) {// 若不处于文本模式则处于禁用的灰色状态
		setButtonColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);
	}
	else {
		setButtonColors("Gray", "White", "Gray", "White", 1);
	}
	ButtonSelection[6] = button(GenUIID(0), IconX, IconY - 6 * (IconH + IconButtonInterval), IconW, IconH, ButtonName[6]);
	setButtonColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	if (ButtonSelection[0] && CreateMode == 2) {// 当且仅当返回值不为0且处于图形模式，产生响应
		FlagSelection = 51;
	}
	if (ButtonSelection[1] && CreateMode == 2) {// 当且仅当返回值不为0且处于图形模式，产生响应
		FlagSelection = 52;
	}
	if (ButtonSelection[2] && CreateMode == 2) {// 当且仅当返回值不为0且处于图形模式，产生响应
		FlagSelection = 53;
	}
	if (ButtonSelection[3]) {// 同时更改为自由模式
		FlagSelection = 93;
		CreateMode = 0;
	}
	if (ButtonSelection[4]) {// 同时更改为自由模式，设定保存状态为1
		FlagSelection = 13;
		CreateMode = 0;
		SaveMode = 1;
	}
	if (ButtonSelection[5] && CreateMode == 3) {// 当且仅当返回值不为0且处于连线模式，产生响应
		FlagSelection = 64;
	}
	if (ButtonSelection[6] && CreateMode == 1) {// 当且仅当返回值不为0且处于文本模式，产生响应
		FlagSelection = 41;
	}
}

void ChangeMode(int a) {
	CreateMode = a;
}