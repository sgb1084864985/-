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

/*��̬�ֲ�ȫ�ֱ���*/
static double StatusBarHeight;		// ״̬���߶�
static int	  SaveMode;				// ����״̬
static int    CreateMode = 0;		// �༭״̬��0-����ģʽ��1-�ı�ģʽ��2-ͼ��ģʽ��3-����ģʽ
static int	  FlagSelection = -1;	// Ӧ���ڼ�¼�û�����˵���ķ���ֵ

/*
* ��������DrawMode
* �ӿڣ�DrawMode(double winwidth,double winheight)
* ������������������������������������������������
* ����������ڻ��ƴ������½ǵ�״̬��������ʾ�û�
* ��ǰ������״̬����Ϊ��ɫ��ɫ������ģʽ����ɫ��ɫ
* ���ı�ģʽ����ɫ��ɫ��ͼ��ģʽ���ɫ��ɫ������ģ
* ʽ�ܼ�����ģʽ��ͨ���ı�CreateMode����ֵ���ı�״
* ̬����ʽ��
*/
void DrawMode(double winwidth, double winheight) {
	double ModeCircleX = winwidth / 20,
		ModeCircleY = winheight / 20 + StatusBarHeight, r;	// ״̬���������뾶
	char* ModeInformation, * ModeColor;						// ״̬����������ɫ�ַ���

	DefineColor("FreeModeColor", 0.07, 0.72, 0.96);
	DefineColor("TextModeColor", 1.00, 0.80, 0.26);
	DefineColor("GraphModeColor", 0.10, 0.63, 0.10);
	DefineColor("LineModeColor", 0.87, 0.31, 0.27);

	SetEraseMode(0);

	// ���ݵ�ǰCreateModeֵѡ��ģʽ������������ɫ�ַ�����ֵ
	switch (CreateMode) {
	case 0:default:ModeInformation = "����ģʽ"; ModeColor = "FreeModeColor"; break;
	case 1:ModeInformation = "�ı�ģʽ"; ModeColor = "TextModeColor"; break;
	case 2:ModeInformation = "ͼ��ģʽ"; ModeColor = "GraphModeColor"; break;
	case 3:ModeInformation = "����ģʽ"; ModeColor = "LineModeColor"; break;
	}

	// �趨�뾶������
	r = TextStringWidth(ModeInformation) * 0.6;
	ModeCircleX += r;
	ModeCircleY += GetFontHeight() / 2;

	// �趨��ɫ�����壬�ƶ����ʽ��������ַ����Ļ滭
	StartFilledRegion(0.5);
	SetPenColor(ModeColor);
	SetFont("����");
	MovePen(ModeCircleX, ModeCircleY);
	DrawArc(r, 0, 360);
	EndFilledRegion();
	SetPenColor("Black");
	MovePen(ModeCircleX - TextStringWidth(ModeInformation) * 1.1, ModeCircleY - GetFontHeight() / 5);
	DrawTextString(ModeInformation);
}

/*
* ��������GetSelection
* �ӿڣ�GetSelection()
* ��������������������
* ���ڽ��ֲ�ȫ�ֱ���
* FlagSelection���ݸ�
* �����ļ��ڵĺ�����
*/
int GetSelection() {
	int PresentSelection = FlagSelection;
	FlagSelection = -1;
	return PresentSelection;
}

