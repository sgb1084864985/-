#include "graphics.h"
#include "extgraph.h"
#include "genlib.h"
#include "simpio.h"
#include "conio.h"
#include "draw.h"
#include "edit.h"
#include "save.h"
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
#include <example.h>

/*常量定义*/
#define TIME1_BLINK  1     /*300ms定时器事件标志号*/
#define TIME2_BLINK  2     /*200ms定时器事件标志号*/
#define TIME3_BLINK  3     /*200ms定时器事件标志号*/
#define TIME4_BLINK  4     /*200ms定时器事件标志号*/

const int ms200 = 200;
const int ms300 = 300;
const int ms600 = 600;
static bool isBlink = FALSE;

/*全局变量*/
static double winwidth, winheight;
static int isButton, TextBlink, TextContinue, isFrame, EditText,CurrentMode,sbutton,istext,switchtext=1;
static node* prehead, * p, * copytemp;							//头指针，当前对象指针，剪贴版指针
static text* pretext, * tt;
static cline* cl;                                               //连接线头指针
static StatusInfo SI;
char __filename[100] = "new.dat";

//For MenuProcess
static double cx, cy, prex, prey, premx, premy;
static int textdir, newflag1 = 0, newflag2, newtext, textmove, textzoom,
dir, flag, linemove, aline, isline=1, isdraw, linezoom,
zoom, zoomdir, ismove, isrotate, newflag3,InsertLine, linewholemove, autoalign__ = 1, autosave = 1, unsave = 1, newfile = 1, MenuChanged = 1;
node* temppl;
static line* pre, * head, * tl;
static node* pl = NULL;
static text* qt = NULL,*copytext,*temptext;
double mx, my;
static cline* CurLine, * LineHead, * templine,*copyline;
FILE* tempfp;

//____________________________________________________________________________________________________
static enum __page { StartPage_,SavePage_,DrawPage_, HelpPage_, ExamplePage_, LoadPage_,LastPage_ } Page, thisPage, prePage;//页面枚举变量设置

/*函数声明*/
void DisplayClear();
void Display();
void MouseEventProcess(int x, int y, int button, int event);
void CharEventProcess(char key);
enum __page DrawMenuProcess();
void KeyboardEventProcess(int key, int event);
void Default_Color_Define();
int DrawMode_Mouse_Process(int x, int y, int button, int event);
int Display_Draw(node* head, node* p, text* t, text* tt, cline* LineHead, cline* presn_line);
int DrawMode_Time_Process(int timerID);
void TimerEventProcess(int timerID);
void cancelTimer(int id);
void startTimer(int id, int timeinterval);

#define EditTextCancel {\
	EditText = 0;\
	istext = 0;\
	tt->color = tt->precolor;\
	tt->show = 0;\
	cancelTimer(TIME4_BLINK);\
}
/*函数实现*/

/*
 * 函数名: Main
 * 接口: Main();
 * ------------------------
 * 程序入口函数，
 * 主要进行了图形
 * 界面的初始化、
 * 预置颜色的定义、
 * 窗口大小的获取
 * 全局变量prehead
 * 和pretext（即图形
 * 链表和文本框链表
 * 的头指针）的空间
 * 分配、回调函数的
 * 注册以及信息数据
 * 结构的初始化。
 */
void Main() {
	//InitConsole(); //调试用
	InitGraphics();

	Default_Color_Define();  //定义颜色

	winwidth = GetWindowWidth();
	winheight = GetWindowHeight();

	prehead = (node*)malloc(sizeof(node));

	assert(prehead);  //报错

	prehead->next = NULL;
	p = NULL;

	pretext = (text*)malloc(sizeof(text));

	assert(pretext);

	pretext->next = NULL;

	LineHead = (cline*)malloc(sizeof(cline));
	assert(LineHead);

	LineHead->next = NULL;

	CurLine = NULL;

	SI.filename = __filename;
	SI.PresentObject = p;
	SI.PresentText = pretext;
	SI.type = 3;

	registerMouseEvent(MouseEventProcess); //鼠标

	registerCharEvent(CharEventProcess); // 字符

	registerKeyboardEvent(KeyboardEventProcess);// 键盘

	registerTimerEvent(TimerEventProcess);  //定时器

	Music(2);

	Display();
}

