#pragma once

#include <log4cxx/HTMLLayout.h>
#include <log4cxx/spi/LoggingEvent.h>
#include <log4cxx/helpers/iso8601dateformat.h>

using namespace std;
using namespace log4cxx;

class CELoadLayout : public HTMLLayout
{
public:
	DECLARE_LOG4CXX_OBJECT(CELoadLayout)
	BEGIN_LOG4CXX_CAST_MAP()
		LOG4CXX_CAST_ENTRY(CELoadLayout)
		LOG4CXX_CAST_ENTRY_CHAIN(HTMLLayout)
	END_LOG4CXX_CAST_MAP()

	CELoadLayout(void);
	~CELoadLayout(void);

	virtual void format(std::ostream& output, const spi::LoggingEventPtr& event) const;
	/**
	Append appropriate HTML headers.
	*/
	virtual void appendHeader(std::ostream& output);
public:
	static string m_header;
private:
	helpers::ISO8601DateFormat m_dateFormat;
	bool m_locationInfo;
	String m_title;
};

/** 
Logs a message to a specified logger with the DEBUG level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define ELOAD_LOG4CXX_DEBUG(logger, item_id , error_code) { \
	if (logger->isDebugEnabled()) {\
	CELoadDocData& docData = CELoadDocData::GetInstance();\
	::log4cxx::StringBuffer oss; \
	oss << docData.GetErrorMessage(error_code); \
	spi::LoggingEventPtr pLoggingEvent = new spi::LoggingEvent(_T("ELoad"), logger , ::log4cxx::Level::DEBUG , oss.str(), __FILE__, __LINE__);\
	CLoadItem* pLoadItem = docData.FindLoadItemByName(item_id);\
	if(pLoadItem) pLoggingEvent->setProperty("Bus ID" , pLoadItem->prop()->GetValue(_T("Panel") , _T("Bus ID")).c_str());\
	pLoggingEvent->setProperty("Item ID" , item_id);\
	oss.str(""); oss << error_code;\
	pLoggingEvent->setProperty("Error Code" , oss.str());\
	logger->callAppenders(pLoggingEvent);}}
	//logger->forcedLog(::log4cxx::Level::DEBUG, oss.str(), __FILE__, __LINE__); }}

/** 
Logs a message to a specified logger with the INFO level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define ELOAD_LOG4CXX_INFO(logger, item_id , error_code) { \
	if (logger->isInfoEnabled()) {\
	CELoadDocData& docData = CELoadDocData::GetInstance();\
	::log4cxx::StringBuffer oss; \
	oss << docData.GetErrorMessage(error_code); \
	spi::LoggingEventPtr pLoggingEvent = new spi::LoggingEvent(_T("ELoad"), logger , ::log4cxx::Level::INFO , oss.str(), __FILE__, __LINE__);\
	CLoadItem* pLoadItem = docData.FindLoadItemByName(item_id);\
	if(pLoadItem) pLoggingEvent->setProperty("Bus ID" , pLoadItem->prop()->GetValue(_T("Panel") , _T("Bus ID")).c_str());\
	pLoggingEvent->setProperty("Item ID" , item_id);\
	oss.str(""); oss << error_code;\
	pLoggingEvent->setProperty("Error Code" , oss.str());\
	logger->callAppenders(pLoggingEvent);}}

/** 
Logs a message to a specified logger with the WARN level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define ELOAD_LOG4CXX_WARN(logger, item_id , error_code) { \
	if (logger->isWarnEnabled()) {\
	CELoadDocData& docData = CELoadDocData::GetInstance();\
	++(docData.m_nErrorCount);\
	::log4cxx::StringBuffer oss; \
	oss << docData.GetErrorMessage(error_code); \
	spi::LoggingEventPtr pLoggingEvent = new spi::LoggingEvent(_T("ELoad"), logger , ::log4cxx::Level::WARN , oss.str(), __FILE__, __LINE__);\
	oss.str("");\
	oss << docData.m_nErrorCount;\
	pLoggingEvent->setProperty("No" , oss.str());\
	CLoadItem* pLoadItem = docData.FindLoadItemByName(item_id);\
	if(pLoadItem) pLoggingEvent->setProperty("Bus ID" , pLoadItem->prop()->GetValue(_T("Panel") , _T("Bus ID")).c_str());\
	pLoggingEvent->setProperty("Item ID" , item_id);\
	oss.str(""); oss << error_code;\
	pLoggingEvent->setProperty("Error Code" , oss.str());\
	logger->callAppenders(pLoggingEvent);}}

/** 
Logs a message to a specified logger with the ERROR level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define ELOAD_LOG4CXX_ERROR(logger, item_id , error_code) { \
	if (logger->isErrorEnabled()) {\
	CELoadDocData& docData = CELoadDocData::GetInstance();\
	++(docData.m_nErrorCount);\
	::log4cxx::StringBuffer oss; \
	oss << docData.GetErrorMessage(error_code); \
	spi::LoggingEventPtr pLoggingEvent = new spi::LoggingEvent(_T("ELoad"), logger , ::log4cxx::Level::ERROR , oss.str(), __FILE__, __LINE__);\
	oss.str("");\
	oss << docData.m_nErrorCount;\
	pLoggingEvent->setProperty("No" , oss.str());\
	CLoadItem* pLoadItem = docData.FindLoadItemByName(item_id);\
	if(pLoadItem) pLoggingEvent->setProperty("Bus ID" , pLoadItem->prop()->GetValue(_T("Panel") , _T("Bus ID")).c_str());\
	pLoggingEvent->setProperty("Item ID" , item_id);\
	oss.str(""); oss << error_code;\
	pLoggingEvent->setProperty("Error Code" , oss.str());\
	logger->callAppenders(pLoggingEvent);}}

/** 
Logs a message to a specified logger with the FATAL level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define ELOAD_LOG4CXX_FATAL(logger, item_id , error_code) { \
	if (logger->isFatalEnabled()) {\
	CELoadDocData& docData = CELoadDocData::GetInstance();\
	++(docData.m_nErrorCount);\
	::log4cxx::StringBuffer oss; \
	oss << docData.GetErrorMessage(error_code); \
	spi::LoggingEventPtr pLoggingEvent = new spi::LoggingEvent(_T("ELoad"), logger , ::log4cxx::Level::FATAL , oss.str() , __FILE__, __LINE__);\
	oss.str("");\
	oss << docData.m_nErrorCount;\
	pLoggingEvent->setProperty("No" , oss.str());\
	CLoadItem* pLoadItem = docData.FindLoadItemByName(item_id);\
	if(pLoadItem) pLoggingEvent->setProperty("Bus ID" , pLoadItem->prop()->GetValue(_T("Panel") , _T("Bus ID")).c_str());\
	pLoggingEvent->setProperty("Item ID" , item_id);\
	oss.str(""); oss << error_code;\
	pLoggingEvent->setProperty("Error Code" , oss.str());\
	logger->callAppenders(pLoggingEvent);}}

#define ELOAD_LOG4CXX_RESET(logger,headerText) { \
	AppenderPtr appender = logger->getAppender("ELoadAppender");\
	appender->close();\
	CELoadDocData& docData = CELoadDocData::GetInstance();\
	docData.m_nErrorCount = 0;\
	CELoadLayout::m_header = headerText;\
	FileAppenderPtr fileappender = (FileAppenderPtr)appender;\
	const string rLogFilePath = fileappender->getFile();\
	::DeleteFile(fileappender->getFile().c_str());\
	log4cxx::LayoutPtr layout = fileappender->getLayout();\
	logger->removeAllAppenders();\
	appender = new log4cxx::FileAppender(layout, rLogFilePath , true);\
	appender->setName(_T("ELoadAppender"));\
	logger->addAppender(appender);}