#include<cmath>
#include<ctime>
#include<cstdio>
#include<cctype>
#include<cstring>
#include<queue>
#include<conio.h>
#include<pthread.h>
#include<windows.h>
#include"lib/PCLib/Console.hpp"
#include"lib/PCLib/Log.hpp"
using std::queue;
int min(int a,int b){return a<b?a:b;}
int max(int a,int b){return a>b?a:b;}
int ints[30010];
/********************************************************************************/
/*									Judge										*/
/********************************************************************************/
Log<10000> Jlog("Judge.log",OVERWRITE);
struct FArg{int n,col;const char* info;int ms;};
queue<FArg> FillQ;
bool FQnEmpty=false;
void FillColor(int n,int col,const char* info,int ms)//fill the color bolck
{
	int l=strlen(info);
	const int x=n%10*11+1,y=n/10*6+3;
	for(int i=x;i<=x+9;i++)
		for(int j=y;j<=y+4;j++)
			ColorPrintf(i,j,col," ");
	ColorPrintf(x+(9-l)/2+1,y+1,col,info);
	ColorPrintf(x,y,col,"#%d",n);
	if(ms==-1)	return;
	char time[9];
	sprintf(time,"%dms",ms);
	l=strlen(time);
	ColorPrintf(x+(9-l)/2+1,y+3,col,time);
	return;
}
void FillColor(FArg a){FillColor(a.n,a.col,a.info,a.ms);return;}
void QFillColor(int n,int col,const char* info,int ms)
	{FillQ.push((FArg){n,col,info,ms});return;}
