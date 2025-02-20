#pragma once

#include"ToolKit.hpp"

int GlobalRecordCnt=0;
FileOp flRecordCount("./Record/Global.rcd","a+");
FileOp flRecord;

void RecordScreen()
{
	ClearScreen();
	ColorPosPrintfEx(clGold,-1,1,1,L"🚧Under Construction🚧");
	PosPrintf(1,2,"Record Cnt: %d",GlobalRecordCnt);
	Sleep(1000);
	return;
}