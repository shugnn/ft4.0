#pragma once
#include<time.h>
#include<sstream>
#include "SeverDataType_Struct.h"
inline TimePhase ServerPhase()
{
	SYSTEMTIME sys_time;
	GetLocalTime(&sys_time);
	int dayofweek = (int)sys_time.wDayOfWeek;
	if (dayofweek <= 0)return PERMIT_Rest;
	double tm = (double)((sys_time.wHour) / 10e1) + (double)((sys_time.wMinute) / 10e3) + (double)((sys_time.wSecond) / 10e5);	//格式时间0.145100
	if (dayofweek >= 6 && tm >= 0.0240)return PERMIT_Rest;
	if (tm >= 0.0240 && tm < 0.0850)return  PERMIT_Rest;
	if (tm >= 0.1510 && tm < 0.2050)return  PERMIT_Rest;
	return PERMIT_Connect;
}
inline TimePhase TradePhase(Ins ins, int distms)
{
	SYSTEMTIME sys_time;
	GetLocalTime(&sys_time);
	double correct_sec = 60.0 * 60 * sys_time.wHour + 60.0*sys_time.wMinute + sys_time.wSecond + distms;
	double tm = (double)(floor(correct_sec / 3600) / 10e1)
		+ (double)(floor(fmod(correct_sec, 3600) / 60) / 10e3)
		+ (double)(fmod(correct_sec, 60) / 10e5);	//格式时间0.145100
	//tm = fmod(tm + 0.17, 0.24);
	int dayofweek = (int)sys_time.wDayOfWeek;
	if (strcmp(ins.ExchangeID, "DCE") == 0 || strcmp(ins.ExchangeID, "CZCE") == 0)
	{
		if (tm >= 0.0900 && tm < 0.1457)return  PERMIT_Trade;
		if (tm >= 0.1457 && tm < 0.1459)return  PERMIT_Close;
		if (tm >= 0.1459 && tm < 0.1500)return  PERMIT_Clean;
		if (tm >= 0.2100 && tm < 0.2327)return  PERMIT_Trade;
		if (tm >= 0.2327 && tm < 0.2329)return  PERMIT_Close;
		if (tm >= 0.2329 && tm < 0.2330)return  PERMIT_Clean;
		return PERMIT_Rest;
	}
	else if (strcmp(ins.ExchangeID, "SHFE") == 0)
	{
		if (strcmp(ins.ProductID, "rb") == 0 || strcmp(ins.ProductID, "hc") == 0 || strcmp(ins.ProductID, "bu") == 0 || strcmp(ins.ProductID, "ru") == 0)
		{
			if (tm >= 0.0900 && tm < 0.1457)return  PERMIT_Trade;
			if (tm >= 0.1457 && tm < 0.1459)return  PERMIT_Close;
			if (tm >= 0.1459 && tm < 0.1500)return  PERMIT_Clean;
			if (tm >= 0.2100 && tm < 0.2257)return  PERMIT_Trade;
			if (tm >= 0.2257 && tm < 0.2259)return  PERMIT_Close;
			if (tm >= 0.2259 && tm < 0.2300)return  PERMIT_Clean;
			return PERMIT_Rest;
		}
		else if (strcmp(ins.ProductID, "au") == 0 || strcmp(ins.ProductID, "ag") == 0)
		{
			if (tm >= 0.0900 && tm < 0.1457)return  PERMIT_Trade;
			if (tm >= 0.1457 && tm < 0.1459)return  PERMIT_Close;
			if (tm >= 0.1459 && tm < 0.1500)return  PERMIT_Clean;
			if (tm >= 0.2100 || tm < 0.0227)return  PERMIT_Trade;
			if (tm >= 0.0227 && tm < 0.0229)return  PERMIT_Close;
			if (tm >= 0.0229 && tm < 0.0230)return  PERMIT_Clean;
			return PERMIT_Rest;
		}
		else
		{
			if (tm >= 0.0900 && tm < 0.1457)return  PERMIT_Trade;
			if (tm >= 0.1457 && tm < 0.1459)return  PERMIT_Close;
			if (tm >= 0.1459 && tm < 0.1500)return  PERMIT_Clean;
			if (tm >= 0.2100 || tm < 0.0057)return  PERMIT_Trade;
			if (tm >= 0.0057 && tm < 0.0059)return  PERMIT_Close;
			if (tm >= 0.0059 && tm < 0.0100)return  PERMIT_Clean;
			return PERMIT_Rest;
		}
	}
	else if (strcmp(ins.ExchangeID, "CFFEX") == 0)
	{
		if (tm >= 0.0930 && tm < 0.1457)return  PERMIT_Trade;
		if (tm >= 0.1457 && tm < 0.1459)return  PERMIT_Close;
		if (tm >= 0.1459 && tm < 0.1500)return  PERMIT_Clean;
		return PERMIT_Rest;
	}
	else return PERMIT_Rest;
}
inline string TimeFtdc(int jdis)
{
	time_t tmt; time(&tmt);
	tmt += jdis;
	int itime = 1000000 + localtime(&tmt)->tm_hour * 10000 + localtime(&tmt)->tm_min * 100 + localtime(&tmt)->tm_sec;
	stringstream strstm; strstm.str("");
	strstm << itime;
	string ftime = strstm.str().substr(1, 2) + string(":") + strstm.str().substr(3, 2) + string(":") + strstm.str().substr(5, 2);
	return ftime;
}
inline int TimeDiff(string tm0, string tm1)
{
	if (tm0.length() < 8 || tm1.length()<8)return 0;
	int diff = (atoi(tm0.substr(0, 2).c_str()) - atoi(tm1.substr(0, 2).c_str())) * 60 * 60
		+ (atoi(tm0.substr(3, 2).c_str()) - atoi(tm1.substr(3, 2).c_str())) * 60
		+ (atoi(tm0.substr(6, 2).c_str()) - atoi(tm1.substr(6, 2).c_str()));
	return abs(diff) <= 12 * 60 * 60 ? diff : (diff > 0 ? diff - 24 * 60 * 60 : 24 * 60 * 60 - diff);
}
inline bool SortOpenInsterst(const Ins ob0, const Ins ob1)
{
	return (ob0.OpenInterest>ob1.OpenInterest);
}
