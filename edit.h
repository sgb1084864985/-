#ifndef _edit_h
#define _edit_h

#include <stdio.h>
#include <stdlib.h>
#include<math.h>
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

//the sample of text when add a new text
static text sample = { 0,0,0,0," ",1,0,-1,1,black,gray,1,NULL };

void Zoom(node *p, int dir, double x, double y);

void MoveWholeLine(cline* LineHead, double dx, double dy);

void Move(node *p, double prex, double prey, double x, double y);

void Rotate(node *p, double x, double y);

node * Copy(node *p);

void Create(node *head, node *t);

node * CreateNew(node * prehead, int n);

node * Delete(node *head, node *p);

void DrawList(node* head);

line *Trveal(cline *cl, double x, double y);

cline* TrvealLine(cline* LineHead, double x, double y);

void MoveLine(line *l, double x, double y);

void ZoomLine(line *l, double x, double y);

int JudgeEdge(node *p, double x, double y);

void AddCline(cline* head, cline* new);

node *Search(node *head, double x, double y);

cline* SearchLine(cline* LineHead, double x, double y);

double Commute(double x, double y, double x0, double y0, double k);

int InRect(double cx, double cy, double w, double h, double x, double y);

int InRegion(double cx, double cy, double w, double h, double x, double y, double angle);

int InArc(double cx, double cy, double x, double y, double r, double angle, double d);

text * AddText(text *pretext, double cx, double cy);

void DisplayText(text* pretext, text* tt);

void DeleteChar(text* t);

void AddChar(text* t, char key);

void DrawString(text* t);

int ZoomText(text* t, double x, double y, int dir);

void MoveText(text* t, double prex, double prey, double x, double y);

int JudgeText(text* t, double x, double y);

text* DeleteText(text* pretext, text* tt);

text* SearchText(text* pretext, double x, double y);

void Cursor(text* t, double w, int k);

int JudgeChinese(text* t);

text* CopyText(text* t);

void PasteText(text* pretext, text* t);

cline* CopyLine(cline* l);

#endif
