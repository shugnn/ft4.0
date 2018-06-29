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
	TThostFtdcFrontIDType	front_id = 0;		//前置编号
	TThostFtdcSessionIDType	session_id = 0;		//会话编号
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

	///合约名称
	TThostFtdcInstrumentNameType	InstrumentName;
	///合约在交易所的代码
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///产品类型
	TThostFtdcProductClassType	ProductClass;
	///交割年份
	TThostFtdcYearType	DeliveryYear;
	///交割月
	TThostFtdcMonthType	DeliveryMonth;
	///市价单最大下单量
	TThostFtdcVolumeType	MaxMarketOrderVolume;
	///市价单最小下单量
	TThostFtdcVolumeType	MinMarketOrderVolume;
	///限价单最大下单量
	TThostFtdcVolumeType	MaxLimitOrderVolume;
	///限价单最小下单量
	TThostFtdcVolumeType	MinLimitOrderVolume;
	///合约数量乘数
	TThostFtdcVolumeMultipleType	VolumeMultiple;
	///最小变动价位
	TThostFtdcPriceType	PriceTick;
	///创建日
	TThostFtdcDateType	CreateDate;
	///上市日
	TThostFtdcDateType	OpenDate;
	///到期日
	TThostFtdcDateType	ExpireDate;
	///开始交割日
	TThostFtdcDateType	StartDelivDate;
	///结束交割日
	TThostFtdcDateType	EndDelivDate;
	///合约生命周期状态
	TThostFtdcInstLifePhaseType	InstLifePhase;
	///当前是否交易
	TThostFtdcBoolType	IsTrading;
	///持仓类型
	TThostFtdcPositionTypeType	PositionType;
	///持仓日期类型
	TThostFtdcPositionDateTypeType	PositionDateType;
	///多头保证金率
	TThostFtdcRatioType	LongMarginRatio;
	///空头保证金率
	TThostFtdcRatioType	ShortMarginRatio;
	///是否使用大额单边保证金算法
	TThostFtdcMaxMarginSideAlgorithmType	MaxMarginSideAlgorithm;

	///交易日
	TThostFtdcDateType	TradingDay;
	///最新价
	TThostFtdcPriceType	LastPrice;
	///上次结算价
	TThostFtdcPriceType	PreSettlementPrice;
	///昨收盘
	TThostFtdcPriceType	PreClosePrice;
	///昨持仓量
	TThostFtdcLargeVolumeType	PreOpenInterest;
	///今开盘
	TThostFtdcPriceType	OpenPrice;
	///最高价
	TThostFtdcPriceType	HighestPrice;
	///最低价
	TThostFtdcPriceType	LowestPrice;
	///数量
	TThostFtdcVolumeType	Volume;
	///成交金额
	TThostFtdcMoneyType	Turnover;
	///持仓量
	TThostFtdcLargeVolumeType	OpenInterest;
	///今收盘
	TThostFtdcPriceType	ClosePrice;
	///本次结算价
	TThostFtdcPriceType	SettlementPrice;
	///涨停板价
	TThostFtdcPriceType	UpperLimitPrice;
	///跌停板价
	TThostFtdcPriceType	LowerLimitPrice;
	///昨虚实度
	TThostFtdcRatioType	PreDelta;
	///今虚实度
	TThostFtdcRatioType	CurrDelta;
	///最后修改时间
	TThostFtdcTimeType	UpdateTime;
	///最后修改毫秒
	TThostFtdcMillisecType	UpdateMillisec;
	///当日均价
	TThostFtdcPriceType	AveragePrice;
	///业务日期
	TThostFtdcDateType	ActionDay;

	///投资者范围
	TThostFtdcInvestorRangeType	InvestorRange;
	///经纪公司代码
	TThostFtdcBrokerIDType	BrokerID;
	///投资者代码
	TThostFtdcInvestorIDType	InvestorID;
	///投机套保标志
	TThostFtdcHedgeFlagType	HedgeFlag;
	///多头保证金率
	TThostFtdcRatioType	LongMarginRatioByMoney;
	///多头保证金费
	TThostFtdcMoneyType	LongMarginRatioByVolume;
	///空头保证金率
	TThostFtdcRatioType	ShortMarginRatioByMoney;
	///空头保证金费
	TThostFtdcMoneyType	ShortMarginRatioByVolume;
	///是否相对交易所收取
	TThostFtdcBoolType	IsRelative;

	///开仓手续费率
	TThostFtdcRatioType	OpenRatioByMoney;
	///开仓手续费
	TThostFtdcRatioType	OpenRatioByVolume;
	///平仓手续费率
	TThostFtdcRatioType	CloseRatioByMoney;
	///平仓手续费
	TThostFtdcRatioType	CloseRatioByVolume;
	///平今手续费率
	TThostFtdcRatioType	CloseTodayRatioByMoney;
	///平今手续费
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