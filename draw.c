#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include <stddef.h>
#include "graphics.h"
#include <assert.h>
#include "genlib.h"
#include "conio.h"
#include "draw.h"

#include <windows.h>
#include <olectl.h>
#include <stdio.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>

/*
 * 函数名: ChangeLine
 * 接口: ChangeLine()
 * -------------------------------------------------
 * 改变连接线颜色
 */
void ChangeLine() {
	clinecolor ^= 1;
}

/*
 * 函数名: forward
 * 接口: forward(double distance)
 * -------------------------------------------------
 * 画线
 */
void forward(double distance) {
	//利用角度画线
	double dx = distance * cos(Angle / 180 * pi);
	double dy = distance * sin(Angle / 180 * pi);
	DrawLine(dx, dy);
}

/*
 * 函数名: move
 * 接口: move(double distance)
 * -------------------------------------------------
 * 画笔移动
 */
void move(double distance) {
	double dx = distance * cos(Angle / 180 * pi);
	double dy = distance * sin(Angle / 180 * pi);
	//获取画笔当前位置
	double x = GetCurrentX();
	double y = GetCurrentY();
	//移动画笔
	MovePen(x + dx, y + dy);
}

/*
 * 函数名: imLine
 * 接口: imLine(double dis)
 * -------------------------------------------------
 * 画虚线
 */
void imLine(double dis) {
	//虚线
	int n = floor(fabs(dis / dot));
	double onestep = (dis>=0) ? dot : -dot;
	int flag = 1, i;
	//间隔一定距离画线
	for (i = 0; i < n; i++) {
		if (flag) {
			forward(onestep);
			flag = 0;
		}
		else {
			//点划线
			move(onestep);
			flag = 1;
		}
	}
}

/*
 * 函数名: Drawline
 * 接口: Drawline(int flag,double dis)
 * -------------------------------------------------
 * 根据参数画实线虚线
 */
void Drawline(int flag, double dis) {
	//画实线虚线
	if (!flag)forward(dis);
	else imLine(dis);
}

/*
 * 函数名: Draw
 * 接口: Draw(node *g, int isDraw)
 * -------------------------------------------------
 * 根据结构体画出指定图形
 */
void Draw(node* g, int isDraw) {
	//设置模式
	SetPenSize(g->pensize);
	SetPenColor(scolor[g->color]);

	//是否擦除
	if (isDraw)SetEraseMode(FALSE);
	else SetEraseMode(TRUE);

	double r, a, R, width, height;
	double x = g->cx, y = g->cy;
	width = g->width;
	height = g->height;

	//选择哪种图形
	switch (g->n) {
	case 1:
		//矩形
		r = sqrt(width * width + height * height) / 2;
		//旋转图形
		a = g->angle / 180 * pi + pi - atan(height / width);
		MovePen(x + r * cos(a), y + r * sin(a));
		Angle = g->angle;
		Drawline(g->frame, width);
		//顺时针旋转
		Angle -= 90;
		Drawline(g->frame, height);
		Angle -= 90;
		Drawline(g->frame, width);
		Angle -= 90;
		Drawline(g->frame, height);
		break;

	case 2:
		//圆角矩形
		R = height / 4;
		double w = width - 2 * R, h = height;
		r = sqrt(w * w + h * h) / 2;
		a = g->angle / 180 * pi + pi - atan(h / w);
		//画旋转图形
		MovePen(x + r * cos(a), y + r * sin(a));
		//画圆角
		DrawArc(R, 90 + g->angle, 90);
		Angle = g->angle - 90;
		//圆角半径为宽的1/4
		Drawline(g->frame, h - 2 * R);
		//圆角半径为宽的1/4
		DrawArc(R, 180 + g->angle, 90);
		Angle += 90;
		Drawline(g->frame, w);
		DrawArc(R, 270 + g->angle, 90);
		//逆时针旋转画图形
		Angle += 90;
		Drawline(g->frame, h - 2 * R);
		DrawArc(R, 0 + g->angle, 90);
		Angle += 90;
		Drawline(g->frame, w);
		break;

	case 3:
		//菱形
		r = sqrt(width * width + height * height) / 2;
		a = g->angle / 180 * pi;
		double x1 = width / 2 * cos(a), y1 = width / 2 * sin(a);
		//包含在矩形边框内
		MovePen(x + x1, y + y1);
		a = atan(height / width) / pi * 180;
		Angle = g->angle + 180 - a;
		//前一个参数是否擦除
		Drawline(g->frame, r);
		Angle += 2 * a;
		//前一个参数是否擦除
		Drawline(g->frame, r);
		Angle += 180 - 2 * a;
		//前一个参数是否擦除
		Drawline(g->frame, r);
		Angle += 2 * a;
		//前一个参数是否擦除
		Drawline(g->frame, r);
		break;
	}
}

/*
 * 函数名: DrawArrow
 * 接口: DrawArrow(double bx, double by, double angle,  double length)
 * -------------------------------------------------
 * 画箭头
 */
