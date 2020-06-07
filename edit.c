#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include "graphics.h"
#include "genlib.h"
#include "conio.h"
#include "draw.h"
#include "menu.h"

#include <windows.h>
#include <olectl.h>
#include <stdio.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>
#include <assert.h>
#include <edit.h>

//————————————————————————————————图形函数
/*
 * 函数名: Zoom
 * 接口: Zoom(node *p, int dir, double x, double y)
 * -------------------------------------------------
 * 缩放当前图形
 */
void Zoom(node* p, int dir, double x, double y) {

	double r = sqrt((p->cx - x) * (p->cx - x) + (p->cy - y) * (p->cy - y));
	//与中心距离
	double angle = GetAngle(p->cx, p->cy, x, y) - p->angle / 180 * pi;
	//与中心连线角度
	double w = fabs(r * cos(angle));
	//改后的宽
	double h = fabs(r * sin(angle));
	//改后的高

	//判断缩放方向
	if (dir == down) {
		p->height = (h - 0.09) * 2;
	}
	else {
		p->width = (w - 0.09) * 2;
	}

}

/*
 * 函数名: Move
 * 接口: Move(node *p, double prex, double prey, double x, double y)
 * -------------------------------------------------
 * 移动当前图形
 */
void Move(node* p, double prex, double prey, double x, double y) {
	//中心相对鼠标位置改成
	p->cx += x - prex;
	p->cy += y - prey;
}

/*
 * 函数名: Rotate
 * 接口: Rotate(node *p, double x, double y)
 * -------------------------------------------------
 * 旋转当前图形
 */
void Rotate(node* p, double x, double y) {
	//根据角度旋转
	p->angle = GetAngle(p->cx, p->cy, x, y) / pi * 180 - 90;
}

/*
 * 函数名: Copy
 * 接口: Copy(node *p)
 * -------------------------------------------------
 * 复制当前图形
 */
node* Copy(node* p) {
	//分配内存
	node* pt = (node*)malloc(sizeof(node));
	//复制当前图形所有内容
	pt->n = p->n;
	pt->width = p->width;
	pt->height = p->height;
	pt->angle = p->angle;
	pt->cx = p->cx+0.16;
	pt->cy = p->cy+0.16;
	pt->pensize = p->pensize;
	pt->color = p->color;
	pt->frame = p->frame;
	pt->filled = p->filled;
	pt->angle = 0;
	return pt;
}

/*
 * 函数名: Create
 * 接口: Create(node *head, node *t)
 * -------------------------------------------------
 * 创建已复制图形
 */
void Create(node* head, node* t) {
	node* p = head;
	//插入链表
	while (p->next) {
		p = p->next;
	}
	p->next = t;
	t->next = NULL;
}

void AddCline(cline* head, cline* new) {
	if (head&&new) {
		new->next = head->next;
		head->next = new;
	}
}

/*
 * 函数名: CreateNew
 * 接口: CreateNew(node *head, node *t)
 * -------------------------------------------------
 * 创建已复制图形的链表
 */
node* CreateNew(node* prehead, int n) {
	//分配内存
	node* pt = (node*)malloc(sizeof(node));
	node* temp = prehead;

	//配置相同属性
	pt->n = n;
	pt->width = 2.6;
	pt->height = 1.8;
	pt->angle = 0;
	pt->cx = 4;
	pt->cy = 3;
	pt->pensize = 1;
	pt->color = black;
	pt->frame = 0;
	pt->filled = 0;
	pt->next = NULL;

	while (temp->next) {
		temp = temp->next;
	}
	temp->next = pt;

	//返回创建的结构体
	return pt;
}

/*
 * 函数名: Trveal
 * 接口: Trveal(cline *cl, double x, double y)
 * -------------------------------------------------
 * 搜寻与当前连接线相连的图形
 */
line* Trveal(cline* cl, double x, double y) {
	if (cl == NULL)return NULL;

	line* temp = cl->begin;
	double cx, cy, w, h;;

	//寻找连接线的头节点
	//根据方向判断节点
	while (temp != NULL) {
		if (temp->dir == xaxs) {
			cx = (temp->bx + temp->ex) / 2;
			cy = temp->by;
			w = fabs(temp->bx - temp->ex) - 0.2;
			h = 0.3;
		}
		else {
			cy = (temp->by + temp->ey) / 2;
			cx = temp->bx;
			h = fabs(temp->by - temp->ey) - 0.2;
			w = 0.3;
		}

		//检测连接线
		if (InRect(cx, cy, w, h, x, y)) {
			return temp;
		}

		temp = temp->next;
	}
	return NULL;
}