/*
 * 函数名: Display
 * 接口: Display();
 * ------------------------
 * 总的显示函数，
 * 根据全局变量Page
 * 决定显示何种页面
 * 并将当前显示函数
 * 的返回值赋给Page,
 * 决定下次是否更换
 * 页面。
 */
void Display() {
	DisplayClear();

	if (Page == LastPage_) Page = prePage;
	if (thisPage != Page) prePage = thisPage;
	thisPage = Page;
	
	switch (Page) {
	case DrawPage_:
		Page = Display_Draw(prehead, p, pretext,tt,LineHead,CurLine);
		break;
	case StartPage_:
		Page = Display_Start(winwidth, winheight);
		break;
	case HelpPage_:
		Page = Display_Help(winwidth, winheight);
		break;
	case ExamplePage_:
		Page = Display_Example();
		break;
	case SavePage_:
		Page = Display_Save(winwidth, winheight, __filename, prehead, pretext,LineHead);
		break;
	case LoadPage_:
		Page = Display_Load(winwidth, winheight,prehead, pretext, LineHead,__filename);
		break;
	default:
		Close(prehead, pretext, LineHead);
	}

	winwidth = GetWindowWidth();
	winheight = GetWindowHeight();

}

/*
 * 函数名: MouseEventProcess
 * 接口: MouseEventProcess( x, y, button, event)
 * ------------------------
 * 总的鼠标回调函数，
 * 根据全局变量Page
 * 决定显示何种页面
 * 并将当前回调函数
 * 的返回值赋给Page,
 * 决定下次是否更换
 * 页面。
 */
void MouseEventProcess(int x, int y, int button, int event) {
	uiGetMouse(x, y, button, event);

	Display();

	if (Page == LastPage_) Page = prePage;
	if (thisPage != Page) prePage = thisPage;
	thisPage = Page;

	switch (Page) {
	case DrawPage_:
		Page = DrawMode_Mouse_Process(x, y, button, event);
		break;
	case StartPage_:
		break;
		//...
	case HelpPage_:
		//...
		break;
	case ExamplePage_:
		break;
	case SavePage_:
		break;
	case LoadPage_:
		break;
	default:
		Close(prehead, pretext, LineHead);
	}
}

/*
 * 函数名: TimerEventProcess
 * 接口: TimerEventProcess(timerID)
 * ------------------------
 * 总的计时器回调函数，
 * 根据全局变量Page
 * 决定显示何种页面
 * 并将当前回调函数
 * 的返回值赋给Page,
 * 决定下次是否更换
 * 页面。
 */
void TimerEventProcess(int timerID)
{
	if (Page == LastPage_) Page = prePage;
	if (thisPage != Page) prePage = thisPage;
	thisPage = Page;

	switch (Page) {
	case DrawPage_:
		Page = DrawMode_Time_Process(timerID);
		break;
	case StartPage_:
		break;
		//...
	case HelpPage_:
		//...
		break;
	case ExamplePage_:
		Example_Time_Process(timerID);
		break;
	case SavePage_:
		break;
	case LoadPage_:
		break;
	default:
		Close(prehead, pretext, LineHead);
	}
	Display();
}

/*
 * 函数名: CharEventProcess
 * 接口: CharEventProcess(char key)
 * ------------------------
 * 总的字符回调函数，
 * 根据全局变量Page
 * 对应不同页面。
 */
