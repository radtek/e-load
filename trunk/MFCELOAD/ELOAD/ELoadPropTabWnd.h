#pragma once

#include <vector>
#include <IsString.h>
#include "ELoadItemProp.h"
#include "EventData.h"

#include <fstream>
using namespace std;
// CELoadPropTabWnd

class CELoadPropTabWnd : public CMFCTabCtrl
{
	DECLARE_DYNAMIC(CELoadPropTabWnd)

	class CPropTabList
	{
	public:
		CPropTabList(const string& rName) : m_rName(rName){}
		/**
			@brief	

			@author BHK	

			@date 2009-04-10 ¿ÀÈÄ 11:09:18	

			@param	

			@return		
		**/
		int Parse(ifstream& ifile)
		{
			if(ifile.is_open())
			{
				string aLine;
				while(!ifile.eof())
				{
					int fpos = ifile.tellg();
					getline(ifile,aLine);
					if(ifile.eof()) break;
					if(aLine.empty()) continue;
					IsString::TrimWhiteSpace(aLine);
					if(('[' == aLine[0]) && (']' == aLine[aLine.length() - 1]))
					{
						ifile.seekg(fpos);
						break;
					}

					m_CategoryEntry.push_back(aLine);
				}
			}

			return ERROR_SUCCESS;
		}

		size_t GetCategorySize() const { return m_CategoryEntry.size(); }
		string GetCategoryNameAt( const size_t& at )
		{ 
			if(at < m_CategoryEntry.size()) return m_CategoryEntry[at]; 

			return string("");
		}
		string name() const
		{
			return m_rName;
		}
	private:
		string m_rName;
		vector<string> m_CategoryEntry;
	};

public:
	CELoadPropTabWnd( const CString& rPropGroupFilePath );
	virtual ~CELoadPropTabWnd();

protected:
	afx_msg LRESULT OnItemSelected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPropertyChanged(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

	CString m_rPropGroupFilePath;
public:
	int SetValue( const string& rCategory , const string& rKey , const string& rValue , const string& rOriginalValue = _T("IGNORE ORIGINAL VALUE") );
	void SetGroupNameFullWidth (BOOL bGroupNameFullWidth = -1, BOOL bRedraw = -1){}
	void SetVSDotNetLook (BOOL bSet){}
	int InitPropList(CELoadItemProp* pItemProp);
private:
	int SetEventDataValues( CEventData* pEventData );
	int ParsePropGroupFile(const CString& rPropGroupFilePath);
	vector<CPropTabList*> m_TabEntry;
public:
        void SetAlphabeticMode(BOOL bSet = 1);
        BOOL IsAlphabeticMode();
        void CommitModifiedValues(void);
};