/*
 * 函数名: Search
 * 接口: Search(node *lhead, double x, double y)
 * -------------------------------------------------
 * 遍历链表搜寻图形
 */
node* Search(node* lhead, double x, double y) {
	node* pt = lhead;
	//遍历链表
	while (pt) {
		if (InRegion(pt->cx, pt->cy, pt->width, pt->height, x, y, pt->angle)) {
			//返回当前图形
			return pt;
		}
		pt = pt->next;
	}

	return NULL;
}

/*
 * 函数名: Delete
 * 接口: Delete(node *head, node *p)
 * -------------------------------------------------
 * 删除当前图形
 */
node* Delete(node* head, node* p) {
	if (p == NULL)return NULL;
	//从链表中删除
	node* t = head;
	while (t->next != p) {
		t = t->next;
	}
	t->next = p->next;
	//释放内存
	free(p);
	return t;
}

/*
 * 函数名: JudgeEdge
 * 接口: JudgeEdge(node *p, double x, double y)
 * -------------------------------------------------
 * 检测图形端点
 */
int JudgeEdge(node* p, double x, double y) {
	if (p == NULL)return 0;
	//根据不同位置返回不同方向
	double w = p->width / 2, h = p->height / 2;
	if (InArc(p->cx, p->cy, x, y, .4, p->angle, w + 0.06)) { //原w+0.06,以下二均是
		//右方向
		return right;
	}
	if (InArc(p->cx, p->cy, x, y, .4, p->angle + 180, w + 0.09)) {
		//左方向
		return left;
	}
	if (InArc(p->cx, p->cy, x, y, .4, p->angle + 270, h + 0.09)) {
		//下方向
		return down;
	}
	if (InArc(p->cx, p->cy, x, y, .5, p->angle + 90, h + 0.08)) {  //原h+0.15
		//上方向
		return ROTATE;
	}
	return 0;
}

//————————————————————————————————连接线函数
/*
 * 函数名: updateline
 * 接口: updateline(node *p)
 * -------------------------------------------------
 * 更新连接线
 */

/*
 * 函数名: DrawList
 * 接口: DrawList(node *head)
 * -------------------------------------------------
 * 绘制图形链表
 */
void DrawList(node* head) {
	node* p = head->next;
	while (p) {
		//遍历链表
		if (p->filled) {
			//是否填充
			Fill(p);
			/*p = p->next;
			continue;*/
		}
		else Draw(p, 1);//画图形

		p = p->next;
	}
}

void MoveWholeLine(cline* LineHead, double dx, double dy) {
	line* temp = LineHead->begin;
	while (temp) {
		temp->bx += dx;
		temp->ex += dx;
		temp->by += dy;
		temp->ey += dy;
		temp = temp->next;
	}
}

void Converse(cline *t, cline *p) {
	line *pl = p->end;
	line *tl = (line *)malloc(sizeof(line));
	t->begin = tl;
	while (pl) {

		tl->bx = pl->ex;
		tl->by = pl->ey;
		tl->ex = pl->bx;
		tl->ey = pl->by;
		tl->dir = pl->dir;
		tl->before = pl->next;
		tl->next = pl->before;

		pl = pl->before;
		tl = tl->next;
	}
}


/*
 * 函数名: TrvealLine
 * 接口: TrvealLine(node *prehead, double x, double y)
 * -------------------------------------------------
 * 遍历链表搜寻选中连接线
 */
cline* TrvealLine(cline* LineHead, double x, double y) {
	cline* temp = LineHead->next;
	//遍历链表
	while (temp) {
		if (Trveal(temp, x, y)) {
			//返回当前相连的图形
			return temp;
		}
		temp = temp->next;
	}
	return NULL;
}

/*
 * 函数名: MoveLine
 * 接口: MoveLine(line *l, double x, double y)
 * -------------------------------------------------
 * 移动选中移动连接线
 */
void MoveLine(line* l, double x, double y) {
	if (l == NULL)return;
	//x方向
	if (l->dir == xaxs) {
		l->by = y;
		l->ey = y;
		//前后节点的位置都要相应改变
		if (l->before != NULL) {
			l->before->ey = y;
		}
		if (l->next != NULL) {
			l->next->by = y;
		}
	}
	else {
		//y方向
		l->bx = x;
		l->ex = x;
		//前后节点的位置都要相应改变
		if (l->before != NULL) {
			l->before->ex = x;
		}
		if (l->next != NULL) {
			l->next->bx = x;
		}
	}
}

