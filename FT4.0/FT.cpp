#pragma once
#include<Windows.h>
#include<iostream>
#include <fstream>
#include<time.h>
#include<iomanip>
#include<process.h>
using namespace std;
#include ".\ThostTraderApi\ThostFtdcTraderApi.h"
#include ".\ThostTraderApi\ThostFtdcMdApi.h"
#include"Function.h"
#include"Server.h"
#include"MdSpi.h"
#include"TdSpi.h"
#include"FT.h"
CFt* cft;
extern CTraderSpi* pUserSpi;
extern CMdSpi *pMdSpi;
extern CServer *pSer;
extern HANDLE  Events[], hMutex, hLock;
CFt::CFt(Ins instru)
{
	ins = instru;
	WaitForSingleObject(hMutex, INFINITE);
	cout << "CFt costruct " << ins.InstrumentID << " " << TimeFtdc(pSer->con.distms) << endl;;
	ReleaseMutex(hMutex);
	cft = this;
}
CFt::CFt(const CFt& cft)
{
	ins = cft.ins;
	WaitForSingleObject(hMutex, INFINITE);
	cout << "CFt copy costruct " << ins.InstrumentID << " " << TimeFtdc(pSer->con.distms) << endl;
	ReleaseMutex(hMutex);
}
CFt::~CFt()
{
	WaitForSingleObject(hMutex, INFINITE);
	time_t tm; time(&tm); cout << "~CFt destroy " << ins.InstrumentID << " " << TimeFtdc(pSer->con.distms) << endl;
	ReleaseMutex(hMutex);
}
void CFt::DRun()
{
	om0 = ins.OpenRatioByMoney; ov0 = ins.OpenRatioByVolume; cm0 = ins.CloseRatioByMoney; cv0 = ins.CloseRatioByVolume;
	ptk = ins.PriceTick; mul = ins.VolumeMultiple; mgl = ins.LongMarginRatio; mgs = ins.ShortMarginRatio;
	double bbars[14] = { 0 };
	double abars[14] = { 0 };
	bool touchpause = false, ini = false;
	Cmd cmd;
	while (run_loaded)
	{
		WaitForSingleObject(hLock, INFINITE);
		tik = mtik;
		mtik.valid = false;
		ReleaseMutex(hLock);
		if (strlen(tik.time)<8 || tik.prc0 <= 0 || tik.ask1 <= 0 || tik.bid1 <= 0 || abs(tik.prc0 / tik.ask1 - 1)>0.1 || abs(tik.prc0 / tik.bid1 - 1)>0.1 || abs(tik.bid1 / tik.ask1 - 1)>0.1)tik.valid = false;
		if (!tik.valid || TimeDiff(string(tik.time), TimeFtdc(pSer->con.distms))<-10)
		{
			Sleep(350);
			continue;
		}
		if (!ini)
		{
			TThostFtdcVolumeType vol = (TThostFtdcVolumeType)floor(1000000.0 / (tik.prc0 * mul));
			if (vol>ins.MaxLimitOrderVolume)vol = ins.MaxLimitOrderVolume;
			if (vol < ins.MinLimitOrderVolume)vol = ins.MinLimitOrderVolume;
			memset(&cmd, 0, sizeof(cmd));
			strcpy(cmd.InstrumentID, ins.InstrumentID);
			cmd.typ = CMD_Null;
			cmd.vol = vol;
			ini = true;
		}
		if (TradePhase(ins, pSer->con.distms) != PERMIT_Trade)continue;
		if (ULtest())continue;
		if (tik.bid1 == bbars[0] && tik.ask1 == abars[0])continue;
		if (cmd.typ == CMD_Selling)
		{
			if ((cmd.gh0 == 1 && tik.bid1 <= cmd.prcing) || (cmd.gh0 == 0 && tik.bid1 >= cmd.prcing))
			{
				cmd.typ = CMD_Selled;
				cmd.prced = tik.bid1;
				cmd.id = ++pSer->con.id_cmd;
				WaitForSingleObject(hMutex, INFINITE);
				cout << fixed << setprecision(2);
				cout << "Ö¸Áî" << cmd.id << "." << ins.InstrumentID << " typ:" << cmd.typ << " prc:" << cmd.prced << " ntm " << TimeFtdc(pSer->con.distms) << endl;
				ReleaseMutex(hMutex);
			}
			else
			{
				SetEvent(Events[3]);
				cmd.typ = CMD_Null;
			}
		}
		if (cmd.typ == CMD_Selled)
		{
			if (tik.ask1 <= cmd.prced - cmd.tpf || tik.ask1 >= cmd.prced + cmd.stp)
			{
				cmd.prccls = tik.ask1;
				cmd.pit = (cmd.prced - cmd.prccls)*cmd.vol*mul;
				WaitForSingleObject(hMutex, INFINITE);
				cout << fixed << setprecision(2);
				cout << "Ö¸Áî" << cmd.id << "." << ins.InstrumentID << " typ:" << cmd.typ << " prc:" << cmd.prccls << " ntm " << TimeFtdc(pSer->con.distms) << " pit " << cmd.pit << endl;
				ReleaseMutex(hMutex);
				RecordOpts(cmd);
				cmds.push_back(cmd);
				cmd.typ = CMD_Null;
			}
		}
		if (cmd.typ == CMD_Buying)
		{
			if ((cmd.gh0 == 1 && tik.ask1 >= cmd.prcing) || (cmd.gh0 == 0 && tik.ask1 <= cmd.prcing))
			{
				cmd.typ = CMD_Buyed;
				cmd.prced = tik.ask1;
				cmd.id = ++pSer->con.id_cmd;
				WaitForSingleObject(hMutex, INFINITE);
				cout << fixed << setprecision(2);
				cout << "Ö¸Áî" << cmd.id << "." << ins.InstrumentID << " typ:" << cmd.typ << " prc:" << cmd.prced << " ntm " << TimeFtdc(pSer->con.distms) << endl;
				ReleaseMutex(hMutex);
			}
			else
			{
				SetEvent(Events[3]);
				cmd.typ = CMD_Null;
			}
		}
		if (cmd.typ == CMD_Buyed)
		{
			if (tik.bid1 >= cmd.prced + cmd.tpf || tik.bid1 <= cmd.prced - cmd.stp)
			{
				cmd.prccls = tik.bid1;
				cmd.pit = (cmd.prccls-cmd.prced)*cmd.vol*mul;
				WaitForSingleObject(hMutex, INFINITE);
				cout << fixed << setprecision(2);
				cout << "Ö¸Áî" << cmd.id << "." << ins.InstrumentID << " typ:" << cmd.typ << " prc:" << cmd.prccls << " ntm " << TimeFtdc(pSer->con.distms) << " pit " << cmd.pit << endl;
				ReleaseMutex(hMutex);
				RecordOpts(cmd);
				cmds.push_back(cmd);
				cmd.typ = CMD_Null;
			}
		}
	}
	if (tik.bid1 != bbars[0])
	{
		for (int i = 13; i >= 1; i--)bbars[i] = bbars[i - 1];
		bbars[0] = tik.bid1;
		int gh7 = 0; for (int i = 0; i <= 6; i++)gh7 += (bbars[i] < bbars[i + 1] ? (int)pow(2, i + 1) : 0);
		for (auto md = ins.InsMgets.begin(); md != ins.InsMgets.end(); md++)
		{
			if (md->drec != 0)continue;
			if (md->mode / 2 == gh7)
			{
				if (cmd.typ == CMD_Null)
				{
					cmd.typ = CMD_Selling;
					cmd.gh0 = (int)fmod(md->mode, 2);
					cmd.tpf = ptk*floor(md->fit[0]);
					cmd.stp = -ptk*ceil(min(-2, min(md->fit[1], min(md->fit[2], min(md->fit[3], min(md->fit[4], md->fit[5]))))));
					cmd.prcing = tik.bid1 + (cmd.gh0 == 1 ? -ptk : ptk);
					strcpy(cmd.tmb, tik.time);
					HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, Carry, (void*)(&cmd), 0, NULL);
					CloseHandle(handle);
					break;
				}
			}
		}
	}
	if (tik.ask1 != abars[0])
	{
		for (int i = 13; i >= 1; i--)abars[i] = abars[i - 1];
		abars[0] = tik.ask1;
		int gh7 = 0; for (int i = 0; i <= 6; i++)gh7 += (abars[i] > abars[i + 1] ? (int)pow(2, i + 1) : 0);
		for (auto md = ins.InsMgets.begin(); md != ins.InsMgets.end(); md++)
		{
			if (md->drec != 1)continue;
			if (md->mode / 2 == gh7)
			{
				if (cmd.typ == CMD_Null)
				{
					cmd.typ = CMD_Buying;
					cmd.gh0 = (int)fmod(md->mode, 2);
					cmd.tpf = ptk*floor(md->fit[0]);
					cmd.stp = -ptk*ceil(min(-2, min(md->fit[1], min(md->fit[2], min(md->fit[3], min(md->fit[4], md->fit[5]))))));
					cmd.prcing = tik.ask1 + (cmd.gh0 == 1 ? ptk : -ptk);
					strcpy(cmd.tmb, tik.time);
					break;
				}
			}
		}
	}
	return;
}
unsigned int _stdcall  CFt::Carry(void* lpParamter)
{
	Cmd* pcmd = (Cmd*)lpParamter;
	if (pcmd->typ == CMD_Selling)
	{
		cft->SellShort(*pcmd);
		if (WaitForSingleObject(Events[3], INFINITE) == WAIT_OBJECT_0)
		{
			if (!cft->CloseShort(*pcmd))pUserSpi->ReqOrderAction(*por);

			if (!pSer->ords.empty())
			for (auto por = pSer->ords.begin(); por != pSer->ords.end(); por++)
			{
				if (por->id_cmd == pcmd->id && por->id_typ==0)
				{
					if (por->OrderStatus == THOST_FTDC_OST_AllTraded)
					{
						cft->CloseShort(*pcmd);
					}
					else pUserSpi->ReqOrderAction(*por);
					break;
				}
			}
		}
	}

	return 0;
}
void CFt::BuyLong()
{
	Ord ord;
	memset(&ord, 0, sizeof(ord));
	ord.id = cmd.id;
	strcpy(ord.InstrumentID, ins.InstrumentID);
	ord.Direction = THOST_FTDC_D_Buy;
	ord.OffsetFlag = THOST_FTDC_OF_Open;
	ord.LimitPrice = tik.ask1 + ptk * 2;
	TThostFtdcVolumeType hdgvol = cmd.vol;
	if (!posdtls.empty())
	for (auto it = posdtls.begin(); it != posdtls.end(); it++)
	{
		if (it->Direction == THOST_FTDC_D_Buy && it->Volume > 0)
		{
			hdgvol -= it->Volume;
		}
	}
	if (hdgvol < cmd.vol)
	{
		WaitForSingleObject(hMutex, INFINITE);
		cout << "³å¼õ" << cmd.id << "." << ins.InstrumentID << " BuyLong hdgvol:" << hdgvol << " shrinked from:" << cmd.vol << endl;
		ReleaseMutex(hMutex);
	}
	TThostFtdcVolumeType avlvol = (TThostFtdcVolumeType)floor(0.8*pSer->account.Available / (mgl*ord.LimitPrice*mul));
	if (hdgvol >= ins.MinLimitOrderVolume && avlvol >= hdgvol)ord.VolumeTotalOriginal = hdgvol;
	else
	{
		if (hdgvol<ins.MinLimitOrderVolume)
		{
			WaitForSingleObject(hMutex, INFINITE);
			cout << "ÆúÁî" << cmd.id << "." << ins.InstrumentID << " BuyLong hdgvol£º" << hdgvol << " less than MinLimit£º" << ins.MinLimitOrderVolume << endl;
			ReleaseMutex(hMutex);
			return;
		}
		else if (avlvol < ins.MinLimitOrderVolume)
		{
			WaitForSingleObject(hMutex, INFINITE);
			cout << "ÆúÁî" << cmd.id << "." << ins.InstrumentID << " BuyLong avlvol£º" << avlvol << " less than MinLimit£º" << ins.MinLimitOrderVolume << endl;
			ReleaseMutex(hMutex);
			return;
		}
		else
		{
			WaitForSingleObject(hMutex, INFINITE);
			cout << "ËõÁ¿" << cmd.id << "." << ins.InstrumentID << " BuyLong ordvol£º" << avlvol << " shrinked from:" << hdgvol << endl;
			ReleaseMutex(hMutex);
			ord.VolumeTotalOriginal = avlvol;
		}
	}
	pUserSpi->ReqOrderInsert(ord);
	return;
}
bool CFt::SellShort(const Cmd &cmd)
{
	Ord ord;
	memset(&ord, 0, sizeof(ord));
	ord.id_cmd = cmd.id;
	ord.id_typ = 0;
	ord.id = ++pSer->con.id_ord;
	strcpy(ord.InstrumentID, cmd.InstrumentID);
	ord.Direction = THOST_FTDC_D_Sell;
	ord.OffsetFlag = THOST_FTDC_OF_Open;
	ord.VolumeTotalOriginal = 1;

	ord.ContingentCondition = cmd.gh0 == 1 ? THOST_FTDC_CC_BidPriceLesserEqualStopPrice : THOST_FTDC_CC_BidPriceGreaterEqualStopPrice;
	ord.StopPrice = cmd.prcing;
	ord.OrderPriceType = THOST_FTDC_OPT_BidPrice1;
	ord.LimitPrice = 0;

	TThostFtdcVolumeType avlvol = (TThostFtdcVolumeType)floor(0.8*pSer->account.Available / (mgs*cmd.prcing*mul));
	if (avlvol >= cmd.vol)ord.VolumeTotalOriginal = cmd.vol;
	else if (avlvol < ins.MinLimitOrderVolume)
	{
		WaitForSingleObject(hMutex, INFINITE);
		cout << "ÆúÁî" << cmd.id << "." << ins.InstrumentID << " SellShort avlvol£º" << avlvol << " less than MinLimit£º" << ins.MinLimitOrderVolume << endl;
		ReleaseMutex(hMutex);
		return false;
	}
	else
	{
		ord.VolumeTotalOriginal = avlvol;
		WaitForSingleObject(hMutex, INFINITE);
		cout << "ËõÁ¿" << cmd.id << "." << ins.InstrumentID << " SellShort ordvol£º" << avlvol << " shrinked from:" << cmd.vol << endl;
		ReleaseMutex(hMutex);
	}
	pUserSpi->ReqOrderInsert(ord);
	return true;
}
void CFt::CloseLong()
{
	if (posdtls.empty())return;
	Ord ord;
	memset(&ord, 0, sizeof(ord));
	ord.id = cmd.id;
	strcpy(ord.InstrumentID, ins.InstrumentID);
	ord.Direction = THOST_FTDC_D_Sell;
	ord.LimitPrice = tik.bid1 - ptk * 2;
	TThostFtdcVolumeType vol0 = 0, vol1 = 0;
	for (auto it = posdtls.begin(); it != posdtls.end(); it++)
	{
		if (it->Direction != THOST_FTDC_D_Buy)continue;
		if (strcmp(it->OpenDate, pSer->con.trading_day) == 0)vol0 += it->Volume;
		else vol1 += it->Volume;
	}
	ord.VolumeTotalOriginal = vol0 + vol1;
	if (ord.VolumeTotalOriginal<cmd.vol)
	{
		WaitForSingleObject(hMutex, INFINITE);
		cout << "Åä¼õ" << cmd.id << "." << ins.InstrumentID << " CloseLong ordvol:" << ord.VolumeTotalOriginal << " shrinked from:" << cmd.vol << endl;
		ReleaseMutex(hMutex);
	}
	if (strcmp(ins.ExchangeID, "SHFE") != 0)
	{
		if (ord.VolumeTotalOriginal > 0)
		{
			ord.OffsetFlag = THOST_FTDC_OF_Close;
			pUserSpi->ReqOrderInsert(ord);
		}
	}
	else
	{
		if (vol0 > 0)
		{
			ord.VolumeTotalOriginal = vol0;
			ord.OffsetFlag = THOST_FTDC_OF_CloseToday;
			pUserSpi->ReqOrderInsert(ord);
		}
		if (vol1 > 0)
		{
			ord.VolumeTotalOriginal = vol1;
			ord.OffsetFlag = THOST_FTDC_OF_CloseYesterday;
			pUserSpi->ReqOrderInsert(ord);
		}
	}
	return;
}
bool CFt::CloseShort(const Cmd &cmd)
{
	Ord ord;
	memset(&ord, 0, sizeof(ord));
	ord.id_cmd = cmd->id;
	ord.id = ++pSer->con.id_ord;
	strcpy(ord.InstrumentID, cmd->InstrumentID);
	ord.Direction = THOST_FTDC_D_Buy;
	ord.OffsetFlag = THOST_FTDC_OF_Close;
	ord.VolumeTotalOriginal = por->VolumeTraded;

	ord.ContingentCondition = THOST_FTDC_CC_Immediately;
	ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	ord.LimitPrice = 0;
	pUserSpi->ReqOrderInsert(ord);

	if (posdtls.empty())return;
	Ord ord;
	memset(&ord, 0, sizeof(ord));
	ord.id = cmd.id;
	strcpy(ord.InstrumentID, ins.InstrumentID);
	ord.Direction = THOST_FTDC_D_Buy;
	ord.LimitPrice = tik.ask1 + ptk * 2;
	TThostFtdcVolumeType vol0 = 0, vol1 = 0;
	for (auto it = posdtls.begin(); it != posdtls.end(); it++)
	{
		if (it->Direction != THOST_FTDC_D_Sell)continue;
		if (strcmp(it->OpenDate, pSer->con.trading_day) == 0)vol0 += it->Volume;
		else vol1 += it->Volume;
	}
	ord.VolumeTotalOriginal = vol0 + vol1;
	if (ord.VolumeTotalOriginal<cmd.vol)
	{
		WaitForSingleObject(hMutex, INFINITE);
		cout << "Åä¼õ" << cmd.id << "." << ins.InstrumentID << " CloseShort ordvol:" << ord.VolumeTotalOriginal << " shrinked from:" << cmd.vol << endl;
		ReleaseMutex(hMutex);
	}
	if (strcmp(ins.ExchangeID, "SHFE") != 0)
	{
		if (ord.VolumeTotalOriginal > 0)
		{
			ord.OffsetFlag = THOST_FTDC_OF_Close;
			pUserSpi->ReqOrderInsert(ord);
		}
	}
	else
	{
		if (vol0 > 0)
		{
			ord.VolumeTotalOriginal = vol0;
			ord.OffsetFlag = THOST_FTDC_OF_CloseToday;
			pUserSpi->ReqOrderInsert(ord);
		}
		if (vol1 > 0)
		{
			ord.VolumeTotalOriginal = vol1;
			ord.OffsetFlag = THOST_FTDC_OF_CloseYesterday;
			pUserSpi->ReqOrderInsert(ord);
		}
	}
	return;
}
void CFt::CleanOrd()
{
	if (ords.empty())return;
	for (auto or = ords.begin(); or != ords.end(); or++)
	{
		if (strcmp(or->InstrumentID, ins.InstrumentID) != 0)continue;
		if (or->OrderStatus == THOST_FTDC_OST_NoTradeQueueing || or->OrderStatus == THOST_FTDC_OST_PartTradedQueueing)
		{
			pUserSpi->ReqOrderAction((*or));
			//cout << "OrdClean " <<or->id<<". "<< or->InstrumentID <<" sts "<<or->OrderStatus<< " ref " << or->OrderRef << " oid " << or->OrderSysID << " drc " << or->Direction << "	flg " << or->OffsetFlag << endl;
		}
	}
	return;
}
void CFt::OnQryPosDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, bool bIsLast)
{
	bool exist = false;
	if (!posdtls.empty())
	{
		for (auto it = posdtls.begin(); it != posdtls.end(); it++)
		{
			if (strcmp(pInvestorPositionDetail->TradeID, it->TradeID) == 0)
			{
				(*it) = (*pInvestorPositionDetail);
				exist = true;
				break;
			}
		}
	}
	if (!exist)posdtls.push_back(*pInvestorPositionDetail);
	if (bIsLast)
	{
		status = FT_POS_Null;
		int plong = 0, pshort = 0;
		for (auto it = posdtls.begin(); it != posdtls.end(); it++)
		{
			if (it->Volume <= 0)continue;
			else if (it->Direction == THOST_FTDC_D_Buy)
			{
				plong += it->Volume;
				if (status == FT_POS_Short || status == FT_POS_Dual)status = FT_POS_Dual;
				else status = FT_POS_Long;
			}
			else if (it->Direction == THOST_FTDC_D_Sell)
			{
				pshort += it->Volume;
				if (status == FT_POS_Long || status == FT_POS_Dual)status = FT_POS_Dual;
				else status = FT_POS_Short;
			}
		}
		//time_t tm; time(&tm); cout << "Pos " << ins.InstrumentID << " status " << status << " pln:" << plong << " psh:" << pshort << " " << ctime(&tm);
		if (plong + pshort > 0)SetEvent(Events[0]);
		RecordPosdetail(posdtls);
	}
	return;
}
void CFt::OnQryOrder(CThostFtdcOrderField *pOrder, bool bIsLast)
{
	bool exist = false;
	if (!ords.empty())
	{
		for (auto or = ords.begin(); or != ords.end(); or++)
		{
			if (strcmp(pOrder->OrderSysID, or->OrderSysID) == 0 && strcmp(pOrder->ExchangeID, or->ExchangeID) == 0)
			{
				strcpy(or->OrderRef, pOrder->OrderRef);
				or->FrontID = pOrder->FrontID;
				or->SessionID = pOrder->SessionID;
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
				RecordOpts((*or));
				exist = true;
				break;
			}
		}
	}
	if (!exist)
	{
		Ord ord;
		memset(&ord, 0, sizeof(ord));
		strcpy(ord.OrderSysID, pOrder->OrderSysID);
		strcpy(ord.ExchangeID, pOrder->ExchangeID);
		strcpy(ord.OrderRef, pOrder->OrderRef);
		ord.FrontID = pOrder->FrontID;
		ord.SessionID = pOrder->SessionID;
		strcpy(ord.OrderLocalID, pOrder->OrderLocalID);
		ord.RequestID = pOrder->RequestID;

		ord.id = -1;
		strcpy(ord.InstrumentID, pOrder->InstrumentID);
		ord.Direction = pOrder->Direction;
		ord.LimitPrice = pOrder->LimitPrice;
		ord.OffsetFlag = pOrder->CombOffsetFlag[0];

		ord.VolumeTotalOriginal = pOrder->VolumeTotalOriginal;
		ord.VolumeTraded = pOrder->VolumeTraded;
		ord.VolumeTotal = pOrder->VolumeTotal;
		ord.OrderSubmitStatus = pOrder->OrderSubmitStatus;
		ord.OrderStatus = pOrder->OrderStatus;
		strcpy(ord.msg, pOrder->StatusMsg);
		strcpy(ord.InsertDate, pOrder->InsertDate);
		strcpy(ord.InsertTime, pOrder->InsertTime);
		strcpy(ord.UpdateTime, pOrder->UpdateTime);
		strcpy(ord.CancelTime, pOrder->CancelTime);
		RecordOpts(ord);
		ords.push_back(ord);
	}
	if (bIsLast)SetEvent(Events[0]);
	return;
}
void CFt::RecordOpts(const Cmd &cmd)
{
	string tm = pSer->con.login_time;
	tm = tm.substr(0, 2) + tm.substr(3, 2);
	string fn = pSer->con.rst_dir + string(ins.InstrumentID) + tm + "Opts.txt";
	ofstream outfile(fn, ios::out | ios::app);
	if (outfile.is_open())
	{
		time_t tm; time(&tm);
		outfile << "id " << cmd.id << " " << cmd.msg << " typ " << cmd.typ << " prc " << cmd.prc << " vol " << cmd.vol << " org " << cmd.org
			<< " pit " << cmd.pit << " pms " << cmd.pms << " tmb " << cmd.tmb << " day " << cmd.day << " ctm " << ctime(&tm);
		outfile.close();
	}
	return;
}
void CFt::RecordOpts(const Ord &ord)
{
	string tm = pSer->con.login_time;
	tm = tm.substr(0, 2) + tm.substr(3, 2);
	string fn = pSer->con.rst_dir + string(ins.InstrumentID) + tm + "Opts.txt";
	ofstream outfile(fn, ios::out | ios::app);
	if (outfile.is_open())
	{
		time_t tm; time(&tm);
		outfile << "id " << ord.id << " " << ord.msg << " drc " << ord.Direction << " flg " << ord.OffsetFlag << " prc " << ord.LimitPrice << " vrg " << ord.VolumeTotalOriginal << " oid " << ord.OrderSysID
			<< " ref " << ord.OrderRef << " sts " << ord.OrderStatus << " sub " << ord.OrderSubmitStatus << " nid " << ord.nRequestID << " ved " << ord.VolumeTraded << " vtl " << ord.VolumeTotal
			<< " itm " << ord.InsertTime << " day " << ord.InsertDate << " ctm " << ctime(&tm);
		outfile.close();
	}
	return;
}
void CFt::RecordOpts(const Tra &tra)
{
	string tm = pSer->con.login_time;
	tm = tm.substr(0, 2) + tm.substr(3, 2);
	string fn = pSer->con.rst_dir + string(ins.InstrumentID) + tm + "Opts.txt";
	ofstream outfile(fn, ios::out | ios::app);
	if (outfile.is_open())
	{
		time_t tm; time(&tm);
		outfile << "id " << tra.id << " " << tra.msg << " drc " << tra.Direction << " flg " << tra.OffsetFlag << " prc " << tra.Price << " vol " << tra.Volume << " oid " << tra.OrderSysID
			<< " tid " << tra.TradeID << " eid " << tra.ExchangeID << " ttm " << tra.TradeTime << " day " << tra.TradeDate << " ctm " << ctime(&tm);
		outfile.close();
	}
	return;
}
void CFt::RecordPosdetail(vector<CThostFtdcInvestorPositionDetailField>posdtl)
{
	if (posdtl.empty())return;
	string fn = pSer->con.rst_dir + ins.InstrumentID + "PosDet.txt";
	ofstream outfile(fn, ios::trunc);
	if (outfile.is_open())
	{
		time_t tm; time(&tm);
		for (auto it = posdtl.begin(); it != posdtl.end(); it++)
		{
			outfile << "-------------------------------------------" << ctime(&tm)
				<< "Volume	" << it->Volume << "	CloseVolume	" << it->CloseVolume << "	Direction	" << it->Direction << "	OpenDate	" << it->OpenDate << endl
				<< "OpenPrice	" << it->OpenPrice << "	SettlementPrice	" << it->SettlementPrice << "	LastSettlementPrice	" << it->LastSettlementPrice << " CloseAmount " << it->CloseAmount << endl
				<< "PositionProfitByDate	" << it->PositionProfitByDate << "	PositionProfitByTrade	" << it->PositionProfitByTrade << "	CloseProfitByDate	" << it->CloseProfitByDate << "	CloseProfitByTrade	" << it->CloseProfitByTrade << endl
				<< "Margin	" << it->Margin << "	ExchMargin	" << it->ExchMargin << " TradeType " << it->TradeType << endl
				<< "TradeID	" << it->TradeID << "	SettlementID	" << it->SettlementID << "	ExchangeID	" << it->ExchangeID << endl;
		}
		outfile.close();
	}
	return;
}
bool CFt::ULtest()
{
	static bool touchpause = false;
	if (tik.prc0 <= ins.LowerLimitPrice || tik.prc0 >= ins.UpperLimitPrice)
	{
		if (!touchpause)
		{
			WaitForSingleObject(hMutex, INFINITE);
			cout << ins.InstrumentID << " Price touch to UL. Pause..." << " tik.prc:" << tik.prc0 << " L:" << ins.LowerLimitPrice << " U:" << ins.UpperLimitPrice << " " << TimeFtdc(pSer->con.distms) << endl;
			ReleaseMutex(hMutex);
			touchpause = true;
		}
		return true;
	}
	else if (touchpause)
	{
		WaitForSingleObject(hMutex, INFINITE);
		cout << ins.InstrumentID << " Price touchpause release. " << TimeFtdc(pSer->con.distms) << endl;
		ReleaseMutex(hMutex);
		touchpause = false;
	}
	return false;
}