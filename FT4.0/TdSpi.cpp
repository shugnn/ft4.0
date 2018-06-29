#pragma once
#include <iostream>
#include <windows.h>
#include<time.h>
#include<fstream>
#include<sstream>
#include<iomanip>
using namespace std;
#include ".\ThostTraderApi\ThostFtdcTraderApi.h"
#include "Server.h"
#include "FT.h"
#include "TdSpi.h"
#include"Function.h"
#pragma warning(disable : 4996)
extern CThostFtdcTraderApi* pUserApi;
extern CServer *pSer;
extern vector<CFt*> pFt;
extern HANDLE Events[], hMutex, hLock;
void CTraderSpi::OnFrontConnected()
{
	//cout << __FUNCTION__ << endl;
	//ReqTradingAccountPasswordUpdate();
	ReqUserLogin();
}
void CTraderSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.UserID, pSer->con.investor_id);
	strcpy(req.Password, pSer->con.password);
	int iResult = pUserApi->ReqUserLogin(&req, ++nRequestID);
	//cout << __FUNCTION__ << " nRequestID: " << nRequestID << " iResult: " << iResult << endl;
}
void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//cout << __FUNCTION__ << " nRequestID: " << nRequestID << endl;
	if (!IsErrorRspInfo(pRspInfo))
	{
		pSer->OnTraderConnect(pRspUserLogin, bIsLast);
	}
}
void CTraderSpi::ReqUserLogout()
{
	CThostFtdcUserLogoutField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.UserID, pSer->con.investor_id);
	int iResult = pUserApi->ReqUserLogout(&req, ++nRequestID);
	return;
}
void CTraderSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) && bIsLast)
	{
		cout << __FUNCTION__ << " " << TimeFtdc(pSer->con.distms) << endl;
		SetEvent(Events[0]);
	}
	return;
}
void CTraderSpi::ReqQrySettlementInfoConfirm()
{
	CThostFtdcQrySettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.InvestorID, pSer->con.investor_id);
	int iResult = pUserApi->ReqQrySettlementInfoConfirm(&req, ++nRequestID);
	//cout << __FUNCTION__ << " nRequestID: " << nRequestID << " iResult: " << iResult << endl;
}
void CTraderSpi::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//cout << __FUNCTION__ << " nRequestID: " << nRequestID << endl;
	if (!IsErrorRspInfo(pRspInfo) && pSettlementInfoConfirm && bIsLast)
	{
		if (string(pSettlementInfoConfirm->ConfirmDate) == string(pSer->con.trading_day))
		{
			WaitForSingleObject(hMutex, INFINITE);
			cout << "Aleady Today comfirned info:" << endl;
			cout << "    BrokerID: " << pSettlementInfoConfirm->BrokerID << " InvestorID: " << pSettlementInfoConfirm->InvestorID << " ConfirmDate: " << pSettlementInfoConfirm->ConfirmDate << " Time: " << pSettlementInfoConfirm->ConfirmTime << endl;
			ReleaseMutex(hMutex);
			ReqQryTradingAccount();
			return;
		}
	}
	if (bIsLast)ReqSettlementInfoConfirm();
	return;
}
void CTraderSpi::ReqQrySettlementInfo()
{
	CThostFtdcQrySettlementInfoField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.InvestorID, pSer->con.investor_id);
	//strcpy(req.TradingDay, TradingDay);
	int iResult = pUserApi->ReqQrySettlementInfo(&req, ++nRequestID);
	//cout << __FUNCTION__ << " nRequestID: " << nRequestID << " iResult: " << iResult << endl;
}
void CTraderSpi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << __FUNCTION__ << " nRequestID: " << nRequestID << "	" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		if (pSettlementInfo)
		{/*
		 cout << "		TradingDay: " << pSettlementInfo->TradingDay << endl;
		 cout << "		BrokerID: " << pSettlementInfo->BrokerID << endl;
		 cout << "		InvestorID: " << pSettlementInfo->InvestorID << endl;
		 cout << "		SettlementID: " << pSettlementInfo->SettlementID << endl;
		 cout << "		SequenceNo: " << pSettlementInfo->SequenceNo << endl;
		 cout << "		Content: " << pSettlementInfo->Content << endl;*/
		}
		else cout << "pSettlementInfo==NULL" << endl;
	}
	return;
}
void CTraderSpi::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.InvestorID, pSer->con.investor_id);
	int iResult = pUserApi->ReqSettlementInfoConfirm(&req, ++nRequestID);
	//cout << __FUNCTION__ << " nRequestID: " << nRequestID << " iResult:" << iResult << endl;
}
void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//cout << __FUNCTION__ << " nRequestID: " << nRequestID << "	" << endl;
	if (!IsErrorRspInfo(pRspInfo) && pSettlementInfoConfirm)
	{
		WaitForSingleObject(hMutex, INFINITE);
		cout << "First of Today comfirmed info:" << endl;
		cout << "    BrokerID: " << pSettlementInfoConfirm->BrokerID << " InvestorID: " << pSettlementInfoConfirm->InvestorID << " ConfirmDate: " << pSettlementInfoConfirm->ConfirmDate << " Time: " << pSettlementInfoConfirm->ConfirmTime << endl;
		ReleaseMutex(hMutex);
	}
	if (bIsLast)ReqQryTradingAccount();
	return;
}
void CTraderSpi::ReqQryTradingAccount()
{
	WaitForSingleObject(hLock, INFINITE);
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.InvestorID, pSer->con.investor_id);
	int iResult = pUserApi->ReqQryTradingAccount(&req, ++nRequestID);
	Sleep(350);
	ReleaseMutex(hLock);
	return;
}
void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//cout << __FUNCTION__ << " nRequestID: " << nRequestID << "	" << endl;
	if (!IsErrorRspInfo(pRspInfo) && pTradingAccount)
	{
		WaitForSingleObject(hMutex, INFINITE);
		cout << fixed << setprecision(2);
		cout << "账户&.Posfit:" << pTradingAccount->PositionProfit << " Clsfit:" << pTradingAccount->CloseProfit << " Commis:" << -pTradingAccount->Commission << "	NETFITS:" << pTradingAccount->PositionProfit + pTradingAccount->CloseProfit - pTradingAccount->Commission << endl;
		cout << "    Bala: " << pTradingAccount->Balance << "  PreB: " << pTradingAccount->PreBalance << " Avai: " << pTradingAccount->Available << " With: " << pTradingAccount->WithdrawQuota << endl;//可取资金
		ReleaseMutex(hMutex);
	}
	pSer->OnSettleAccout(pTradingAccount, bIsLast);
	return;
}
void CTraderSpi::ReqQryInvestorPosition(char* instrument)
{
	WaitForSingleObject(hLock, INFINITE);
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.InvestorID, pSer->con.investor_id);
	strcpy(req.InstrumentID, instrument);
	int iResult = pUserApi->ReqQryInvestorPosition(&req, ++nRequestID);
	Sleep(350);
	ReleaseMutex(hLock);
	return;
}
void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) && pInvestorPosition)
	{
		static int numps = 0;
		WaitForSingleObject(hMutex, INFINITE);
		cout << fixed << setprecision(2);
		cout << "仓位" << ++numps << "#" << pInvestorPosition->InstrumentID << endl;
		cout << "    现持仓:" << pInvestorPosition->Position << " 前持仓:" << pInvestorPosition->YdPosition << " 今持仓:" << pInvestorPosition->TodayPosition << " 多空方向:" << pInvestorPosition->PosiDirection << endl;
		cout << "    平仓量:" << pInvestorPosition->CloseVolume << " 开仓量:" << pInvestorPosition->OpenVolume << " 本次结算价:" << pInvestorPosition->SettlementPrice << " 上次结算价:" << pInvestorPosition->PreSettlementPrice << endl;
		cout << "    平仓盈亏:" << pInvestorPosition->CloseProfit << " 持仓盈亏:" << pInvestorPosition->PositionProfit << " 逐日盈亏:" << pInvestorPosition->CloseProfitByDate << " 逐笔盈亏:" << pInvestorPosition->CloseProfitByTrade << endl;
		cout << "    手续费:" << -pInvestorPosition->Commission << " 持仓成本:" << pInvestorPosition->PositionCost << " 开仓成本:" << pInvestorPosition->OpenCost << " 保证金率:" << pInvestorPosition->MarginRateByMoney << endl;
		ReleaseMutex(hMutex);
		if (bIsLast)numps = 0;
	}
	pSer->OnReqPos(pInvestorPosition, bIsLast);
	return;
}
void CTraderSpi::ReqQryInvestorPositionDetail(char* instrument)
{
	WaitForSingleObject(hLock, INFINITE);
	CThostFtdcQryInvestorPositionDetailField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.InvestorID, pSer->con.investor_id);
	strcpy(req.InstrumentID, instrument);
	int iResult = pUserApi->ReqQryInvestorPositionDetail(&req, ++nRequestID);
	Sleep(350);
	ReleaseMutex(hLock);
	return;
}
void CTraderSpi::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo) || !pInvestorPositionDetail)return;
	if (!pFt.empty())
	for (auto pf = pFt.begin(); pf != pFt.end(); pf++)
	{
		if (strcmp((*pf)->ins.InstrumentID, pInvestorPositionDetail->InstrumentID) == 0)
		{
			(*pf)->OnQryPosDetail(pInvestorPositionDetail, bIsLast);
			break;
		}
	}
	return;
}
void CTraderSpi::ReqQryInstrument(char* instrument)
{
	WaitForSingleObject(hLock, INFINITE);
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, instrument);
	int iResult = pUserApi->ReqQryInstrument(&req, ++nRequestID);
	Sleep(350);
	ReleaseMutex(hLock);
	//cout << __FUNCTION__ << " nRequestID: " << nRequestID << " iResult:" << iResult << endl;
	return;
}
void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo))
	{
		pSer->OnReqIns(pInstrument, bIsLast);
	}
	return;
}
void CTraderSpi::ReqQryDepthMarketData(char* instrument, int mode)
{
	WaitForSingleObject(hLock, INFINITE);
	CThostFtdcQryDepthMarketDataField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, instrument);
	int iResult = pUserApi->ReqQryDepthMarketData(&req, ++nRequestID);
	DEP dep;
	dep.nRequestID = nRequestID;
	dep.mode = mode;
	depnq.push_back(dep);
	Sleep(350);
	ReleaseMutex(hLock);
	return;
}
void CTraderSpi::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) && pDepthMarketData && bIsLast)
	{
		if (!depnq.empty())
		for (auto it = depnq.begin(); it != depnq.end(); it++)
		{
			if (it->nRequestID == nRequestID)
			{
				pSer->OnQryDepthMarket(pDepthMarketData, it->mode);
				break;
			}
		}
	}
	return;
}
void CTraderSpi::ReqQryInstrumentMarginRate(char* instrument)
{
	WaitForSingleObject(hLock, INFINITE);
	CThostFtdcQryInstrumentMarginRateField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.InvestorID, pSer->con.investor_id);
	strcpy(req.InstrumentID, instrument);
	req.HedgeFlag = THOST_FTDC_HF_Speculation;	//投机
	int iResult = pUserApi->ReqQryInstrumentMarginRate(&req, ++nRequestID);
	Sleep(350);
	ReleaseMutex(hLock);
	return;
}
void CTraderSpi::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInstrumentMarginRate && bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		pSer->OnQryMarginRate(pInstrumentMarginRate);
	}
	return;
}
/*
///请求查询合约保证金率
void CTraderSpi::ReqQryInstrumentMarginRate()
{
CThostFtdcQryInstrumentMarginRateField req;
memset(&req, 0, sizeof(req));
strcpy(req.BrokerID, pSer->ref.brocker_id);
strcpy(req.InvestorID, pSer->ref.investor_id);
strcpy(req.InstrumentID, pSer->ref.instrument_id[0]);
req.HedgeFlag = THOST_FTDC_HF_Speculation;	//投机
int iResult = pUserApi->ReqQryInstrumentMarginRate(&req, ++nRequestID);
cout << __FUNCTION__ << " nRequestID: " << nRequestID << " iResult:" << iResult << endl;
}
void CTraderSpi::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
cout << __FUNCTION__ << " nRequestID: " << nRequestID << "	" << endl;
if (pInstrumentMarginRate && bIsLast && !IsErrorRspInfo(pRspInfo))
{
double bzj_b = 100 * (pInstrumentMarginRate->LongMarginRatioByMoney);
cout << "		合约保证金率:" << pInstrumentMarginRate->InstrumentID << "_" << bzj_b << "%" << endl;
}
}
void CTraderSpi::ReqQryInstrumentCommissionRate()
{
CThostFtdcQryInstrumentCommissionRateField req;
memset(&req, 0, sizeof(req));
strcpy(req.BrokerID, pSer->ref.brocker_id);
strcpy(req.InvestorID, pSer->ref.investor_id);
strcpy(req.InstrumentID, pSer->ref.instrument_id[0]);
int iResult = pUserApi->ReqQryInstrumentCommissionRate(&req, ++nRequestID);
cout << __FUNCTION__ << " nRequestID: " << nRequestID << " iResult:" << iResult << endl;
}
void CTraderSpi::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
cout << __FUNCTION__ << " nRequestID: " << nRequestID << "	" << endl;
if (pInstrumentCommissionRate && bIsLast && !IsErrorRspInfo(pRspInfo))
{
double sxf_w = 100 * 100 * (pInstrumentCommissionRate->OpenRatioByMoney);
cout << "		合约手续费率:" << pInstrumentCommissionRate->InstrumentID << "_" << sxf_w << "%%_" << pInstrumentCommissionRate->CloseTodayRatioByVolume << "RMB" << endl;
}
}
*/
void CTraderSpi::ReqQryInstrumentCommissionRate(char* instrument)
{
	WaitForSingleObject(hLock, INFINITE);
	CThostFtdcQryInstrumentCommissionRateField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.InvestorID, pSer->con.investor_id);
	strcpy(req.InstrumentID, instrument);
	int iResult = pUserApi->ReqQryInstrumentCommissionRate(&req, ++nRequestID);
	Sleep(350);
	ReleaseMutex(hLock);
	return;
}
void CTraderSpi::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInstrumentCommissionRate && bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		pSer->OnQryCommissionRate(pInstrumentCommissionRate);
	}
	return;
}
void CTraderSpi::ReqOrderInsert(Ord &ord)
{
	WaitForSingleObject(hLock, INFINITE);
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, ord.InstrumentID);
	req.Direction = ord.Direction;
	req.CombOffsetFlag[0] = ord.OffsetFlag;
	req.VolumeTotalOriginal = ord.VolumeTotalOriginal;

	req.ContingentCondition = ord.ContingentCondition;
	req.StopPrice = ord.StopPrice;
	req.OrderPriceType = ord.OrderPriceType;
	req.LimitPrice = ord.LimitPrice;

	req.TimeCondition = THOST_FTDC_TC_GFD;
	req.VolumeCondition = THOST_FTDC_VC_AV;

	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	req.MinVolume = 1;
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	req.IsAutoSuspend = 0;
	req.UserForceClose = 0;

	stringstream ss;
	ss << ++pSer->con.i_order_ref;
	ss >> ord.OrderRef;
	ss.clear();
	strcpy(req.OrderRef, ord.OrderRef);
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.InvestorID, pSer->con.investor_id);

	int iResult = pUserApi->ReqOrderInsert(&req, ++nRequestID);
	if (iResult == 0)
	{
		strcpy(ord.msg, "挂单");
		ord.nRequestID = nRequestID;
	}
	else strcpy(ord.msg, "挂单失败");
	pSer->ords.push_back(ord);
	Sleep(350);
	ReleaseMutex(hLock);
	return;
}
// 报单后，如不能通过THOST校验，THOST拒绝报单，返回OnRspOrderInsert（含错误编码）。如果校验通过，THOST接收报单，转发给交易所
// 交易所收到报单后，通过校验。用户会收到OnRtnOrder、OnRtnTrade。
// 如果交易所认为报单错误，用户就会收到OnErrRtnOrder
void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrder && !pSer->ords.empty())
	for (auto por = pSer->ords.begin(); por != pSer->ords.end(); por++)
	{
		if (por->nRequestID == nRequestID)
		{
			if (IsErrorRspInfo(pRspInfo))strcpy(por->msg, pRspInfo->ErrorMsg);
			else strcpy(por->msg, "挂单错误");
			WaitForSingleObject(hMutex, INFINITE);
			time_t tm; time(&tm);
			cout << "响应" << por->id << "." << pInputOrder->InstrumentID << "	typ:" << ((pInputOrder->Direction == '0') ? "买" : "卖") << ((pInputOrder->CombOffsetFlag[0] == '0') ? "开" : "平") << "	Ref：" << pInputOrder->OrderRef << "	" << por->msg << "	" << ctime(&tm);
			ReleaseMutex(hMutex);
			break;
		}
	}
	//time_t tm; time(&tm);
	//cout << "响应." << pInputOrder->InstrumentID << " Ref " << pInputOrder->OrderRef << " 信息: " << pInputOrder->InstrumentID << "_" << ((pInputOrder->Direction == '0') ? "买" : "卖")
	//	<< "_" << ((pInputOrder->CombOffsetFlag[0] == '0') ? "开仓" : "平仓") << "_" << pRspInfo->ErrorMsg << ctime(&tm);
	return;
}
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	if (pOrder && !pSer->ords.empty())
	for (auto por = pSer->ords.begin(); por != pSer->ords.end(); por++)
	{
		if (strcmp(por->OrderRef, pOrder->OrderRef) == 0)
		{
			por->FrontID = pOrder->FrontID;
			por->SessionID = pOrder->SessionID;
			strcpy(por->ExchangeID, pOrder->ExchangeID);
			strcpy(por->OrderSysID, pOrder->OrderSysID);
			strcpy(por->OrderLocalID, pOrder->OrderLocalID);
			por->RequestID = pOrder->RequestID;

			por->VolumeTraded = pOrder->VolumeTraded;
			por->VolumeTotal = pOrder->VolumeTotal;
			por->OrderSubmitStatus = pOrder->OrderSubmitStatus;
			por->OrderStatus = pOrder->OrderStatus;
			strcpy(por->msg, pOrder->StatusMsg);
			strcpy(por->InsertDate, pOrder->InsertDate);
			strcpy(por->InsertTime, pOrder->InsertTime);
			strcpy(por->UpdateTime, pOrder->UpdateTime);
			strcpy(por->CancelTime, pOrder->CancelTime);
			if (pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)SetEvent(Events[3]);
			//(*pf)->RecordOpts((*or));
			break;
		}
	}
	if (!pFt.empty())
	for (auto pf = pFt.begin(); pf != pFt.end(); pf++)
	{
		if (strcmp((*pf)->ins.InstrumentID, pOrder->InstrumentID) != 0)continue; 
		if ((*pf)->ords.empty())continue;
		for (auto or = (*pf)->ords.begin(); or != (*pf)->ords.end(); or++)
		{
			if (strcmp(or->OrderRef, pOrder->OrderRef) == 0)
			{
				or->FrontID = pOrder->FrontID;
				or->SessionID = pOrder->SessionID;
				strcpy(or->ExchangeID, pOrder->ExchangeID);
				strcpy(or->OrderSysID, pOrder->OrderSysID);
				strcpy(or->OrderLocalID, pOrder->OrderLocalID);
				or->RequestID = pOrder->RequestID;

				or->VolumeTraded = pOrder->VolumeTraded;
				or->VolumeTotal = pOrder->VolumeTotal;
				or->OrderSubmitStatus = pOrder->OrderSubmitStatus;
				or->OrderStatus = pOrder->OrderStatus;
				strcpy(or->msg, pOrder->StatusMsg);
				strcpy(or->InsertDate, pOrder->InsertDate);
				strcpy(or->InsertTime, pOrder->InsertTime);
				strcpy(or->UpdateTime, pOrder->UpdateTime);
				strcpy(or->CancelTime, pOrder->CancelTime);
				(*pf)->RecordOpts((*or));
				if (pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)SetEvent(Events[3]);
				break;
			}
		}
	}
	//time_t tm; time(&tm);cout << "	Ref:" << pOrder->OrderRef << " "<<pOrder->InstrumentID<<" StatusMsg:" << pOrder->StatusMsg << ctime(&tm);
	return;
}
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	if (!pTrade)return;
	if (!pFt.empty())
	for (auto pf = pFt.begin(); pf != pFt.end(); pf++)
	{
		if (strcmp((*pf)->ins.InstrumentID, pTrade->InstrumentID) != 0)continue;
		Tra tra;
		memset(&tra, 0, sizeof(tra));
		strcpy(tra.OrderRef, pTrade->OrderRef);
		strcpy(tra.ExchangeID, pTrade->ExchangeID);
		strcpy(tra.TradeID, pTrade->TradeID);
		strcpy(tra.OrderSysID, pTrade->OrderSysID); 
		strcpy(tra.msg, "成交");

		tra.Direction = pTrade->Direction;
		tra.OffsetFlag = pTrade->OffsetFlag;
		tra.Price = pTrade->Price;
		tra.Volume = pTrade->Volume;
		strcpy(tra.TradeDate, pTrade->TradeDate);
		strcpy(tra.TradeTime, pTrade->TradeTime);
		tra.TradeType = pTrade->TradeType;
		if (!(*pf)->ords.empty())
		for (auto or = (*pf)->ords.begin(); or != (*pf)->ords.end(); or++)
		{
			if (strcmp(or->OrderSysID, tra.OrderSysID) == 0)
			{
				tra.id = or->id;
				break;
			}
		}
		WaitForSingleObject(hMutex, INFINITE);
		cout << fixed << setprecision(2);
		cout << "成交" << tra.id << "." << pTrade->InstrumentID << " typ:" << ((pTrade->Direction == '0') ? "买" : "卖") << ((pTrade->OffsetFlag == '0') ? "开" : "平") << " vol:" << pTrade->Volume << " prc:" << pTrade->Price << " Ref:" << pTrade->OrderRef << " " << pTrade->TradeTime << endl;
		ReleaseMutex(hMutex);
		(*pf)->RecordOpts(tra);
		(*pf)->tras.push_back(tra);
		(*pf)->posdtls.clear();
		ReqQryInvestorPositionDetail(pTrade->InstrumentID);
	}
	ReqQryTradingAccount();
	return;
}
void CTraderSpi::ReqOrderAction(Ord &ord)
{
	WaitForSingleObject(hLock, INFINITE);
	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, ord.InstrumentID);
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.InvestorID, pSer->con.investor_id);

	//strcpy(req.OrderRef, ord.OrderRef);
	//req.FrontID = ord.FrontID;
	//req.SessionID = ord.SessionID;
	strcpy(req.ExchangeID, ord.ExchangeID);
	strcpy(req.OrderSysID, ord.OrderSysID);

	req.ActionFlag = THOST_FTDC_AF_Delete;
	req.OrderActionRef = ++pSer->con.i_order_action_ref;
	int iResult = pUserApi->ReqOrderAction(&req, ++nRequestID);
	if (iResult == 0)
	{
		ord.action.nRequestID = nRequestID;
		ord.action.OrderActionRef = req.OrderActionRef;
		strcpy(ord.msg, "撤单");
	}
	WaitForSingleObject(hMutex, INFINITE);
	cout << "撤单" << ord.id << "." << ord.InstrumentID << " 方向：" << ((ord.Direction == '0') ? "买" : "卖") << " 标志：" << ((ord.OffsetFlag == '0') ? "开" : "平") << ((iResult == 0) ? " 发出" : " 失败") << " Ref:" << ord.OrderRef << endl;
	ReleaseMutex(hMutex);
	if (!pFt.empty())
	for (auto pf = pFt.begin(); pf != pFt.end(); pf++)
	{
		if (strcmp((*pf)->ins.InstrumentID, ord.InstrumentID) == 0)
		{
			(*pf)->RecordOpts(ord);
			break;
		}
	}
	Sleep(350);
	ReleaseMutex(hLock);
	return;
}
void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction && !pSer->ords.empty())
	for (auto por = pSer->ords.begin(); por != pSer->ords.end(); por++)
	{
		if (por->action.nRequestID == nRequestID)
		{
			if (IsErrorRspInfo(pRspInfo))strcpy(por->msg, pRspInfo->ErrorMsg);
			else strcpy(por->msg, "撤单无效");
			//(*pf)->RecordOpts((*or));
			WaitForSingleObject(hMutex, INFINITE);
			time_t tm; time(&tm);
			cout << "撤单无效" << por->id << "." << pInputOrderAction->InstrumentID << "	" << por->msg << "	Ref：" << pInputOrderAction->OrderRef << "	" << ctime(&tm);
			ReleaseMutex(hMutex);
			break;
		}
	}
	//cout << "OnAction " << pInputOrderAction ->InstrumentID<< "	Ref " << pInputOrderAction->OrderRef << " 撤单校验错" << endl;
	IsErrorRspInfo(pRspInfo);
}
void CTraderSpi::OnFrontDisconnected(int nReason)
{
	WaitForSingleObject(hMutex, INFINITE);
	time_t tm; time(&tm);
	cerr << "交易断开! Reason =" + nReason << ctime(&tm);
	ReleaseMutex(hMutex);
	return;
}
void CTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
	WaitForSingleObject(hMutex, INFINITE);
	time_t tm; time(&tm);
	cout << __FUNCTION__ << "	nTimerLapse = " << nTimeLapse << "	" << ctime(&tm);
	ReleaseMutex(hMutex);
	return;
}
void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int iRequestID, bool bIsLast)
{
	cout << __FUNCTION__ << endl;
	IsErrorRspInfo(pRspInfo);
}
bool CTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
	{
		WaitForSingleObject(hMutex, INFINITE);
		time_t tm; time(&tm);
		cout << "	ErrorID=" << pRspInfo->ErrorID << " ErrorMsg=" << pRspInfo->ErrorMsg << " " << ctime(&tm);
		ReleaseMutex(hMutex);
	}
	return bResult;
}
void CTraderSpi::ReqQryOrder(char* instrument)
{
	CThostFtdcQryOrderField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.InvestorID, pSer->con.investor_id);
	strcpy(req.InstrumentID, instrument);
	int iResult = pUserApi->ReqQryOrder(&req, ++nRequestID);
	return;
}
void CTraderSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) || !pOrder)return;
	if (!pFt.empty())
	for (auto pf = pFt.begin(); pf != pFt.end(); pf++)
	{
		if (strcmp((*pf)->ins.InstrumentID, pOrder->InstrumentID) == 0)
		{
			(*pf)->OnQryOrder(pOrder, bIsLast);
		}
	}
	return;
}
void CTraderSpi::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus)
{
	if (!pInstrumentStatus)return;
	bool exist = false;
	if (!pSer->insts.empty())
	for (auto it = pSer->insts.begin(); it != pSer->insts.end(); it++)
	{
		if (strcmp(it->ExchangeInstID, pInstrumentStatus->ExchangeInstID) == 0)
		{
			(*it) = (*pInstrumentStatus);
			exist = true;
			break;
		}
	}
	if (!exist)pSer->insts.push_back(*pInstrumentStatus);
	if (!pFt.empty())
	for (auto pf = pFt.begin(); pf != pFt.end(); pf++)
	if (((*pf)->ins.InstrumentID, pInstrumentStatus->InstrumentID) == 0 || ((*pf)->ins.ProductID, pInstrumentStatus->InstrumentID) == 0)(*pf)->ins.InstrumentStatus = pInstrumentStatus->InstrumentStatus;
	return;

}
int CTraderSpi::ReqTradingAccountPasswordUpdate()
{
	CThostFtdcTradingAccountPasswordUpdateField req;
	strcpy(req.BrokerID, pSer->con.brocker_id);
	strcpy(req.AccountID, pSer->con.investor_id);
	strcpy(req.OldPassword, "yiqian369");
	strcpy(req.NewPassword, "123456abc");
	int iResult = pUserApi->ReqTradingAccountPasswordUpdate(&req, ++nRequestID);
	return 0;
}
void CTraderSpi::OnRspTradingAccountPasswordUpdate(CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "pRspInfo " << pRspInfo->ErrorID << " " << pRspInfo->ErrorMsg << endl;
	if (pTradingAccountPasswordUpdate)
		cout << pTradingAccountPasswordUpdate->NewPassword << endl;
	else cout << "no pUserPasswordUpdate" << endl;
	return;
}