/*
* ��������DrawMenu
* �ӿڣ�DrawMenu(double winwidth, double winheight)
* ������������������������������������������������
* ����simpleGUI���ƽ����Ϸ��Ĳ˵����������û��ĵ��
* ���м�⣬ʵʱ��¼���°�����Ӧ�Ĳ���ֵ��������
* FlagSelection�С�ͬʱ�ı�CreateMode��ֵ��ʾ���ģ
* ʽ�����ڱȽϼ򵥵ĺ�������Music��ֱ�ӽ��е��á�
*/
void DrawMenu(double winwidth, double winheight)
{
	SetEraseMode(0);
	// ��������simpleGUI�涨��Ԥ�ȶ����8���˵�������
	static char* menuListFile[] = { "�ļ�",
		"�½�  | Ctrl-N",
		"��  | Ctrl-O",
		"����  | Ctrl-S",
		"���Ϊ",
		"����",
		"���",
		"�˳�  | Ctrl-Q" };
	static char* menuListEdit[] = { "�༭",
		"����  | Ctrl-C",
		"ճ��  | Ctrl-V",
		"����  | Ctrl-X",
		"ɾ��  | Ctrl-D",
		"ѡ��" };
	static char* menuListMode[] = { "ģʽ",
		"�ı�ģʽ",
		"ͼ��ģʽ",
		"����ģʽ" };
	static char* menuListText[] = { "�ı�",
		"�����ı�",
		"�ı�ѡ��",
		"�༭�ı�",
		"�л���ɫ",
		"�����С",
		"������" };
	static char* menuListGraphic[] = { "ͼ��",
		"�������",
		"��������",
		"����Բ��",
		"�л���ɫ",
		"�л���ʵ",
		"�л���ϸ",
		"�л����",
		/*"�л���˸"*/ };
	static char* menuListLine[] = { "����",
		"�л���ɫ",
		"�л���ʵ",
		"�л���ϸ" };
	static char* menuListMusic[] = { "����",
		"�㽭��ѧУ��",
		"The Congress Reel",
		"Yellow",
		"ֹͣ����" };
	static char* menuListHelp[] = { "����",
		"ʹ�ð���",
		"����ȼ�",
		"�������" };

	double h; // �ؼ��߶�
	double w; // �ؼ����
	double x = 0;// ������ť��xλ�ã��𽥵���
	double y = winheight;// ��ť��yλ�ã�����
	double MenuButtonInterval = 0.6;// ��ť֮��ļ��
	int    ReturnFlag = 0;// ����ȷ�����ս��ĸ���ť�ķ���ֵ����FlagSelection
	int	   MenuLen;       // ����ĳ���˵�ѡ��ĸ���
	int    FileSelection, EditSelection, ModeSelection, TextSelection, GraphicSelection,
		LineSelection, MusicSelection, HelpSelection;// �ֱ��¼ÿһ����ť�ķ���ֵ
	static int IsInText = 0;// ��־�Ƿ����ı�ģʽ���Ա䶯�ı��˵���ť�е�����

	// �Զ�����ɫ
	DefineColor("ButtonFrameCommon", 0.07, 0.72, 0.96);
	DefineColor("ButtonFrameHot", 0.20, 0.60, 1.00);
	setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	// �����������С
	SetFont("΢���ź�");
	SetPointSize(18);

	//�ڹ涨���������С�󣬶Կؼ���ȡ��߶ȸ�ֵ
	w = TextStringWidth(menuListFile[0]) * 1.5;
	h = GetFontHeight() * 1.2;

	// ���ƴ��ڱ���
	SetWindowTitle("С���㷨����ͼ���ƹ���-���ƽ���");

	// ���λ��Ʋ˵���������˵���ť
	drawMenuBar(0, y - h, winwidth, h);

	// File �˵�
	setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);
	FileSelection = menuList(GenUIID(0), x, y - h, w, TextStringWidth(menuListFile[1]) * 1.2, h,
		menuListFile, sizeof(menuListFile) / sizeof(menuListFile[0])) + 10;
	if (FileSelection > 10) {
		ReturnFlag = 1;
		CreateMode = 0;// תΪ����ģʽ
		if (FileSelection == 13 || FileSelection == 14) {
			SaveMode = 1;// תΪ�ѱ���״̬
		}
	}

	// Edit �˵�
	EditSelection = menuList(GenUIID(0), x + w + MenuButtonInterval, y - h, w, TextStringWidth(menuListEdit[1]) * 1.2, h,
		menuListEdit, sizeof(menuListEdit) / sizeof(menuListEdit[0])) + 20;
	if (EditSelection > 20) {
		ReturnFlag = 2;
		if (EditSelection == 25) {
			CreateMode = 0; // תΪ����ģʽ
		}
	}

	// Mode �˵�
	ModeSelection = menuList(GenUIID(0), x + 2 * (w + MenuButtonInterval), y - h, w, TextStringWidth(menuListMode[1]) * 1.2, h,
		menuListMode, sizeof(menuListMode) / sizeof(menuListMode[0])) + 30;
	if (ModeSelection > 30) {
		ReturnFlag = 3;
		switch (ModeSelection) {
		case 31: if (CreateMode != 1) IsInText = 0; CreateMode = 1; break;// תΪ�ı�ģʽ
		case 32: CreateMode = 2; break;// תΪͼ��ģʽ
		case 33: CreateMode = 3; break;// תΪ����ģʽ
		}
	}

	// Text �˵�
	if (CreateMode == 1) {// ���������ı�ģʽ���ڽ��õĻ�ɫ״̬
		setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);
		MenuLen = sizeof(menuListText) / sizeof(menuListText[0]);
	}
	else {
		setMenuColors("Gray", "White", "Gray", "White", 1);
		MenuLen = 1;
	}
	menuListText[1] = IsInText ? "�˳��ı�" : "�����ı�";// �����Ƿ����ı�ģʽ�仯����
	TextSelection = menuList(GenUIID(0), x + 3 * (w + MenuButtonInterval), y - h, w, TextStringWidth(menuListText[1]) * 1.2, h,
		menuListText, MenuLen) + 40;
	if (TextSelection > 40 && CreateMode == 1) {// ���ҽ��������ı�ģʽ������˵����µİ�ť�Ż�����Ӧ
		switch (TextSelection) {
		case 41:
			IsInText = IsInText ? 0 : 1;// �����һ����ť��ת��IsInText���̶�ת������ģʽ
			if (IsInText)	CreateMode = 1;
			else			CreateMode = 0;
			ReturnFlag = 4;
			break;
		case 42: case 43: case 44: case 45: case 46: ReturnFlag = 4; break;
		default:break;
		}
	}
	setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	// Graphic �˵�
	if (CreateMode == 2) {// ��������ͼ��ģʽ���ڽ��õĻ�ɫ״̬
		setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);
		MenuLen = sizeof(menuListGraphic) / sizeof(menuListGraphic[0]);
	}
	else {
		setMenuColors("Gray", "White", "Gray", "White", 1);
		MenuLen = 1;
	}
	GraphicSelection = menuList(GenUIID(0), x + 4 * (w + MenuButtonInterval), y - h, w, TextStringWidth(menuListGraphic[1]) * 1.2, h,
		menuListGraphic, MenuLen) + 50;
	if (GraphicSelection > 50 && CreateMode == 2) {// ���ҽ�������ͼ��ģʽ������˵����µİ�ť�Ż�����Ӧ
		ReturnFlag = 5;
	}
	setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	// Line �˵�
	if (CreateMode == 3) {// ������������ģʽ���ڽ��õĻ�ɫ״̬
		setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);
		MenuLen = sizeof(menuListLine) / sizeof(menuListLine[0]);
	}
	else {
		setMenuColors("Gray", "White", "Gray", "White", 1);
		MenuLen = 1;
	}
	LineSelection = menuList(GenUIID(0), x + 5 * (w + MenuButtonInterval), y - h, w, TextStringWidth(menuListLine[1]) * 1.2, h,
		menuListLine, MenuLen) + 60;// ���ҽ�����������ģʽ������˵����µİ�ť�Ż�����Ӧ
	if (LineSelection > 60 && CreateMode == 3) {
		ReturnFlag = 6;
	}
	setMenuColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	// Music �˵�
	MusicSelection = menuList(GenUIID(0), x + 6 * (w + MenuButtonInterval), y - h, w, TextStringWidth(menuListMusic[2]) * 1.2, h,
		menuListMusic, sizeof(menuListMusic) / sizeof(menuListMusic[0])) + 70;
	if (MusicSelection > 70) {// �����ֽ��в�����ֹͣ����
		Music(MusicSelection - 70);
		CreateMode = 0;// תΪ����ģʽ
		ReturnFlag = 7;
	}

	// Help �˵�
	HelpSelection = menuList(GenUIID(0), x + 7 * (w + MenuButtonInterval), y - h, w, TextStringWidth(menuListHelp[1]) * 1.2, h,
		menuListHelp, sizeof(menuListHelp) / sizeof(menuListHelp[0])) + 80;
	if (HelpSelection > 80) {
		CreateMode = 0; // תΪ����ģʽ
		ReturnFlag = 8;
	}

	// ͨ����ReturnFlag����switch-case����¼ǡ���ķ���ֵ������һЩ�������SaveModeΪ0
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
	default: FlagSelection = -1; break;
	}
	// ����ReturnFlag
	ReturnFlag = 0;
}


