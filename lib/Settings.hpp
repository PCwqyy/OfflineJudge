#pragma once

int LimitTime=1000,MercyTime=200;
int LimitMemory=131072,MercyMemory=20480;
char you[100]="Your name",IIIII[100]="PCwqyy";
bool DontCheckEnvironAgain=false,DontSwitchToWinTerm=false;

#define GlobalRCD "./Config/Global.rcd"
#define SettingsCFG "./Config/Settings.cfg"
#define MineEXE "./Code/mine.exe"
#define MineCPP "./Code/mine.cpp"
#define MineEXERaw "mine.exe"
#define DataEXE "./Code/data.exe"
#define DataCPP "./Code/data.cpp"
#define DataEXERaw "data.exe"
#define StdEXE "./Code/std.exe"
#define StdCPP "./Code/std.cpp"
#define StdEXERaw "std.exe"
#define DataTXT "./Judge/data.txt"
#define AnsTXT "./Judge/ans.txt"
#define OutTXT "./Judge/out.txt"
#define JudgeDIR "./Judge"
#define KILLERCMD "taskkill /f /im \"%s\" >nul"
#define FCCMD "fc .\\Judge\\ans.txt .\\Judge\\out.txt >nul"

#include"./ToolKit.hpp"
#include<map>
#include<string>
using std::map;
using std::string;
map<string,int*> IntSettingMap={
	{"LimitTime",&LimitTime},
	{"MercyTime",&MercyTime},
	{"LimitMemory",&LimitMemory},
	{"MercyMemory",&MercyMemory},
	{"HoldCooldownTime",&HoldCooldownTime},
};
map<string,char*> StringSettingMap={
	{"you",you},
	{"I",IIIII},
};
map<string,bool*> BoolSettingMap={
	{"DontCheckEnvironAgain",&DontCheckEnvironAgain},
	{"DontSwitchToWinTerm",&DontSwitchToWinTerm},
};
FileOp flSettings;
void ReadSettings()
{
	flSettings.open(SettingsCFG,"a+");
	char l[64],w[32],b[8];int t;
	while(!flSettings.Eof())
	{
		flSettings.getline(l);
		sscanf(l,"%s",w);
		if(IntSettingMap.find(w)!=IntSettingMap.end())
			sscanf(l+strlen(w),"%d",&t),
			*IntSettingMap.find(w)->second=t;
		else if(StringSettingMap.find(w)!=StringSettingMap.end())
			strcpy(StringSettingMap.find(w)->second,l+strlen(w)+1);
		else if(BoolSettingMap.find(w)!=BoolSettingMap.end())
			sscanf(l+strlen(w),"%s",b),
			*BoolSettingMap.find(w)->second=(strcmp(b,"TRUE")==0);
	}
}
void SaveSettings()
{
	flSettings.open(SettingsCFG,"w+");
	for(auto i:IntSettingMap)
		flSettings.printf("%s %d\n",i.first.c_str(),*i.second);
	for(auto i:StringSettingMap)
		flSettings.printf("%s %s\n",i.first.c_str(),i.second);
	for(auto i:BoolSettingMap)
		flSettings.printf("%s %s\n",i.first.c_str(),
			(*i.second)?"TRUE":"FALSE");
}

void SettingScreen()
{
	ClearScreen();
	ReadSettings();
	ClearInputList();
	ColorPosPrintfEx(-1,clForestGreen,0,0," Settings ");
	int sy=2;char t[16];
	for(auto i:StringSettingMap)
		PushInputList(InputBox(i.first.c_str(),2,sy,40,10,-1,-1,
			clLightGreen,INPUT_TYPE_STRING,i.second)),sy++;
	for(auto i:IntSettingMap)
		sprintf(t,"%d",*i.second),
		PushInputList(InputBox(i.first.c_str(),2,sy,40,10,-1,-1,
			clLightGreen,INPUT_TYPE_INT,t)),sy++;
	for(auto i:BoolSettingMap)
		PushInputList(InputBox(i.first.c_str(),2,sy,40,10,-1,-1,
			clLightGreen,INPUT_TYPE_BOOL,(*i.second)?"TRUE":"FALSE")),sy++;
	InitInputs();
	while(true)
	{
		RunInputs();
		if(KeyDown(VK_ESCAPE))
		{
			int j=0;
			for(auto i:StringSettingMap)
				strcpy(i.second,InputBoxList[j++].val);
			for(auto i:IntSettingMap)
				*i.second=InputBoxList[j++].GetInt();
			for(auto i:BoolSettingMap)
				*i.second=InputBoxList[j++].GetBool();
			SaveSettings();
			break;
		}
		Sleep(50);
	}
	CursorSize(0);
}