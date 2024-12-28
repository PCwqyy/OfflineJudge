#pragma once

#include"../lib/PClib/Console.hpp"
#include"../lib/PClib/Log.hpp"
#include<vector>
#include<cstdlib>
using std::vector;

Log<100> LogOut("..\\log\\log.log",OVERWRITE);

char prBuffer[2000];

#undef RGB
#define RGB(r,g,b) ((b)|((g)<<8)|((r)<<16))

int HighContrust(int col,int index=1)
{
	int R=(col&0xff0000)>>16;
	int G=(col&0x00ff00)>>8;
	int B=(col&0x0000ff);
	R*=index,R+=0xff,R/=(index+1);
	G*=index,G+=0xff,G/=(index+1);
	B*=index,B+=0xff,B/=(index+1);
	return RGB(R,G,B);
}

char CommandTmp[400];
template<typename... Tps>
int systemf(const char* format,Tps... args)
{
	sprintf(CommandTmp,format,args...);
	return system(CommandTmp);
}

void ClearScreen()
{
	systemf("cls");
	return;
}
void Fill(int col,int sx,int sy,int ex,int ey)
{
	lkOutput.lock();
	for(int i=sx;i<ex;i++)
		prBuffer[i-sx]=' ';
	prBuffer[ex-sx]='\0';
	for(int i=sy;i<ey;i++)
	{
		CursorGoto(50,0);
		SetColorIOEx(-1,col);
		CursorGoto(sx,i);
		printf("%s",prBuffer);
	}
	lkOutput.unlock();
	return;
}
template<typename... Tps>
void PrintfCenter(int sx,int ex,int y,const char* format,Tps... args)
{
	lkOutput.lock();
	sprintf(prBuffer,format,args...);
	int len=strlen(prBuffer);
	lkOutput.unlock();
	PosPrintf((ex+sx-len)/2,y,"%s",prBuffer);
	return;
}
template<typename... Tps>
void ColPrintfCenter(int fore,int back,int sx,int ex,int y,const char* format,Tps... args)
{
	Fill(back,sx,y,ex,y+1);
	lkOutput.lock();
	sprintf(prBuffer,format,args...);
	int len=strlen(prBuffer);
	lkOutput.unlock();
	ColorPosPrintfEx(fore,back,(ex+sx-len)/2,y,"%s",prBuffer);
	return;
}
template<typename... Tps>
void ColPrintfCenterNoFill(int fore,int back,int sx,int ex,int y,const char* format,Tps... args)
{
	lkOutput.lock();
	sprintf(prBuffer,format,args...);
	int len=strlen(prBuffer);
	lkOutput.unlock();
	ColorPosPrintfEx(fore,back,(ex+sx-len)/2,y,"%s",prBuffer);
	return;
}

#define MAXLEN 50
struct Button
{
	int x,y,len,fore,back;
	char Text[MAXLEN];
	void (*Func)();
	Button(int X,int Y,int Len,int Fore,int Back,const char* text,void (*func)())
	{
		x=X,y=Y,len=Len,fore=Fore,back=Back;
		strcpy(Text,text);
		Func=func;
	}
	void Print(int index)
	{
		ColPrintfCenter(
			index?HighContrust(fore,index):fore,
			index?HighContrust(back,index):back,
			x,x+len,y,"%s",Text);
		return;
	}
};
vector<Button> ButList;
int SelectBut=-1;
void ClearButList(){ButList.clear();return;}

void PushButList(){return;}
template<typename ...Tps>
void PushButList(Button Th,Tps... args)
{
	ButList.push_back(Th);
	PushButList(args...);
	return;
}
void InitButs()
{
	for(auto i:ButList)
		i.Print(0);
	SelectBut=-1;
	return;
}
void RunButs()
{
	if(KeyDown(VK_TAB))
	{
		if(SelectBut!=-1)
			ButList[SelectBut].Print(0);
		SelectBut++,SelectBut%=ButList.size();
		ButList[SelectBut].Print(1);
	}
	if(SelectBut==-1)	return;
	if(KeyDown(VK_RETURN))
		ButList[SelectBut].Func(),
		ClearScreen(),
		InitButs();
}
/** @param cnt 次序
  * @param retcnt 每行个数
  * @param mar 左留白
  * @param pad 每个间距
  * @param len 每个宽度
  */ 
int RectCulcX(int cnt,int retcnt,int mar,int pad,int len)
	{return cnt%retcnt*(len+pad)+mar;}
/** @param cnt 次序
  * @param retcnt 每行个数
  * @param mar 上留白
  * @param pad 每个间距
  * @param len 每个高度
  */ 
int RectCulcY(int cnt,int retcnt,int mar,int pad,int len)
	{return cnt/retcnt*(len+pad)+mar;}

#define RS_UKE 0
#define RS_AC 1
#define RS_RE 2
#define RS_WA 3
#define RS_TLE 4
#define IF_WAIT 0
#define IF_DATA 1
#define IF_ANS 2
#define IF_JUDGE 3

/*
#0e1d69
#52c41a
#9d3dcf
#e74c3c
#052242
*/
int ResCol[]={0x0e1d69,0x52c41a,0x9d3dcf,0xe74c3c,0x052242};
const char* ResText[]={"UKE","AC","RE","WA","TLE"};
/*
#a0a0a0
#009356
#00a48e
#3498db
*/
int InfoCol[]={0xa0a0a0,0x009356,0x00a48e,0x3498db};
const char* InfoText[]={"Waiting","Dataing","Ansing","Judging"};

int ResBoxWid=10,ResBoxHei=5,ResPerRow=10;

void DrawResultBox(int No,int x,int y,int RS,int TU,bool Small)
{
	Fill(ResCol[RS],x,y,x+ResBoxWid,y+ResBoxHei);
	if(Small)	return;
	ColorPosPrintfEx(-1,ResCol[RS],x,y,"#%d",No);
	ColPrintfCenterNoFill(-1,ResCol[RS],x,x+ResBoxWid,y+ResBoxHei*0.2,"%s",ResText[RS]);
	ColPrintfCenterNoFill(-1,ResCol[RS],x,x+ResBoxWid,y+ResBoxHei*0.6,"%dms",TU);
	return;
}
void DrawInfoBox(int No,int x,int y,int IF,bool Small)
{
	Fill(InfoCol[IF],x,y,x+ResBoxWid,y+ResBoxHei);
	if(Small)	return;
	ColorPosPrintfEx(-1,InfoCol[IF],x,y,"#%d",No);
	ColPrintfCenterNoFill(-1,InfoCol[IF],x,x+ResBoxWid,y+ResBoxHei*0.2,"%s",InfoText[IF]);
	return;
}