/*
* ��������DrawStatusBar
* �ӿڣ�DrawStatusBar(LinkStatusInfo PresentLinkStatusInfo, double winwidth, double winheight)
* ��������������������������������������������������������������������������������������������
* ͨ�����ݽ����ĺ�����Ϣ�Ľṹ��ָ�룬�˽��ļ�������ǰ�û��Ƿ�ѡ����Ԫ�أ���ѡ�У����һ���˽�
* ѡ��Ԫ�ص�������ԡ���������Ϣȫ��չ���ڵײ�����Ϣ�����Ա��û������������ļ��Ƿ��ڱ���״̬��
* �������ļ��������*��־��ʾδ���棬û�����ʾ�ѱ����档
*/
void DrawStatusBar(LinkStatusInfo PresentLinkStatusInfo, double winwidth, double winheight) {
	static char PositionX[100], PositionY[100];// ������ѡԪ�ص�λ������
	static char MouseX[100], MouseY[100];// ��������λ������
	static char ItemWidth[100], ItemHeight[100];// ����Ԫ�صĿ����߶�
	static char LineBeginX[100], LineBeginY[100];// ������������λ������
	static char LineEndX[100], LineEndY[100];// ���������յ��λ������
	static char PenSize[100];// ���滭�ʴ�С
	static char* IfFrameOrFilled[] = { "��","��" };// ������ʾ��˸�������Ǻͷ�
	static char* DottedOrFull[] = { "����", "ʵ��" };// ������ʾ���͵���ʵ
	static char* SelectMsg[] = { "| ��ǰѡ��Ԫ��Ϊ���Σ������ϢΪ��",
		"| ��ǰѡ��Ԫ��Ϊ���Σ������ϢΪ��",
		"| ��ǰѡ��Ԫ��ΪԲ�Ǿ��Σ������ϢΪ��",
		"| ��ǰѡ��Ԫ��Ϊ�����ߣ������ϢΪ��",
		"| ��ǰѡ��Ԫ��Ϊ�ı��������ϢΪ��" };// ������ʾ��ʾ��Ϣ

	// �Զ�����ɫ���趨�������ֺš�������ɫ
	DefineColor("StatusBarBackground", 0.07, 0.72, 0.96);
	SetPenColor("StatusBarBackground");

	// �趨��Ϣ���߶�
	StatusBarHeight = GetFontHeight() * 1.3;

	// ������Ϣ������-����
	drawRectangle(0, 0, winwidth, StatusBarHeight, 1);

	SetFont("Arial");
	SetPointSize(12);
	SetPenColor("White");

	// ���Ȼ����ļ����뱣��״̬
	MovePen(TextStringWidth(" "), StatusBarHeight / 2);
	DrawTextString(PresentLinkStatusInfo->filename);
	if (!SaveMode) {
		DrawTextString(" * ");
	}

	// ���ݽṹ�е�Type���з��������
	switch (PresentLinkStatusInfo->type) {
	case 0://ͼ�Σ����Σ����Ρ�Բ�Ǿ��Σ�����ʾ���ࡢλ�����ꡢ��ȸ߶ȡ���ɫ���͡����ʴ�С���Ƿ����
		sprintf(PositionX, "%.2f", PresentLinkStatusInfo->PresentObject->cx);
		sprintf(PositionY, "%.2f", PresentLinkStatusInfo->PresentObject->cy);
		sprintf(ItemWidth, "%.2f", PresentLinkStatusInfo->PresentObject->width);
		sprintf(ItemHeight, "%.2f", PresentLinkStatusInfo->PresentObject->height);
		sprintf(PenSize, "%d", PresentLinkStatusInfo->PresentObject->pensize);
		DrawTextString(SelectMsg[PresentLinkStatusInfo->PresentObject->n]);
		DrawTextString("λ�����꣺( ");
		DrawTextString(PositionX);
		DrawTextString(",");
		DrawTextString(PositionY);
		DrawTextString(")  ��ȣ�");
		DrawTextString(ItemWidth);
		DrawTextString("  �߶ȣ�");
		DrawTextString(ItemHeight);
		DrawTextString("  ��ɫ��");
		DrawTextString(scolor[PresentLinkStatusInfo->PresentObject->color]);
		DrawTextString("  ���ͣ�");
		DrawTextString(DottedOrFull[PresentLinkStatusInfo->PresentObject->frame]);
		DrawTextString("  ���ʴ�С��");
		DrawTextString(PenSize);
		DrawTextString("  �Ƿ���䣺");
		DrawTextString(IfFrameOrFilled[PresentLinkStatusInfo->PresentObject->filled]);
		break;
	case 1://�����ߣ���ʾ��ʼ��������ֹ����
		sprintf(LineBeginX, "%.2f", PresentLinkStatusInfo->PresentObject->cl->begin->bx);
		sprintf(LineBeginY, "%.2f", PresentLinkStatusInfo->PresentObject->cl->begin->by);
		sprintf(LineEndX, "%.2f", PresentLinkStatusInfo->PresentObject->cl->end->ex);
		sprintf(LineEndY, "%.2f", PresentLinkStatusInfo->PresentObject->cl->end->ey);
		DrawTextString(SelectMsg[3]);
		DrawTextString("��ʼ���꣺( ");
		DrawTextString(LineBeginX);
		DrawTextString(",");
		DrawTextString(LineBeginY);
		DrawTextString(")  ��ֹ���꣺( ");
		DrawTextString(LineEndX);
		DrawTextString(",");
		DrawTextString(LineEndY);
		DrawTextString(")");
		break;
	case 2://�ı�����ʾλ�����ꡢ��ȸ߶�����ɫ����
		sprintf(PositionX, "%.2f", PresentLinkStatusInfo->PresentText->cx);
		sprintf(PositionY, "%.2f", PresentLinkStatusInfo->PresentText->cy);
		sprintf(ItemWidth, "%.2f", PresentLinkStatusInfo->PresentText->width);
		sprintf(ItemHeight, "%.2f", PresentLinkStatusInfo->PresentText->height);
		DrawTextString(SelectMsg[4]);
		DrawTextString("λ�����꣺( ");
		DrawTextString(PositionX);
		DrawTextString(",");
		DrawTextString(PositionY);
		DrawTextString(")  ��ȣ�");
		DrawTextString(ItemWidth);
		DrawTextString("  �߶ȣ�");
		DrawTextString(ItemHeight);
		DrawTextString("  ��ɫ��");
		DrawTextString(scolor[PresentLinkStatusInfo->PresentText->color]);
		/*DrawTextString("  ���壺");
		DrawTextString(fontstyle[PresentLinkStatusInfo->PresentText->fontstyle]);*/
		break;
	case 3:// δѡ�У���ʾ���λ��
		sprintf(MouseX, "%.2f", PresentLinkStatusInfo->mouseX);
		sprintf(MouseY, "%.2f", PresentLinkStatusInfo->mouseY);
		DrawTextString("| ��ǰδѡ���κ�Ԫ��");
		DrawTextString(" ��ǰ���λ�ã�( ");
		DrawTextString(MouseX);
		DrawTextString(",");
		DrawTextString(MouseY);
		DrawTextString(")");
		break;
	default:// ����������ʾ������Ϣ
		DrawTextString("| ��Ϣ������");
		break;
	}
}

