#pragma once

#include"./PClib/Color.hpp"
#include"./PClib/Log.hpp"
#include"./PClib/Console.hpp"
#include"Settings.hpp"
#include"ToolKit.hpp"
#include"Record.hpp"
#include<thread>
#include<algorithm>
#include<cstdlib>
#include<ctime>
using std::min;
using std::thread;

bool MLEMark=false;
int StartT,EndT,MaxMemory;
int RetData,RetStd,RetMine;

mutex lkCheck;
bool ThisFinish=false;
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
		MaxMemory=max(MaxMemory,MemoNow);
		if(MemoNow>LimitMemory)
			MLEMark=true;
		if(MemoNow>LimitMemory+MercyMemory)
		{
			TerminateProcess(hProcess,0);
			return;
		}
	}
	return;
}

void SetResult(int i,int RS)
{
	Result[i]=RS;
	char text[15];
	if(RS==RS_AC||RS==RS_WA||
	   (RS==RS_TLE&&TimeUse[i]<LimitTime+MercyTime)||
	   (RS==RS_MLE&&MemoryUse[i]>=LimitMemory+MercyMemory))
		sprintf(text,"%dms",TimeUse[i]);
	else if((RS==RS_TLE&&TimeUse[i]>=LimitTime+MercyTime)||
			(RS==RS_MLE&&MemoryUse[i]<LimitMemory+MercyMemory))
		sprintf(text,"%.2lfmB",MemoryUse[i]/1024.0);
	else if(RS==RS_RE)
		sprintf(text,"%#x",RetMine);
	if(JudgeCnt<=40)
		ResPerRow=10,ResBoxHei=5,ResBoxWid=10,
		DrawResultBox(i,
			RectCalcX(i,ResPerRow,1,1,ResBoxWid),
			RectCalcY(i,ResPerRow,7,1,ResBoxHei),
			RS,text,false);
	else if(JudgeCnt<=1000)
		ResPerRow=50,ResBoxHei=1,ResBoxWid=2,
		DrawResultBox(i,
			RectCalcX(i,ResPerRow,1,0,ResBoxWid),
			RectCalcY(i,ResPerRow,7,0,ResBoxHei),
			RS,text,true);
	return;
}
void SetInfo(int i,int IF)
{
	if(JudgeCnt<=40)
		ResPerRow=10,ResBoxHei=5,ResBoxWid=10,
		DrawInfoBox(i,
			RectCalcX(i,ResPerRow,1,1,ResBoxWid),
			RectCalcY(i,ResPerRow,7,1,ResBoxHei),
			IF,false);
 			else if(JudgeCnt<=1000)
		ResPerRow=50,ResBoxHei=1,ResBoxWid=2,
		DrawInfoBox(i,
			RectCalcX(i,ResPerRow,1,0,ResBoxWid),
			RectCalcY(i,ResPerRow,7,0,ResBoxHei),
			IF,true);
	return;
}

