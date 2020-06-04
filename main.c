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

/*��������*/
#define TIME1_BLINK  1     /*300ms��ʱ���¼���־��*/
#define TIME2_BLINK  2     /*200ms��ʱ���¼���־��*/
#define TIME3_BLINK  3     /*200ms��ʱ���¼���־��*/
#define TIME4_BLINK  4     /*200ms��ʱ���¼���־��*/

const int ms200 = 200;
const int ms300 = 300;
const int ms600 = 600;
static bool isBlink = FALSE;

/*ȫ�ֱ���*/
static double winwidth, winheight;
static int isButton, TextBlink, TextContinue, isFrame, EditText,CurrentMode,sbutton,istext,switchtext;
static node* prehead, * p, * copytemp;							//ͷָ�룬��ǰ����ָ�룬������ָ��
static text* pretext, * tt;
static cline* cl;                                               //������ͷָ��
static StatusInfo SI;
char __filename[100] = "new.dat";

//For MenuProcess
static double cx, cy, prex, prey, premx, premy;
static int textdir, newflag1 = 0, newflag2, newtext, textmove, textzoom,
dir, flag, linemove, aline, isline, isdraw, linezoom,
zoom, zoomdir, ismove, isrotate, newflag3;
node* temppl;
static line* pre, * head, * tl;
static node* pl = NULL;
static text* qt = NULL;
double mx, my;
int MenuChanged = 1;
FILE* tempfp;

//____________________________________________________________________________________________________
static enum __page { StartPage_,SavePage_,DrawPage_, HelpPage_, ExamplePage_, LoadPage_,LastPage_ } Page, thisPage, prePage;//ҳ��ö�ٱ�������

/*��������*/
void DisplayClear();
void Display();
void MouseEventProcess(int x, int y, int button, int event);
void CharEventProcess(char key);
enum __page DrawMenuProcess();
void KeyboardEventProcess(int key, int event);
void Default_Color_Define();
int DrawMode_Mouse_Process(int x, int y, int button, int event);
int Display_Draw(node* head, node* p, text* t,text* tt);
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

void WhereToExit(void) {
	FILE* fp = fopen("ExitInfo", "w");
	char buf[100];
	sprintf(buf,"%s %d", __FILE__, __LINE__);
	fputs(buf, fp);
	fclose(fp);
}

/*����ʵ��*/

/*
 * ������: Main
 * �ӿ�: Main();
 * ------------------------
 * ������ں�����
 * ��Ҫ������ͼ��
 * ����ĳ�ʼ����
 * Ԥ����ɫ�Ķ��塢
 * ���ڴ�С�Ļ�ȡ
 * ȫ�ֱ���prehead
 * ��pretext����ͼ��
 * ������ı�������
 * ��ͷָ�룩�Ŀռ�
 * ���䡢�ص�������
 * ע���Լ���Ϣ����
 * �ṹ�ĳ�ʼ����
 */
void Main() {
	//InitConsole(); //������
	InitGraphics();

	Default_Color_Define();  //������ɫ

	winwidth = GetWindowWidth();
	winheight = GetWindowHeight();

	prehead = (node*)malloc(sizeof(node));

	assert(prehead);  //����

	prehead->next = NULL;
	p = NULL;

	pretext = (text*)malloc(sizeof(text));

	assert(pretext);

	pretext->next = NULL;

	cl = (cline*)malloc(sizeof(cline));
	assert(cl);

	SI.filename = __filename;
	SI.PresentObject = p;
	SI.PresentText = pretext;
	SI.type = 3;

	registerMouseEvent(MouseEventProcess); //���

	registerCharEvent(CharEventProcess); // �ַ�

	registerKeyboardEvent(KeyboardEventProcess);// ����

	registerTimerEvent(TimerEventProcess);  //��ʱ��

	Music(2);

	atexit(WhereToExit);

	Display();
}

/*
 * ������: Display
 * �ӿ�: Display();
 * ------------------------
 * �ܵ���ʾ������
 * ����ȫ�ֱ���Page
 * ������ʾ����ҳ��
 * ������ǰ��ʾ����
 * �ķ���ֵ����Page,
 * �����´��Ƿ����
 * ҳ�档
 */
void Display() {
	DisplayClear();

	if (Page == LastPage_) Page = prePage;
	if (thisPage != Page) prePage = thisPage;
	thisPage = Page;
	
	switch (Page) {
	case DrawPage_:
		Page = Display_Draw(prehead, p, pretext,tt);
		break;
	case StartPage_:
		Page = Display_Start(winwidth, winheight);
		break;
	case HelpPage_:
		//printf("Run to here: %s %d\n", __FILE__, __LINE__);
		Page = Display_Help(winwidth, winheight);
		break;
	case ExamplePage_:
		Page = Display_Example();
		break;
	case SavePage_:
		Page = Display_Save(winwidth, winheight, __filename, prehead, pretext);
		break;
	case LoadPage_:
		Page = Display_Load(winwidth, winheight,prehead, pretext, __filename);
		break;
	default:
		ExitGraphics();
	}

	winwidth = GetWindowWidth();
	winheight = GetWindowHeight();

}

