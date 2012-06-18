#include "StdAfx.h"
#include "LoadSummaryOptionData.h"

CLoadSummaryOptionData::CLoadSummaryOptionData(void)
{
        LTG_PF          = 0;
        LTG_EFF         = 0;
	WELDING_PF      = 0;
        WELDING_EFF     = 0;
	INST_PF         = 0;
        INST_EFF        = 0;
	COMM_PF         = 0;
        COMM_EFF        = 0;
	OTHERS_PF       = 0;
        OTHERS_EFF      = 0;
        m_dContinuous   = 0;
        m_dIntermittent = 0;        
        m_dStandby      = 0;
}

CLoadSummaryOptionData::~CLoadSummaryOptionData(void)
{
}
