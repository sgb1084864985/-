//save.c
#include "graphics.h"
#include "extgraph.h"
#include "genlib.h"
#include "simpio.h"
#include "conio.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include <windows.h>
#include <olectl.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>
#include <stdio.h>
#include <assert.h>
#include <io.h>
#include "imgui.h"
#include "file.h"
#include "save.h"

/*读取文件和保存界面，
	支持输入文件名在本地目录存取，
	也支持输入绝对路径+文件名存取。
	支持列出本地目录所有二进制
	文件（.dat）进行存取
*/


//save:
	//文本框：输入文件名（默认为new1或原文件名）/绝对路径
	//按钮1：选择文件名/选择绝对路径
	//按钮2：返回
	//按钮3：保存
	//标签栏1：另存为（文件后缀.prc)
	//标签栏2：文件将保存在本地目录//输入示例"C:\\abc\\xx.prc"//文件名重复！

//load:
	//文本框：输入文件名（默认为new1或原文件名）/绝对路径
	//按钮1：选择文件名/选择绝对路径
	//按钮2：本地文件(s)
	//按钮3：上一张/下一张
	//optional:按钮：最近打开的文件（s)


/*常量定义*/
#define MaxStrLen 80
#define FileLen  20
#define AddressLen 200

/*窗口相对比例*/

#define ButtonW (1./7)
#define ButtonH (0.1)
#define FileButtonH (0.04)
#define BackButtonX (1./7)
#define SaveButtonX (5./7)
#define TextBoxW (5./7)
#define TextBoxX (1./7)
#define TextBoxH (0.17)
#define SaveLabelX (3.3/7)
#define EmptyLineH (0.03)
#define EmptyColumn (0.02)
#define FileRow 9

/*全局变量定义*/

enum __page { StartPage_, SavePage_, DrawPage_, HelpPage_, ExamplePage_, LoadPage_, LastPage_ };

static char labels[2][MaxStrLen] = { "请直接输入文件名（文件存在本地,后缀默认dat）或绝对路径（如\"C:/abc/xx.dat\"）","文件已存在或无法打开！" };
static char filenames[FileRow][FileLen],Isfolder[FileRow], __address[AddressLen] = "*.*" ;  //filenames用于在打开（加载）界面储存一页的文件名信息，
															//每一页最大行数为FileRow，每一个文件名长度不超过FileLen；
															//Isfolder用于记录某行的文件是否是一个文件夹

/*函数接口*/
int Display_Save(double wholeW, double wholeH, char* filename, Link head, text* thead);
int Display_Load(double wholeW, double wholeH, Link ptrh, text* ptrth,char* Name);

/*内部函数*/
static int SetFileNames_(int page, char* relafname, char* baseAddress);
static void AddressCatch(char* base, char* source);


/*函数实现*/

/*
 * 函数名: Display_Save
 * 接口: Display_Save(wholeW, wholeH,filename,head, textboxhead);
 * ------------------------
 * 绘制保存交互界面，默认文件名
 * 为filname,根据head指针和
 * textboxhead指针实现另存为功能。
 * wholeW和wholeH为窗口大小。
 */

