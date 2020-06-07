#include "conio.h"

#include <windows.h>
#include <olectl.h>
#include <stdio.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>
#include <stdlib.h>
#include <assert.h>
#include "imgui.h"
#include "edit.h"
#include "draw.h"

/*常量定义*/
#define false 0
#define true 1

/*类型别名*/
typedef _Bool status; //状态status分为false和true两种
typedef node* Link;


/*函数接口*/

status Save(char* filename, Link head, text* thead, cline* LineHead, int len, int SaveCopyAs); //保存函数，参数分别为：文件名，主链表头指针，文本链表头指针，链表长度
status Load(char* filename, Link ptrh, text* ptrth, cline* LineHead);	//读取函数，参数分别为：文件名，指向主链表头指针的指针，指向文本链表头指针的指针

int GetListLen(Link head);                                     //返回链表的长度

void Close(Link head, text* texthead,cline* HeadLine);                         //破坏链表后退出程序

void _Destroy(Link head, text* texthead, cline* LineHead);  //破坏链表

void CLine_Destroy(cline* cl);

/*内部用函数*/

static void TextBox_Destroy(text* head);

static void MainChain_Destroy(Link head);

static void SaveTBox(FILE* fp, text* thead);

static void SaveMainChain(FILE* fp, Link head);

static void SaveLine(FILE* fp, struct ConnectLine* cline);

static text* LoadTBox(FILE* fp);

void AllLines_Destroy(cline* head);

static void AllLines_Save(FILE* fp, cline* head);

static cline* AllLines_Load(FILE* fp);

static struct ConnectLine* LoadLine(fp);

static Link LoadMainChain(FILE* fp, int ListLen);


/*函数实现*/

/*
 * 函数名: Save
 * 接口: Save(filename, head, textboxhead, len, SaveCopyAs);
 * ------------------------
 * 该函数将给定的图形（矩形、
 * 菱形、连接线等元素）链表
 * 和文本框链表写入到指定文
 *件中。
 * 其中若SaveCopyAs为1，
 * 为另存为状态，不允许
 * 有同一地址相同文件名，
 * 否则返回false；
 *
 * 若SaveCopyAs为0，
 * 为继续保存状态,
 * 将覆盖之前的同名文件。
 * 若保存成功，将返回true。
 */

status Save(char* filename, Link head, text* thead, cline* LineHead, int len, int SaveCopyAs) {
	if (len == 0) return true;
	if (head && thead && filename) {

		FILE* fp;

		if (SaveCopyAs && (fp = fopen(filename, "rb")) != NULL) {
			fclose(fp);
			return false;
		}

		if ((fp = fopen(filename, "wb")) == NULL)
			return false;

		/*写入主链表长度*/
		fwrite(&len, sizeof(int), 1, fp);

		SaveMainChain(fp, head);
		SaveTBox(fp, thead);
		AllLines_Save(fp, LineHead);

		fclose(fp);
		return true;
	}
	else return false;
}

/*
 * 函数名: SaveTBox
 * 接口: SaveTBox(fp, textboxhead);
 * ------------------------
 * 该函数将给定的文本框链表
 * 及其长度按照指定文件指针
 * 写入到文件中
 */

static void SaveTBox(FILE* fp, text* thead) {
	text* p = thead->next;

	int len = 0;					   //len 用于记录链表长度

	long pos = ftell(fp),endpos;		//用pos记录开始位置

	fwrite(&len, sizeof(int), 1, fp);  //先写入占据储存位置

	while (p != NULL) {
		fwrite(p, sizeof(*p), 1, fp);
		p = p->next;
		len++;                         //得到长度
	}

	endpos = ftell(fp);    
	fseek(fp, pos, SEEK_SET);            //跳到开头处
	fwrite(&len, sizeof(int), 1, fp);    //写入长度
	fseek(fp, endpos, SEEK_SET);         //跳回末尾
}

/*
 * 函数名: SaveSaveLine
 * 接口: SaveLine(FILE* fp, struct ConnectLine* cline);
 * ------------------------
 * 该函数将给定的连接线链表
 * 及其总括结构按照指定文件
 * 指针写入到文件中
 */