/*
 * ������: MouseEventProcess
 * �ӿ�: MouseEventProcess( x, y, button, event)
 * ------------------------
 * �ܵ����ص�������
 * ����ȫ�ֱ���Page
 * ������ʾ����ҳ��
 * ������ǰ�ص�����
 * �ķ���ֵ����Page,
 * �����´��Ƿ����
 * ҳ�档
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
		ExitGraphics();
	}
}

/*
 * ������: TimerEventProcess
 * �ӿ�: TimerEventProcess(timerID)
 * ------------------------
 * �ܵļ�ʱ���ص�������
 * ����ȫ�ֱ���Page
 * ������ʾ����ҳ��
 * ������ǰ�ص�����
 * �ķ���ֵ����Page,
 * �����´��Ƿ����
 * ҳ�档
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
		ExitGraphics();
	}
	Display();
}

/*
 * ������: CharEventProcess
 * �ӿ�: CharEventProcess(char key)
 * ------------------------
 * �ܵ��ַ��ص�������
 * ����ȫ�ֱ���Page
 * ��Ӧ��ͬҳ�档
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
 * ������: KeyboardEventProcess
 * �ӿ�: KeyboardEventProcess(key��event);
 * ------------------------
 * �ܵļ��̻ص�������
 * ����ȫ�ֱ���Page
 * ��Ӧ��ͬҳ�档
 */
void KeyboardEventProcess(int key, int event) {
	uiGetKeyboard(key, event); // GUI��ȡ����
	if (Page==DrawPage_ && EditText && event == KEY_DOWN) {
			int l = strlen(tt->s);
			switch (key) {
			case VK_DELETE:
				DeleteChar(tt);
				break; //ɾ��
			case VK_LEFT:
				if (tt->position >= 0) {
					if (tt->s[tt->position] < 0)tt->position -= 2;
					else tt->position -= 1;
				}
				break; //����
			case VK_RIGHT:
				if (tt->position < l - 1) {
					if (tt->s[tt->position + 1] < 0)tt->position += 2;
					else tt->position += 1;
				}
				break; //����
			case VK_UP:
				if (tt->position >= tt->linenum - 1) {
					tt->position -= tt->linenum;
					if (JudgeChinese(tt))tt->position -= 1;
				}

				break; //����
			case VK_DOWN:
				if (tt->position + tt->linenum <= l - 1) {
					tt->position += tt->linenum;
					if (JudgeChinese(tt))tt->position += 1;
				}
				//else tt->position = tt->linenum - 1;
				break; //����
			}

	}
	Display(); // ˢ����ʾ
}
	


/*
 * ������: Default_Color_Define
 * �ӿ�: Default_Color_Define();
 * ------------------------
 * ����Ԥ�õ���ɫ
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
 * ������: Display_Draw
 * �ӿ�: Display_Draw(head,p,t,tt);
 * ------------------------
 * ����ҳ�����ʾ������
 * head��t�ֱ�ͼ������
 * ���ı��������ͷָ�룬
 * pΪ��ǰ��ͼ�ζ���ָ�룬
 * ttΪ��ǰ���ı������
 * ָ�룬���ư����˵�����
 * ״̬����ͼ�깤������
 * ״̬���Լ�ͼ�Ρ��ı���
 * �����ߵ���ʾ��
 */
int Display_Draw(node* head, node* p, text* t,text* tt) {

	enum __page result;

	DrawList(head);

	if (p != head)Drawedge(p);

	DisplayText(t,tt);

	DrawMenu(winwidth, winheight);

	DrawIconButton(winwidth, winheight);

	result=DrawMenuProcess();

	DrawStatusBar(&SI, winwidth, winheight);

	DrawMode(winwidth, winheight);

	return result;
}

/*
 * ������: DrawMode_Mouse_Process
 * �ӿ�: DrawMode_Mouse_Process(x,y,button,event);
 * ------------------------
 * ����ҳ������ص�
 * ��������Ҫ���������ܣ�
 * һ�ǽ��ջ���ҳ���
 * �˵�ѡ���������Ӧ
 * �ķ�Ӧ��������Ӧ����
 * �����������������
 * ͼ�ε��϶�����ת��ѡ��
 * ����ȡ�
 */