int Display_Save(double wholeW, double wholeH,char* filename,Link head, text* thead) {             //返回0不退出，返回1退出

	static char textbuf[FileLen];         //textbuf 为文本框上的文本储存区

	static int labelmode, backflag = 1;   //labelmode为现今labels[]字符串数组中，
	                                      //应该选择显示的字符串的对应的数组下标；
										  //backflag为1代表退出保存界面或进入保存界面，
										  //为0则代表继续保持该界面

	DefineColor("ButtonFrameCommon", 0.07, 0.72, 0.96);
	DefineColor("ButtonFrameHot", 0.20, 0.60, 1.00);
	setButtonColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	SetFont("微软雅黑");
	SetPointSize(18);

	if (backflag == 1) {                  //刚刚进入该保持界面进行初始化
		backflag = 0;
		strcpy(textbuf, filename);        //填入文件名
		labelmode = 0;
	}

	if (textbox(GenUIID(0), TextBoxX * wholeW, (8* EmptyLineH + 4* ButtonH) * wholeH, TextBoxW * wholeW, TextBoxH* wholeH, textbuf, FileLen))
		labelmode = 0;//文本框，用于输入要保存文件的绝对或相对地址

	drawLabel(SaveLabelX * wholeW, (9 * EmptyLineH + 5 * ButtonH) * wholeH, "另存为");     //界面标题
	drawLabel(TextBoxX * wholeW, (7 * EmptyLineH + ButtonH) * wholeH, labels[labelmode]);  //显示保存的某种状态


	if (button(GenUIID(0), BackButtonX * wholeW, (6*EmptyLineH)*wholeH, ButtonW*wholeW, FileButtonH*wholeH, "返回")) {      //返回
		backflag = 1;
	}

	if (button(GenUIID(0), SaveButtonX * wholeW, (6*EmptyLineH)*wholeH, ButtonW * wholeW, FileButtonH * wholeH, "保存")) {  //保存
		
		if (Save(textbuf,head,thead,GetListLen(head),1))  //如保存成功，则返回
			backflag = 1;
		else {
			labelmode = 1; //否则，显示保存失败
		}

	}

	if (backflag) return LastPage_;  //跳出界面
	return SavePage_;                //保持界面
}

/*
 * 函数名: Display_Load
 * 接口: Display_Load(wholeW, wholeH,head, textboxhead,filename);
 * ------------------------
 * 绘制打开交互界面，默认文件名为filname,
 * 根据head指针和textboxhead指针实现
 * wholeW和wholeH为窗口大小。
 * 打开后，将文件名传回filename;
 */

int Display_Load(double wholeW, double wholeH, Link ptrh, text* ptrth, char* Name) {

	static char textbuf[AddressLen];
	char tmp[AddressLen];
	static int labelmode, backflag = 1,Isfull,page=1; //Isfull代表当今显示的文件是该地址的最后一页，page代表页数，其他变量含义同上

	DefineColor("ButtonFrameCommon", 0.07, 0.72, 0.96);
	DefineColor("ButtonFrameHot", 0.20, 0.60, 1.00);
	setButtonColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	SetFont("微软雅黑");
	SetPointSize(18);

	//重新打开改界面后的初始化步骤
	if (backflag == 1) {
		backflag = 0;
		strcpy(textbuf, "\0");
		memset(filenames[0], 0, 8*sizeof(char));      //清空缓存文件名信息
		Isfull=SetFileNames_(page,"RESTART",__address);
		page = 1;
	}

	/*下一页*/
	if (!Isfull && button(GenUIID(0), (SaveButtonX) * wholeW, (5.5* EmptyLineH) * wholeH, ButtonW * wholeW, FileButtonH * wholeH, "下一页")) {
		Isfull = SetFileNames_(++page,NULL, __address);
	}

	/*上一页*/
	if (page>1 && button(GenUIID(0), (BackButtonX)*wholeW, (5.5* EmptyLineH) * wholeH, ButtonW * wholeW, FileButtonH * wholeH, "上一页")) {
		Isfull = SetFileNames_(--page,NULL, __address);
	}

	/*显示当前文件夹中的文件的一页*/
	usePredefinedColors(3);
	for (int i = 0; i < FileRow; i++) {
		strcpy(tmp, filenames[i]);
		if (Isfolder[i]) sprintf(tmp, "%s%10s", tmp, "Folder");

		if (button(													GenUIID(i), ( BackButtonX ) * wholeW,
																	((19) * EmptyLineH-i* FileButtonH) * wholeH,
																	(5*ButtonW) * wholeW, FileButtonH * wholeH, 
																	tmp))
		{
			if (Isfolder[i])
				Isfull = SetFileNames_(page = 1, filenames[i],__address);
			else
				sprintf(textbuf, "%s\\%s", __address, filenames[i]);
		}
		if (filenames[i + 1][0] == '\0') break;
	}

	setButtonColors("ButtonFrameCommon", "White", "ButtonFrameHot", "White", 1);

	/*文本交互栏，用于输入和显示要保存的文件地址*/
	if (textbox(GenUIID(0), TextBoxX * wholeW, (8 * EmptyLineH + 4 * ButtonH) * wholeH, TextBoxW * wholeW, TextBoxH * wholeH, textbuf, MaxStrLen))
		labelmode = 0;

	/*标题栏*/
	drawLabel(SaveLabelX * wholeW, (9 * EmptyLineH + 5 * ButtonH) * wholeH, "读取");

	/*显示当前界面状态*/
	drawLabel(TextBoxX * wholeW, (1 * EmptyLineH + ButtonH) * wholeH, labels[labelmode]);

	if (button(GenUIID(0), BackButtonX * wholeW, (2 * EmptyLineH) * wholeH, ButtonW * wholeW, FileButtonH * wholeH, "返回")) {  //返回
		backflag = 1;
	}

	if (button(GenUIID(0), SaveButtonX * wholeW, (2 * EmptyLineH) * wholeH, ButtonW * wholeW, FileButtonH * wholeH, "打开")) {  //保存

		if (Load(textbuf, ptrh, ptrth)) {
			backflag = 1;
			assert(Name);
			strcpy(Name, textbuf);    //传回文件名
			return DrawPage_;
		}
		else {
			labelmode = 1;            //打开失败则显示错误
		}

	}

	if (backflag) return LastPage_;
	else return LoadPage_;
}