/*
 * 函数名: ZoomLine
 * 接口: ZoomLine(line *l, double x, double y)
 * -------------------------------------------------
 * 缩放选中连接线
 */
void ZoomLine(line* l, double x, double y) {
	//判断方向做出相应改变
	if (l->dir == xaxs) {
		l->ex = x;
	}
	else {
		l->ey = y;
	}
}

/*
 * 函数名: SearchLine
 * 接口: SearchLine(node *prehead, double x, double y)
 * -------------------------------------------------
 * 寻找选中缩放连接线
 */
cline* SearchLine(cline* LineHead, double x, double y) {
	cline* temp = LineHead->next;
	while (temp) {
		line* l = temp->end;
		//判断是选中
		if (/*l->ey < l->by &&*/ InArc(l->ex, l->ey , x, y, 0.2, 0, 0)) {  //改
			return temp;
		}
		//else {
		//	if (InArc(l->ex, l->ey + 0.05, x, y, 0.06, 0, 0))
		//		//返回结构体
		//		return temp;
		//}

		temp = temp->next;
	}
	return NULL;
}


//————————————————————————————————处理函数（计算，检测……）
/*
 * 函数名: Commute
 * 接口: Commute(double x, double y, double x0, double y0, double k)
 * -------------------------------------------------
 * 计算点线距离
 */
double Commute(double x, double y, double x0, double y0, double k) {
	//根据点到直线距离公式计算距离
	// y = kx + b
	// (x,y)
	double d = fabs((y - y0 + k * (x0 - x)));
	double t = sqrt(k * k + 1);
	double dis = d / t;
	return dis;
}


/*
 * 函数名: InRect
 * 接口: InRect(double cx, double cy, double w, double h, double x, double y)
 * -------------------------------------------------
 * 检测矩形区域
 */
int InRect(double cx, double cy, double w, double h, double x, double y) {
	//检测与矩形中心距离是否达标
	if (fabs(x - cx) <= w / 2 && fabs(y - cy) <= h / 2) {
		return 1;
	}
	return 0;
}

/*
 * 函数名: InRegion
 * 接口: InRegion(double cx, double cy, double w, double h, double x, double y, double angle)
 * -------------------------------------------------
 * 检测非矩形区域
 */
int InRegion(double cx, double cy, double w, double h, double x, double y, double angle) {
	int a = angle;
	if (a % 90 == 0) {
		//tan(90)不存在，需要单独考虑
		return InRect(cx, cy, w, h, x, y);
	}
	else {
		//检测选中旋转后的图形
		//通过角度的正切值
		double k1 = tan(angle / 180 * pi);
		double k2 = -1 / k1;
		double dh = Commute(cx, cy, x, y, k1);
		double dw = Commute(cx, cy, x, y, k2);
		//画过中心的平行线检测
		if (dh < h / 2 && dw < w / 2)return 1;
	}
	return 0;
}

/*
 * 函数名: InArc
 * 接口: InArc(double cx, double cy, double x, double y, double r, double angle, double d)
 * -------------------------------------------------
 * 检测圆形区域
 */
int InArc(double cx, double cy, double x, double y, double r, double angle, double d) {
	//计算距离
	double dx = d * cos(angle / 180 * pi);
	double dy = d * sin(angle / 180 * pi);
	x -= dx;
	y -= dy;
	//判断距离是否大于半径
	double dis = sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy));
	if (dis > r)return 0;
	else return 1;
}

//————————————————————————————————文本函数
/*
 * 函数名: AddText
 * 接口: AddText(text *pretext, double cx, double cy)
 * -------------------------------------------------
 * 增添新文本框
 */
text* AddText(text* pretext, double cx, double cy) {
	//申请内存
	text* t = (text*)malloc(sizeof(text));
	//复制样例文本
	*t = sample;
	t->cx = cx;
	t->cy = cy;

	text* temp = pretext;
	//插入链表
	while (temp->next) {
		temp = temp->next;
	}
	temp->next = t;
	return t;
}

/*
 * 函数名: DeleteText
 * 接口: DeleteText(text *pretext, text *tt)
 * -------------------------------------------------
 * 删除当前文本框
 */
