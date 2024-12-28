#include"./PClib/Log.hpp"
#include"./PClib/Console.hpp"
#include"ToolKit.hpp"
#include<pthread.h>
#include<algorithm>
#include<cstdlib>
#include<ctime>
using std::min;

#define MAX_TEST_CNT 50000

int LimitTime=1000,MercyTime=200;
int JudgeCnt=10,StartT,EndT;
int RetData,RetStd,RetMine;
int Result[MAX_TEST_CNT],TimeUse[MAX_TEST_CNT];
pthread_t ptKill;

#define MineEXERaw "mine.exe"
#define MineEXE "..\\Code\\mine.exe"
#define DataEXE "..\\Code\\data.exe"
#define StdEXE "..\\Code\\std.exe"
#define DataTXT "..\\Judge\\Data\\data%d.txt"
#define AnsTXT "..\\Judge\\Ans\\ans%d.txt"
#define OutTXT "..\\Judge\\Out\\out%d.txt"
#define WATXT  "..\\Judge\\Wrong\\wrong%d.txt"

#define KILLERCMD "taskkill -f -im \"" MineEXERaw "\" 1>nul 2>nul"
#define FCCMD "fc " AnsTXT " " OutTXT " 1>nul 2>nul"
void* Killer(void* arg)
{
	Sleep(LimitTime+MercyTime);
	systemf(KILLERCMD);
	return NULL;
}

void SetResult(int i,int RS)
{
	Result[i]=RS;
	if(JudgeCnt<=40)
		ResPerRow=10,ResBoxHei=5,ResBoxWid=10,
		DrawResultBox(i,
			RectCulcX(i,ResPerRow,1,1,ResBoxWid),
			RectCulcY(i,ResPerRow,4,1,ResBoxHei),
			RS,TimeUse[i],false);
	else
		ResPerRow=50,ResBoxHei=1,ResBoxWid=2,
		DrawResultBox(i,
			RectCulcX(i,ResPerRow,1,0,ResBoxWid),
			RectCulcY(i,ResPerRow,4,0,ResBoxHei),
			RS,TimeUse[i],true);
	return;
}
void SetInfo(int i,int IF)
{
	if(JudgeCnt<=40)
		ResPerRow=10,ResBoxHei=5,ResBoxWid=10,
		DrawInfoBox(i,
			RectCulcX(i,ResPerRow,1,1,ResBoxWid),
			RectCulcY(i,ResPerRow,4,1,ResBoxHei),
			IF,false);
	else
		ResPerRow=50,ResBoxHei=1,ResBoxWid=2,
		DrawInfoBox(i,
			RectCulcX(i,ResPerRow,1,0,ResBoxWid),
			RectCulcY(i,ResPerRow,4,0,ResBoxHei),
			IF,true);
	return;
}

char CopyWADst[1000],CopyWASrc[1000];
void CopyWA()
{
	for(int i=0;i<JudgeCnt;i++)
		if(Result[i]!=RS_AC)
		{
			sprintf(CopyWADst,WATXT,i);
			sprintf(CopyWASrc,DataTXT,i);
			if(!CopyFileA(CopyWASrc,CopyWADst,FALSE))
				LogOut.lprintf("Error",
					"Fail to move wrong datas.\n"
					"%s -> %s",CopyWASrc,CopyWADst);
		}
	return;
}
void Judge()
{
	ClearScreen();
	/*input*/
	SetCursorPos(0,0);
	printf("Judge Count:");
	scanf("%d",&JudgeCnt);
	/*judge*/
	LogOut.lprintf("Info","Start judge %d time...",JudgeCnt);
	for(int i=0;i<JudgeCnt;i++)
		SetInfo(i,IF_WAIT);
	for(int i=0;i<JudgeCnt;i++)
	{
		/*GenData*/
		SetInfo(i,IF_DATA);
		RetData=systemf(DataEXE " >" DataTXT,i);
		/*Gen ans*/
		SetInfo(i,IF_ANS);
		RetStd=systemf(StdEXE " <" DataTXT " >" AnsTXT,i,i);
		/*Judge*/
		SetInfo(i,IF_JUDGE);
		pthread_create(&ptKill,NULL,Killer,NULL);
		StartT=clock();
		RetMine=systemf(MineEXE " <" DataTXT " >" OutTXT,i,i);
		EndT=clock();
		pthread_kill(ptKill,SIGTERM);
		TimeUse[i]=EndT-StartT;
		TimeUse[i]=min(TimeUse[i],LimitTime+MercyTime);
		/*Result*/
		if(RetData!=0||RetStd!=0)
			SetResult(i,RS_UKE),
			LogOut.lprintf("Error","data.exe ret%#x, std.exe ret %#x.",RetData,RetStd);
		else if(TimeUse[i]>LimitTime)
			SetResult(i,RS_TLE);
		else if(RetMine!=0)
			SetResult(i,RS_RE);
		else if(systemf(FCCMD,i,i))
			SetResult(i,RS_WA);
		else
			SetResult(i,RS_AC);
		LogOut.lprintf("Judge","#%d %s ret%#x",i,ResText[Result[i]],RetMine);
	}
	CopyWA();
	while(!KeyDown(VK_ESCAPE))
		Sleep(75);
}