/*
 * 函数名: SetFileNames_
 * 接口: SetFileNames_(page,relafname);
 * ------------------------
 * 在内部地址的基础上，根据相对
 * 地址（relafname)显示出文件夹
 * 下的所有文件夹和.dat文件，page
 * 代表页数。
 * 下一次调用改函数时，将会把上一次
 * 调用的相对地址增加到其基础地址上。
 * 如果relafname="RESTART",将会清空
 * 基础地址
 */

static int SetFileNames_(int page,char* relafname,char* baseAddress) {
	int count = 0;               //表示当前数到的文件数目

	struct _finddata_t fileinfo; //定义文件信息结构

	static char tmp[200];

	/*改变文件名,若输入"RESTART"文件目录重置为程序工作目录*/
	if (relafname) {
		if (!strcmp("RESTART", relafname)) {
			GetCurrentDirectory(AddressLen,baseAddress);
		}
		else {
			AddressCatch(baseAddress, relafname);
		}

		memset(filenames[0], 0, sizeof(char) * FileLen * FileRow);
		sprintf(tmp, "%s\\*.*", baseAddress);
		//puts(baseAddress);        //调试用
	}

	long handle = _findfirst(tmp, &fileinfo);

	assert(handle != -1L);

	/*跳过指定页数之前的文件*/
	while (count < (page-1) * FileRow && _findnext(handle, &fileinfo) != -1) {
		count++;
	}

	/*加载指定页数的文件*/
	do {
		if ((fileinfo.attrib & 0x01) == _A_NORMAL) {  //文件要保证为非系统文件且可见、可修改文件
			if (strstr(fileinfo.name, ".dat")) {                                                    //筛选出符合后缀名的文件
				strcpy(filenames[count - (page - 1) * FileRow], fileinfo.name);
				Isfolder[count - (page - 1) * FileRow] = 0;
				count++;
			}

			else if ((fileinfo.attrib & 0x10) == _A_SUBDIR  && strcmp(".",fileinfo.name)!=0) { //判断文件夹
				strcpy(filenames[count - (page - 1) * FileRow], fileinfo.name);
				Isfolder[count - (page - 1) * FileRow] = 1;
				count++;
			}
		}

	} while ((_findnext(handle, &fileinfo) != -1) && count < page * FileRow);

	_findclose(handle);

	return count < page * FileRow; //如果是最后一页返回1，不是返回0
}

void AddressCatch(char* base, char* source) {
	if (!strcmp("..", source)) {
		int pos = 0, i;
		for (i = 0; base[i]; i++) {
			if (base[i] == '\\') pos = i;
		}
		base[pos] = '\0';
	}
	else sprintf(base,"%s\\%s", base, source);
}