static void SaveLine(FILE* fp, struct ConnectLine* cline) {
	struct Line* p = cline->begin;

	fwrite(cline, sizeof(*cline), 1, fp);

	while (p != NULL) {
		fwrite(p, sizeof(*p), 1, fp);
		p = p->next;
	}
}

/*
 * 函数名: SaveMainChain
 * 接口: SaveMainChain(FILE* fp, Link head);
 * ------------------------
 * 该函数将给定的图形元素链
 * 表按照指定文件指针写入到
 * 文件中
 */

static void SaveMainChain(FILE* fp, Link head) {
	if (head) {
		Link p = head->next;
		while (p != NULL) {
			fwrite(p, sizeof(*p), 1, fp);
			p = p->next;
		}
	}
}

/*
 * 函数名: Load
 * 接口: Load(filename, head, textboxhead);
 * ------------------------
 * 该函数将指定文件中图形
 *（矩形、菱形、连接线等元
 * 素）数据和文本框数据读
 * 入到内存中并形成链表，
 * 并连接到传入的head指针
 * 和textboxhead指针后
 * 若载入成功，将返回true;
 * 若文件无法打开或数据加
 * 载失败，将返回false.
 */

status Load(char* filename, Link ptrh, text* ptrth, cline* LineHead) {

	int ListLen;

	FILE* fp;

	if ((fp = fopen(filename, "rb")) == NULL) return false;//文件打开失败

	fread(&ListLen, sizeof(int), 1, fp);

	if (ListLen <= 0 || ListLen >= 100) return false;

	ptrh->next = LoadMainChain(fp, ListLen);

	ptrth->next = LoadTBox(fp);

	LineHead->next = AllLines_Load(fp);

	fclose(fp);
	return true;
}

/*
 * 函数名: LoadMainChain
 * 接口: LoadMainChain(fp, ListLen);
 * ------------------------
 * 该函数将给定长度（ListLen)
 * 的图形（矩形、菱形、连接线
 * 等元素）数据从指定文件指针
 * 中读入到内存中并形成链表，
 * 并返回头部指针。
 * 若长度为0，则返回NULL.
 */

static Link LoadMainChain(FILE* fp, int ListLen) {
	Link last = NULL, head = NULL, result = NULL;

	for (int j = 0; j < ListLen; j++) {
		assert(head = (Link)malloc(sizeof(*head)));

		fread(head, sizeof(*head), 1, fp);

		if (j == 0) result = head;

		if (last) last->next = head;

		last = head;

	}

	if (last) last->next = NULL;

	return result;
}

/*
 * 函数名: LoadLine
 * 接口: LoadLine(fp);
 * ------------------------
 * 该函数将连接线数据(struct Line)
 * 及其总括结构（struct ConnectLine)
 * 从指定文件指针中读入到
 * 内存中并形成链表，并返
 * 回头部指针。
 * 若长度为0，则返回NULL.
 */

static struct ConnectLine* LoadLine(FILE* fp) {
	struct ConnectLine* cline = (struct ConnectLine*)malloc(sizeof(*cline));
	assert(cline);

	fread(cline, sizeof(*cline), 1, fp);

	struct Line* last = NULL, * p = NULL;

	cline->begin = NULL;


	for (int j = 0; j < cline->num; j++) {
		p = (struct Line*)malloc(sizeof(struct Line));

		fread(p, sizeof(struct Line), 1, fp);

		if (j == 0) cline->begin = p;

		if (last)  last->next = p;

		p->before = last;

		last = p;
	}

	if(last) last->next = NULL;

	cline->end = last;

	return cline;
}

/*
 * 函数名: LoadTBox
 * 接口: LoadTBox(fp);
 * ------------------------
 * 该函数先读取链表长度
 *（ListLen)，然后根据
 * 链表长度（ListLen)将
 * 文本框数据从指定文件
 * 指针中读入到内存中并
 * 形成链表，并返回头部指针。
 * 若长度为0，则返回NULL.
 */

static text* LoadTBox(FILE* fp) {
	text* last = NULL, * head = NULL,*p=NULL;

	int ListLen;
	fread(&ListLen, sizeof(int), 1, fp);
	if (ListLen == 0) return NULL;

	

	for (int j = 0; j < ListLen; j++) {

		assert(p = (text*)malloc(sizeof(*head)));

		fread(p, sizeof(*p), 1, fp);

		if (j == 0) head = p;

		if (last) last->next = p;

		last = p;
	}

	if (last) last->next = NULL;

	return head;
}

