#pragma once
#include <iostream>
#include <fstream>
#include<sstream>
#include <cstring>
#include <windows.h>
#include<time.h>
#include<process.h>
#include <algorithm>
#include<iomanip>
#include<io.h>
#include<direct.h>
#include <conio.h>
using namespace std;
#include ".\ThostTraderApi\ThostFtdcTraderApi.h"
#include ".\ThostTraderApi\ThostFtdcMdApi.h"
#include"TdSpi.h"
#include"MdSpi.h"
#include"FT.h"
#include"Server.h"
#include"Function.h"
CThostFtdcTraderApi *pUserApi;
CTraderSpi* pUserSpi;
CThostFtdcMdApi *pMdApi;
CMdSpi *pMdSpi;
CServer *pSer;
vector<CFt*> pFt;
HANDLE Events[MAXNUM], hMutex, hLock;
CServer::CServer(bool fire)
{
	con.fire = fire;
	if (con.fire)con.bro_acc = 1;
	else con.bro_acc = 3;
	if (con.bro_acc == 0)
	{
		strcpy(con.brocker_id, "1080");
		strcpy(con.investor_id, "901250099");
		strcpy(con.password, "301415");
	}
	if (con.bro_acc == 1 || con.bro_acc == 5)
	{
		strcpy(con.brocker_id, "9999");
		strcpy(con.investor_id, "013858");//SimNow Account a tel.18923888897
		strcpy(con.password, "yiqian369");
	}
	if (con.bro_acc == 2 || con.bro_acc == 6)
	{
		strcpy(con.brocker_id, "9999");
		strcpy(con.investor_id, "096504");//SimNow Account a tel.13928403028
		strcpy(con.password, "yiqian369");
	}
	if (con.bro_acc == 3 || con.bro_acc == 7)
	{
		strcpy(con.brocker_id, "9999");
		strcpy(con.investor_id, "117698");//SimNow Account a tel.18588293291
		strcpy(con.password, "293291");
	}
	if (con.bro_acc == 4 || con.bro_acc == 8)
	{
		strcpy(con.brocker_id, "9999");
		strcpy(con.investor_id, "117760");//SimNow Account a tel.13760321909
		strcpy(con.password, "321909");
	}
	hMutex = CreateMutex(NULL, FALSE, "cout");
	hLock = CreateMutex(NULL, FALSE, "trade");
	for (int n = 0; n < MAXNUM; n++)Events[n] = CreateEvent(NULL, FALSE, FALSE, NULL);
}
CServer::~CServer()
{
	ServerOut(true);
	CloseHandle(hMutex);
	CloseHandle(hLock);
	for (int n = 0; n < MAXNUM; n++)CloseHandle(Events[n]);
}
void CServer::ServerIn()
{
	time_t tm; time(&tm); cout << __FUNCTION__ << "	" << ctime(&tm);
	if (!TraderConnect())return;
	if (con.fire)
	{
		SettleAccout();
		PosClean();
	}
	if (!ComIns())return;
	if (!MarkedConnect())return;

	if (!TraderLoad())
	{
		ServerIn();
		return;
	}
	if (!con.fire)ServerOut(false);
	return;
}
void CServer::ServerOut(bool bothout)
{
	if (bothout)
	{
		RecordFTResult();
		double tpit = 0, tfit = 0, tpms = 0, tcms = 0;
		if (!pFt.empty())
		for (auto pft = pFt.begin(); pft != pFt.end(); pft++)
		{
			if ((*pft)->cmds.empty())continue;
			for (auto cm = (*pft)->cmds.begin(); cm != (*pft)->cmds.end(); cm++)
			{
				tpit += cm->pit;
				tpms += cm->pms;
				tfit += cm->fit;
				tcms += cm->cms;
			}
		}
		cout << fixed << setprecision(2)
			<< "总结** " << con.trading_day << " " << TimeFtdc(con.distms) << " tpit " << tpit << " tpms " << -tpms << " rfit " << tfit << " nfit " << tpit - tpms << endl;
		string fn = string(con.rst_dir) + string("FT4.0FIT.txt");
		ofstream outfile(fn, ios::app);
		if (outfile.is_open())
		{
			outfile << con.trading_day << " " << TimeFtdc(con.distms)<< "tpit " << tpit << " tpms " << -tpms << " tfit " << tfit << " tcms " << -tcms << " NETFIT " << tpit - tpms << endl;
			outfile.close();
		}
		if (con.fire)
		{
			SettleAccout();
			WaitForSingleObject(Events[0], 50000);
			poss.clear();
			pUserSpi->ReqQryInvestorPosition("");
			WaitForSingleObject(Events[0], 50000);
		}
		if (!pFt.empty())
		for (auto cft = pFt.begin(); cft != pFt.end();)
		{
			WaitForSingleObject(hLock, INFINITE);
			(*cft)->run_loaded = false;
			ReleaseMutex(hLock);
			Sleep(10000);
			delete *cft;
			*cft = NULL;
			cft = pFt.erase(cft);
		}
		/*
		if (pMdApi && pMdSpi)
		{
		pMdSpi->ReqUserLogout();
		WaitForSingleObject(Events[0], 15000);
		}*/
		if (pMdApi)
		{
			pMdApi->RegisterSpi(NULL);
			pMdApi->Release();
			pMdApi = NULL;
		}
		if (pMdSpi)
		{
			delete pMdSpi;
			pMdSpi = NULL;
		}
	}
	if (pUserApi && pUserSpi)
	{
		pUserSpi->ReqUserLogout();
		WaitForSingleObject(Events[0], 15000);
	}
	if (pUserApi)
	{
		pUserApi->RegisterSpi(NULL);
		pUserApi->Release();
		pUserApi = NULL;
	}
	if (pUserSpi)
	{
		delete pUserSpi;
		pUserSpi = NULL;
	}
	time_t tm; time(&tm); cout << __FUNCTION__ << " " << bothout << " " << ctime(&tm);
	return;
}
bool CServer::TraderConnect()
{
	int kk = 0;
	do
	{
		if (pUserApi)
		{
			pUserApi->RegisterSpi(NULL);
			pUserApi->Release();
			pUserApi = NULL;
		}
		if (pUserSpi)
		{
			delete pUserSpi;
			pUserSpi = NULL;
		}
		pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi("./thosttraderapi.dll");			// 创建UserApi//"./thosttraderapi.dll"
		pUserSpi = new CTraderSpi();
		pUserApi->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi);			// 注册事件类
		pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);				// 注册公有流
		pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);			// 注册私有流
		if (con.bro_acc == 0)
		{
			pUserApi->RegisterFront("tcp://180.169.112.52:41205");//上海电信
			pUserApi->RegisterFront("tcp://180.169.112.53:41205");
			pUserApi->RegisterFront("tcp://180.169.112.54:41205");
			pUserApi->RegisterFront("tcp://180.169.112.55:41205");
			pUserApi->RegisterFront("tcp://140.206.101.109:41205");//上海联通
			pUserApi->RegisterFront("tcp://140.206.101.110:41205");
			pUserApi->RegisterFront("tcp://140.207.168.9:41205");
			pUserApi->RegisterFront("tcp://140.207.168.10:41205");
			pUserApi->RegisterFront("tcp://106.37.231.6:41205");//北京电信
			pUserApi->RegisterFront("tcp://106.37.231.7:41205");
			pUserApi->RegisterFront("tcp://111.205.217.40:41205");//北京联通
			pUserApi->RegisterFront("tcp://111.205.217.41:41205");
		}
		if (con.bro_acc == 1 || con.bro_acc == 2 || con.bro_acc == 3 || con.bro_acc == 4)
		{
			pUserApi->RegisterFront("tcp://180.168.146.187:10000");//SimNow
			pUserApi->RegisterFront("tcp://180.168.146.187:10001");//SimNow
			pUserApi->RegisterFront("tcp://218.202.237.33:10002");//SimNow
		}
		if (con.bro_acc == 5 || con.bro_acc == 6 || con.bro_acc == 7 || con.bro_acc == 8)
		{
			pUserApi->RegisterFront("tcp://180.168.146.187:10030");//SimNow第二套 交易日，16：00～次日09：00；非交易日，16：00～次日15：00。
		}
		pUserApi->Init();
	} while (WaitForSingleObject(Events[0], 15000) != WAIT_OBJECT_0 && ++kk < 5);
	if (kk < 5)return true;
	else return false;
}
void CServer::OnTraderConnect(CThostFtdcRspUserLoginField *pRspUserLogin, bool bIsLast)
{
	time_t tm; time(&tm); cout << __FUNCTION__ << "	" << ctime(&tm);
	if (pRspUserLogin && bIsLast)
	{
		con.distms = TimeDiff(string(pRspUserLogin->LoginTime), TimeFtdc(0));
		if (abs(con.distms) > 180)
		{
			cout << "WARNNING:distms = " << con.distms << " secs, pls corrects!" << endl;
		}
		strcpy(con.trading_day, pUserApi->GetTradingDay());
		strcpy(con.login_time, pRspUserLogin->LoginTime);
		con.front_id = pRspUserLogin->FrontID;
		con.session_id = pRspUserLogin->SessionID;

		stringstream ss;
		ss << pRspUserLogin->MaxOrderRef;
		ss >> pSer->con.i_order_ref;
		ss.clear();

		char path[MAX_PATH], drive[10];
		GetModuleFileName(NULL, path, sizeof(path));
		_splitpath(path, drive, NULL, NULL, NULL);
		con.std_dir = string(drive) + string("\\FDatas\\STD4.0\\");
		if (access(con.std_dir.c_str(), 0) != 0)mkdir(con.std_dir.c_str());
		if (con.fire)
		{
			con.acc_dir = string(drive) + string("\\FDatas\\RST4.0\\fire\\");
			if (access(con.acc_dir.c_str(), 0) != 0)mkdir(con.acc_dir.c_str());
			con.rst_dir = con.acc_dir + string(con.trading_day) + string("\\");
			if (access(con.rst_dir.c_str(), 0) != 0)mkdir(con.rst_dir.c_str());
		}
		else
		{
			con.rst_dir = string(drive) + string("\\FDatas\\RST4.0\\simu\\");
			if (access(con.rst_dir.c_str(), 0) != 0)mkdir(con.rst_dir.c_str());
		}
		SetEvent(Events[0]);
	}
	else
		cout << "		pRspUserLogin return NULL " << endl;
	return;
}
void CServer::SettleAccout()
{
	int trys = 0;
	do
	{
		pUserSpi->ReqQrySettlementInfoConfirm();
	} while (WaitForSingleObject(Events[0], 20000) != WAIT_OBJECT_0 && ++trys < 5);
	return;
}
void CServer::OnSettleAccout(CThostFtdcTradingAccountField *pacc, bool bIsLast)
{
	if (pacc)
	{
		account = *pacc;
		RecordAcc(pacc);
	}
	if (bIsLast)SetEvent(Events[0]);
	return;
}
void CServer::PosClean()
{
	poss.clear();
	pUserSpi->ReqQryInvestorPosition("");
	WaitForSingleObject(Events[0], INFINITE);
	if (!poss.empty())
	for (auto it = poss.begin(); it != poss.end(); it++)
	{
		if (it->Position <= 0)continue;
		it->SettlementID = -1;
		HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, ThreadPosClean, (void*)(&*it), 0, NULL);
		CloseHandle(handle);
		if (WaitForSingleObject(Events[1], INFINITE) == WAIT_OBJECT_0 && it->SettlementID == 0)
		{
			it->SettlementID = 1;
			handle = (HANDLE)_beginthreadex(NULL, 0, ThreadPosClean, (void*)(&*it), 0, NULL);
			CloseHandle(handle);
		}
	}
	return;
}
void CServer::OnReqPos(CThostFtdcInvestorPositionField *pInvestorPosition, bool bIsLast)
{
	if (pInvestorPosition)poss.push_back(*pInvestorPosition);
	if (bIsLast)SetEvent(Events[0]);
	return;
}
unsigned int _stdcall  CServer::ThreadPosClean(void* lpParamter)
{
	CThostFtdcInvestorPositionField* it = (CThostFtdcInvestorPositionField*)lpParamter;
	if (it->SettlementID <= -1)
	{
		pUserSpi->ReqQryDepthMarketData(it->InstrumentID, 1);
		WaitForSingleObject(Events[2], 30 * 1000);
		WaitForSingleObject(hMutex, INFINITE);
		cout << endl << "清仓:" << it->InstrumentID << " now price:" << it->SettlementPrice << "?! Press 'C' to clean in 60 secs：  ";
		int nsec = 0;
		while (!kbhit() && nsec <= 60)
		{
			cout << ((nsec <= 9) ? "\b" : "\b\b") << nsec++;
			Sleep(1000);
		}
		cout << endl;
		if (nsec > 60)
		{
			cout << "Expired 60 secs. Abort Clean." << endl;
		}
		else if ('C' == getchar())
		{
			cout << "OK! Sure clean it." << endl;
			it->SettlementID = 0;
		}
		else
		{
			cout << "Skipped. Abort Clean." << endl;
		}
		fflush(stdin);
		ReleaseMutex(hMutex);
		SetEvent(Events[1]);
	}
	else if (it->SettlementID >= 1)
	{
		while (false)
		{
			for (auto ist = pSer->insts.begin(); ist != pSer->insts.end(); ist++)
			if (strcmp(ist->InstrumentID, it->InstrumentID) == 0 && ist->InstrumentStatus == THOST_FTDC_IS_Continous)break;
			Sleep(30 * 1000);
		}
		int trys = 0;
		do
		{
			pUserSpi->ReqQryDepthMarketData(it->InstrumentID, 1);
		} while (WaitForSingleObject(Events[2], 10 * 1000) != WAIT_OBJECT_0 && ++trys < 10);
		if (trys<10)
		{
			Ord ord;
			memset(&ord, 0, sizeof(ord));
			ord.id = 0;
			strcpy(ord.InstrumentID, it->InstrumentID);
			if (it->PosiDirection == THOST_FTDC_PD_Long)ord.Direction = THOST_FTDC_D_Sell;
			if (it->PosiDirection == THOST_FTDC_PD_Short)ord.Direction = THOST_FTDC_D_Buy;
			ord.LimitPrice = it->SettlementPrice;
			if (it->TodayPosition > 0)
			{
				ord.OffsetFlag = THOST_FTDC_OF_CloseToday;
				ord.VolumeTotalOriginal = it->TodayPosition;
				pUserSpi->ReqOrderInsert(ord);
			}
			if (it->Position - it->TodayPosition > 0)
			{
				ord.OffsetFlag = THOST_FTDC_OF_CloseYesterday;
				ord.VolumeTotalOriginal = it->Position - it->TodayPosition;
				pUserSpi->ReqOrderInsert(ord);
			}
			cout << "清仓:" << it->InstrumentID << " sent price at " << it->SettlementPrice << endl;
		}
		else
		{
			cout << "清仓:" << it->InstrumentID << " can't get price,abort! " << endl;
		}
	}
	return 0;
}
bool CServer::ComIns()
{
	if(!ReadCarryInsList())return false;
	pdts.clear();
	for (auto it = vins.begin(); it != vins.end(); it++)
	{
		bool pdtexit = false;
		if (!pdts.empty())for (auto pp = pdts.begin(); pp != pdts.end(); pp++)
		{
			if (string(pp->ProductID) == string(it->ProductID))pdtexit = true;
		}
		if (!pdtexit)
		{
			Pdt pdt;
			memset(&pdt,0,sizeof(pdt));
			strcpy(pdt.ProductID, it->ProductID);
			pdts.push_back(pdt);
		}
	}
	pdts.shrink_to_fit();
	if (pdts.empty())return false;
	for (auto it = pdts.begin(); it != pdts.end(); it++)
	{
		pUserSpi->ReqQryInstrument(it->ProductID);
	}
	WaitForMultipleObjects((int)pdts.size(), Events, true, (int)pdts.size() * 10 * 1000);
	for (auto it = pdts.begin(); it != pdts.end(); it++)
	{
		for (auto ins = it->inss.begin(); ins != it->inss.end(); ins++)
		{
			pUserSpi->ReqQryDepthMarketData(ins->InstrumentID, 0);
		}
	}
	WaitForMultipleObjects((int)pdts.size(), Events, true, (int)pdts.size() * 10 * 1000);
	for (auto it = vins.begin(); it != vins.end(); it++)
	{
		for (auto pp = pdts.begin(); pp != pdts.end(); pp++)
		{
			if (string(pp->ProductID) == string(it->ProductID))
			{
				int ser = atoi(string(it->InstrumentID).substr(0, 1).c_str());
				if (pp->inss.size() >= ser + 1)
				{
					pp->inss.at(ser).InsMgets = it->InsMgets;
					(*it) = pp->inss.at(ser);
				}
			}
		}
	}
	for (auto it = vins.begin(); it != vins.end(); it++)
	{
		pUserSpi->ReqQryInstrumentMarginRate(it->InstrumentID);
	}
	WaitForMultipleObjects((int)vins.size(), Events, true, 2000 * (int)vins.size());
	for (auto it = vins.begin(); it != vins.end(); it++)
	{
		pUserSpi->ReqQryInstrumentCommissionRate(it->InstrumentID);
	}
	WaitForMultipleObjects((int)vins.size(), Events, true, 2000 * (int)vins.size());
	for (auto it = vins.begin(); it != vins.end();)
	{
		if (!it->dep_get|| it->PreOpenInterest<10000 || !it->mrg_get || !it->cmt_get || it->InsMgets.empty())
		{
			cout << fixed << setprecision(2)
				<< "Unverified " << it->InstrumentID << " PrOpIn " << it->PreOpenInterest << " OpInst " << it->OpenInterest << " IsTrad " << it->IsTrading << " mrg_get " << it->mrg_get << " cmt_get " << it->cmt_get << endl;
			it = vins.erase(it);
		}
		else
		{
			RecordIns(&(*it));
			it++;
		}
	}
	vins.shrink_to_fit();
	if (vins.empty())return false;
	else if (vins.size() >= CARRYNUM)vins.erase(vins.begin() + CARRYNUM, vins.end());
	cout << "ComIns verified " << vins.size() << endl;
	return true;
}
void CServer::OnReqIns(CThostFtdcInstrumentField *pInstrument, bool bIsLast)
{
	if (pInstrument && !pdts.empty())
	for (auto pdt = pdts.begin(); pdt != pdts.end(); pdt++)
	{
		if (strcmp(pdt->ProductID, pInstrument->ProductID) == 0)
		{
			if (pInstrument->InstLifePhase == THOST_FTDC_IP_Started && pInstrument->IsTrading)
			{
				Ins ins;
				memset(&ins, 0, sizeof(ins));
				strcpy(ins.InstrumentID, pInstrument->InstrumentID);
				strcpy(ins.ProductID, pInstrument->ProductID);
				strcpy(ins.ExchangeID, pInstrument->ExchangeID);
				ins.IsTrading = pInstrument->IsTrading;
				ins.InstLifePhase = pInstrument->InstLifePhase;
				strcmp(ins.ExpireDate, pInstrument->ExpireDate);
				strcmp(ins.StartDelivDate, pInstrument->StartDelivDate);
				strcpy(ins.OpenDate, pInstrument->OpenDate);
				ins.VolumeMultiple = pInstrument->VolumeMultiple;
				ins.PriceTick = pInstrument->PriceTick;
				ins.LongMarginRatio = pInstrument->LongMarginRatio;
				ins.ShortMarginRatio = pInstrument->ShortMarginRatio;
				ins.MaxLimitOrderVolume = pInstrument->MaxLimitOrderVolume;
				ins.MaxMarginSideAlgorithm = pInstrument->MaxMarginSideAlgorithm;
				ins.MaxMarketOrderVolume = pInstrument->MaxMarketOrderVolume;
				ins.MinLimitOrderVolume = pInstrument->MinLimitOrderVolume;
				ins.MinMarketOrderVolume = pInstrument->MinMarketOrderVolume;
				pdt->inss.push_back(ins);
			}
			if (bIsLast)
			{
				pdt->inss.shrink_to_fit();
				SetEvent(Events[pdt - pdts.begin()]);
				//cout << pdt->ProductID << " OnReqIns Events " << pdt - pdts.begin() << endl;
				break;
			}
		}
	}
	return;
}
void CServer::OnQryDepthMarket(CThostFtdcDepthMarketDataField *pDepthMarketData, int mode)
{
	if (mode <= 0)
	{
		if (pDepthMarketData && !pdts.empty())
		for (auto pdt = pdts.begin(); pdt != pdts.end(); pdt++)
		{
			if (pdt->inss.empty())continue;
			for (auto it = pdt->inss.begin(); it != pdt->inss.end(); it++)
			{
				if (strcmp(it->InstrumentID, pDepthMarketData->InstrumentID) == 0)
				{
					it->OpenInterest = pDepthMarketData->OpenInterest;
					it->PreOpenInterest = pDepthMarketData->PreOpenInterest;
					it->UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
					it->LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
					it->dep_get = true;
					if (++pdt->depqried >= (int)pdt->inss.size())
					{
						sort(pdt->inss.begin(), pdt->inss.end(), SortOpenInsterst);
						SetEvent(Events[pdt - pdts.begin()]);
					}
					break;
				}
			}
		}
	}
	else if (mode >= 1)
	{
		if (!poss.empty())
		for (auto it = poss.begin(); it != poss.end(); it++)
		{
			if (string(pDepthMarketData->InstrumentID) == string(it->InstrumentID))
			{
				it->PreSettlementPrice = pDepthMarketData->PreSettlementPrice;
				if (it->PosiDirection == THOST_FTDC_PD_Long)it->SettlementPrice = pDepthMarketData->BidPrice1;
				if (it->PosiDirection == THOST_FTDC_PD_Short)it->SettlementPrice = pDepthMarketData->AskPrice1;
				if (abs(it->SettlementPrice / it->PreSettlementPrice - 1) * 100 < 10)SetEvent(Events[2]);
				break;
			}
		}
	}
	return;
}
void CServer::OnQryMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate)
{
	for (auto it = vins.begin(); it != vins.end(); it++)
	{
		if (strcmp(pInstrumentMarginRate->InstrumentID, it->InstrumentID) == 0 || strcmp(pInstrumentMarginRate->InstrumentID, it->ProductID) == 0)
		{
			it->InvestorRange = pInstrumentMarginRate->InvestorRange;
			it->IsRelative = pInstrumentMarginRate->IsRelative;
			it->LongMarginRatioByMoney = pInstrumentMarginRate->LongMarginRatioByMoney;
			it->LongMarginRatioByVolume = pInstrumentMarginRate->LongMarginRatioByVolume;
			it->ShortMarginRatioByMoney = pInstrumentMarginRate->ShortMarginRatioByMoney;
			it->ShortMarginRatioByVolume = pInstrumentMarginRate->ShortMarginRatioByVolume;
			it->mrg_get = true;
			SetEvent(Events[it - vins.begin()]);
			break;
		}
	}
	return;
}
void CServer::OnQryCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate)
{
	for (auto it = vins.begin(); it != vins.end(); it++)
	{
		if (strcmp(pInstrumentCommissionRate->InstrumentID, it->InstrumentID) == 0 || strcmp(pInstrumentCommissionRate->InstrumentID, it->ProductID) == 0)
		{
			it->CloseRatioByMoney = pInstrumentCommissionRate->CloseRatioByMoney;
			it->CloseRatioByVolume = pInstrumentCommissionRate->CloseRatioByVolume;
			it->CloseTodayRatioByMoney = pInstrumentCommissionRate->CloseTodayRatioByMoney;
			it->CloseTodayRatioByVolume = pInstrumentCommissionRate->CloseTodayRatioByVolume;
			it->OpenRatioByMoney = pInstrumentCommissionRate->OpenRatioByMoney;
			it->OpenRatioByVolume = pInstrumentCommissionRate->OpenRatioByVolume;
			it->cmt_get = true;
			SetEvent(Events[it - vins.begin()]);
			break;
		}
	}
	return;
}
bool CServer::MarkedConnect()
{
	int kk = 0;
	do
	{
		if (pMdApi)
		{
			pMdApi->RegisterSpi(NULL);
			pMdApi->Release();
			pMdApi = NULL;
		}
		if (pMdSpi)
		{
			delete pMdSpi;
			pMdSpi = NULL;
		}
		pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("./thostmduserapi.dll");					// 创建MdApi//"./thostmduserapi.dll"
		pMdSpi = new CMdSpi();
		pMdApi->RegisterSpi(pMdSpi);								// 注册事件类
		if (con.bro_acc == 0)
		{
			pMdApi->RegisterFront("tcp://180.169.112.53:41213");//上海电信
			pMdApi->RegisterFront("tcp://180.169.112.53:41213");
			pMdApi->RegisterFront("tcp://180.169.112.54:41213");
			pMdApi->RegisterFront("tcp://180.169.112.55:41213");
			pMdApi->RegisterFront("tcp://140.206.101.109:41213");//上海联通
			pMdApi->RegisterFront("tcp://140.206.101.110:41213");
			pMdApi->RegisterFront("tcp://140.207.168.9:41213");
			pMdApi->RegisterFront("tcp://140.207.168.10:41213");
			pMdApi->RegisterFront("tcp://106.37.231.6:41213");//北京电信
			pMdApi->RegisterFront("tcp://106.37.231.7:41213");
			pMdApi->RegisterFront("tcp://111.205.217.40:41213");//北京联通
			pMdApi->RegisterFront("tcp://111.205.217.41:41213");
		}
		if (con.bro_acc == 1 || con.bro_acc == 2 || con.bro_acc == 3 || con.bro_acc == 4)
		{
			pMdApi->RegisterFront("tcp://180.168.146.187:10010");//SimNow
			pMdApi->RegisterFront("tcp://180.168.146.187:10011");//SimNow
			pMdApi->RegisterFront("tcp://218.202.237.33:10012");//SimNow
		}
		if (con.bro_acc == 5 || con.bro_acc == 6 || con.bro_acc == 7 || con.bro_acc == 8)
		{
			pMdApi->RegisterFront("tcp://180.168.146.187:10031");//SimNow第二套 交易日，16：00～次日09：00；非交易日，16：00～次日15：00。
		}
		pMdApi->Init();
	} while (WaitForSingleObject(Events[0], 15000) != WAIT_OBJECT_0 && ++kk<5);
	if (kk < 5)return true;
	else return false;
}
void CServer::OnMarkedConnect(CThostFtdcRspUserLoginField *pRspUserLogin)
{
	//time_t tm; time(&tm); cout << __FUNCTION__ << "	" << ctime(&tm);
	if (pRspUserLogin)
	{
		SubMarket();
	}
	else
		cout << "		pRspUserLogin return NULL " << endl;
	return;
}
void CServer::SubMarket()
{
	//time_t tm; time(&tm); cout << __FUNCTION__ << "	" << ctime(&tm);
	char* instrument[MAXNUM];
	int i = 0;
	for (auto it = vins.begin(); it != vins.end(); it++)
	{
		instrument[i++] = it->InstrumentID;
	}
	pMdSpi->SubscribeMarketData(instrument, (int)vins.size());
	return;
}
void CServer::OnSubMarket(CThostFtdcSpecificInstrumentField *pSpecificInstrument, bool bIsLast)
{
	//time_t tm; time(&tm); cout << __FUNCTION__ << "	" << ctime(&tm);
	if (pSpecificInstrument)
	{
		if (bIsLast)SetEvent(Events[0]);
	}
	else cout << "		pSpecificInstrument return NULL." << endl;
}
bool CServer::TraderLoad()
{
	if (!pFt.empty())
	{
		pFt.clear();
		pFt.swap(vector<CFt*>());
	}
	for (auto it = vins.begin(); it != vins.end(); it++)
	{
		CFt* cft = new CFt(*it);
		pFt.push_back(cft);
		HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, ThreadFtRun, (void*)pFt.back(), 0, NULL);
		CloseHandle(handle);
		Sleep(500);
	}
	if (pFt.size() != vins.size())
	{
		time_t tm; time(&tm);
		cout << __FUNCTION__ << "	Failore pFt.size() " << pFt.size() << "	" << ctime(&tm);
		return false;
	}
	else return true;
}
unsigned int _stdcall  CServer::ThreadFtRun(void* lpParamter)
{
	CFt* it = (CFt*)lpParamter;
	it->run_loaded = true;
	it->DRun();
	return 0;
}
bool CServer::ReadCarryInsList()
{
	stringstream sss;
	sss << con.std_dir << "modes.txt";
	string fn = sss.str();
	ifstream infile(fn, ios::in);
	static char line[1024] = { 0 };
	if (infile.is_open())
	{
		vins.clear();
		vins.reserve(MAXNUM);
		while (infile.getline(line, sizeof(line)))
		{
			Mget mget;
			double buf;
			sscanf(line, "%[^;];%d;%d;%lf;%lf;%lf;%lf;%lf;%lf",
				mget.name,&mget.mode,&mget.drec,
				&mget.fit[0], &mget.fit[1], &mget.fit[2], &mget.fit[3], &mget.fit[4], &mget.fit[5]);
			//if(con.fire && strcmp(in.ExchangeID, "CFFEX") == 0)continue;
			bool insexit = false;
			if (!vins.empty())for (auto pins = vins.begin(); pins != vins.end(); pins++)
			{
				if (string(pins->InstrumentID) == mget.name)
				{
					pins->InsMgets.push_back(mget);
					insexit = true;
				}
			}
			if (!insexit)
			{
				Ins ins;
				memset(&ins, 0, sizeof(ins));
				ins.InsMgets.push_back(mget);
				strcpy(ins.InstrumentID,mget.name.c_str());
				strcpy(ins.ProductID, mget.name.substr(2, mget.name.length() - 2).c_str());
				vins.push_back(ins);
			}
			if (vins.size() >= READYNUM)break;
		}
		infile.close();
	}
	else
	{
		cerr << __FUNCTION__ << "	File Error! " << fn << endl;
		return;
	}
	vins.shrink_to_fit();
	int nn = (int)vins.size();
	if (nn <= 0 || nn>MAXNUM)
	{
		cout << " Empty || Exceed MAXNUM!!,Please Modify!" << endl;
		return false;
	}
	else
	{
		return true;
	}
}
void CServer::RecordAcc(CThostFtdcTradingAccountField *pacc)
{
	string fn = con.acc_dir + "Accounts.txt";
	ofstream outfile(fn, ios::app);
	if (outfile.is_open())
	{
		SYSTEMTIME sys_time; GetLocalTime(&sys_time);
		outfile << fixed << setprecision(2)
			<< "Clsfit	" << pacc->CloseProfit << " Posfit	" << pacc->PositionProfit << "	cms	" << -pacc->Commission << "	bal " << pacc->Balance << "	pre	" << pacc->PreBalance << "	avl " << pacc->Available << "	day	" << pacc->TradingDay
			<< "	" << sys_time.wMonth << "-" << sys_time.wDay << "-" << sys_time.wHour << ":" << sys_time.wMinute << endl;
		outfile.close();
	}
	return;
}
void CServer::RecordIns(Ins *pInstrument)
{
	string fn = con.rst_dir + "InstrumentD.txt";
	ofstream outfile(fn, ios::app);
	if (outfile.is_open())
	{
		time_t tm; time(&tm);
		outfile << "QryInstrumentInfo:" << "------------------" << ctime(&tm)
			<< "合约代码 " << pInstrument->InstrumentID << " 交易所代码 " << pInstrument->ExchangeID << " 产品代码 " << pInstrument->ProductID << " 合约数量乘数 " << pInstrument->VolumeMultiple << " 最小变动价位 " << pInstrument->PriceTick << endl
			<< "开仓手续费率 " << pInstrument->OpenRatioByMoney << " 平仓手续费率 " << pInstrument->CloseRatioByMoney << " 平今手续费率 " << pInstrument->CloseTodayRatioByMoney << " 多头保证金率 " << pInstrument->LongMarginRatio << " 空头保证金率 " << pInstrument->ShortMarginRatio << endl
			<< "上市日 " << pInstrument->OpenDate << " 到期日 " << pInstrument->ExpireDate << " PreSettlementPrice " << pInstrument->PreSettlementPrice<< " PreOpenInterest " << pInstrument->PreOpenInterest << endl;
		if (!pInstrument->InsMgets.empty())for (auto mg = pInstrument->InsMgets.begin(); mg != pInstrument->InsMgets.end(); mg++)
			outfile << "Mget name " << mg->name << " mode " << mg->mode << " drec " << mg->drec << " expfit " << mg->fit[0] << endl;
		outfile.close();
	}
	return;
}
void CServer::RecordFTResult()
{
	if (pFt.empty())return;
	string tm = con.login_time;
	tm = tm.substr(0, 2) + tm.substr(3, 2);
	string fn = con.rst_dir + string(con.trading_day) + tm + "FTRst30.txt";
	ofstream outfile(fn, ios::trunc);
	if (outfile.is_open())
	{
		time_t tm; time(&tm);
		outfile << ctime(&tm);
		for (auto it = pFt.begin(); it != pFt.end(); it++)
		{
			if ((*it)->cmds.empty())continue;
			for (auto cm = (*it)->cmds.begin(); cm != (*it)->cmds.end(); cm++)
				outfile << (*it)->ins.InstrumentID << "	id " << cm->id << " pit " << cm->pit << " pms " << cm->pms << " fit " << cm->fit << " typ " << cm->typ << " prc " << cm->prc << endl;
		}
		outfile.close();
	}
	return;
}
