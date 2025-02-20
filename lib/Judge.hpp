#pragma once

#include"./PClib/Color.hpp"
#include"./PClib/Log.hpp"
#include"./PClib/Console.hpp"
#include"ToolKit.hpp"
#include"Record.hpp"
#include<thread>
#include<algorithm>
#include<cstdlib>
#include<ctime>
using std::min;
using std::thread;

#define MAX_TEST_CNT 50000

int LimitTime=1000,MercyTime=200;
int LimitMemory=256,MercyMemory=32;
bool MLEMark=false;
int JudgeCnt=10,StartT,EndT;
int RetData,RetStd,RetMine;
int Result[MAX_TEST_CNT],TimeUse[MAX_TEST_CNT];

#define MineEXERaw "mine.exe"
#define MineEXE ".\\Code\\mine.exe"
#define DataEXE ".\\Code\\data.exe"
#define StdEXE ".\\Code\\std.exe"
#define DataTXT ".\\Judge\\Data\\data%d.txt"
#define AnsTXT ".\\Judge\\Ans\\ans%d.txt"
#define OutTXT ".\\Judge\\Out\\out%d.txt"
#define AnsDIR ".\\Judge\\Ans"
#define OutDIR ".\\Judge\\Out"
#define DataDIR ".\\Judge\\Data"

#define KILLERCMD "taskkill /f /im \"" MineEXERaw "\" >nul"
#define FCCMD "fc " AnsTXT " " OutTXT " >nul"

mutex lkCheck;
bool ThisFinish=false;
void Killer()
{
	int timenow=clock();
	while(clock()-timenow<LimitTime+MercyTime)
	{
		lkCheck.lock();
		if(ThisFinish)
		{
			lkCheck.unlock();
			return;
		}
		lkCheck.unlock();
		Sleep(10);
	}
	systemf(KILLERCMD);
	return;
}
void MemoryMonitor(HANDLE hProcess)
{
	int MemoNow;
	while(true)
	{
		lkCheck.lock();
		if(ThisFinish)
		{
			lkCheck.unlock();
			return;
		}
		lkCheck.unlock();
		MemoNow=GetProcessMemory(hProcess);
		if(MemoNow>LimitMemory)
			MLEMark=true;
		if(MemoNow>LimitMemory+MercyMemory)
		{
			systemf(KILLERCMD);
			return;
		}
	}
}

void SetResult(int i,int RS)
{
	Result[i]=RS;
	if(JudgeCnt<=40)
		ResPerRow=10,ResBoxHei=5,ResBoxWid=10,
		DrawResultBox(i,
			RectCalcX(i,ResPerRow,1,1,ResBoxWid),
			RectCalcY(i,ResPerRow,6,1,ResBoxHei),
			RS,TimeUse[i],false);
	else if(JudgeCnt<=1000)
		ResPerRow=50,ResBoxHei=1,ResBoxWid=2,
		DrawResultBox(i,
			RectCalcX(i,ResPerRow,1,0,ResBoxWid),
			RectCalcY(i,ResPerRow,6,0,ResBoxHei),
			RS,TimeUse[i],true);
	return;
}
void SetInfo(int i,int IF)
{
	if(JudgeCnt<=40)
		ResPerRow=10,ResBoxHei=5,ResBoxWid=10,
		DrawInfoBox(i,
			RectCalcX(i,ResPerRow,1,1,ResBoxWid),
			RectCalcY(i,ResPerRow,6,1,ResBoxHei),
			IF,false);
 			else if(JudgeCnt<=1000)
		ResPerRow=50,ResBoxHei=1,ResBoxWid=2,
		DrawInfoBox(i,
			RectCalcX(i,ResPerRow,1,0,ResBoxWid),
			RectCalcY(i,ResPerRow,6,0,ResBoxHei),
			IF,true);
	return;
}
void ViewResult()
{
	ClearScreen();
	for(int i=0;i<JudgeCnt;i++)
		SetResult(i,Result[i]);
	return;
}