text* DeleteText(text* pretext, text* tt) {
	if (tt == NULL)return NULL;
	text* t = pretext;
	while (t->next != tt) {
		//删除当前节点
		t = t->next;
		//if (t == NULL)return NULL;
	}
	t->next = tt->next;
	//释放内存
	free(tt);
	return t;
}

/*
 * 函数名: SearchText
 * 接口: SearchText(text *pretext, double x, double y)
 * -------------------------------------------------
 * 搜寻当前文本框
 */
text* SearchText(text* pretext, double x, double y) {
	text* t = pretext->next;
	double cx, cy;
	while (t) {
		cx = t->cx + t->width / 2;
		cy = t->cy - t->height / 2;
		//判断是否在矩形文本框内
		if (InRect(cx, cy, t->width + 0.1, t->height + 0.1, x, y))
			return t;
		t = t->next;
	}
	//不存在则返回空指针
	return NULL;
}

/*
 * 函数名: JudgeText
 * 接口: JudgeText(text *t,double x, double y)
 * -------------------------------------------------
 * 检测移动还是缩放
 */
int JudgeText(text* t, double x, double y) {
	//获取数据
	double cx = t->cx + t->width / 2;
	double cy = t->cy - t->height / 2;
	if (!InRect(cx - 0.03, cy + 0.03, t->width - 0.06, t->height - 0.06, x, y) && InRect(cx + 0.03, cy - 0.03, t->width + 0.06, t->height + 0.06, x, y))
	{
		//内部矩形则执行移动操作
		if (fabs(x - t->cx - t->width) <= 0.3)
			return xaxs;
		else return yaxs;
	}
	//外部矩形则执行缩放操作
	else return Textmove;
}

/*
 * 函数名: JudgeChinese
 * 接口: JudgeChinese(text *t)
 * -------------------------------------------------
 * 判断汉字
 */
int JudgeChinese(text* t) {
	int k = 0;
	for (int i = 0; i <= t->position; i++) {
		if (t->s[i] < 0)k++;
	}
	return k % 2;
}

/*
 * 函数名: MoveText
 * 接口: MoveText(text *t, double prex, double prey, double x, double y)
 * -------------------------------------------------
 * 移动文本框
 */
void MoveText(text* t, double prex, double prey, double x, double y) {
	//根据鼠标位置移动文本框
	t->cx += x - prex;
	t->cy += y - prey;
}

/*
 * 函数名: ZoomText
 * 接口: ZoomText(text *t, double x, double y, int dir)
 * -------------------------------------------------
 * 缩放文本框
 */
int ZoomText(text* t, double x, double y, int dir) {
	//判断移动方向
	if (dir == xaxs) {
		//若文本框过小则不显示
		if (x - t->cx > 0.3) {
			t->width = x - t->cx;
			return 0;
		}
	}
	else {
		//根据鼠标位置确定缩放程度
		if (t->cy - y > 0.24) {
			t->height = t->cy - y;
			return 0;
		}
	}
	return 1;
}

/*
 * 函数名: DisplayText
 * 接口: DisplayText(text *pretext, text *tt)
 * -------------------------------------------------
 * 更新文本框
 */
void DisplayText(text* pretext, text* tt) {
	text* t = pretext->next;

	while (t) {
		//设置橡皮擦模式
		SetEraseMode(0);

		//绘制文本
		DrawString(t);

		t = t->next;
	}

	//绘制当前文本框的边框
	if (tt)DrawTextEdge(tt);
}


/*
 * 函数名: DrawString
 * 接口: DrawString(text *t)
 * -------------------------------------------------
 * 绘制文本
 */
void DrawString(text* t) {
	//设置当前字体颜色
	SetPenColor(scolor[t->color]);

	int k = 1, j = 0, i = 0, flag = 0;
	int l = strlen(t->s);
	char str[10] = "a";
	//使用等宽字体
	//获取每行字符数量
	double w = TextStringWidth(str);
	t->linenum = (t->width - 0.12) / w + 1;
	//使用临时数组进行绘制
	char temp[5] = "";


	//光标
	if (t->position < 0) {
		if (t->show) {
			if (t->width >= 0.2 && t->height >= 0.2)
				Cursor(t, 0.0, k);
		}
	}

	while (i < l) {
		//汉字字符成对处理
		if (t->s[i] < 0 && t->s[i + 1] < 0) {
			temp[0] = t->s[i++];
			temp[1] = t->s[i];
			temp[3] = 0;
			//位于末尾
			if (j == t->linenum - 1) {
				k++;
				j = 2;
				MovePen(t->cx + 0.02, t->cy - k * 0.22);
				if(temp) DrawTextString(temp);
			}
			//正常位置
			else {
				MovePen(t->cx + 0.02 + j * w, t->cy - k * 0.22);
				if (temp) DrawTextString(temp);
				j += 2;
				if (j == t->linenum) {
					j = 0;
					k++;
				}
			}
		}
		//非汉字字符
		else {
			temp[0] = t->s[i];
			temp[1] = 0;
			MovePen(t->cx + 0.02 + j * w, t->cy - k * 0.22);
			if (temp) DrawTextString(temp);
			j += 1;
			if (j == t->linenum) {
				j = 0;
				k++;
			}
		}
		//光标
		if (i == t->position) {
			if (t->show) {
				if (t->width >= 0.2 && t->height >= 0.2)
					Cursor(t, j * w, k);
			}
		}
		i++;
	}

}

