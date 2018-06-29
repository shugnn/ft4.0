#if !defined(SERVER_H)
#define SERVER_H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
using namespace std;
#include "SeverDataType_Struct.h"
class CServer
{
public:
	CServer(bool fire);
	~CServer();
	void ServerIn();
	void ServerOut(bool bothout);
	void OnTraderConnect(CThostFtdcRspUserLoginField *pRspUserLogin, bool bIsLast);
	void OnSettleAccout(CThostFtdcTradingAccountField *pacc, bool bIsLast);
	void OnReqPos(CThostFtdcInvestorPositionField *pInvestorPosition, bool bIsLast);
	void OnReqIns(CThostFtdcInstrumentField *pInstrument,bool bIsLast);
	void OnQryDepthMarket(CThostFtdcDepthMarketDataField *pDepthMarketData, int mode);
	void OnQryMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate);
	void OnQryCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate);
	void OnMarkedConnect(CThostFtdcRspUserLoginField *pRspUserLogin);
	void OnSubMarket(CThostFtdcSpecificInstrumentField *pSpecificInstrument, bool bIsLast);
	void RecordFTResult();
private:
	bool TraderConnect();
	void SettleAccout();
	void PosClean();
	static unsigned int _stdcall ThreadPosClean(void* lpParamter);
	bool ComIns();
	bool MarkedConnect();
	void SubMarket();
	bool TraderLoad();
	static unsigned int _stdcall ThreadFtRun(void* lpParamter);
	void RecordAcc(CThostFtdcTradingAccountField *pacc);
	void RecordIns(Ins *pInstrument);
	bool ReadCarryInsList();
public:
	Con con;
	vector<Ins> vins;
	vector<Pdt> pdts;
	CThostFtdcTradingAccountField account;
	vector<CThostFtdcInvestorPositionField> poss;
	vector<CThostFtdcInstrumentStatusField>insts;
	vector<Ord> ords;
};


#endif