/*
 * 函数名: _Destroy
 * 接口: _Destroy(head, texthead);
 * ------------------------
 * 该函数通过传入的图
 * 形元素链表头指针和
 * 文本框链表头指针逐
 * 步释放链表空间，其中
 * 头指针不会被释放。
 */

void _Destroy(Link head, text* texthead, cline* LineHead) {
	MainChain_Destroy(head);
	TextBox_Destroy(texthead);
	AllLines_Destroy(LineHead);
}
/*
 * 函数名: MainChain_Destroy
 * 接口: MainChain_Destroy(head);
 * ------------------------
 * 该函数通过传入的图
 * 形元素链表头指针逐
 * 步释放链表空间，如
 * 果某元素含有连接线，
 * 先释放其连接线链表。
 * 其中头指针不会被释放。
 */

static void MainChain_Destroy(Link head) {
	if (head) {
		Link tmp = NULL, h = head;
		head = head->next;
		while (head) {
			tmp = head;
			head = head->next;
			free(tmp);
		}
		h->next = NULL;
	}
}
/*
 * 函数名: CLine_Destroy
 * 接口: CLine_Destroy(cl)
 * ------------------------
 * 该函数通过传入的连接线
 * 链表总括结构指针逐步释
 * 放链表空间。
 */

void CLine_Destroy(cline* cl) {
	if (cl) {
		line* tmp = NULL, * head = cl->begin;
		while (head) {
			tmp = head;
			head = head->next;
			free(tmp);
		}
		free(cl);
	}
}

/*
 * 函数名: TextBox_Destroy
 * 接口: TextBox_Destroy(head);
 * ------------------------
 * 该函数通过传入的文
 * 本框链表头指针逐步
 * 释放链表空间其中头
 * 指针不会被释放。
 */

static void TextBox_Destroy(text* head) {
	if (head) {
		text* tmp = NULL,*h=head;
		head = head->next;
		while (head) {
			tmp = head;
			head = head->next;
			free(tmp);
		}
		h->next = NULL;
	}
}

/*
 * 函数名: Close
 * 接口: Close(head, texthead);
 * ------------------------
 * 该函数通过传入的图
 * 形元素链表头指针和
 * 文本框链表头指针逐
 * 步释放链表空间，最
 * 后退出程序。
 */

void Close( Link head, text* texthead,cline* LineHead) {
	_Destroy(head,texthead,LineHead);
	free(head);
	free(texthead);
	free(LineHead);
	ExitGraphics();
}

/*
 * 函数名: GetListLen
 * 接口: GetListLen(head);
 * ------------------------
 * 该函数通过传入的图
 * 形元素链表头指针返
 * 回链表的总长度。
 */

int GetListLen(Link head) {
	assert(head);
	Link p = head->next;
	int i = 0;
	while (p) {
		p = p->next;  //野指针
		i++;
	}
	return i;
}

void AllLines_Destroy(cline* head) {
	cline* temp = head->next,*p;
	while (temp) {
		p = temp->next;
		CLine_Destroy(temp);
		temp = p;
	}
	head->next = NULL;
}

static void AllLines_Save(FILE* fp, cline* head) {
	cline* p = head->next;

	int len = 0;					   //len 用于记录链表长度

	long pos = ftell(fp), endpos;		//用pos记录开始位置

	fwrite(&len, sizeof(int), 1, fp);  //先写入占据储存位置

	while (p != NULL) {
		SaveLine(fp, p);
		p = p->next;
		len++;                         //得到长度
	}

	endpos = ftell(fp);
	fseek(fp, pos, SEEK_SET);            //跳到开头处
	fwrite(&len, sizeof(int), 1, fp);    //写入长度
	fseek(fp, endpos, SEEK_SET);         //跳回末尾
}

static cline* AllLines_Load(FILE* fp) {
	cline* last = NULL, * head = NULL, * p = NULL;

	int ListLen;
	fread(&ListLen, sizeof(int), 1, fp);
	if (ListLen == 0) return NULL;


	for (int j = 0; j < ListLen; j++) {

		p = LoadLine(fp);

		if (j == 0) head = p;

		if (last) last->next = p;

		last = p;
	}

	if (last) last->next = NULL;

	return head;
}