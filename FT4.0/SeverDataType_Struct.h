#if !defined(SERVER_DATATYPE_STRUCT_H)
#define SERVER_DATATYPE_STRUCT_H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include ".\ThostTraderApi\ThostFtdcTraderApi.h"
#include <vector>
#pragma warning(disable : 4996)
#define MAXNUM 300
#define READYNUM 20
#define CARRYNUM 10

////////////////////////////////////////
#define CMD_Null '0'
#define CMD_Buying '1'
#define CMD_Selling '2'
#define CMD_Buyed '3'
#define CMD_Selled '4'
typedef char SERVERCmdType;
///////////////////////////////////////
#define FT_POS_Null '0'
#define FT_POS_Long '1'
#define FT_POS_Short '2'
#define FT_POS_Dual '3'
typedef char PosType;
///////////////////////////////////////
#define PERMIT_Rest '0'
#define PERMIT_Trade '1'
#define PERMIT_Close '2'
#define PERMIT_Clean '3'
#define PERMIT_Connect '4'
typedef char TimePhase;
////////////////////////////////////////
struct Cmd
{
	TThostFtdcInstrumentIDType	InstrumentID;
	int id = 0;
	SERVERCmdType typ = CMD_Null;
	int gh0 = 0;
	TThostFtdcPriceType prcing = 0;
	TThostFtdcPriceType prced = 0;
	TThostFtdcPriceType prccls = 0;
	TThostFtdcPriceType tpf = 0;
	TThostFtdcPriceType stp = 0;
	TThostFtdcVolumeType vol = 0;
	double pit = 0;
	TThostFtdcTimeType	tmb;
	TThostFtdcDateType	day;
};
struct Ord
{
	int id_cmd = 0;
	int id_typ = 0;//0=open,1=pft,2=stp
	int id = 0;
	TThostFtdcInstrumentIDType	InstrumentID;
	TThostFtdcDirectionType	Direction;
	TThostFtdcOffsetFlagType OffsetFlag;
	TThostFtdcVolumeType	VolumeTotalOriginal;
	TThostFtdcContingentConditionType	ContingentCondition;
	TThostFtdcPriceType	StopPrice;
	TThostFtdcOrderPriceTypeType	OrderPriceType;
	TThostFtdcPriceType LimitPrice;

	TThostFtdcOrderRefType	OrderRef;
	int nRequestID;
	char msg[1024];

	TThostFtdcFrontIDType	FrontID;
	TThostFtdcSessionIDType	SessionID;
	TThostFtdcExchangeIDType	ExchangeID;
	TThostFtdcOrderSysIDType	OrderSysID;
	TThostFtdcRequestIDType	RequestID;
	TThostFtdcOrderLocalIDType	OrderLocalID;

	TThostFtdcVolumeType	VolumeTraded;
	TThostFtdcVolumeType	VolumeTotal;
	TThostFtdcOrderSubmitStatusType	OrderSubmitStatus;
	TThostFtdcOrderStatusType	OrderStatus;
	TThostFtdcDateType	InsertDate;
	TThostFtdcDateType	InsertTime;
	TThostFtdcTimeType	UpdateTime;
	TThostFtdcTimeType	CancelTime;
	struct
	{
		int nRequestID;
		TThostFtdcOrderActionRefType	OrderActionRef;
	}action;
};
struct Tra
{
	int id = 0;
	TThostFtdcOrderRefType	OrderRef;
	TThostFtdcExchangeIDType	ExchangeID;
	TThostFtdcTradeIDType	TradeID;
	TThostFtdcOrderSysIDType	OrderSysID;
	char msg[1024];