void DrawArrow(double bx, double by, double angle, double length) {
	//设置角度
	Angle = angle;
	//设置模式
	SetPenSize(1);
	//设置颜色
	SetPenColor(scolor[black]);
	MovePen(bx, by);
	//图标柄
	forward(length);
	double x = GetCurrentX(), y = GetCurrentY();
	//画出箭头的两边
	Angle += 160;
	forward(0.2);
	Angle -= 320;
	MovePen(x, y);
	forward(0.2);
}

/*
 * 函数名: DrawFrame
 * 接口: DrawFrame(node *g, int color)
 * -------------------------------------------------
 * 画图形边框
 */
void DrawFrame(node *g, int color)
{
	//画边框
	SetPenSize(1);
	//定颜色
	SetPenColor(scolor[color]);
	double w = g->width + 0.09, h = g->height + 0.09;

	//根据角度画出边框
	MovePen(g->cx, g->cy);
	Angle = g->angle + 90;
	move(h / 2);
	//角度顺时针旋转90
	Angle -= 90;
	Drawline(g->frame, w / 2);
	//角度顺时针旋转90
	Angle -= 90;
	Drawline(g->frame, h);
	//角度顺时针旋转90
	Angle -= 90;
	Drawline(g->frame, w);
	//角度顺时针旋转90
	Angle -= 90;
	Drawline(g->frame, h);
	//角度顺时针旋转90
	Angle -= 90;
	Drawline(g->frame, w / 2);
}

/*
 * 函数名: Drawedge
 * 接口: Drawedge(node *g)
 * -------------------------------------------------
 * 画图形图标
 */
void Drawedge(node* g) {
	//图标
	if (g == NULL)return;
	double h = g->height / 2, w = g->width / 2;
	//旋转图标
	MovePen(g->cx, g->cy);
	Angle = g->angle + 90;
	move(h);
	SetPenSize(1);
	//颜色为红色
	SetPenColor(scolor[Red]);
	//条码大小
	forward(0.12);
	move(0.12);
	DrawArc(0.06, Angle, 360);

	//颜色黑色
	//缩放图标
	//左面图标
	SetPenColor(scolor[black]);
	Angle -= 90;
	MovePen(g->cx, g->cy);
	//大小
	//方向
	//圆形
	move(w);
	forward(0.05);
	move(0.08);
	DrawArc(0.04, Angle, 360);
	//下方图标
	Angle -= 90;
	MovePen(g->cx, g->cy);
	move(h);
	//大小
	//方向
	//圆形
	forward(0.05);
	move(0.08);
	DrawArc(0.04, Angle, 360);
	//右边图标
	Angle -= 90;
	MovePen(g->cx, g->cy);
	//大小
	//方向
	//圆形
	move(w);
	forward(0.05);
	move(0.08);
	DrawArc(0.04, Angle, 360);
}

/*
 * 函数名: GetAngle
 * 接口: GetAngle(double ax, double ay, double bx, double by)
 * -------------------------------------------------
 * 获取当前直线角度
 */
double GetAngle(double ax, double ay, double bx, double by) {
	//x轴正方向的弧度值(a->b)
	double x = bx - ax;
	double y = by - ay;
	if (x == 0) {
		//平行于y轴额外考虑
		if (y > 0)return pi / 4;
		else return pi * 0.75;
	}
	//其他情况根据正切计算
	if (x > 0)return atan(y / x);
	else return atan(y / x) + pi;
}

/*
 * 函数名: Liner
 * 接口: Liner(cline *l)
 * -------------------------------------------------
 * 画出当前连接线
 */
void Liner(cline *l) {
	//画出连接线
	if (l == NULL)return;
	//设置基本模式
	SetEraseMode(l->erase);
	SetPenSize(l->size);
	SetPenColor(scolor[l->color]);

	MovePen(l->begin->bx, l->begin->by);
	line *temp = l->begin;
	do {
		//根据虚实画出连接线
		if(l->solid)
			DrawLine(temp->ex - temp->bx, temp->ey - temp->by);
		else {
			if (temp->dir == xaxs) {
				Angle = 0;
				imLine(temp->ex - temp->bx);
			}
			else {
				Angle = 90;
				imLine(temp->ey - temp->by);
			}
		}
		temp = temp->next;
	} while (temp != NULL);

	//根据方向画线
	if (l->end->dir == xaxs) {
		if (l->end->ex > l->end->bx) {
			DrawArc(0.05, 180, 360);
			Angle = 160;
			forward(0.2);
			move(-.2);
			Angle -= 320;
			forward(.2);
		}
		else {
			DrawArc(0.05, 0, 360);
			Angle = 20;
			forward(0.2);
			move(-.2);
			Angle -= 40;
			forward(.2);
		}
	}
	else {
		l->middle = NULL;
		//画出末尾线段
		if (l->end->ey < l->end->by) {
			DrawArc(0.05, 90, 360);
			Angle = 70;
			forward(0.2);
			move(-.2);
			Angle += 40;
			forward(.2);
		}
		else {
			DrawArc(0.05, 270, 360);
			Angle = -70;
			forward(0.2);
			move(-.2);
			Angle -= 40;
			forward(.2);
		}
	}
}