/*
 * 函数名: Cursor
 * 接口: Cursor(text *t)
 * -------------------------------------------------
 * 绘制光标
 */
void Cursor(text* t, double w, int k) {
	//闪烁标志
	SetEraseMode(t->blink);
	//颜色为蓝色
	SetPenColor(scolor[Blue]);
	//大小为2
	SetPenSize(2);

	//在显示完字符串之后显示光标
	MovePen(t->cx + 0.04 + w, t->cy - k * 0.22 - 0.02);
	DrawLine(0, 0.2);
	//改回绘制模式
	SetEraseMode(0);
	SetPenColor(scolor[t->color]);
}

/*
 * 函数名: AddChar
 * 接口: AddChar(text *t, char key)
 * -------------------------------------------------
 * 添加字符
 */
void AddChar(text* t, char key) {
	int l = strlen(t->s);
	//当前位置+1
	t->position += 1;
	//循环后移
	for (int i = l; i > t->position; i--) {
		t->s[i] = t->s[i - 1];
	}

	//当前位置插入字符
	t->s[t->position] = key;
	//末尾补0
	t->s[l + 1] = 0;

}

/*
 * 函数名: DeleteCha
 * 接口: DeleteChar(text *t)
 * -------------------------------------------------
 * 删除字符
 */
void DeleteChar(text* t) {
	//已处在最前端则无法删除
	if (t->position < 0)return;
	int l = strlen(t->s);
	int flag = 0;
	//判断是否是中文字符
	if (t->s[t->position] < 0) flag = 1;

	//当前位置循环向后删除一个字符
	for (int i = t->position; i < l - 1; i++) {
		t->s[i] = t->s[i + 1];
	}
	//当前位置-1
	t->position -= 1;
	l -= 1;

	//若是中文，再删除一个字符
	if (flag) {
		MovePen(0, 0);
		DrawLine(2, 2);
		for (int i = t->position; i < l - 1; i++) {
			t->s[i] = t->s[i + 1];
		}
		t->position -= 1;
		l -= 1;
	}
	//末尾补0
	t->s[l] = 0;
}


text* CopyText(text* t) {
	text* pt = (text*)malloc(sizeof(text));
	pt->blink = 0;
	pt->color =t->color;
	pt->show = 0;
	pt->cx = t->cx + 0.16;
	pt->cy = t->cy - 0.16;
	pt->fontstyle = t->fontstyle;
	pt->width = t->width;
	pt->height = t->height;
	pt->linenum = t->linenum;
	strcpy(pt->s, t->s);
	pt->position = strlen(t->s) - 1;
	pt->precolor = t->precolor;
	pt->next = NULL;
	return pt;
}

void PasteText(text* pretext, text* t) {
	text* temp = pretext;
	while (temp->next) {
		temp = temp->next;
	}
	temp->next = t;
	t->next = NULL;
}

cline* CopyLine(cline* l) {
	if (!l) return NULL;
	cline* newone = (cline*)malloc(sizeof(cline));
	assert(newone);
	*newone = *l;

	line* last = NULL, * p = NULL,*lp=l->begin;

	newone->begin = NULL;

	for (int j = 0; j < newone->num; j++) {
		p = (struct Line*)malloc(sizeof(struct Line));
		assert(p);

		*p = *lp;

		p->bx += 0.16;
		p->by += 0.16;
		p->ex += 0.16;
		p->ey += 0.16;

		if (j == 0) newone->begin = p;

		if (last)  last->next = p;

		p->before = last;

		last = p;
		lp = lp->next;
	}

	if (last) last->next = NULL;

	newone->end = last;
	newone->next = NULL;

	return newone;
}