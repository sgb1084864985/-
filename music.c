#include<stdio.h>
#include<windows.h>
#include<mmsystem.h>
#pragma comment(lib, "WINMM.LIB")

/*
* 函数名：Music
* 接口：Music(int Choice)
* ————————————
* 这个函数应用Windows系统
* 的库，进行wav格式音乐的
* 播放与停止播放。wav音乐
* 应放在exe文件同目录下。
*/
void Music(int Choice)
{
    switch (Choice) {
    case 1: PlaySound(TEXT("ZJU_Song.wav"), 0, SND_FILENAME | SND_ASYNC | SND_LOOP);// 浙江大学校歌，循环播放
        break;
    case 2: PlaySound(TEXT("The Congress Reel.wav"), 0, SND_FILENAME | SND_ASYNC | SND_LOOP);// The Congress Reel纯音乐，循环播放
        break;
    case 3: PlaySound(TEXT("Yellow.wav"), 0, SND_FILENAME | SND_ASYNC | SND_LOOP); // Yellow纯音乐，循环播放
        break;
    case 4: PlaySound(TEXT(NULL), 0, SND_FILENAME | SND_PURGE); break;// 停止播放当前音乐
    default: break;
    }
}