int Judge(int dir, line *pre, double x, double y) {
	//判断连接线移动
	double cx = pre->ex, cy = pre->ey;

	if (dir == yaxs) {
		if (x - cx > 0.2 || cx - x > 0.2)
			return xaxs;
		else return yaxs;
	}
	else {
		if (y - cy > 0.2 || cy - y > 0.2)
			return yaxs;
		else return xaxs;
	}
}

/*
 * 函数名: AddLine
 * 接口: AddLine(int dir,line *pre, double x, double y)
 * -------------------------------------------------
 * 增添连接线
 */
line* AddLine(int dir, line* pre, double x, double y) {
	//获取空间
	line* l = (line*)malloc(sizeof(line));
	//前置节点
	l->before = pre;
	pre->next = l;
	//结构体赋值
	l->bx = pre->ex;
	l->by = pre->ey;
	l->ex = x;
	l->ey = y;
	l->dir = dir;
	l->next = NULL;
	return l;
}

/*
 * 函数名: Arrow
 * 接口: Arrow(int dir, line *pre, double prex, double prey, double x, double y, int erase)
 * -------------------------------------------------
 * 动态画出连接线
 */
void Arrow(int dir, line* pre, double prex, double prey, double x, double y, int erase) {
	//动态线
	if (pre == NULL)return;
	//大小
	SetPenSize(1);
	//颜色
	SetPenColor("black");

	double cx = pre->ex, cy = pre->ey;

	//根据前一个位置更新
	//根据方向画线
	if (dir == xaxs) {
		//是否擦除
		SetEraseMode(TRUE);
		//位置
		MovePen(cx, cy);
		DrawLine(prex - cx, 0);
		SetEraseMode(erase);
		MovePen(cx, cy);
		DrawLine(x - cx, 0);
	}
	else {
		//不擦除
		SetEraseMode(TRUE);
		//位置
		MovePen(cx, cy);
		DrawLine(0, prey - cy);
		//是否擦除
		SetEraseMode(erase);
		MovePen(cx, cy);
		DrawLine(0, y - cy);
	}
}

/*
 * 函数名: Fill
 * 接口: Fill(node *g)
 * -------------------------------------------------
 * 填充图形
 */
void Fill(node* g) {
	//填充
	double r, a, R, width, height;
	double x = g->cx, y = g->cy;
	//获取大小
	width = g->width;
	height = g->height;

	//填充密度
	StartFilledRegion(0.5);
	SetPenColor(scolor[g->color]);

	//与Draw函数一样
	//画出图形
	//构建封闭区域
	switch (g->n) {
	case 1:
		//矩形
		r = sqrt(width * width + height * height) / 2;
		a = g->angle / 180 * pi + pi - atan(height / width);
		MovePen(x + r * cos(a), y + r * sin(a));
		//旋转画出区域
		Angle = g->angle;
		Drawline(g->frame, width);
		Angle -= 90;
		Drawline(g->frame, height);
		Angle -= 90;
		Drawline(g->frame, width);
		Angle -= 90;
		Drawline(g->frame, height);
		break;

	case 2:
		//圆角矩形
		R = height / 4;
		double w = width - 2 * R, h = height;
		r = sqrt(w * w + h * h) / 2;
		a = g->angle / 180 * pi + pi - atan(h / w);
		MovePen(x + r * cos(a), y + r * sin(a));
		//画出圆角
		DrawArc(R, 90 + g->angle, 90);
		Angle = g->angle - 90;
		Drawline(g->frame, h - 2 * R);
		DrawArc(R, 180 + g->angle, 90);
		//画出圆角
		Angle += 90;
		Drawline(g->frame, w);
		DrawArc(R, 270 + g->angle, 90);
		Angle += 90;
		Drawline(g->frame, h - 2 * R);
		//画出圆角
		DrawArc(R, 0 + g->angle, 90);
		Angle += 90;
		Drawline(g->frame, w);
		break;

	case 3:
		//菱形
		r = sqrt(width * width + height * height) / 2;
		a = g->angle / 180 * pi;
		double x1 = width / 2 * cos(a), y1 = width / 2 * sin(a);
		MovePen(x + x1, y + y1);
		a = atan(height / width) / pi * 180;
		//根据角度画出菱形
		Angle = g->angle + 180 - a;
		Drawline(g->frame, r);
		Angle += 2 * a;
		Drawline(g->frame, r);
		Angle += 180 - 2 * a;
		Drawline(g->frame, r);
		Angle += 2 * a;
		Drawline(g->frame, r);
		break;
	}
	//结束填充
	EndFilledRegion();
}

/*
 * 函数名: DrawTextEdge
 * 接口: DrawTextEdge(text *t)
 * -------------------------------------------------
 * 画出文本框
 */
void DrawTextEdge(text* t) {

	MovePen(t->cx, t->cy);
	//颜色默认黑色
	SetPenColor(scolor[black]);
	SetPenSize(1);

	//大小根据结构体改变
	Angle = 0;
	imLine(t->width);
	Angle = -90;
	imLine(t->height);
	Angle = 180;
	imLine(t->width);
	Angle = 90;
	imLine(t->height);
}
