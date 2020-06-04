#ifndef _draw_h
#define _draw_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include <assert.h>
#include "graphics.h"
#include "genlib.h"
#include "conio.h"

#include <windows.h>
#include <olectl.h>
#include <stdio.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>

#define pi 3.1415926539
#define dot 0.07

#define white 0
#define Red 1
#define Green 2
#define Blue 3
#define black 4
#define yellow 5
#define pupple 6
#define pink 7
#define orange 8
#define cyan 9
#define deepgreen 10
#define gray 11

#define _TEXT 1
#define LINE 2
#define GRAPH 3
#define FREE 4

#define left 1
#define right 2
#define down 3
#define up 4
#define xaxs 5
#define yaxs 6
#define ROTATE 7
#define Textzoom 1
#define Textmove 2


//定义字体

static char fontstyle[][20] = { "宋体","source code pro","consolas","楷体_GB2312" };
//字体表

static double Angle;
//全局角度
static int clinecolor;
//连接线颜色
static char scolor[20][10] = { "white","red","green","blue","black" ,"yellow",
				"pupple" ,"pink","orange","cyan","deepgreen","gray" };
//颜色表


typedef struct Line {
	//双向链表
	double bx, by;
	//起始位置
	double ex, ey;
	//结束位置
	int dir;
	//方向
	struct Line* next;
	//下一个
	struct Line* before;
	//上一个
}line;

typedef struct ConnectLine {
	//开始
	line* begin;
	//中间
	line* middle;
	//结束
	line* end;
	//连接线数量
	int num;
	//颜色
	//大小
	//虚实
	//是否擦除
	int color, size, solid, erase;
	//起始方向
	//结束方向
	int blineangle, elineangle;
}cline;

typedef struct graphics {
	//图形模式
	int n;
	//图库大小（矩形闭包）
	double width, height;
	//旋转方向
	double angle;
	//中心位置
	double cx, cy;
	//颜色
	int color;
	//大小
	int pensize;
	//边框
	int frame;
	//填充
	int filled;
	//下一个
	struct graphics* next;
	//连接线
	cline* cl;
	//连接图形
	struct graphics* connect;
}node;

typedef struct Textbox {
	//左上角位置
	double cx, cy;
	//大小
	double width, height;
	//文本
	char s[100];
	int show; //显示光标 showcuror
	int blink; //光标闪烁
	int position; //光标位置
	int linenum; //每行字符数
	int precolor; //设定颜色
	int color; //当前颜色
	int fontstyle; //字体
	struct Textbox* next; //下一个
}text;

void ChangeLine();

void forward(double distance);

void move(double distance);

void imLine(double dis);

void Drawline(int flag, double dis);

void Draw(node *g, int isDraw);

void DrawArrow(double bx, double by, double angle, double length);

void DrawFrame(node* g, int color);

void Drawedge(node *g);

double GetAngle(double ax, double ay, double bx, double by);

void Liner(cline *l);

int Judge(int dir, line *pre, double x, double y);

line * AddLine(int dir, line *pre, double x, double y);

void Arrow(int dir, line *pre, double prex, double prey, double x, double y, int erase);

void Fill(node *p);

void DrawButton(double x, double y, int n, int isErase);

void SelectButton(double x, double y, int n, int isDraw);

void DrawCADButton(int button);

void DrawTextEdge(text *t);

void DrawCursor(text *t);

#endif