void CharEventProcess(char key)
{
	uiGetChar(key);
	if (Page == DrawPage_&& EditText) {
		int l = strlen(tt->s);
		if (key == 8) {
			DeleteChar(tt);
		}
		else if (key == 13) {
			istext = 0;
			EditText = 0;
			tt->color = tt->precolor;
			tt->show = 0;
			switchtext = 1;
			ChangeMode(1);
		}
		else {
			AddChar(tt, key);
		}
	}
	Display();
}

/*
 * 函数名: KeyboardEventProcess
 * 接口: KeyboardEventProcess(key，event);
 * ------------------------
 * 总的键盘回调函数，
 * 根据全局变量Page
 * 对应不同页面。
 */
void KeyboardEventProcess(int key, int event) {
	uiGetKeyboard(key, event); // GUI获取键盘
	if (Page==DrawPage_ && EditText && event == KEY_DOWN) {
			int l = strlen(tt->s);
			switch (key) {
			case VK_DELETE:
				DeleteChar(tt);
				break; //删除
			case VK_LEFT:
				if (tt->position >= 0) {
					if (tt->s[tt->position] < 0)tt->position -= 2;
					else tt->position -= 1;
				}
				break; //左移
			case VK_RIGHT:
				if (tt->position < l - 1) {
					if (tt->s[tt->position + 1] < 0)tt->position += 2;
					else tt->position += 1;
				}
				break; //右移
			case VK_UP:
				if (tt->position >= tt->linenum - 1) {
					tt->position -= tt->linenum;
					if (JudgeChinese(tt))tt->position -= 1;
				}

				break; //上移
			case VK_DOWN:
				if (tt->position + tt->linenum <= l - 1) {
					tt->position += tt->linenum;
					if (JudgeChinese(tt))tt->position += 1;
				}
				//else tt->position = tt->linenum - 1;
				break; //下移
			}

	}
	Display(); // 刷新显示
}
	


/*
 * 函数名: Default_Color_Define
 * 接口: Default_Color_Define();
 * ------------------------
 * 定义预置的颜色
 */
void Default_Color_Define() {
	DefineColor("yellow", 0.99, 0.99, 0.01);
	DefineColor("pink", 0.99, 0.41, 0.71);
	DefineColor("pupple", 0.99, 0.01, 0.99);
	DefineColor("deepgreen", 0.18, 0.70, 0.34);
	DefineColor("cyan", 0.01, 0.99, 0.99);
	DefineColor("orange", 0.99, 0.647, 0.01);
	DefineColor("gray", 0.66, 0.67, 0.66);
}

/*
 * 函数名: Display_Draw
 * 接口: Display_Draw(head,p,t,tt);
 * ------------------------
 * 绘制页面的显示函数，
 * head和t分别图形链表
 * 和文本框链表的头指针，
 * p为当前的图形对象指针，
 * tt为当前的文本框对象
 * 指针，绘制包括菜单栏、
 * 状态栏、图标工具栏、
 * 状态球以及图形、文本、
 * 连接线的显示。
 */
int Display_Draw(node* head, node* p, text* t, text* tt, cline* LineHead, cline* presn_line) {
	enum __page result;

	DrawList(head);

	Lines_Draw(LineHead,CurLine);

	if (p != head)Drawedge(p);

	SetFont("宋体");

	DisplayText(t, tt);

	DrawMenu(winwidth, winheight);

	DrawIconButton(winwidth, winheight);

	result = DrawMenuProcess();

	DrawStatusBar(&SI, winwidth, winheight);

	DrawMode(winwidth, winheight);

	return result;
}

/*
 * 函数名: DrawMode_Mouse_Process
 * 接口: DrawMode_Mouse_Process(x,y,button,event);
 * ------------------------
 * 绘制页面的鼠标回调
 * 函数，主要有两个功能：
 * 一是接收绘制页面的
 * 菜单选项，并做出相应
 * 的反应；而是相应绘制
 * 界面的鼠标操作，比如
 * 图形的拖动、旋转、选择、
 * 拉伸等。
 */
