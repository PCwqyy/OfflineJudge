#define UNICODE
#include"./lib/ToolKit.hpp"
#include"./lib/Record.hpp"
#include"./lib/Judge.hpp"

bool CheckEnvironment()
{
	if(isWindowsTerminal())
		return true;
	printf("Switching to Windows Terminal...\n");
	char Path[256];
	GetModuleFileNameA(NULL,Path,MAX_PATH);
	if(systemf("wt.exe %s",Path)==0)
		return false;
	char ch;
	ColorPrintf(0x04,"[Error]Windows Terminal not found!\n");
	printf("Download Windows Terminal? (Y/N): ");
	scanf("%c",&ch);
	if(ch=='Y'||ch=='y')
		system("winget install Microsoft.WindowsTerminal");
	else	return false;
}
void FirstTimeLaunch()
{
	CreateDirectoryA("./Record/",NULL);
	CreateDirectoryA("./Judge/",NULL);
	CreateDirectoryA("./Judge/Data/",NULL);
	CreateDirectoryA("./Judge/Ans/",NULL);
	CreateDirectoryA("./Judge/Out/",NULL);
	CreateDirectoryA("./Code/",NULL);
	CreateDirectoryA("./log/",NULL);
	flRecordCount.open("./Record/Global.rcd","w+");
	LogOut.OpenFile("./log/log.log","w+");
	flRecordCount.printf("0");
	return;
}
void Init()
{
	if(flRecordCount.pointer==NULL)
		FirstTimeLaunch();
	ConDefaultColor=0x0f;
	CursorSize(0);
	ConTitleA("Offline Judge");
	freopen("./log/err.log","w+",stderr);
	strcpy(LogFormat,"%04d%02d%02d %02d:%02d:%02d[%s]%s\n");
	flRecordCount.scanf("%d",&GlobalRecordCnt);
	return;
}

int loadingStatus=0;
Bar barTest(2,2,20,clWhite,clGray,&loadingStatus,514,BAR_STYLE_VALUE);
void TestScreen()
{
	ClearScreen();
	ColPrintfCenter(clWhite,clDodgerBlue,0,15,0,"Test Screen");
	ColorPosPrintfEx(clGold,-1,2,4,L"Emojis available! 😊🤣👌🐧");
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
	return;
}

Button btJudge(9,4,10,clWhite,clDodgerBlue,"Judge",Judge);
Button btRecord(21,4,10,clWhite,clGold,"Record",RecordScreen);
Button btTest(9,6,10,clWhite,clForestGreen,"Test",TestScreen);
Button btExit(21,6,10,clWhite,clRed,"Exit",[](){exit(0);});

void WelcomeScreen()
{
	ColPrintfCenter(clWhite,-1,10,30,2,"Offline Judge");
	PushButList(btJudge,btRecord,btTest,btExit);
	InitButs();
	while(true)
		RunButs([](){ColPrintfCenter(clWhite,-1,10,30,2,"Offline Judge");}),
		Sleep(90);
}
int main()
{
	if(!CheckEnvironment())
		return 0;
	Init();
	WelcomeScreen();
	return 0;
}