#include"../lib/PClib/Console.hpp"
#include"../lib/PClib/Log.hpp"
#include"../lib/ToolKit.hpp"
#include"../lib/Judge.hpp"

void Init()
{
	ConDefaultColor=0x0f;
	CursorSize(0);
	ConTitleA("Offline Judge");
	freopen("../log/err.log","w+",stderr);
	strcpy(LogFromat,"%04d%02d%02d %02d:%02d:%02d[%s]%s\n");
	return;
}

int loadingStutus=0;
Bar barTest(50,8,20,0xffffff,0x7f7f7f,&loadingStutus,514);
void TestScreen()
{
	ClearScreen();
	PrintfCenter(0,120,6,"Test Screen");
	ClearBarList();
	PushBarList(barTest);
	InitBars();
	while(true)
	{
		if(loadingStutus>=514)	break;
		loadingStutus++;
		RunBars();
		Sleep(10);
	}
	Sleep(1000);
	return;
}

Button btJudge(50,9,9,-1,0x3498db,"Judge",Judge);
Button btExit(60,9,9,-1,0xe74c3c,"Exit",[](){exit(0);});
Button btTest(50,11,9,-1,0x009356,"Test",TestScreen);

void WelcomeScreen()
{
	PrintfCenter(0,120,6,"Offline Judge");
	PushButList(btJudge,btExit,btTest);
	InitButs();
	while(true)
		RunButs(),
		Sleep(75);
}

int main()
{
	Init();
	WelcomeScreen();
	return 0;
}