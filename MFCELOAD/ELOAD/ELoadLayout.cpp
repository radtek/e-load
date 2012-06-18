#include "StdAfx.h"
#include "ELoadLayout.h"
#include <log4cxx/spi/loggingevent.h>
#include <log4cxx/helpers/optionconverter.h>
#include <log4cxx/level.h>
#include <log4cxx/helpers/transform.h>
#include <log4cxx/helpers/iso8601dateformat.h>
#include <log4cxx/helpers/stringhelper.h>

using namespace log4cxx;
using namespace log4cxx::helpers;
using namespace log4cxx::spi;

string CELoadLayout::m_header;

IMPLEMENT_LOG4CXX_OBJECT(CELoadLayout)

CELoadLayout::CELoadLayout(void) : m_locationInfo(false) , m_title(_T("Log4cxx Log Messages")) , m_dateFormat(TimeZone::getTimeZone(_T("GMT")))
{
}

CELoadLayout::~CELoadLayout(void)
{
}

/**
	@brief	

	@author BHK	

	@date 2009-03-30 오후 2:44:45	

	@param	

	@return		
**/
void CELoadLayout::format(std::ostream& output, const spi::LoggingEventPtr& event) const
{
	output << std::endl << _T("<tr>") << std::endl;

	output << _T("<td title=\"No\">");
	Transform::appendEscapingTags(output, event->getProperty(_T("No")));
	output << _T("</td>")  << std::endl;

	output << _T("<td title=\"Level\">");
	if (event->getLevel()->equals(Level::DEBUG))
	{
		output << _T("<font color=\"#339933\">");
		output << event->getLevel()->toString();
		output << _T("</font>");
	}
	else if(event->getLevel()->isGreaterOrEqual(Level::WARN))
	{
		output << _T("<font color=\"#993300\"><strong>");
		output << event->getLevel()->toString();
		output << _T("</strong></font>");
	}
	else
	{
		output << event->getLevel()->toString();
	}

	output << _T("</td>") << std::endl;

	if(m_locationInfo)
	{
		USES_CONVERSION;
		output << _T("<td>");
		Transform::appendEscapingTags(output, A2T(event->getFile()));
		output.put(_T(':'));
		if (event->getLine() != 0)
		{
			output << event->getLine();
		}
		output << _T("</td>") << std::endl;
	}

	output << _T("<td title=\"Bus ID\">");
	Transform::appendEscapingTags(output, event->getProperty(_T("Bus ID")));
	output << _T("</td>")  << std::endl;

	output << _T("<td title=\"Item ID\">");
	Transform::appendEscapingTags(output, event->getProperty(_T("Item ID")));
	output << _T("</td>")  << std::endl;

	output << _T("<td title=\"Error Code\">");
	Transform::appendEscapingTags(output, event->getProperty(_T("Error Code")));
	output << _T("</td>")  << std::endl;

	output << _T("<td title=\"Message\">");
	Transform::appendEscapingTags(output, event->getRenderedMessage());
	output << _T("</td>")  << std::endl;
	output << _T("</tr>") << std::endl;

	if (event->getNDC().length() != 0)
	{
		output << _T("<tr><td bgcolor=\"#EEEEEE\" ");
		output << _T("style=\"font-size : xx-small;\" colspan=\"6\" ");
		output << _T("title=\"Nested Diagnostic Context\">");
		output << _T("NDC: ");
		Transform::appendEscapingTags(output, event->getNDC());
		output << _T("</td></tr>") << std::endl;
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-03-30 오후 3:00:59	

	@param	

	@return		
**/
void CELoadLayout::appendHeader(std::ostream& output)
{
	output << _T("<!DOCTYPE HTML PUBLIC ");
	output << _T("\"-//W3C//DTD HTML 4.01 Transitional//EN\" ");
	output << _T("\"http://www.w3.org/TR/html4/loose.dtd\">") << std::endl;
	output << _T("<html>") << std::endl;
	output << _T("<head>") << std::endl;
	output << _T("<title>ELOAD Log Messages</title>") << std::endl;
	output << _T("<style type=\"text/css\">") << std::endl;
	output << _T("<!--") << std::endl;
	output << _T("body, table {font-family: arial,sans-serif; font-size: x-small;}") << std::endl;
	output << _T("th {background: #336699; color: #FFFFFF; text-align: left;}") << std::endl;
	output << _T("-->") << std::endl;
	output << _T("</style>") << std::endl;
	output << _T("</head>") << std::endl;
	output << _T("<body bgcolor=\"#FFFFFF\" topmargin=\"6\" leftmargin=\"6\">") << std::endl;
	output << _T("<p style=\"font-family:verdana;color:red;font-size:20px\">") << CELoadLayout::m_header << _T("</p>") << std::endl;
	output << _T("<hr size=\"1\" noshade>") << std::endl;
	output << _T("Log session start time ");
	SYSTEMTIME st;
	GetLocalTime( &st );
	CString strTimeData;
	strTimeData.Format("%d년 %d월 %d일 - %d시 %d분" , st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute);
	output << strTimeData;
	///m_dateFormat.format(output, time(0));
	output << _T("<br>") << std::endl;
	output << _T("<br>") << std::endl;
	output << _T("<table cellspacing=\"0\" cellpadding=\"4\" border=\"1\" bordercolor=\"#224466\" width=\"100%\">") << std::endl;
	
	output << _T("<tr>") << std::endl;
	output << _T("<th>No</th>") << std::endl;
	output << _T("<th>Level</th>") << std::endl;
	if(m_locationInfo)
	{
		output << _T("<th>File:Line</th>") << std::endl;
	}
	output << _T("<th>Bus ID</th>") << std::endl;
	output << _T("<th>Item ID</th>") << std::endl;
	output << _T("<th>Error Code</th>") << std::endl;
	output << _T("<th>Message</th>") << std::endl;
	output << _T("</tr>") << std::endl;
}