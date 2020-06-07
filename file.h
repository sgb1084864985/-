#pragma once

#include "draw.h"

#define false 0
#define true 1

typedef _Bool status;
typedef node* Link;

//函数接口：

status Save(char* filename, Link head, text* thead, cline* LineHead, int len, int SaveCopyAs);  //保存函数，参数分别为：文件名，主链表头指针，文本链表头指针，链表长度

status Load(char* filename, Link ptrh, text* ptrth, cline* LineHead);            //读取函数，参数分别为：文件名，指向主链表头指针的指针，指向文本链表头指针的指针

int GetListLen(Link head);                                     //返回链表的长度

void Close(Link head, text* texthead,cline* LineHead);                         //破坏链表后退出程序

void _Destroy(Link head, text* texthead, cline* LineHead);

void CLine_Destroy(cline* cl);