int JudgeIdNow=0;
Bar barJudge(2,4,-1,clSkyBlue,clGray,&JudgeIdNow,-1);
void JInput()
{
	CursorGoto(0,0);
	printf("Judge Count:");
intputsite:
	scanf("%d",&JudgeCnt);
	while(JudgeCnt<=0||JudgeCnt>MAX_TEST_CNT)
	{
		ColorPrintfEx(0xff0000,-1,"Invalid Judge Count.\n");
		goto intputsite;
	}
	flRecord.printf("$TOTAL %d\n",JudgeCnt);
	barJudge.maxv=JudgeCnt-1;
	barJudge.len=min(max((int)ceil(7.0*log10(JudgeCnt)/log10(exp(1))-16),10),60);
	PushBarList(barJudge);
	ClearScreen();
	InitBars();
	return;
}
void RunPrograms(int i)
{
	/*GenData*/
	SetInfo(i,IF_DATA);
	RetData=systemf(DataEXE " >" DataTXT,i);
	/*GenAns*/
	SetInfo(i,IF_ANS);
	RetStd=systemf(StdEXE " <" DataTXT " >" AnsTXT,i,i);
	/*Ready to judge*/
	ThisFinish=false;
	SetInfo(i,IF_JUDGE);
	thread ptKill(Killer);
	/*Judge*/
	StartT=clock();
	PROCESS_INFORMATION MineProcess;
	char DataNow[100],OutNow[100];
	sprintf(DataNow,DataTXT,i);
	sprintf(OutNow,OutTXT,i);
	MineProcess=CreateProgramProcess(MineEXE,DataNow,OutNow);
	thread ptMemo(MemoryMonitor,MineProcess.hProcess);
	RetMine=GetProgramReturn(MineProcess);
	EndT=clock();
	/*Wait Killer*/
	lkCheck.lock();
	ThisFinish=true;
	lkCheck.unlock();
	ptKill.join();
	ptMemo.join();
	return;
}
int ctAC=0,ctWA=0,ctTLE=0,ctRE=0,ctUKE=0;
void ShowProgress(int i)
{
	switch(Result[i])
	{
		case RS_AC:		ctAC++;		break;
		case RS_WA:		ctWA++;		break;
		case RS_TLE:	ctTLE++;	break;
		case RS_RE:		ctRE++;		break;
		case RS_UKE:	ctUKE++;	break;
	}
	ColorPosPrintfEx(clWhite,clLightGray,80,1,"Finish %d",i+1);
	ColorPosPrintfEx(ResCol[RS_AC],clLightGray,80,2,"    AC %d",ctAC);
	ColorPosPrintfEx(ResCol[RS_WA],clLightGray,80,3,"    WA %d",ctWA);
	ColorPosPrintfEx(ResCol[RS_TLE],clLightGray,95,1,"TLE %d",ctTLE);
	ColorPosPrintfEx(ResCol[RS_RE],clLightGray,95,2," RE %d",ctRE);
	ColorPosPrintfEx(ResCol[RS_UKE],clLightGray,95,3,"UKE %d",ctUKE);
	return;
}
FileOp flWAReader;
void Resulting(int i)
{
	TimeUse[i]=EndT-StartT;
	TimeUse[i]=min(TimeUse[i],LimitTime+MercyTime);
	if(RetData!=0||RetStd!=0)
	{
		SetResult(i,RS_UKE);
		LogOut.lprintf("Error","data ret%#x, std ret %#x.",RetData,RetStd);
		flRecord.printf("$STATUS UKE\n");
	}
	else if(TimeUse[i]>LimitTime)
	{
		SetResult(i,RS_TLE);
		flRecord.printf("$STATUS TLE\n");
	}
	else if(RetMine!=0)
	{
		SetResult(i,RS_RE);
		flRecord.printf("$STATUS RE\n$REUTEN %#x\n",RetMine);
	}
	else if(systemf(FCCMD,i,i))
	{
		SetResult(i,RS_WA);
		flRecord.printf("$STATUS WA\n");
		void (*Copier)(const char* Text,const char* Src,int i)
		=[](const char* Text,const char* Src,int i)
		{
			flRecord.printf("$%s\n",Text);
			flWAReader.open(Src,"r",i);
			char ch;
			while(true)
			{
				ch=flWAReader.getchar();
				if(ch==EOF)	break;
				flRecord.putchar(ch);
			}
			flRecord.putchar('\n');
		};
		Copier("DATA",DataTXT,i);
		Copier("ANS",AnsTXT,i);
		Copier("OUT",OutTXT,i);
	}
	else
	{
		DeleteFileAf(DataTXT,i);
		DeleteFileAf(AnsTXT,i);
		DeleteFileAf(OutTXT,i);
		SetResult(i,RS_AC);
		flRecord.printf("$STATUS AC\n");
	}
	flRecord.printf("$TIME %d\n",TimeUse[i]);
	return;
}
void Judge()
{
	/*init*/
	ClearDirectory(AnsDIR);
	ClearDirectory(OutDIR);
	ClearDirectory(DataDIR);
	ClearScreen();
	ClearBarList();
	JudgeIdNow=0;
	GlobalRecordCnt++;
	ctAC=ctWA=ctTLE=ctRE=ctUKE=0;
	flRecord.open("./Record/R%d.rcd","w+",GlobalRecordCnt);
	JInput();
	/*print GUI*/
	LogOut.lprintf("Info","Start judge %d time(s)...",JudgeCnt);
	ColorPosPrintfEx(clWhite,-1,2,1,"Judge times: %d",JudgeCnt);
	ColorPosPrintfEx(clWhite,-1,2,2,"Time limits: %dms +%dms",LimitTime,MercyTime);
	Fill(clLightGray,79,1,106,4);
	for(int i=0;i<JudgeCnt;i++)
		SetInfo(i,IF_WAIT);
	/*main loop*/
	for(int i=0;i<JudgeCnt;i++)
	{
		JudgeIdNow=i;
		RunPrograms(i);
		Resulting(i);
		ShowProgress(i);
		RunBars();
	}
	while(!KeyDown(VK_ESCAPE))
	Sleep(75);
}