#if !defined(FT_H)
#define FT_H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
using namespace std;
class   CFt
{
public:
	CFt(Ins instru);
	~CFt();
	CFt(const CFt& cft);
	void DRun();
	void OnQryPosDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, bool bIsLast);
	void OnQryOrder(CThostFtdcOrderField *pOrder, bool bIsLast);
	void RecordOpts(const Cmd &cmd);
	void RecordOpts(const Ord &ord);
	void RecordOpts(const Tra &tra);
private:
	static unsigned int _stdcall Carry(void* lpParamter);
	void BuyLong();
	void CloseLong();
	bool SellShort(const Cmd &cmd);
	bool CloseShort(const Cmd &cmd);
	void CleanOrd();
	void RecordPosdetail(vector<CThostFtdcInvestorPositionDetailField>posdtl);
	bool ULtest();
	//void RecordPos(vector<CThostFtdcInvestorPositionField> poss);
public:
	bool run_loaded = false;
	Tik mtik, tik;
	Ins	ins;
	vector<Cmd> cmds;
	vector<Tra> tras;
	vector<CThostFtdcInvestorPositionDetailField>posdtls;
private:
	//vector<Tik>tiks;
	PosType status = FT_POS_Null;
	double om0 = 0, ov0 = 0, cm0 = 0, cv0 = 0, ptk = 0, mul = 0, mgl = 0, mgs = 0;
};


#endif