	TThostFtdcDirectionType	Direction;
	TThostFtdcOffsetFlagType OffsetFlag;
	TThostFtdcPriceType	Price;
	TThostFtdcVolumeType	Volume;
	TThostFtdcDateType	TradeDate;
	TThostFtdcTimeType	TradeTime;
	TThostFtdcTradeTypeType	TradeType;
};
struct Tik
{
	//int id = 0;
	bool valid = false;
	TThostFtdcDateType	day;				//tick_data[i][1] = atof(pDepthMarketData->TradingDay);
	TThostFtdcTimeType	time;			//tick_data[i][2] = Q_BarTime_2;//pDepthMarketData->UpdateTime;
	TThostFtdcMillisecType	mill = 0;			//tick_data[i][3] = pDepthMarketData->UpdateMillisec;
	TThostFtdcPriceType	prc0 = 0;			//tick_data[i][4] = pDepthMarketData->LastPrice;
	TThostFtdcPriceType	bid1 = 0;			//tick_data[i][4] = pDepthMarketData->LastPrice;
	TThostFtdcPriceType	bid2 = 0;			//tick_data[i][4] = pDepthMarketData->LastPrice;
	TThostFtdcPriceType	ask1 = 0;			//tick_data[i][4] = pDepthMarketData->LastPrice;
	TThostFtdcPriceType	ask2 = 0;			//tick_data[i][4] = pDepthMarketData->LastPrice;
	TThostFtdcPriceType	aprc = 0;				//tick_data[i][5] = pDepthMarketData->AveragePrice;
	TThostFtdcLargeVolumeType opin = 0;
	TThostFtdcPriceType	hprc = 0;				//tick_data[i][6] = pDepthMarketData->HighestPrice;
	TThostFtdcPriceType	lprc = 0;				//tick_data[i][7] = pDepthMarketData->LowestPrice;
	TThostFtdcPriceType uplmt = 0;			//tick_data[i][8] = pDepthMarketData->UpperLimitPrice;
	TThostFtdcPriceType	lwlmt = 0;			//tick_data[i][9] = pDepthMarketData->LowerLimitPrice;
};
struct Con
{
	TThostFtdcBrokerIDType brocker_id;
	TThostFtdcUserIDType investor_id;
	TThostFtdcPasswordType password;
	int bro_acc;
	string std_dir;
	string rst_dir;
	string acc_dir;
	TThostFtdcDateType	trading_day;
	TThostFtdcTimeType login_time;
	int distms;
	TThostFtdcFrontIDType	front_id = 0;		//ǰ�ñ��
	TThostFtdcSessionIDType	session_id = 0;		//�Ự���
	int i_order_ref = 0;
	int i_order_action_ref = 100;
	int id_cmd = 0;
	int id_ord = 0;
	bool fire = false;
};
struct Mget
{
	string name;
	bool drec;
	int mode;
	double fit[6];
};
struct Ins
{
	TThostFtdcInstrumentIDType	InstrumentID;
	TThostFtdcExchangeIDType	ExchangeID;
	TThostFtdcInstrumentIDType	ProductID;
	bool dep_get = false;
	bool cmt_get = false;
	bool mrg_get = false;
	vector <Mget> InsMgets;

	TThostFtdcInstrumentStatusType	InstrumentStatus;
	TThostFtdcTimeType	EnterTime;

	///��Լ����
	TThostFtdcInstrumentNameType	InstrumentName;
	///��Լ�ڽ������Ĵ���
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///��Ʒ����
	TThostFtdcProductClassType	ProductClass;
	///�������
	TThostFtdcYearType	DeliveryYear;
	///������
	TThostFtdcMonthType	DeliveryMonth;
	///�м۵�����µ���
	TThostFtdcVolumeType	MaxMarketOrderVolume;
	///�м۵���С�µ���
	TThostFtdcVolumeType	MinMarketOrderVolume;
	///�޼۵�����µ���
	TThostFtdcVolumeType	MaxLimitOrderVolume;
	///�޼۵���С�µ���
	TThostFtdcVolumeType	MinLimitOrderVolume;
	///��Լ��������
	TThostFtdcVolumeMultipleType	VolumeMultiple;
	///��С�䶯��λ
	TThostFtdcPriceType	PriceTick;
	///������
	TThostFtdcDateType	CreateDate;
	///������
	TThostFtdcDateType	OpenDate;
	///������
	TThostFtdcDateType	ExpireDate;
	///��ʼ������
	TThostFtdcDateType	StartDelivDate;
	///����������
	TThostFtdcDateType	EndDelivDate;
	///��Լ��������״̬
	TThostFtdcInstLifePhaseType	InstLifePhase;
	///��ǰ�Ƿ���
	TThostFtdcBoolType	IsTrading;
	///�ֲ�����
	TThostFtdcPositionTypeType	PositionType;
	///�ֲ���������
	TThostFtdcPositionDateTypeType	PositionDateType;
	///��ͷ��֤����
	TThostFtdcRatioType	LongMarginRatio;
	///��ͷ��֤����
	TThostFtdcRatioType	ShortMarginRatio;
	///�Ƿ�ʹ�ô��߱�֤���㷨
	TThostFtdcMaxMarginSideAlgorithmType	MaxMarginSideAlgorithm;

