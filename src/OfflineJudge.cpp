#include"../lib/PClib/Console.hpp"
#include"../lib/PClib/Log.hpp"
#include"../lib/ToolKit.hpp"
#include"../lib/Judge.hpp"

void Init()
{
	CursorSize(0);
	ConTitleA("Offline Judge");
	freopen("../log/err.log","w+",stderr);
	strcpy(LogFromat,"%04d%02d%02d %02d:%02d:%02d[%s]%s\n");
	return;
}


Button btJudge(50,9,9,-1,0x3498db,"Judge",Judge);
Button btTest(60,9,9,-1,0xc4b400,"不要按",NULL);
void WelcomeScreen()
{
	PrintfCenter(0,120,6,"Offline Judge");
	PushButList(btJudge,btTest);
	InitButs();
	while(true)
		RunButs(),
		Sleep(75);
}

int main()
{
	// while(true)
	// 	SetInfo(1,rand()%4);
	// printf("%d",systemf(DataEXE " >" DataTXT,0));
	Init();
	WelcomeScreen();
	return 0;
}