int DrawMode_Mouse_Process(int x, int y, int button, int event) {
	mx = ScaleXInches(x); //pixels --> inches
	my = ScaleYInches(y); //pixels --> inches

	SI.mouseX = mx;
	SI.mouseY = my;

	switch (event) {
	case BUTTON_DOWN:

		if (EditText == 0) SI.type = 3;  //编辑状态下，状态不改变

		if (button == LEFT_BUTTON) {


			if (newflag2 && !newflag1) {
				tt = AddText(pretext, mx, my);
				switchtext = 0;
				newtext = 1;
				SI.PresentText = tt;
				SI.type = 2;
				CurrentMode = _TEXT;
				break;
			}

			if (istext) {            //表示选中了“编辑文本”
				qt = SearchText(pretext, mx, my);
				if (qt) {
					tt = qt;
					tt->show = 1;
					tt->color = gray;
					EditText = 1;
					istext = 0;
					startTimer(TIME4_BLINK, 200);
					SI.PresentText = tt;
					SI.type = 2;
					CurrentMode = _TEXT;
					break;
				}
			}

			if (switchtext) {
				qt = SearchText(pretext, mx, my);
				if (qt) {
					if (EditText && qt != tt) {             //可能直接退出编辑模式
						tt->show = 0;
						EditText = 0;
						cancelTimer(TIME4_BLINK);
					}
					tt = qt;
					textdir = JudgeText(tt, mx, my);
					if (textdir == Textmove) {
						textmove = 1;
						prex = mx;
						prey = my;
					}
					else {
						textzoom = 1;
					}
					SI.PresentText = tt;
					SI.type = 2;
					CurrentMode = _TEXT;
					ChangeMode(1);
					break;
				}
			}


			if (isline) {
				if (!isdraw && (templine = SearchLine(LineHead, mx, my))) {  
					CurLine = templine;
					linezoom = 1;
					SI.PresentLine = CurLine;
					SI.type = 1;
					CurrentMode = LINE;
					ChangeMode(3);
					break;
				}
				if (!isdraw && (templine = TrvealLine(LineHead, mx, my))) { 
					CurLine = templine;
					tl = Trveal(CurLine, mx, my);
					if (tl) {
						if (tl != CurLine->begin) {
							linemove = 1;
						}
						else {
							linewholemove = 1;
							premx = mx;
							premy = my;
						}
						SI.PresentLine = CurLine;
						SI.type = 1;
						CurrentMode = LINE;
						ChangeMode(3);
					}
					break;
				}

				if (InsertLine) {
					InsertLine = 0;
					CurLine = NULL;
					SI.type = 3;
					CurLine = (cline*)malloc(sizeof(cline));
					memset(CurLine, 0, sizeof(*CurLine));

					isdraw = 1;
					cx = mx; cy = my;
					prex = cx; prey = cy;
					head = (line*)malloc(sizeof(line));
					assert(head);
					pre = head;
					head->before = NULL;  //assert(head) 保证指针非空
					head->next = NULL;
					head->bx = mx;
					head->by = my;
					head->ex = mx;
					head->ey = my;


					CurLine->begin = head;  //增加
					CurLine->end = pre;
					CurLine->color = Red;
					CurLine->size = 1;
					CurLine->solid = 1;
					CurLine->erase = 0;    
					CurLine->num = 1;

					flag = 1;

					AddCline(LineHead, CurLine);

					CurrentMode = LINE;
					ChangeMode(3);
					break;
				}
			}

			if (!ismove) {
				if (temppl = Search(prehead, mx, my)) {
					p = temppl; 
					ismove = 1;
					premx = mx;
					premy = my;
					SI.PresentObject = p;
					SI.type = 0;
					CurrentMode = GRAPH;
					ChangeMode(2);
					break;
				}
			}

			if (!zoom && p) {
				zoomdir = JudgeEdge(p, mx, my);
				if (zoomdir == left || zoomdir == right || zoomdir == down) {
					zoom = 1;
					SI.PresentObject = p;
					SI.type = 0;
					CurrentMode = GRAPH;
					ChangeMode(2);
					break;
				}

			}

			if (!isrotate && p) {
				if (JudgeEdge(p, mx, my) == ROTATE) {
					isrotate = 1;
					SI.PresentObject = p;
					SI.type = 0;
					CurrentMode = GRAPH;
					break;
				}
			}

			//移动连接线
		}
		break;


	case BUTTON_UP:
		if (my > winheight - 0.3 || MenuChanged) break;

		switch (CurrentMode) {
		case _TEXT:
			if (switchtext) {
				if (textmove) {
					textmove = 0;
				}
				if (textzoom) {
					textzoom = 0;
				}
				break;
			}

			if (newtext) {
				newtext = 0;
				if (!newflag1) {
					newflag2 = 0;
					if (tt->width < 0.3 || tt->height < 0.22) {
						tt = DeleteText(pretext, tt);
						if (tt == NULL || tt == pretext) {
							tt = NULL;
							SI.type = 3;
						}
						break;
					}
					EditText = 1;
					startTimer(TIME4_BLINK, 200);
				}
				else newflag1 = 0;
				break;
			}
			break;
		case GRAPH:
			if (zoom)zoom = 0;

			if (ismove)ismove = 0;

			if (isrotate)isrotate = 0;
			break;
		case LINE:

			if (linemove)linemove = 0;

			if (linezoom)linezoom = 0;

			if (linewholemove) { 
				if (autoalign__) {
					temppl = prehead->next;
					while (temppl) {
						dir = JudgeEdge(temppl, CurLine->begin->bx, CurLine->begin->by);
						switch (dir) {
						case right:
							MoveWholeLine(CurLine, temppl->cx + temppl->width / 2 * cos(pi * temppl->angle / 180) - CurLine->begin->bx, temppl->cy + temppl->width / 2 * sin(pi * temppl->angle / 180) - CurLine->begin->by);
							break;
						case left:
							MoveWholeLine(CurLine, temppl->cx - temppl->width / 2 * cos(pi * temppl->angle / 180) - CurLine->begin->bx, temppl->cy - temppl->width/ 2 * sin(pi * temppl->angle / 180) - CurLine->begin->by);
							break;
						case down:
							MoveWholeLine(CurLine, temppl->cx - temppl->height / 2 * cos(pi * (temppl->angle+90) / 180) - CurLine->begin->bx, temppl->cy - temppl->height/ 2 * sin(pi * (temppl->angle + 90) / 180) - CurLine->begin->by);
							break;
						case ROTATE:
							MoveWholeLine(CurLine, temppl->cx + temppl->height / 2 * cos(pi * (temppl->angle + 90) / 180) - CurLine->begin->bx, temppl->cy + temppl->height / 2 * sin(pi * (temppl->angle + 90) / 180) - CurLine->begin->by);
							break;
						default: break;
						}
						if (dir) break;
						temppl = temppl->next;
					}
				}
				linewholemove = 0; 
			}

			if (isline && isdraw) {

				if (pre->before == NULL) {
					if (fabs(pre->bx - pre->ex) < .2 || fabs(pre->by - pre->ey) < .2) {
						templine = LineHead;
						while (templine) {
							if (templine->next == CurLine) {
								templine->next = CurLine->next;
								break;
							}
							templine = templine->next;
						}
						CLine_Destroy(CurLine);
						CurLine = NULL;
						SI.type = 3;
					}
				}

				isdraw = 0;
			}
			break;
		}

		if (SI.type == 2 || SI.type == 3) {
			p = NULL;
		}
		else if (SI.type != 2) {
			tt = NULL;
		}
		break;

	case MOUSEMOVE:
		switch (CurrentMode) {
		case _TEXT:
			if (newtext) {
				if (tt == NULL) break;
				double w = fabs(mx - tt->cx), h = fabs(my - tt->cy);
				if (w >= 0.2 && h >= 0.2) {
					tt->width = fabs(mx - tt->cx);
					tt->height = fabs(my - tt->cy);
				}
				break;
			}

			if (switchtext) {
				if (textmove) {
					MoveText(tt, prex, prey, mx, my);
					prex = mx;
					prey = my;
					break;
				}
				if (textzoom) {
					if (ZoomText(tt, mx, my, textdir)) {
						textzoom = 0;
					}
					break;
				}
				break;
			}
		case GRAPH:

			if (zoom) {
				Zoom(p, zoomdir, mx, my);
			}

			if (ismove) {
				Move(p, premx, premy, mx, my);
				premx = mx;
				premy = my;

			}

			if (isrotate) {
				Rotate(p, mx, my);
			}

		case LINE:
			if (isline) {

				if (linemove) {

					MoveLine(tl, mx, my);

					break;
				}
				if (linezoom) {

					ZoomLine(CurLine->end, mx, my);

					break;
				}
				if (linewholemove) {
					MoveWholeLine(CurLine, mx - premx, my - premy);
					premx = mx;
					premy = my;
					break;
				}

				if (isdraw) {

					if (flag==1) {
						if (fabs(mx - prex) > fabs(my - prey)) {
							dir = xaxs;
							pre->dir = dir;
							pre->ey = my;
						}
						else {
							dir = yaxs;
							pre->dir = dir;
							pre->ex = mx;
						}
						flag = 0;
					}
					else {
						if (dir == yaxs) {
							if (dir != Judge(dir, pre, mx, my) && flag == 0) {
								dir = xaxs;
								cy = prey;
								pre = AddLine(dir, pre, cx, cy);
								CurLine->end = pre;
								CurLine->num++;
							}
							else {
								pre->ey = my;
								
							}
						}
						else {
							if (dir != Judge(dir, pre, mx, my) && flag == 0) {
								dir = yaxs;
								cx = prex;
								pre = AddLine(dir, pre, cx, cy);
								CurLine->end = pre;
								CurLine->num++;
							}
							else {
								pre->ex = mx;
							}
						}
					}
					prex = mx; prey = my;
				}
			}
		}
		break;
	}
	return DrawPage_;
}