	///������
	TThostFtdcDateType	TradingDay;
	///���¼�
	TThostFtdcPriceType	LastPrice;
	///�ϴν����
	TThostFtdcPriceType	PreSettlementPrice;
	///������
	TThostFtdcPriceType	PreClosePrice;
	///��ֲ���
	TThostFtdcLargeVolumeType	PreOpenInterest;
	///����
	TThostFtdcPriceType	OpenPrice;
	///��߼�
	TThostFtdcPriceType	HighestPrice;
	///��ͼ�
	TThostFtdcPriceType	LowestPrice;
	///����
	TThostFtdcVolumeType	Volume;
	///�ɽ����
	TThostFtdcMoneyType	Turnover;
	///�ֲ���
	TThostFtdcLargeVolumeType	OpenInterest;
	///������
	TThostFtdcPriceType	ClosePrice;
	///���ν����
	TThostFtdcPriceType	SettlementPrice;
	///��ͣ���
	TThostFtdcPriceType	UpperLimitPrice;
	///��ͣ���
	TThostFtdcPriceType	LowerLimitPrice;
	///����ʵ��
	TThostFtdcRatioType	PreDelta;
	///����ʵ��
	TThostFtdcRatioType	CurrDelta;
	///����޸�ʱ��
	TThostFtdcTimeType	UpdateTime;
	///����޸ĺ���
	TThostFtdcMillisecType	UpdateMillisec;
	///���վ���
	TThostFtdcPriceType	AveragePrice;
	///ҵ������
	TThostFtdcDateType	ActionDay;

	///Ͷ���߷�Χ
	TThostFtdcInvestorRangeType	InvestorRange;
	///���͹�˾����
	TThostFtdcBrokerIDType	BrokerID;
	///Ͷ���ߴ���
	TThostFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TThostFtdcHedgeFlagType	HedgeFlag;
	///��ͷ��֤����
	TThostFtdcRatioType	LongMarginRatioByMoney;
	///��ͷ��֤���
	TThostFtdcMoneyType	LongMarginRatioByVolume;
	///��ͷ��֤����
	TThostFtdcRatioType	ShortMarginRatioByMoney;
	///��ͷ��֤���
	TThostFtdcMoneyType	ShortMarginRatioByVolume;
	///�Ƿ���Խ�������ȡ
	TThostFtdcBoolType	IsRelative;

	///������������
	TThostFtdcRatioType	OpenRatioByMoney;
	///����������
	TThostFtdcRatioType	OpenRatioByVolume;
	///ƽ����������
	TThostFtdcRatioType	CloseRatioByMoney;
	///ƽ��������
	TThostFtdcRatioType	CloseRatioByVolume;
	///ƽ����������
	TThostFtdcRatioType	CloseTodayRatioByMoney;
	///ƽ��������
	TThostFtdcRatioType	CloseTodayRatioByVolume;
};
struct Pdt
{
	TThostFtdcInstrumentIDType	ProductID;
	vector<Ins>inss;
	int depqried = 0;
	int cmtqried = 0;
	int mrgqried = 0;
};

struct Deal
{
	TThostFtdcInstrumentIDType	InstrumentID;
	TThostFtdcTradeIDType	TradeID;
	TThostFtdcDirectionType	Direction;
	TThostFtdcOffsetFlagType	OffsetFlag;
	TThostFtdcPriceType	Price;
	TThostFtdcVolumeType	Volume;
	TThostFtdcDateType	TradeDate;
	TThostFtdcTimeType	TradeTime;

	TThostFtdcDateType	OpenDate;
	TThostFtdcPriceType	OpenPrice;
	TThostFtdcSettlementIDType	SettlementID;
	TThostFtdcMoneyType	CloseProfitByTrade;
	TThostFtdcMoneyType	Margin;
	TThostFtdcPriceType	SettlementPrice;
	TThostFtdcVolumeType	CloseVolume;
	TThostFtdcMoneyType	CloseAmount;
};


#endif