/*
* ��������DrawIconButton
* �ӿڣ�DrawIconButton(double winwidth, double winheight)
* ��������������������������������������������������������
* ����simpleGUI���ƽ����Ҳ��ͼ�깤������ʵ�ֳ��ù��ܵĿ�
* ��ʹ�á�ʵ��ԭ���ȡ��DrawMenu��ͬ�ķ�����������������
* FlagSelection������ͳһ����ͳһ�Խӡ�
*/
void DrawIconButton(double winwidth, double winheight) {
	static double IconX, IconY, IconH, IconW;// ͼ�갴ť��λ�á��߶�����
	static double IconButtonInterval = 0.4;// ͼ�갴ť�Ĵ�ֱ����
	static int	  ButtonSelection[5] = { 0,0,0,0,0 };// ��¼ѡ��
	static char* ButtonName[] = {// ͼ�갴ť������
		"�������",
		"��������",
		"����Բ��",
		"���ҳ��",
		"�����ļ�"
	};

	// �趨�������ֺ�
	SetFont("΢���ź�");
	SetPointSize(16);

	// ȷ��ͼ�갴ť�Ĳ���
	IconX = winwidth * 8 / 9 - TextStringWidth(ButtonName[0]);
	IconY = winheight * 3 / 4;
	IconH = GetFontHeight() * 1.5;
	IconW = TextStringWidth(ButtonName[0]) * 1.2;

	// �Զ�����ɫ
	DefineColor("ButtonFrameCommon", 0.07, 0.72, 0.96);
	DefineColor("ButtonFrameHot", 0.20, 0.60, 1.00);
	setButtonColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	// �����ĸ���ť���Ҽ�¼����ֵ
	ButtonSelection[0] = button(GenUIID(0), IconX, IconY, IconW, IconH, ButtonName[0]);
	ButtonSelection[1] = button(GenUIID(0), IconX, IconY - (IconH + IconButtonInterval), IconW, IconH, ButtonName[1]);
	ButtonSelection[2] = button(GenUIID(0), IconX, IconY - 2 * (IconH + IconButtonInterval), IconW, IconH, ButtonName[2]);
	ButtonSelection[3] = button(GenUIID(0), IconX, IconY - 3 * (IconH + IconButtonInterval), IconW, IconH, ButtonName[3]);
	ButtonSelection[4] = button(GenUIID(0), IconX, IconY - 4 * (IconH + IconButtonInterval), IconW, IconH, ButtonName[4]);


	if (ButtonSelection[0] && CreateMode == 2) {// ���ҽ�������ֵ��Ϊ0�Ҵ���ͼ��ģʽ��������Ӧ
		FlagSelection = 51;
	}
	if (ButtonSelection[1] && CreateMode == 2) {// ���ҽ�������ֵ��Ϊ0�Ҵ���ͼ��ģʽ��������Ӧ
		FlagSelection = 52;
	}
	if (ButtonSelection[2] && CreateMode == 2) {// ���ҽ�������ֵ��Ϊ0�Ҵ���ͼ��ģʽ��������Ӧ
		FlagSelection = 53;
	}
	if (ButtonSelection[3]) {// ͬʱ����Ϊ����ģʽ
		FlagSelection = 16;
		CreateMode = 0;
	}
	if (ButtonSelection[4]) {// ͬʱ����Ϊ����ģʽ���趨����״̬Ϊ1
		FlagSelection = 13;
		CreateMode = 0;
		SaveMode = 1;
	}
}

void ChangeMode(int a) {
	CreateMode = a;
}