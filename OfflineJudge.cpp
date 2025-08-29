#define UNICODE
#include"./lib/ToolKit.hpp"
#include"./lib/Record.hpp"
#include"./lib/Judge.hpp"

void CreateDirs()
{
	CreateDirectoryA("./Record/",NULL);
	CreateDirectoryA("./Config/",NULL);
	CreateDirectoryA("./Judge/",NULL);
	CreateDirectoryA("./Judge/Data/",NULL);
	CreateDirectoryA("./Judge/Ans/",NULL);
	CreateDirectoryA("./Judge/Out/",NULL);
	CreateDirectoryA("./Code/",NULL);
	CreateDirectoryA("./log/",NULL);
}
bool CheckEnvironment()
{
	ClearScreen();
	if(DontCheckEnvironAgain)
		return true;
	if(isVirtualTerminal())
		return true;
	if(!DontSwitchToWinTerm)
	{
		printf("Switching to Windows Terminal...\n");
		char Path[256];
		GetModuleFileNameA(NULL,Path,MAX_PATH);
		if(systemf("wt.exe %s",Path)==0)
			return false;
		ColorPrintf(0x04,"[Error]Windows Terminal not found!\n");
	}
	char ch[5];
	printf("Try anyway? (Y/N): ");
	scanf("%s",ch);
	if(ch[0]=='Y'||ch[0]=='y')
	{
		printf("Here we go!");
		DontCheckEnvironAgain=true;
		SaveSettings();
		return true;
	}
	printf("Download Windows Terminal? (Y/N): ");
	scanf("%s",ch);
	if(ch[0]=='Y'||ch[0]=='y')
		system("winget install Microsoft.WindowsTerminal");
	else	return false;
	return true;
}
void FirstTimeLaunch()
{
	flGlobalRecord.open(GlobalRCD,"w+");
	flGlobalRecord.printf("0");
	LogOut.open("./log/log.log","w+");
	SaveSettings();
	return;
}
void Init()
{
	ConTitleA("Offline Judge");
	if(!CheckFileExist(SettingsCFG))
		FirstTimeLaunch();
	ConDefaultColor=0x0f;
	CursorSize(0);
	freopen("./log/err.log","w+",stderr);
	strcpy(LogFormat,"%02d:%02d:%02d[%s]%s\n");
	LogOut.open("./log/log.log",OVERWRITE);
	CreateDirs();
	GetGlobalRecord();
	ReadSettings();
	return;
}

int loadingStatus=0;
Bar barTest(2,2,20,clWhite,clGray,&loadingStatus,514,BAR_STYLE_VALUE);
void TestScreen()
{
	ClearScreen();
	ColPrintfCenter(clWhite,clBlueViolet,0,15,0,"Test Screen");
	ColorPosPrintfEx(clGold,-1,2,4,L"Emojis available! 😊🤣👌🐧");
	ReadSettings();
	ColorPosPrintfEx(clDeepSkyBlue,-1,2,5,"You are %s.",you);
	SaveSettings();
	ClearBarList();
	PushBarList(barTest);
	InitBars();
	while(true)
	{
		if(loadingStatus>=514)	break;
		loadingStatus++;
		RunBars();
		Sleep(20);
	}
	Sleep(1000);
	ClearScreen();
	return;
}

Button btJudge(10,4,13,clWhite,clDodgerBlue,"Judge",Judge);
Button btRecord(25,4,13,clWhite,clGold,"Record",RecordScreen);
Button btSettings(10,6,13,clWhite,clForestGreen,"Settings",SettingScreen);
Button btTest(25,6,13,clWhite,clBlueViolet,"Test",TestScreen);
Button btExit(10,8,13,clWhite,clRed,"Exit",[](){ClearScreen();exit(0);});

void WelcomeScreen()
{
	void (*BasicGUI)()=[]()
	{
		ClearScreen();
		ClearButList();
		PushButList(btJudge,btRecord,btSettings,btTest,btExit);
		ColPrintfCenter(clWhite,-1,10,40,2,"Offline Judge");
		ColorPosPrintfEx(clGray,-1,0,12,"OJ v1.1 by %s",IIIII);
		InitButs();
	};
	BasicGUI();
	while(true)
		RunButs(BasicGUI),
		Sleep(90);
}
int main()
{
	Init();
	if(!CheckEnvironment())
		return 0;
	WelcomeScreen();
	return 0;
}