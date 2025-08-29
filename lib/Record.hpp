#pragma once

#include"ToolKit.hpp"
#include"Settings.hpp"

int GlobalRecordCnt=0,JudgeCnt=10;
FileOp flGlobalRecord;
FileOp flRecord;

void GetGlobalRecord()
{
	if(flGlobalRecord.pointer==NULL)
		flGlobalRecord.open(GlobalRCD,"w+");
		flGlobalRecord.printf("0");
	flGlobalRecord.SetCursorPos(0);
	flGlobalRecord.scanf("%d",&GlobalRecordCnt);
	return;
}
void GlobalRecordPlusPlus()
{
	GlobalRecordCnt++;
	flGlobalRecord.SetCursorPos(0);
	flGlobalRecord.printf("%-5d",GlobalRecordCnt);
	return;
}

#define RecordRCD "./Record/R%d.rcd"

#define MAX_TEST_CNT 50000
#define MAX_FILE_LEN 100000
int Result[MAX_TEST_CNT],TimeUse[MAX_TEST_CNT],MemoryUse[MAX_TEST_CNT];
int ResultStat[RS_TOT];
long long DataFilePos[MAX_TEST_CNT],AnsFilePos[MAX_TEST_CNT],OutFilePos[MAX_TEST_CNT];
char DataFile[MAX_FILE_LEN],AnsFile[MAX_FILE_LEN],OutFile[MAX_FILE_LEN];

void ReadSubfile(char* dest,long long pos)
{
	flRecord.SetCursorPos(pos);
	int i=0;
	while(true)
	{
		if(flRecord.Eof())	break;
		dest[i]=flRecord.getchar();
		if(dest[i]=='$')	break;
		i++;
	}
	dest[i]='\0';
	return;
}
Button btClipboardData(60,1,12,clBlack,clLightGray,"Copy Data",
	[](){WriteToClipboard(DataFile);});
Button btClipboardAns(60,2,12,clWhite,ResCol[RS_AC],"Copy Ans",
	[](){WriteToClipboard(AnsFile);});
Button btClipboardOut(60,3,12,clWhite,ResCol[RS_WA],"Copy Out",
	[](){WriteToClipboard(OutFile);});
void ViewPoint(int i)
{
	ClearScreen();
	Fill(clGray,44,1,59,4);
	ColorPosPrintfEx(clWhite,clGray,45,1,"#%d",i);
	ColorPosPrintfEx(clWhite,clGray,45,2,"Time: %dms",TimeUse[i]);
	ColorPosPrintfEx(clWhite,clGray,45,3,"Memo: %dKB",MemoryUse[i]);
	CursorGoto(0,0);
	if(Result[i]==RS_AC||Result[i]==RS_UKE)	return;
	ReadSubfile(DataFile,DataFilePos[i]);
	ColorPrintfEx(clBlack,clWhite," Data \n");
	printf("%s",DataFile);
	if(Result[i]!=RS_WA)	goto Loop;
	ReadSubfile(AnsFile,AnsFilePos[i]);
	ReadSubfile(OutFile,OutFilePos[i]);
	ColorPrintfEx(clWhite,ResCol[RS_AC]," Ans \n");
	printf("%s",AnsFile);
	ColorPrintfEx(clWhite,ResCol[Result[i]]," Out \n");
	printf("%s",OutFile);
Loop:
	ClearButList();
	PushButList(btClipboardData,btClipboardAns,btClipboardOut);
	InitButs();
	while(true)
	{
		RunButs([](){ColorPosPrintfEx(clGold,-1,60,4,"Copied!");});
		if(KeyDown(VK_ESCAPE))	break;
		Sleep(75);
	}
	ClearScreen();
	return;
}

char TextInRect[3]="  ";
Selector slViewRecordRect(
	1,4,[](int id){return Color(clWhite);},
	[](int id)
	{
		if(id>JudgeCnt)
			return Color(clBlack);
		return Color(ResCol[Result[id]]);
	},clBlack,50,10,0,0,2,ViewPoint,
	[](int id){return id<JudgeCnt&&Result[id]!=RS_AC&&Result[id]!=RS_UKE;},
	[](int id){return TextInRect;});
int RIDNow;
void ViewRecord(int rid)
{
	ClearScreen();
	RIDNow=rid+1;
	char In[1000];
	int NowId=-1;
	for(int i=0;i<RS_TOT;i++)
		ResultStat[i]=0;
	flRecord.open(RecordRCD,"r+",rid+1);
	while(true)
	{
		flRecord.getword(In);
		if(In[0]=='$')
		{
			if(strcmp(In+1,"TOTAL")==0)
				flRecord.getword(In),
				sscanf(In,"%d",&JudgeCnt);
			else if(strcmp(In+1,"STATUS")==0)
			{
				NowId++;
				flRecord.getword(In);
				for(int i=0;i<RS_TOT;i++)
					if(strcmp(In,ResText[i])==0)
					{
						Result[NowId]=i;
						ResultStat[i]++;
						break;
					}
			}
			else if(strcmp(In+1,"TIME")==0)
				flRecord.getword(In),
				sscanf(In,"%d",&TimeUse[NowId]);
			else if(strcmp(In+1,"MEMO")==0)
				flRecord.getword(In),
				sscanf(In,"%d",&MemoryUse[NowId]);
			else if(strcmp(In+1,"DATA")==0)
				DataFilePos[NowId]=flRecord.GetCursorPos();
			else if(strcmp(In+1,"ANS")==0)
				AnsFilePos[NowId]=flRecord.GetCursorPos();
			else if(strcmp(In+1,"OUT")==0)
				OutFilePos[NowId]=flRecord.GetCursorPos();
			else
				LogOut.lprintf("Error","Unknown tag \"%s\"",In+1);
		}
		if(flRecord.Eof())	break;
	}
	void (*BasicGUI)()=[]()
	{
		ColorPosPrintfEx(clWhite,clGold,0,0," Result #%d ",RIDNow);
		double Score=100.0*ResultStat[RS_AC]/JudgeCnt;
		PosPrintf(1,2,"Score: ");
		ColorPosPrintfEx(ScoreColor(Score),clBlack,8,2,"%.2lf",Score);
	};
	BasicGUI();
	slViewRecordRect.Init();
	while(true)
	{
		slViewRecordRect.Run(BasicGUI);
		if(KeyDown(VK_ESCAPE))	break;
		Sleep(75);
	}
	ClearScreen();
	return;
}

char slTextBuff[1000];
Selector slRecord(
	1,2,[](int id){return Color(clGold);},[](int id){return Color(clBlack);},clGray,15,14,1,1,6,
	[](int id){ViewRecord(id);},[](int id){return id<GlobalRecordCnt;},
	[](int id){sprintf(slTextBuff,"R%d",id+1);return slTextBuff;});	
void RecordScreen()
{
	ClearScreen();
	void (*BasicGUI)()=[]()
	{
		ColorPosPrintfEx(clWhite,clGold,0,0," Record ");
		return;
	};
	BasicGUI();
	slRecord.Init();
	while(true)
	{
		slRecord.Run(BasicGUI);
		if(KeyDown(VK_ESCAPE))
			break;
		Sleep(75);
	}
	ClearScreen();
	return;
}