int DrawMode_Mouse_Process(int x, int y, int button, int event) {

	mx = ScaleXInches(x); //pixels --> inches
	my = ScaleYInches(y); //pixels --> inches

	SI.mouseX = mx;
	SI.mouseY = my;

	
	switch (event) {
	case BUTTON_DOWN:

		if (EditText == 0) SI.type = 3;  //�༭״̬�£�״̬���ı�

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

			if (istext) {            //��ʾѡ���ˡ��༭�ı���
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
					if (EditText && qt != tt) {             //����ֱ���˳��༭ģʽ
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
				if (!isdraw && (temppl = SearchLine(prehead, mx, my))) {  //�ظ���������Ч��
					pl = temppl;
					linezoom = 1;
					SI.PresentObject = pl;
					SI.type = 1;
					CurrentMode = LINE;
					break;
				}
				if (!isdraw && (temppl = TrvealLine(prehead, mx, my))) { //�ظ���������Ч��
					pl = temppl;
					tl = Trveal(pl->cl, mx, my);
					if (tl && tl != pl->cl->begin) {
						linemove = 1;
					}
					SI.PresentObject = pl;
					SI.type = 1;
					CurrentMode = LINE;
					break;
				}

				int linedir = JudgeEdge(p, mx, my);

				if (p && p->cl == NULL && (linedir == left || linedir == right || linedir == down)) {
					p->cl = (cline*)malloc(sizeof(cline));
					memset(p->cl, 0, sizeof(p->cl));
					p->cl->blineangle = linedir; //�����ظ����ã�Ч�ʵ��£�line 374)��JudgeEdge(p, mx, my) �����Ϊlinedir(�Ѹģ�
					isdraw = 1;
					cx = mx; cy = my;
					prex = cx; prey = cy;
					head = (line*)malloc(sizeof(line));
					if (head == NULL) assert(head);
					pre = head;
					head->before = NULL;
					head->next = NULL;
					head->bx = mx;
					head->ey = my;
					head->ex = mx;
					head->ey = my;
					if (linedir == left || linedir == right) head->dir = xaxs; //���Ϊδ����״̬
					else head->dir = yaxs;

					p->cl->begin = head;  //����
					p->cl->end = pre;
					p->cl->color = Red;
					p->cl->size = 1;
					p->cl->solid = 1;
					p->cl->erase = 0;     //����
					p->cl->num = 1;

					flag = 1;

					SI.PresentObject = p;
					SI.type = 1;
					CurrentMode = LINE;
					break;
				}
			}

			if (!ismove) {
				if (temppl = Search(prehead, mx, my)) {
					p = temppl;  //�ظ���������Ч��
					ismove = 1;
					premx = mx;
					premy = my;
					SI.PresentObject = p;
					SI.type = 0;
					CurrentMode = GRAPH;
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

			//�ƶ�������
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
					/*ChangeMode(4);*/  //��ʱ��׸���Խ���
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

			if (isline && isdraw) {

				JudgeConnect(prehead, p);//���ô���

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
				/*DisplayClear();
				Display();*/
				//Drawedge(p);      //��취�����ƺ���������display��
			}

		case LINE:
			if (isline) {

				if (linemove) {
					if (pl->connect && tl == pl->cl->end)
						break;

					JudgeConnect(prehead, pl);

					/*	DisplayClear();
						cl->erase = 1;*/
					MoveLine(tl, mx, my);

					break;
				}
				if (linezoom) {
					if (pl->connect)
						break;

					JudgeConnect(prehead, pl);

					ZoomLine(pl->cl->end, mx, my);

					break;
				}
				if (isdraw) {

					if (dir == yaxs) {
						if (dir != Judge(dir, pre, mx, my) && flag == 0) {
							dir = xaxs;
							cy = prey;
							/*		if (pre == head && pre->dir == 0) {
										pre->dir = dir;
										pre->bx = pre->ex;
										pre->by = pre->ey;
										pre->ex = mx;
										pre->ey = my;
									}*/
									/*else */
							pre = AddLine(dir, pre, cx, cy);
							p->cl->end = pre;
							p->cl->num++;
						}
						else {
							pre->ey = my;
							flag = 0;
						}
					}
					else {
						if (dir != Judge(dir, pre, mx, my) && flag == 0) {
							dir = yaxs;
							cx = prex;
							pre = AddLine(dir, pre, cx, cy);
							p->cl->end = pre;
							p->cl->num++;
						}
						else {
							pre->ex = mx;
							flag = 0;
						}
					}
					prex = mx; prey = my;
				}
			}
			break;
		}
	}
	return DrawPage_;
}

/*
 * ������: DrawMode_Time_Process
 * �ӿ�: DrawMode_Time_Process(timerID);
 * ------------------------
 * ����ҳ��ļ�ʱ���ص�������
 * ��Ҫ���������ܣ�
 * һ�ǽ���ͼ�ζ����
 * �����˸�����ǽ���
 * �ı���ͼ�����˸
 */
int DrawMode_Time_Process(int timerID) {
	bool erasemode;
	static int fcolor;

	switch (timerID) {
	case TIME1_BLINK:    /*300ms�߿���˸��ʱ��*/
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

	switch (sbutton) {
	case 11:
		_Destroy(prehead, pretext);
		strcpy(__filename, "new.dat");
		break;
	case 12:
		return LoadPage_;
	case 13:

		if (tempfp = fopen(__filename, "rb")) {
			fclose(tempfp);
			if (strcmp("new.dat", __filename)) {
				Save(__filename, prehead, pretext, GetListLen(prehead), 0);
				break;
			}
		}
		//����ļ������ڣ���ת��case14,ת�����Ϊ����
	case 14:
		return SavePage_;
	case 15:
		return StartPage_;
	case 16:
		_Destroy(prehead, pretext);
		break;
	case 17:
		Close(prehead, pretext);
		break;
	case 21:
		//copy
		if (SI.type == 0 || p) {
			if (copytemp) free(copytemp);
			copytemp = Copy(p);
		}
		break;
	case 22:
		//paste
		if (copytemp) {
			temppl = Copy(copytemp);
			Create(prehead, temppl);
			p = temppl;
		}
		break;
	case 23:
		//cut
		if (SI.type == 0 || p) {
			if (copytemp) free(copytemp);
			copytemp = Copy(p);
			p = Delete(prehead, p);
			SI.type = 3;
		}
		break;
	case 24:
		//delete
		if (SI.type == 1 || isline) {
			if (p) {
				CLine_Destroy(p->cl);
				p->cl = NULL;
			}
		}
		else if (SI.type == 2 || tt) {
			DeleteText(pretext, tt);
		}
		else if (SI.type == 0 || p) p = Delete(prehead, p);
		SI.type = 3;
		break;
	case 25:   //ѡ���ô��д���ȶ
		isline = 0;
		istext = 0;
		break;
	case 31:
		if (CurrentMode == _TEXT) break;
		CurrentMode = _TEXT;
		tt = NULL;
		SI.type = 3;
		newflag1 = 0;
		newflag2 = 0;
		istext = 0;                 //�ı�����Ҫ���༭��־
		switchtext = 1;             //�ı�ѡ���־
		cancelTimer(TIME4_BLINK);
		break;
	case 32:
		if(EditText||istext) EditTextCancel
		isline = 0;
		CurrentMode = GRAPH;
		break;
	case 33:
		if (EditText||istext) EditTextCancel
		isline = 1;
		CurrentMode = LINE;
		break;
	case 41:
		//createtext
		//newflag3 ^= 1;
		//if (newflag3) {
		//	newtext = 1;
		//	newflag1 = 0;
		//	newflag2 = 1;
		//	ChangeMode(1);
		//}
		//else {
		//	CurrentMode = FREE;
		//	tt = NULL;
		//	SI.type = 3;
		//	cancelTimer(TIME4_BLINK);
		//	ChangeMode(0);
		//}
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
			istext = 0;         //ȡ���༭
			ChangeMode(1);
		}
		break;
	case 43:
		//edittext
		if (istext == 0&&EditText==0) {
			istext = 1;
			switchtext = 0;     //ȡ��ѡ��
			ChangeMode(1);
		}
		break;
	case 44:
		if (switchtext && tt) {
			tt->precolor = (tt->precolor + 1) % 9 + 1;
			tt->color = tt->precolor;
		}
		break;
	case 45:
		if (switchtext && tt) {
			tt->fontstyle = (tt->fontstyle + 1) % 10 + 1;
		}
		break;
	case 51:
		//����
		p = CreateNew(prehead, 1);
		DrawList(prehead);
		SI.PresentObject = p;
		SI.type = 0;
		break;
	case 52:
		//����
		p = CreateNew(prehead, 3);
		DrawList(prehead);
		SI.PresentObject = p;
		SI.type = 0;
		break;
	case 53:
		//Բ��
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
	case 61: //������ɫ����ʵ����ϸ
		if (p && p->cl) {
			p->cl->color = (p->cl->color + 1) % 9 + 1;
		}
		break;
	case 62:
		if (p && p->cl) {
			p->cl->solid ^= 1;
		}
		break;
	case 63:
		if (p && p->cl) {
			p->cl->size = (p->cl->size + 1 <= 4) ? (p->cl->size + 1) : 1;
		}
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
		//case 45:
		//	//line
		//	isline ^= 1;
		//	ChangeLine();����changeline�ȱ���
		//	break;
	default:
		if (sbutton == 0 || sbutton == -1)
			MenuChanged = 0;
		break;
	}
	return DrawPage_;
}