void* QFCFlusher(void* arg)
{
	while(true)
		if(!FillQ.empty())
			FillColor(FillQ.front()),
			FillQ.pop(),
			FQnEmpty=true,
			pthread_testcancel();
		else FQnEmpty=false;
}
pthread_t qfcflush;
int GrCol(int ac,int tot)//get the color of each status
{
	if(ac*3<tot)		return 0x04;
	else if(ac*3<tot*2)	return 0x06;
	else if(ac!=tot)	return 0x02;
	else return 0x0a;
}
char fn[100]={'\0'};
template<typename... types>
int fsystem(const char* format,types... args)
{
	char* cmd=new char[110];
	sprintf(cmd,format,args...);
	const int ret=system(cmd);
	delete cmd;
	return ret;
}
int Rodd,N,Tlimit,AC,WA,TLE,RE,UKE,Left;
int Status[30010],Tused[30010];
bool Finish[30010];
pthread_t mine[30030],kill[30030];
void* RunMine(void* arg)
{
	int i=*(int*)arg;
	int Begin=clock();
	int ret=fsystem(".\\Code\\mine.exe <.\\Judge\\data%d.txt >.\\Judge\\out%d.txt",i,i);
	Tused[i]=clock()-Begin;
	if(ret!=0)	Status[i]=2;
	else	Status[i]=3;
	pthread_cancel(kill[i]);
	Finish[i]=true;
	return NULL;
}
void* Killer(void* arg)
{
	int i=*(int*)arg;
	Sleep(Tlimit+505);
	fsystem("taskkill -f -pid %d",mine[i]);
	Finish[i]=true;
	return NULL;
}
void* Judger(void *arg)//judge one point inside this
{
	int i=*(int*)arg;
	QFillColor(i,0xf0,"Dataing",-1);
	fsystem(".\\Code\\make.exe >.\\Judge\\data%d.txt",i);
	QFillColor(i,0x70,"Ansing",-1);
	fsystem(".\\Code\\std.exe <.\\Judge\\data%d.txt >.\\Judge\\ans%d.txt",i,i);
	QFillColor(i,0x8f,"Judging",-1);
	pthread_create(&kill[i],NULL,Killer,&ints[i]);
	pthread_create(&mine[i],NULL,RunMine,&ints[i]);
	while(Finish[i]==false);
	if(Status[i]==0||Tused[i]>Tlimit)
		Status[i]=1,Tused[i]=min(Tlimit+200,Tused[i]),QFillColor(i,0x1f,"TLE",Tused[i]);
		// fsystem("copy .\\Judge\\data%d.txt .\\Wrong\\TLE%d.txt >sys.log",i,TLE);
	else if(Status[i]==2)
		QFillColor(i,0x5f,"RE",Tused[i]);
		// fsystem("copy .\\Judge\\data%d.txt .\\Wrong\\RE%d.txt >sys.log",i,RE);
	else if(fsystem("fc .\\Judge\\ans%d.txt .\\Judge\\out%d.txt >nul",i,i))
		QFillColor(i,0x4f,"WA",Tused[i]),Status[i]=3;
		// fsystem("copy .\\Judge\\data%d.txt .\\Wrong\\WA%d.txt >sys.log",i,WA);
	else	QFillColor(i,0xaf,"AC",Tused[i]),Status[i]=4;
	Left--;
	return NULL;
}
void Judge()//judge once
{
	system("cls");
	ConSize(140,35);
	/*input the number of the points*/
	char in;int p=0;
	for(int i=0;i<100;i++)
		fn[i]='\0';
	printf("Input the number of the points: |");
	while(true)
	{
		while(kbhit())
		{
			in=getch();
			if(isdigit(in)&&p!=5)
			{
				if(p==0&&in=='0'||fn[0]-'0'>=3&&p==4)
				{
					ColorPrintf(0,1,0x06,"[Warn]The num should be in 1~30000.");
					continue;
				}
				fn[p]=in,p=min(p+1,5);
			}
			else if(in=='\b')
				p=max(0,p-1),fn[p]='\0';
			else if(in=='\r')
				break;
		}
		if(clock()/500%2==0)
			ColorPrintf(32,0,ConDefaultColor,"%s| ",fn);
		else	ColorPrintf(32,0,ConDefaultColor,"%s  ",fn);
		if(KeyDown(VK_ESCAPE))	return;
		if(in=='\r')	break;
		Sleep(50);
	}
	/*start judge*/
	sscanf(fn,"%d",&N);
	Rodd=0,Tlimit=1000,AC=0,WA=0,TLE=0,RE=0,UKE=0,Left=N;
	int ti=0;
	do	sprintf(fn,".\\Judge\\ans%d.txt",ti++);
	while(DeleteFileA(fn));ti=0;
	do	sprintf(fn,".\\Judge\\data%d.txt",ti++);
	while(DeleteFileA(fn));ti=0;
	do	sprintf(fn,".\\Judge\\out%d.txt",ti++);
	while(DeleteFileA(fn));
	/*read and write the odd of the judge*/
/**/Jlog.lprintf("Debug","Read and write the odd of the judge");
	FILE* Rcdr=fopen(".\\Record\\odd.rcd","r");
	fscanf(Rcdr,"%d",&Rodd),Rodd++,fclose(Rcdr);
	Rcdr=fopen(".\\Record\\odd.rcd","w");
	fprintf(Rcdr,"%d",Rodd),fclose(Rcdr);
	/*paint start screen*/
/**/Jlog.lprintf("Debug","Paint start screen");
	for(int i=0;i<N;i++)
		FillColor(i,0x8f,"Waiting",-1),Finish[i]=false;
	/*judge*/
/**/Jlog.lprintf("Debug","Judge %d times",N);
	for(int i=0;i<N;i++)
		pthread_create(&judge[i],NULL,Judger,&ints[i]),
		Sleep(20);
	/*wait*/
	while(Left!=0);
	while(FQnEmpty);
	/*stat*/
/**/Jlog.lprintf("Debug","Stat");
	for(int i=0;i<N;i++)
	{
		if(Status[i]==1)		TLE++;
		else if(Status[i]==2)	RE++;
		else if(Status[i]==3)	WA++;
		else if(Status[i]==4)	AC++;
		else					UKE++,FillColor(i,0x0f,"UKE",Tused[i]);
		Jlog.lprintf("Stat","<R%d>#%d Status:%d",Rodd,i,Status[i]);
	}
	/*write the Record file*/
/**/Jlog.lprintf("Debug","Write the Record file");
	sprintf(fn,".\\Record\\R%d.rcd",Rodd);
	FILE* Recordd=fopen(fn,"w");
	fprintf(Recordd,"%d\n",N);
	for(int i=0;i<N;i++)
		fprintf(Recordd,"%d %d\n",Status[i],Tused[i]);
	fclose(Recordd);
	ColorPrintf(1,1,GrCol(AC,N),"%0.lf",100.0*AC/N);
	// for(int i=WA+1;i<=N;i++)
	// 	sprintf(fn,".\\Wrong\\WA%d.txt",i),DeleteFile(fn);
	// for(int i=TLE+1;i<=N;i++)
	// 	sprintf(fn,".\\Wrong\\TLE%d.txt",i),DeleteFile(fn);
	// for(int i=RE+1;i<=N;i++)
	// 	sprintf(fn,".\\Wrong\\RE%d.txt",i),DeleteFile(fn);
	CursorGoto(0,35);
	system("pause >>sys.log");
	return;
}
/********************************************************************************/
/*									Record										*/
/********************************************************************************/
void DrawButton(int x,int y,int col,char* Text)
{
	for(int i=x;i<=x+9;i++)
		for(int j=y;j<=y;j++)
			ColorPrintf(i,j,col," ");
	const int l=strlen(Text);
	ColorPrintf(x+(9-l)/2+1,y,col,Text);
	return;
}
bool InButton(int x,int y,COORD M){return x<=M.X&&M.X<=x+9&&M.Y==y;}
void DrawRecord(int x,int y,int col,int odd,int ac,int tot)
{
	if(col==0x5f)	col=(GrCol(ac,tot)<<4)+0x7;
	for(int i=x;i<=x+60;i++)
		for(int j=y;j<=y+2;j++)
			ColorPrintf(i,j,col," ");
	ColorPrintf(x+2,y+1,col,"R%d",odd);
	ColorPrintf(x+48,y+1,ac==tot?0xa8:0x48," %sccepted ",ac==tot?"A":"Una");
	ColorPrintf(x+49,y+2,col,"%.0lf %d/%d",100.0*ac/tot,ac,tot);
	return;
}
void ViewPage(int Rodd,int ps,int Rs[][40][2],int *Rn,int *RAC)
{
	ConSize(140,35);
	char fn[100];
	for(int i=ps;i<=min(ps+6,Rodd);i++)
	{
		sprintf(fn,".\\Record\\R%d.rcd",i);
		FILE* This=fopen(fn,"r");
		fscanf(This,"%d",&Rn[i-ps]);
		RAC[i-ps]=0;
		for(int j=0;j<Rn[i-ps];j++)
			fscanf(This,"%d %d",&Rs[i-ps][j][0],&Rs[i-ps][j][1]),
			RAC[i-ps]+=(Rs[i-ps][j][0]==4);
		DrawRecord(1,(i-ps)*4+3,0x8f,i,RAC[i-ps],Rn[i-ps]);
		fclose(This);
	}
}
bool InRecord(int x,int y,COORD M)
{return x<=M.X&&M.X<=x+60&&y<=M.Y&&M.Y<=y+2;}
void ViewRecord(int Rs[][2],int N)
{
	system("cls");
	for(int i=0;i<N;i++)
	{
		if(Rs[i][0]==1)	FillColor(i,0x1f,"TLE",(double)Rs[i][1]);
		if(Rs[i][0]==2)	FillColor(i,0x5f,"RE",(double)Rs[i][1]);
		if(Rs[i][0]==3)	FillColor(i,0x4f,"WA",(double)Rs[i][1]);
		if(Rs[i][0]==4)	FillColor(i,0xaf,"AC",(double)Rs[i][1]);
	}
	COORD Mouse;
	DrawButton(1,1,0x3f,"<Back");
	while(true)
	{
		GetMousexy(Mouse);
		if(InButton(1,1,Mouse))
		{
			DrawButton(1,1,0xaf,"< Back");
			if(KeyDown(VK_LBUTTON))	{system("cls");return;}
		}else DrawButton(1,1,0x3f,"<Back");
		CursorGoto(0,0);
		Sleep(50);
	}
}
void DrawBar(int x,int y,int col,int barh,int bary)
{
	for(int i=y+bary;i<y+bary+barh;i++)
		ColorPrintf(x,i,col,"  ");
}
void DrawScrollBar(int x,int y,int h,int col,int barh,int bary)
{
	for(int i=y;i<=y+h;i++)
		if(y+bary<=i&&i<y+bary+barh)
			ColorPrintf(x,i,col,"  ");
		else ColorPrintf(x,i,0x8f,"  ");
	return;
}
bool InBar(int x,int y,int barh,int bary,COORD M)
{return (M.X==x||M.X==x+1)&&y+bary<=M.Y&&M.Y<y+bary+barh;}
bool InScroll(int x,int y,COORD M)
{return (M.X==x||M.X==x+1)&&y<=M.Y&&M.Y<y+26;}
void Record()
{
	SetSelectState(true);
	system("cls");
	ConSize(72,35);
	int Rodd=0,ps=1;
	int Rs[11][40][2],Rn[11],RAC[11]={0};
	FILE* Rcdr=fopen(".\\Record\\odd.rcd","r");
	if(Rcdr==0)
		fclose(Rcdr),Rcdr=fopen(".\\Record\\odd.rcd","w"),
		fprintf(Rcdr,"0"),
		fclose(Rcdr),Rcdr=fopen(".\\Record\\odd.rcd","r");
	fscanf(Rcdr,"%d",&Rodd),fclose(Rcdr);
	COORD Mouse,LMouse;
	bool Click=true,LClick=true;
	DrawButton(1,1,0x3f,"<Back");
	if(Rodd==0)
	{
		ColorPrintf(3,3,0x0d,"Empty");
		while(true)
		{
			LMouse=Mouse;GetMousexy(Mouse);
			LClick=Click;Click=KeyDown(VK_LBUTTON);
			if(InButton(1,1,Mouse)&&!InButton(1,1,LMouse))
				DrawButton(1,1,0xaf,"< Back");
			if(!InButton(1,1,Mouse)&&InButton(1,1,LMouse))
				DrawButton(1,1,0x3f,"<Back");
			if(!LClick&&Click&&InButton(1,1,Mouse))	return;
		}
	}
	ViewPage(Rodd,ps,Rs,Rn,RAC);
	int barh=26.0*min(7,Rodd)/Rodd;
	int bary=26.0*(ps-1)/Rodd,cy;
	DrawScrollBar(64,3,26,0x7f,barh,bary);
	bool Drag;
	while(true)
	{
		LMouse=Mouse;GetMousexy(Mouse);
		LClick=Click;Click=KeyDown(VK_LBUTTON);
		if(InButton(1,1,Mouse)&&!InButton(1,1,LMouse))
			DrawButton(1,1,0xaf,"< Back");
		if(!InButton(1,1,Mouse)&&InButton(1,1,LMouse))
			DrawButton(1,1,0x3f,"<Back");
		if(!LClick&&Click&&InButton(1,1,Mouse))	return;
		if(InBar(64,3,barh,bary,Mouse)&&!InBar(63,3,barh,bary,LMouse))
			DrawBar(64,3,0xff,barh,bary);
		if(!InBar(64,3,barh,bary,Mouse)&&InBar(63,3,barh,bary,LMouse))
			DrawBar(64,3,0x7f,barh,bary);
		if(InBar(64,3,barh,bary,Mouse)&&!Drag&&Click)
			Drag=true,cy=Mouse.Y-bary-4;
		if(!Click)	Drag=false;
		else DrawBar(64,3,0x7f,barh,bary);
		if(Drag)
			bary=min(26-barh+1,max(0,Mouse.Y-cy-4)),
			ps=min(ceil(1.0*bary/(26/Rodd))+1,Rodd-6),
			DrawScrollBar(64,3,26,0xff,barh,bary),
			ViewPage(Rodd,ps,Rs,Rn,RAC);
		if(!LClick&&Click&&InScroll(64,3,Mouse)&&Mouse.Y<bary)
			ps-=3,ps=max(ps,1),bary=26.0*(ps-1)/Rodd,
			DrawScrollBar(64,3,26,0x7f,barh,bary),
			ViewPage(Rodd,ps,Rs,Rn,RAC);
		if(!LClick&&Click&&InScroll(64,3,Mouse)&&Mouse.Y>3+bary+barh)
			ps+=3,ps=min(ps,Rodd-6),bary=26.0*(ps-1)/Rodd,
			DrawScrollBar(64,3,26,0x7f,barh,bary),
			ViewPage(Rodd,ps,Rs,Rn,RAC);
		for(int i=ps;i<=min(ps+6,Rodd);i++)
		{
			if(InRecord(1,(i-ps)*4+3,Mouse)&&!InRecord(1,(i-ps)*4+3,LMouse))
				DrawRecord(1,(i-ps)*4+3,0x5f,i,RAC[i-ps],Rn[i-ps]);
			if(!InRecord(1,(i-ps)*4+3,Mouse)&&InRecord(1,(i-ps)*4+3,LMouse))
				DrawRecord(1,(i-ps)*4+3,0x8f,i,RAC[i-ps],Rn[i-ps]);
			if(!LClick&&Click&&InRecord(1,(i-ps)*4+3,Mouse))
				ViewRecord(Rs[i-ps],Rn[i-ps]),ViewPage(Rodd,ps,Rs,Rn,RAC),
				DrawScrollBar(64,3,26,0x7f,barh,bary),
				DrawButton(1,1,0x3f,"<Back");
		}
		Sleep(50);
	}
}
const char* Warn="Record BooooooooooooM!";
const char* Version="24w1a";
char Title[100]="Offline Judge";
int main()
{
	freopen("err.log","w",stderr);
	sprintf(Title,"%s %s",Title,Version);
	ConTitle("Offline Judge");
	InitCharSize();
	ConSize(50,12);
	CursorSize(0);
	SetSelectState(true);
	ColorPrintf(16,1,0x07,"Offline Judge");
	ColorPrintf(0,10,0x07,"Version:%s",Version);
	ColorPrintf(0,9,0x04,Warn);
	pthread_create(&qfcflush,NULL,QFCFlusher,NULL);
	COORD Mouse;
	for(int i=0;i<30000;i++)
		ints[i]=i;
	while(true)
	{
		GetMousexy(Mouse);
		if(InButton(11,3,Mouse))
		{
			DrawButton(11,3,0xaf,"Judge");
			if(KeyDown(VK_LBUTTON))
				Judge(),system("cls"),ConSize(50,12),
				ColorPrintf(16,1,0x07,"Offline Judge"),
				ColorPrintf(0,10,0x07,"Version:%s",Version);
		}else DrawButton(11,3,0x3f,"Judge");
		if(InButton(23,3,Mouse))
		{
			DrawButton(23,3,0xaf,"Record");
			if(KeyDown(VK_LBUTTON))
				Record(),system("cls"),ConSize(50,12),
				ColorPrintf(16,1,0x07,"Offline Judge"),
				ColorPrintf(0,10,0x07,"Version:%s",Version);
		}else DrawButton(23,3,0x3f,"Record");
		Sleep(50);
	}
	pthread_cancel(qfcflush);
	return 0;
}