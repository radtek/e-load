#pragma once

class CLoadSummaryOptionData
{
public:
        CLoadSummaryOptionData(void);
        ~CLoadSummaryOptionData(void);
public:
	double LTG_PF    , LTG_EFF;
	double WELDING_PF, WELDING_EFF;
	double INST_PF   , INST_EFF;
	double COMM_PF   , COMM_EFF;
	double OTHERS_PF , OTHERS_EFF;
        double m_dContinuous;
        double m_dIntermittent;
        double m_dStandby;
};
