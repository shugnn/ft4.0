#pragma once
#include <iostream>
#include <windows.h>
#include<time.h>
using namespace std;
#include ".\ThostTraderApi\ThostFtdcMdApi.h"
#include"Server.h"
#include "MdSpi.h"
#include"FT.h"
#include"Function.h"
#pragma warning(disable : 4996)
extern CThostFtdcMdApi* pMdApi;
extern CServer *pSer;
extern vector <CFt*> pFt;
extern HANDLE Events[MAXNUM], hMutex, hLock;
void CMdSpi::OnFrontConnected()
{
	ReqUserLogin();
}
void CMdSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.UserID, pSer->con.investor_id);
	strcpy(req.Password, pSer->con.password);
	int iResult = pMdApi->ReqUserLogin(&req, ++nRequestID);
	return;
}
void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
		pSer->OnMarkedConnect(pRspUserLogin);
}
void CMdSpi::ReqUserLogout()
{
	CThostFtdcUserLogoutField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.UserID, pSer->con.investor_id);
	int iResult = pMdApi->ReqUserLogout(&req, ++nRequestID);
	return;
}
void CMdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) && bIsLast)
	{
		cout << __FUNCTION__ << " " << TimeFtdc(pSer->con.distms) << endl;
		SetEvent(Events[0]);
	}
	return;
}
void CMdSpi::SubscribeMarketData(char *ppInstrumentID[], int nCount)
{
	nSub = 0;
	int iResult = pMdApi->SubscribeMarketData(ppInstrumentID, nCount);
	return;
}
void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo))
	{
		pSer->OnSubMarket(pSpecificInstrument, bIsLast);
		++nSub;
		if (bIsLast)
		{
			WaitForSingleObject(hMutex, INFINITE);
			cout << "SubMarket Successed nSub: " << nSub << endl;
			ReleaseMutex(hMutex);
		}
	}
	return;
}
void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	WaitForSingleObject(hMutex, INFINITE);
	cout << __FUNCTION__ << endl;
	ReleaseMutex(hMutex);
	return;
}
void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	//cout << __FUNCTION__ << endl;
	if (pDepthMarketData)
	{
		for (int i = 0; i < (int)pFt.size(); i++)
		{
			if (!pFt[i])continue;
			if (strcmp(pFt[i]->ins.InstrumentID, pDepthMarketData->InstrumentID) == 0)
			{
				WaitForSingleObject(hLock, INFINITE);
				strcpy(pFt[i]->mtik.day, pDepthMarketData->TradingDay);
				strcpy(pFt[i]->mtik.time, pDepthMarketData->UpdateTime);
				pFt[i]->mtik.mill = pDepthMarketData->UpdateMillisec;
				pFt[i]->mtik.prc0 = pDepthMarketData->LastPrice;
				pFt[i]->mtik.bid1 = pDepthMarketData->BidPrice1;
				pFt[i]->mtik.bid2 = pDepthMarketData->BidPrice2;
				pFt[i]->mtik.ask1 = pDepthMarketData->AskPrice1;
				pFt[i]->mtik.ask2 = pDepthMarketData->AskPrice2;
				pFt[i]->mtik.aprc = pDepthMarketData->AveragePrice;
				pFt[i]->mtik.hprc = pDepthMarketData->HighestPrice;
				pFt[i]->mtik.lprc = pDepthMarketData->LowestPrice;
				pFt[i]->mtik.uplmt = pDepthMarketData->UpperLimitPrice;
				pFt[i]->mtik.lwlmt = pDepthMarketData->LowerLimitPrice;
				pFt[i]->mtik.valid = true;
				ReleaseMutex(hLock);
			}
		}

	}
}
void CMdSpi::OnFrontDisconnected(int nReason)
{
	WaitForSingleObject(hMutex, INFINITE);
	time_t tm; time(&tm); cout << __FUNCTION__ << "	Reason = " << nReason << "	" << ctime(&tm);
	ReleaseMutex(hMutex);
	return;
}
void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	WaitForSingleObject(hMutex, INFINITE);
	time_t tm; time(&tm); cout << __FUNCTION__ << "	nTimerLapse = " << nTimeLapse << "	" << ctime(&tm);
	ReleaseMutex(hMutex);
	return;
}
void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	WaitForSingleObject(hMutex, INFINITE);
	cout << __FUNCTION__ << endl;
	ReleaseMutex(hMutex);
	IsErrorRspInfo(pRspInfo);
	return;
}
bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
	{
		WaitForSingleObject(hMutex, INFINITE);
		cout << __FUNCTION__ << "	ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
		ReleaseMutex(hMutex);
	}
	return bResult;
}


