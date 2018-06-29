#pragma once
#include <iostream>
#include <fstream>
#include<sstream>
#include<Windows.h>
using namespace std;
#include"Function.h"
#include "Server.h"
bool fire = true;
extern CServer *pSer;
void main(void)
{
	cout << __FUNCTION__ << " Welcom FT4.0  fire=" << fire<< endl;
	while (true)
	{
		TimePhase time_phase = ServerPhase();
		if (time_phase == PERMIT_Connect)
		{
			if (!pSer)
			{
				pSer = new CServer(fire);
				if (pSer)pSer->ServerIn();
			}
			if (pSer)pSer->RecordFTResult();
		}
		if (time_phase == PERMIT_Rest)
		{
			if (pSer)
			{
				delete pSer;
				pSer = NULL;
			}
		}
		Sleep(1000 * 60);
	}
	return;
}