/*
 * 函数名: DrawMode_Time_Process
 * 接口: DrawMode_Time_Process(timerID);
 * ------------------------
 * 绘制页面的计时器回调函数，
 * 主要有两个功能：
 * 一是进行图形对象的
 * 鼠标闪烁，二是进行
 * 文本框图标的闪烁
 */
int DrawMode_Time_Process(int timerID) {
	bool erasemode;
	static int fcolor;

	switch (timerID) {
	case TIME1_BLINK:    /*300ms边框闪烁定时器*/
		if (isFrame) {
			SetEraseMode(isBlink);
			if (fcolor)
				DrawFrame(p, fcolor);
			else
				DrawFrame(p, black);
			fcolor = (fcolor + 1) % 11;
			isBlink = !isBlink;
			SetEraseMode(0);
		}
		break;
	case TIME4_BLINK:
		if (tt == NULL) {
			cancelTimer(TIME4_BLINK);
			break;
		}
		tt->blink ^= 1;
		break;
	default:
		break;
	}
	return DrawPage_;
}

enum __page DrawMenuProcess() {
	sbutton = GetSelection();

	if (sbutton && (EditText || istext)&&(sbutton<40||sbutton>50)) EditTextCancel

	switch (sbutton) {
	case 11:
		if (prehead->next == NULL && pretext->next == NULL && LineHead->next == NULL) break;
		unsave = 1;
		newfile = 1;
		_Destroy(prehead, pretext, LineHead);
		strcpy(__filename, "new.dat");
		break;
	case 12:
		unsave = 1;
		return LoadPage_;
	case 13://保存
		if (prehead->next == NULL && pretext->next == NULL && LineHead->next == NULL) break;
		if (!newfile&&(tempfp = fopen(__filename, "rb"))) {
			unsave = 0;
			fclose(tempfp);
			Save(__filename, prehead, pretext, LineHead, GetListLen(prehead), 0);
			break;
		}
		//如果文件不存在，则转到case14,转入另存为界面
	case 14://另存为
		if (prehead->next == NULL && pretext->next == NULL && LineHead->next == NULL) break;
		return SavePage_;
	case 15:
	case 16:
		if (prehead->next || pretext->next  || LineHead->next)
		{
			if (unsave && autosave) {
				unsave = 0;
				if (!newfile&&(tempfp = fopen(__filename, "rb"))) {
					fclose(tempfp);
					Save(__filename, prehead, pretext, LineHead, GetListLen(prehead), 0);
					unsave = 1;
					_Destroy(prehead, pretext, LineHead);
					if (sbutton == 15) return StartPage_;
					break;
				}
				else {
					newfile = 0;
					return SavePage_;
				}
			}
			else {
				unsave = 1;
				_Destroy(prehead, pretext, LineHead);
			}
		}
		if (sbutton == 15) return StartPage_;
		break;
	case 17:
		if (unsave && autosave&&(prehead->next || pretext->next || LineHead->next)) {
			unsave = 0;
			if (!newfile&&(tempfp = fopen(__filename, "rb"))) {
				fclose(tempfp);
				Save(__filename, prehead, pretext, LineHead, GetListLen(prehead), 0);
				Close(prehead, pretext, LineHead);
				break;
			}
			else {
				return SavePage_;
			}
		}
		else {
			Close(prehead, pretext, LineHead);
		}
		break;
	case 21:
		//copy
		switch (CurrentMode) {
		case _TEXT:
			if (tt == NULL) break;
			if (copytext) {
				free(copytext);
				copytext = NULL;
			}
			copytext = CopyText(tt);
			break;
		case GRAPH:
			if (p == NULL) break;
			if (copytemp) {
				free(copytemp);
				copytemp = NULL;
			}
			copytemp = Copy(p);
			break;
		case LINE:
			if (CurLine) {
				if (copyline) {
					free(copyline);
					copyline = NULL;
				}
				copyline = CopyLine(CurLine);
				break;
			}
		}
		break;
	case 22:
		//paste
		switch (CurrentMode) {
		case _TEXT:
			if (copytext) {
				temptext = CopyText(copytext);
				PasteText(pretext, temptext);
				tt = temptext;
			}
			break;
		case GRAPH:
			if (copytemp) {
				temppl = Copy(copytemp);
				Create(prehead, temppl);
				p = temppl;
			}
			break;
		case LINE:
			if (copyline) {
				templine = CopyLine(copyline);
				templine->next=LineHead->next;
				LineHead->next = templine;
				CurLine = temppl;
			}
			break;
		}
		break;
	case 23:
		//cut
		switch (CurrentMode) {
		case _TEXT:
			if (tt == NULL) break;
			if (copytext) {
				free(copytext);
				copytext = NULL;
			}
			copytext = CopyText(tt);
			tt = DeleteText(pretext, tt);
			SI.type = 3;
			break;
		case GRAPH:
			if (p) {
				if (copytemp) {
					free(copytemp);
					copytemp = NULL;
				}
				copytemp = Copy(p);
				p = Delete(prehead, p);
				SI.type = 3;
			}
			break;
		case LINE:
			if (CurLine) {
				if (copyline) {
					free(copyline);
					copyline = NULL;
				}
				copyline = CopyLine(CurLine);
			}
		}
	case 24:
		//delete
		if (CurrentMode==LINE) {
			if (CurLine) {
				templine = LineHead;
				while (templine) {
					if (templine->next == CurLine) {
						templine->next = CurLine->next;
						break;
					}
					templine = templine->next;
				}
				CLine_Destroy(CurLine);
			}
			CurLine = NULL;
		}
		else if (CurrentMode==_TEXT) {
			DeleteText(pretext, tt);
		}
		else if (CurrentMode==GRAPH) p = Delete(prehead, p);
		SI.type = 3;
		break;
	case 25:   //选择
		istext = 0;
		break;
	case 31:
		if (CurrentMode == _TEXT) break;
		CurrentMode = _TEXT;
		tt = NULL;
		SI.type = 3;
		newflag1 = 0;
		newflag2 = 0;
		istext = 0;                
		switchtext = 1;             //文本选择标志
		cancelTimer(TIME4_BLINK);
		break;
	case 32:
		CurrentMode = GRAPH;
		break;
	case 33:
		isline = 1;
		CurrentMode = LINE;
		break;
	case 41:

		if (newtext == 0 && EditText == 0) {
			newflag1 = 0;
			newflag2 = 1;
			tt = NULL;
			ChangeMode(1);
		}
		break;
	case 42:
		//movetext
		if (switchtext == 0) {
			switchtext = 1;
			istext = 0;         //取消编辑
			ChangeMode(1);
		}
		break;
	case 43:
		//edittext
		if (istext == 0&&EditText==0) {
			istext = 1;
			//switchtext = 0;     //取消选择
			ChangeMode(1);
		}
		break;
	case 44:
		if (switchtext && tt) {
			tt->precolor = (tt->precolor + 1) % 9 + 1;
			tt->color = tt->precolor;
		}
		break;
	case 51:
		//矩形
		p = CreateNew(prehead, 1);
		DrawList(prehead);
		SI.PresentObject = p;
		SI.type = 0;
		break;
	case 52:
		//菱形
		p = CreateNew(prehead, 3);
		DrawList(prehead);
		SI.PresentObject = p;
		SI.type = 0;
		break;
	case 53:
		//圆角
		p = CreateNew(prehead, 2);
		DrawList(prehead);
		SI.PresentObject = p;
		SI.type = 0;
		break;
	case 54:
		//color
		p->color = (p->color + 1) % 9 + 1;
		break;
	case 55:
		//valid or invalid
		if (!p->filled) {
			p->frame ^= 1;
			p->filled = 0;
		}
		break;
	case 56:
		//size
		p->pensize = (p->pensize + 1) % 3 + 1;
		break;
	case 57:
		//fill
		p->frame = 0;
		p->filled ^= 1;
		break;
	case 58:
		//frame
		isFrame ^= 1;
		startTimer(TIME1_BLINK, 150);
		break;
	case 61: //线条颜色、虚实、粗细
		if (CurLine) {
			CurLine->color = (CurLine->color + 1) % 9 + 1;
		}
		break;
	case 62:
		if (CurLine) {
			CurLine->solid ^= 1;
		}
		break;
	case 63:
		if (CurLine) {
			CurLine->size = (CurLine->size + 1 <= 4) ? (CurLine->size + 1) : 1;
		}
		break;
	case 64:
		InsertLine = 1;
		break;
	case 81:
		SetHelpMode(1);
		return HelpPage_;
	case 82:
		SetHelpMode(2);
		return HelpPage_;
	case 83:
		SetHelpMode(3);
		return HelpPage_;
	case 91:
		autoalign__ = !autoalign__;
		break;
	case 92:
		autosave = !autosave;
		break;
	case 93:
		unsave = 1;
		_Destroy(prehead, pretext, LineHead);
	default:
		if (sbutton == 0 || sbutton == -1)
			MenuChanged = 0;
		break;
	}
	return DrawPage_;
}

void besaved() {
	unsave = 0;
	newfile = 0;
}