int JudgeIdNow=0;
Bar barJudge(2,5,-1,clSkyBlue,clGray,&JudgeIdNow,-1);
InputBox ibJudgeCnt("Judge Count:",0,0,-1,5,-1,-1,clSkyBlue,INPUT_TYPE_INT);
bool JInput()
{
	ibJudgeCnt.focus=true;
	ibJudgeCnt.Print();
	CooldownFormLastCall();
	ClearGetchQueue();
	while(true)
	{
		ibJudgeCnt.Run();
		if(KeyDown(VK_RETURN)&&CooldownFormLastCall())
		{
			JudgeCnt=ibJudgeCnt.GetInt();
			if(JudgeCnt<=0||JudgeCnt>MAX_TEST_CNT)
				ColorPosPrintfEx(clRed,-1,0,2,"Invalid Judge Count.\n");
			else break;
		}
		if(KeyDown(VK_ESCAPE))
		{
			CursorSize(0);
			return false;
		}
		Sleep(50);
	}
	CursorSize(0);
	return true;
}
void TerminatePrograms()
{
	systemf(KILLERCMD,DataEXERaw);
	systemf(KILLERCMD,StdEXERaw);
	systemf(KILLERCMD,MineEXERaw);
}
bool CompilePrograms()
{
	ColorPosPrintfEx(clGold,-1,0,3,"Compiling...\n");
	int d=systemf("g++ -xc++ -std=c++14 -s %s -o %s",DataCPP,DataEXE);
	int s=systemf("g++ -xc++ -std=c++14 -s %s -o %s",StdCPP,StdEXE);
	int m=systemf("g++ -xc++ -std=c++14 -s %s -o %s",MineCPP,MineEXE);
	if(d||s||m)
	{
		ColorPosPrintfEx(clGold,-1,-1,-1,"Compile Error\n%d %d %d",d,s,m);
		DrawInfoBox(-1,
			RectCalcX(0,1,2,0,0),
			RectCalcY(0,1,6,0,0),
			IF_CE,false);
		return false;
	}
	ColorPosPrintfEx(clGold,-1,-1,-1,"Compiled!\n");
	return true;
}
void RunPrograms(int i)
{
	/*GenData*/
	SetInfo(i,IF_DATA);
	RetData=RunProgramAndGetReturn(DataEXE,NULL,DataTXT);
	/*GenAns*/
	SetInfo(i,IF_ANS);
	RetStd=RunProgramAndGetReturn(StdEXE,DataTXT,AnsTXT);
	/*Ready to judge*/
	ThisFinish=false;
	MLEMark=false;
	SetInfo(i,IF_JUDGE);
	/*Judge*/
	PROCESS_INFORMATION MineProcess;
	StartT=clock();
	MineProcess=CreateProgramProcess(MineEXE,DataTXT,OutTXT);
	thread ptMemo(MemoryMonitor,MineProcess.hProcess);
	int res=WaitForSingleObject(MineProcess.hProcess,LimitTime+MercyTime);
	EndT=clock();
	TerminateProcess(MineProcess.hProcess,0);
	RetMine=GetProgramReturn(MineProcess);
	/*Wait Killer*/
	lkCheck.lock();
	ThisFinish=true;
	lkCheck.unlock();
	ptMemo.join();
	return;
}
int ctAC=0,ctWA=0,ctTLE=0,ctMLE=0,ctRE=0,ctUKE=0;
void ShowProgress(int i)
{
	ColorPosPrintfEx(-1,-1,60,1,"[Details]",i);
	ColorPosPrintfEx(-1,-1,60,2,"#%d ",i);
	ColorPosPrintfEx(-1,ResCol[Result[i]],-1,-1," %s ",ResText[Result[i]]);
	ColorPosPrintfEx(-1,-1,-1,-1,"   ");
	ColorPosPrintfEx(-1,-1,60,3,"Time: %dms  ",TimeUse[i]);
	ColorPosPrintfEx(-1,-1,60,4,"Memo: %.2lfMB  ",MemoryUse[i]/1024.0);
	ColorPosPrintfEx(-1,-1,60,5,"Return: %#x       ",RetMine);
	switch(Result[i])
	{
		case RS_AC:		ctAC++;		break;
		case RS_WA:		ctWA++;		break;
		case RS_TLE:	ctTLE++;	break;
		case RS_MLE:	ctMLE++;	break;
		case RS_RE:		ctRE++;		break;
		case RS_UKE:	ctUKE++;	break;
	}
	ColorPosPrintfEx(clWhite,-1,80,1,"[Statistics]",i+1);
	ColorPosPrintfEx(clWhite,-1,80,2,"Finish %d",i+1);
	ColorPosPrintfEx(ResCol[RS_AC],-1,80,3,"AC %d",ctAC);
	ColorPosPrintfEx(ResCol[RS_WA],-1,80,4,"WA %d",ctWA);
	ColorPosPrintfEx(ResCol[RS_RE],-1,80,5,"RE %d",ctRE);
	ColorPosPrintfEx(-1,ResCol[RS_TLE],95,3,"TLE %d",ctTLE);
	ColorPosPrintfEx(-1,ResCol[RS_MLE],95,4,"MLE %d",ctMLE);
	ColorPosPrintfEx(-1,ResCol[RS_UKE],95,5,"UKE %d",ctUKE);
	return;
}
FileOp flWAReader;
void CopyNoneACData(int i)
{
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
	return;
}
void Resulting(int i)
{
	TimeUse[i]=min(EndT-StartT,LimitTime+MercyTime);
	MemoryUse[i]=min(MaxMemory,LimitMemory+MercyMemory);
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
		CopyNoneACData(i);
	}
	else if(MLEMark)
	{
		SetResult(i,RS_MLE);
		flRecord.printf("$STATUS MLE\n");
		CopyNoneACData(i);
	}
	else if(RetMine!=0)
	{
		SetResult(i,RS_RE);
		flRecord.printf("$STATUS RE\n$REUTEN %#x\n",RetMine);
		CopyNoneACData(i);
	}
	else if(system(FCCMD))
	{
		SetResult(i,RS_WA);
		flRecord.printf("$STATUS WA\n");
		CopyNoneACData(i);
	}
	else
	{
		SetResult(i,RS_AC);
		flRecord.printf("$STATUS AC\n");
	}
	DeleteFileAf(DataTXT,i);
	DeleteFileAf(AnsTXT,i);
	DeleteFileAf(OutTXT,i);
	flRecord.printf("$TIME %d\n",TimeUse[i]);
	flRecord.printf("$MEMO %d\n",MemoryUse[i]);
	return;
}
void Judge()
{
	/*init*/
	ClearDirectory(JudgeDIR);
	ClearScreen();
	ClearBarList();
	JudgeIdNow=0;
	GlobalRecordPlusPlus();
	ctAC=ctWA=ctTLE=ctMLE=ctRE=ctUKE=0;
	flRecord.open(RecordRCD,"w+",GlobalRecordCnt);
	if(!JInput())	return;
	TerminatePrograms();
	if(!CompilePrograms())
		goto End;
	flRecord.printf("$TOTAL %d\n",JudgeCnt);
	/*print GUI*/
	barJudge.maxv=JudgeCnt-1;
	barJudge.len=min(max((int)ceil(7.0*log10(JudgeCnt)/log10(exp(1))-16),10),60);
	PushBarList(barJudge);
	ClearScreen();
	InitBars();
	LogOut.lprintf("Info","Start judge %d time(s)...",JudgeCnt);
	ColorPosPrintfEx(clWhite,-1,2,1,"Judge times: %d",JudgeCnt);
	ColorPosPrintfEx(clWhite,-1,2,2,"Time limits: %dms +%dms",LimitTime,MercyTime);
	ColorPosPrintfEx(clWhite,-1,2,3,"Memo limits: %.2lfmB +%.2lfmB",LimitMemory/1024.0,MercyMemory/1024.0);
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
	LogOut.lprintf("Info","R%d AC %d",GlobalRecordCnt,ctAC);
End:
	while(!KeyDown(VK_ESCAPE))
		Sleep(75);
	ClearScreen();
	return;
}