#pragma once

#include"./PClib/Color.hpp"
#include"./PClib/Console.hpp"
#include"./PClib/Log.hpp"
#include<cstdlib>
#include<vector>
#include<cmath>
#include<psapi.h>
using std::vector;
using std::max;
using std::min;

Log<100> LogOut(false);

char CommandTmp[400];
template<typename... Tps>
int systemf(const char* format,Tps... args)
{
	sprintf(CommandTmp,format,args...);
	return system(CommandTmp);
}
template<typename... args>
int DeleteFileAf(const char* Path,args... arg)
{
	char PathNow[1000];
	sprintf(PathNow,Path,arg...);
	return DeleteFileA(PathNow);
}
void ClearDirectory(const char* Path)
{
	systemf("del /f /s /q %s\\* >nul",Path);
	return;
}
void ClearScreen()
{
	systemf("cls");
	return;
}

char prBuffer[2000];
void Fill(Color col,int sx,int sy,int ex,int ey)
{
	lkOutput.lock();
	for(int i=sx;i<ex;i++)
		prBuffer[i-sx]=' ';
	prBuffer[ex-sx]='\0';
	for(int i=sy;i<ey;i++)
	{
		SetColorIOEx(-1,col);
		CursorGoto(sx,i);
		printf("%s",prBuffer);
	}
	SetColorIO(ConDefaultColor);
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
void ColPrintfCenter(Color fore,Color back,int sx,int ex,int y,const char* format,Tps... args)
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
void ColPrintfCenterNoFill(Color fore,Color back,int sx,int ex,int y,const char* format,Tps... args)
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
	int x,y,len;
	Color fore,back;
	char Text[MAXLEN];
	void (*Func)();
	Button(int X,int Y,int Len,Color Fore,Color Back,const char* text,void (*func)())
	{
		x=X,y=Y,len=Len,fore=Fore,back=Back;
		strcpy(Text,text);
		Func=func;
	}
	void Print(double index)
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
void RunButs(void (*CallBack)()=[](){})
{
	if(KeyDown(VK_TAB))
	{
		if(SelectBut!=-1)
		ButList[SelectBut].Print(0);
		SelectBut++,SelectBut%=ButList.size();
		ButList[SelectBut].Print(0.5);
	}
	if(SelectBut==-1)	return;
	if(KeyDown(VK_RETURN))
		ButList[SelectBut].Func(),
		CallBack(),
		InitButs();
	return;
}

#define BAR_STYLE_PERCENT 0
#define BAR_STYLE_VALUE 1
struct Bar
{
	int x,y,len,*var,maxv,style;
	Color fore,back;
	Bar(int X,int Y,int Len,Color Fore,Color Back,int* Var,int Maxv,int Style=BAR_STYLE_PERCENT):
		x(X),y(Y),len(Len),fore(Fore),back(Back),var(Var),maxv(Maxv),style(Style){}
	void Print(bool update=true)
	{
		if(!update)	Fill(back,x,y,x+len,y+1);
		double percent=max(1.0*(*var)/maxv,0.0);
		Fill(fore,x,y,x+(int)floor(percent*len),y+1);
		if(style==BAR_STYLE_PERCENT)
			ColorPosPrintfEx(fore,-1,x+len+1,y,"%d%%",(int)floor(percent*100));
		else if(style==BAR_STYLE_VALUE)
			ColorPosPrintfEx(fore,-1,x+len+1,y,"%d/%d",*var,maxv);
		return;
	}
};
vector<Bar> BarList;
void ClearBarList(){BarList.clear();return;}
void PushBarList(){return;}
template<typename ...Tps>
void PushBarList(Bar Th,Tps... args)
{
	BarList.push_back(Th);
	PushBarList(args...);
	return;
}
void InitBars()
{
	for(auto i:BarList)
		i.Print(false);
	return;
}
void RunBars()
{
	for(auto i:BarList)
		i.Print();
	return;
}

struct Selector
{
	int x,y,col,row,mx,my,wid,sid,sx,sy;
	Color (*fore)(int id),(*back)(int id),invalid;
	void (*func)(int id);
	bool (*selectable)(int id);
	char* (*text)(int id);
	Selector(int X,int Y,Color (*Fore)(int id),Color (*Back)(int id),Color Inv,
		int Col,int Row,int Mx,int My,int Wid,
		void (*Func)(int id),bool (*Selectable)(int id),char* (*Text)(int id)):
		x(X),y(Y),fore(Fore),back(Back),invalid(Inv),
		col(Col),row(Row),mx(Mx),my(My),wid(Wid),sx(0),sy(0),
		func(Func),selectable(Selectable),text(Text){};
	int GetId(int i,int j){return i+j*col;}
	bool InvalidId(int id){return id<0||id>=row*col;}
	void PrintOne(int i,int j,bool HighLight=false)
	{
		if(InvalidId(GetId(i,j)))	return;
		ColPrintfCenter(
			selectable(GetId(i,j))?
				(HighLight?
					HighContrust(fore(GetId(i,j))):
					fore(GetId(i,j))):
				invalid,
			HighLight?
				HighContrust(back(GetId(i,j))):
				back(GetId(i,j)),
			x+i*(wid+mx),x+wid+i*(wid+mx),y+j*(1+my),
			"%s",text(GetId(i,j)));
		return;
	}
	void Print(int hx,int hy)
	{
		if(hx!=-2)	goto Update;
		for(int j=0;j<row;j++)
			for(int i=0;i<col;i++)
				PrintOne(i,j);
		hx=-1,hy=0,sid=-1;
	Update:
		hy=min(max(hy,0),row-1);
		PrintOne(sx,sy);
		PrintOne(hx,hy,true);
		sx=hx,sy=hy;
		sid=sx+sy*col;
		return;
	}
	void Init(){Print(-2,-2);return;}
	void Run(void (*CallBack)()=[](){})
	{
		int tid=sid;
		if(KeyDown(VK_UP))
			while(true)
			{
				tid-=col;
				if(InvalidId(tid))	return;
				if(selectable(tid))	goto PrintSite;
			}
		if(KeyDown(VK_DOWN))
			while(true)
			{
				tid+=col;
				if(InvalidId(tid))	return;
				if(selectable(tid))	goto PrintSite;
			}
		if(KeyDown(VK_LEFT))
			while(true)
			{
				tid--;
				if(InvalidId(tid))	return;
				if(selectable(tid))	goto PrintSite;
			}
		if(KeyDown(VK_RIGHT))
			while(true)
			{
				tid++;
				if(InvalidId(tid))	return;
				if(selectable(tid))	goto PrintSite;
			}
		if(KeyDown(VK_RETURN))
			func(tid),
			CallBack(),
			Init();
		return;
	PrintSite:
		Print(tid%col,tid/col);
		return;
	}
};


/** @param cnt 次序
  * @param retcnt 每行个数
  * @param mar 左留白
  * @param pad 每个间距
  * @param len 每个宽度
  */ 
int RectCalcX(int cnt,int retcnt,int mar,int pad,int len)
	{return cnt%retcnt*(len+pad)+mar;}
/** @param cnt 次序
  * @param retcnt 每行个数
  * @param mar 上留白
  * @param pad 每个间距
  * @param len 每个高度
  */ 
int RectCalcY(int cnt,int retcnt,int mar,int pad,int len)
	{return cnt/retcnt*(len+pad)+mar;}

#define RS_UKE 0
#define RS_AC 1
#define RS_RE 2
#define RS_WA 3
#define RS_TLE 4
#define RS_MLE 5
#define RS_TOT 6
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
int ResCol[]={0x0e1d69,0x52c41a,0x9d3dcf,0xe74c3c,0x052242,0x052242};
const char* ResText[]={"UKE","AC","RE","WA","TLE","MLE"};
/*
#a0a0a0
#009356
#00a48e
#3498db
*/
int InfoCol[]={0xa0a0a0,0x009356,0x00a48e,0x3498db};
const char* InfoText[]={"Waiting","Dataing","Ansing","Judging"};
Color ScoreColor(double s)
{
	if(s==100)	return ResCol[RS_AC];
	if(s>=80)	return clLime;
	if(s>=50)	return clYellow;
	if(s>=20)	return clOrange;
	else return clRed;
}

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

char FilePathTmp[1000];
struct FileOp
{
	FILE* pointer;
	FileOp(){}
	FileOp(const char* Path,const char* Mode)
		{pointer=fopen(Path,Mode);}
	~FileOp(){fclose(pointer);}
	bool Eof(){return feof(pointer);}
	template<typename... Tps>
	int printf(const char* format,Tps... args)
	{
		int ret=fprintf(pointer,format,args...);
		fflush(pointer);
		return ret;
	}
	template<typename... Tps>
	int scanf(const char* format,Tps... args)
	{
		int ret=fscanf(pointer,format,args...);
		fflush(pointer);
		return ret;
	}
	char getchar(){return fgetc(pointer);}
	void getword(char* dest)
	{
		while(true)
		{
			dest[0]=FileOp::getchar();
			if(Eof()||dest[0]!=' '||dest[0]=='\n')
				break;
		}
		int i=1;
		while(true)
		{
			dest[i]=FileOp::getchar();
			if(Eof()||dest[i]==' '||dest[i]=='\n')
				break;
			i++;
		}
		dest[i]='\0';
		return;
	}
	int putchar(char ch)
	{
		int ret=fputc(ch,pointer);
		fflush(pointer);
		return ret;
	}
	template<typename... Tps>
	void open(const char* Path,const char* Mode,Tps... args)
	{
		fflush(pointer);
		fclose(pointer);
		sprintf(FilePathTmp,Path,args...);
		pointer=fopen(FilePathTmp,Mode);
		return;
	}
	int CursorMove(int offset)
		{return fseek(pointer,offset,SEEK_CUR);}
	int CursorSeek(int x,int origin=SEEK_SET)
		{return fseek(pointer,x,origin);}
	long long GetCursorPos()
	{
		long long ret;
		fgetpos(pointer,&ret);
		return ret;
	}
	int SetCursorPos(long long x)
		{return fsetpos(pointer,&x);}
};

PROCESS_INFORMATION CreateProgramProcess
(const char* Path,const char* FileIn,const char* FileOut)
{
	STARTUPINFO si;
	ZeroMemory(&si,sizeof(si));
	si.cb=sizeof(STARTUPINFO);
	si.dwFlags=STARTF_USESTDHANDLES;
	// Path char to wchar
	wchar_t wPath[1000],wIn[1000],wOut[1000];
	mbstowcs(wPath,Path,strlen(Path)+1);
	if(FileIn!=NULL)
		mbstowcs(wIn,FileIn,strlen(FileIn)+1);
	mbstowcs(wOut,FileOut,strlen(FileOut)+1);
	// Handle Output
	SECURITY_ATTRIBUTES saAttrOut={sizeof(SECURITY_ATTRIBUTES),NULL,TRUE},saAttrIn;
	HANDLE hOutputFile=CreateFileW(wOut,GENERIC_WRITE,FILE_SHARE_READ,&saAttrOut,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL),hInputFile;
	si.hStdOutput=hOutputFile;
	si.hStdError=hOutputFile;
	// Handle Input
	if(FileIn==NULL)	goto NoInput;
	saAttrIn={sizeof(SECURITY_ATTRIBUTES),NULL,TRUE};
	hInputFile=CreateFileW(wIn,GENERIC_READ,FILE_SHARE_READ,&saAttrIn,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	si.hStdInput=hInputFile;
NoInput:
	// Create Process
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	if(!CreateProcess(wPath,NULL,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi))
	{
		LogOut.lprintf("Error","run %s fail %#d",Path,GetLastError());
		pi.hProcess=NULL;
		return pi;
	}
	CloseHandle(hOutputFile);
	if(FileIn!=NULL)
		CloseHandle(hInputFile);
	return pi;
}
int GetProgramReturn(PROCESS_INFORMATION pi)
{
	DWORD ret;
	WaitForSingleObject(pi.hProcess,INFINITE);
	GetExitCodeProcess(pi.hProcess,&ret);
	return ret;
}
int RunProgramAndGetReturn(const char* Path,const char* FileIn,const char* FileOut)
{
	PROCESS_INFORMATION pro;
	pro=CreateProgramProcess(Path,FileIn,FileOut);
	return GetProgramReturn(pro);
}

/// @return 内存占用 (KB)
int GetProcessMemory(HANDLE hProcess)
{
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(hProcess,&pmc,sizeof(pmc));
	return pmc.WorkingSetSize/1024;
}

void WriteToClipboard(const char* text){
    OpenClipboard(nullptr);
    EmptyClipboard();
    HGLOBAL hMem=GlobalAlloc(GMEM_MOVEABLE,strlen(text) + 1);
    memcpy(GlobalLock(hMem),text,strlen(text)+1);
    GlobalUnlock(hMem);
    SetClipboardData(CF_TEXT,hMem);
    CloseClipboard();
	return;
}