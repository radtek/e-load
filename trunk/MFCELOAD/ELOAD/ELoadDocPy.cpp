#include "stdafx.h"
#include "MainFrm.h"
#include "resource.h"
#include "EloadDocData.h"
#include "Tables/LoadSummaryRefTable.h"
#include "Tables/ProjectSettingTable.h"
#include "PythonCall.h"

#include <list>
#include <sstream>

#ifdef _DEBUG
#undef _DEBUG
#include <python.h>
#define _DEBUG
#else
#include <python.h>
#endif

#include "ELoadDocpy.h"

// cfrompy ist ein in den C++ Code eingebautes Pythonmodul, mit dem
// die R?kverbindung python->C m?lich wird.

int ParsePyArg(PyObject* arg , CStringList& list)
{
	if( arg )
	{
		if( PyList_Check(arg) )
		{
			int nMax = PyList_Size(arg), nIndex = 0;
			while( nIndex < nMax )
			{
				PyObject* item = PyList_GET_ITEM(arg, nIndex);
				if(item) ParsePyArg(item , list);
				/*
				if(PyString_Check(item))
				{
					ASSERT( PyString_Check(item) );

					list.AddTail( PyString_AsString(item) );
				}
				else if(PyFloat_Check(item))
				{
					stringstream oss;
					const double value = PyFloat_AsDouble(item);
					oss << value;
					list.AddTail( oss.str().c_str() );
				}
				else if(Py_None == item)
				{
					int d = 1;
				}
				*/

				nIndex++;
			}
		}
		else if( PyTuple_Check(arg) )
		{
			int nMax = PyTuple_Size(arg), nIndex = 0;
			while( nIndex < nMax )
			{
				PyObject* item = PyTuple_GET_ITEM(arg, nIndex);
				if(item) ParsePyArg(item , list);
				/*
				if(PyList_Check(item))
				{
					ParsePyArg(item , list);
				}
				else if(PyFloat_Check(item))
				{
					stringstream oss;
					const double value = PyFloat_AsDouble(item);
					oss << value;
					list.AddTail( oss.str().c_str() );
				}
				else if(PyString_Check(item))
				{
					list.AddTail( PyString_AsString(item) );
				}
				else if(PyInt_Check(item))
				{
					const long value = PyInt_AsLong(item);
					stringstream oss;
					oss << value;
					list.AddTail(oss.str().c_str());
				}
				else if(Py_None == item)
				{
					int d = 1;
				}
				*/
				nIndex++;
			}
		}
		else if(PyFloat_Check(arg))
		{
			stringstream oss;
			const double value = PyFloat_AsDouble(arg);
			oss << value;
			list.AddTail( oss.str().c_str() );
		}
		else if(PyString_Check(arg))
		{
			list.AddTail( PyString_AsString(arg) );
		}
		else if(PyInt_Check(arg))
		{
			const long value = PyInt_AsLong(arg);
			stringstream oss;
			oss << value;
			list.AddTail(oss.str().c_str());
		}
		else if(Py_None == arg)
		{
			int d = 1;
		}
		else
		{
			ASSERT( FALSE ); // result is neither list nor tuple
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

// Hilfsfunktion: None an Python zur?kgeben
PyObject* Py_ReturnNone()
{
	PyObject* result = Py_None;
	Py_INCREF(result);
	return result;
}

/**
	@brief	return application execution path

	@author	BHK
*/
static PyObject* func_GetExecPath(PyObject* self, PyObject* args)
{
	CString rExecPath = GetExecPath();
        if("\\" != rExecPath.Right(1))
                rExecPath += "\\";
	return Py_BuildValue("s" , rExecPath);
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_GetProjectNo(PyObject* self, PyObject* args)
{
	CELoadDocData& docData  = CELoadDocData::GetInstance();
	const string rProjectNo = docData.GetProjectNo();
	return Py_BuildValue("s" , rProjectNo.c_str());
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_GetProjectName(PyObject* self, PyObject* args)
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	const string rProjectName = docData.GetProjectName();
	return Py_BuildValue("s" , rProjectName.c_str());
}

/**
	@brief	

	@author BHK	

	@date 2009-06-25 오후 2:50:01	

	@param	

	@return		
*/
static PyObject* func_GetProjectDesc(PyObject* self, PyObject* args)
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	const string rValue = docData.GetProjectDesc();
	return Py_BuildValue("s" , rValue.c_str());
}

/**
	@brief	

	@author BHK	

	@date 2009-06-25 오후 2:49:58	

	@param	

	@return		
*/
static PyObject* func_GetProjectClient(PyObject* self, PyObject* args)
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	const string rValue = docData.GetProjectClient();
	return Py_BuildValue("s" , rValue.c_str());
}

/**
	@brief	return job no to python.

	@author	KHS,HumKyung.BAEK
*/
static PyObject* func_GetDocumentNo(PyObject* self, PyObject* args)
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	const string rJobNo = docData.GetJobNo();
	return Py_BuildValue(_T("s") , rJobNo.c_str());
}

/**
	@brief	프로젝트 폴더 경로를 리턴한다.

	@author	BHK
*/
static PyObject* func_GetProjectFolderPath(PyObject* self, PyObject* args)
{
	CELoadDocData& docData      = CELoadDocData::GetInstance();
	const string rProjectFolder = docData.GetProjectFolderPath();
	return Py_BuildValue("s" , rProjectFolder.c_str());
}

/**
	@brief	프로젝트 세팅 값을 설정한다.

	@author	BHK
*/
static PyObject* func_SetProjectSettingValue(PyObject* self, PyObject* args)
{
	CStringList strList;
	ParsePyArg(args , strList);
	Py_DECREF(args);

	if(2 == strList.GetSize())
	{
		int nPos = 0;
		POSITION pos = strList.FindIndex(0);
		string rKey  = strList.GetAt(pos);
		pos = strList.FindIndex(1);
		string rValue= strList.GetAt(pos);

		CELoadDocData& docData      = CELoadDocData::GetInstance();
		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		pSettingTable->SetFieldValue(rKey , rValue);
	}

	return Py_ReturnNone();
}

static PyObject* func_CreateBus(PyObject* self, PyObject* args)
{
	LPSTR arg;
	if( PyArg_ParseTuple(args,"s",&arg) )
	{
		///CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		///pFrame->GetOutputBar()->m_wndScintilla.SetText(arg);
		AfxMessageBox("CreateBusPy");
	}

	return Py_ReturnNone();
}

/**
	@brief	현재 생성된 BUS GROUP의 NAME을 리턴한다.

	@param

	@author	BHK
*/
static PyObject* func_GetBusGroupNameList(PyObject* self, PyObject* args)
{
	string rValue;

	vector<CBusGroupItem*> BusGroupItemEntry;
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> BusGroupList;
	docData.GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
	//if(pBusGroupList)
	{
		for(list<CELoadItem*>::iterator itr = BusGroupList.begin();itr != BusGroupList.end();++itr)
		{
			CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);
			
			if(pBusGroup->IsDeleted()) continue;	//! except deleted bus group
			BusGroupItemEntry.push_back(pBusGroup);
		}
	}

	PyObject* result = PyList_New(0);
	if(result)
	{
		int nItem = 0;
		for(vector<CBusGroupItem*>::iterator itr = BusGroupItemEntry.begin();itr != BusGroupItemEntry.end();++itr)
		{
                        if((*itr)->IsDeleted()) continue;
			PyObject* value = PyString_FromString((*itr)->GetName().c_str());
			if(!value)
			{
				Py_DECREF(result);
				return Py_ReturnNone();
			}
			PyList_Append(result , value);
		}
	}

	return result;
}

/**
	@brief	현재 생성된 BUS GROUP의 NAME을 리턴한다.

	@param

	@author	BHK
*/
static PyObject* func_GetBusGroupNameHasBus(PyObject* self, PyObject* args)
{
	CStringList strList;
	ParsePyArg(args , strList);
	Py_DECREF(args);

	if(1 == strList.GetSize())
	{
		int nPos = 0;
		POSITION pos  = strList.FindIndex(0);
		string rBusId = strList.GetAt(pos);
		
		string rValue;

		vector<CBusGroupItem*> BusGroupItemEntry;
		CELoadDocData& docData = CELoadDocData::GetInstance();
		list<CELoadItem*> BusGroupList;
		docData.GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
		///if(pBusGroupList)
		{
			for(list<CELoadItem*>::iterator itr = BusGroupList.begin();itr != BusGroupList.end();++itr)
			{
				CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);

				if(pBusGroup->IsDeleted()) continue;	//! except deleted bus group
				BusGroupItemEntry.push_back(pBusGroup);
			}
		}

		for(vector<CBusGroupItem*>::iterator itr = BusGroupItemEntry.begin();itr != BusGroupItemEntry.end();++itr)
		{
                        CBusItem* pBusItem = (*itr)->FindBusByName(rBusId);
			if(NULL != pBusItem)
			{
				return Py_BuildValue("s" , (*itr)->GetName().c_str());
			}
			
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	Cable RouteName List를 리턴

	@author KHS	

	@date 2009-06-05 오후 3:15:01	

	@param	

	@return		
**/
static PyObject* func_GetRouteNameList(PyObject* self, PyObject* args)
{
	string rValue;

	vector<CRouteItem*> RouteItemEntry;
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		list<CELoadItem*> RouteItemList;
		docData.GetELoadItemListOf(RouteItemList , CRouteItem::TypeString());
		///if(pRouteItemList)
		{
			for(list<CELoadItem*>::iterator itr = RouteItemList.begin();itr != RouteItemList.end();++itr)
			{
				CRouteItem* pRouteItem = static_cast<CRouteItem*>(*itr);
				if(pRouteItem)
				{
					if(pRouteItem->IsDeleted()) continue;

					RouteItemEntry.push_back(pRouteItem);	
				}
			}
		}
	}

	PyObject* result = PyList_New(0);
	if(result)
	{
		int nItem = 0;
		for(vector<CRouteItem*>::iterator itr = RouteItemEntry.begin();itr != RouteItemEntry.end();++itr)
		{
                        if((*itr)->IsDeleted()) continue;
			PyObject* value = PyString_FromString((*itr)->GetName().c_str());
			if(!value)
			{
				Py_DECREF(result);
				return Py_ReturnNone();
			}
			PyList_Append(result , value);
		}
	}

	return result;
}

/**
	@brief	type에 대응하는 Item들을 리턴

	@author BHK	

	@date 2009-06-15 오전 9:41:58	

	@param	

	@return		
*/
static PyObject* func_GetELoadItemListOf(PyObject* self, PyObject* args)
{
	if(args)
	{
		CStringList strList;
		ParsePyArg(args , strList);
		Py_DECREF(args);

		if(1 == strList.GetSize())
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			const string rTypeString = strList.GetAt(pos);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			list<CELoadItem*> ItemList;
			docData.GetELoadItemListOf(ItemList , rTypeString);
			
			PyObject* result = PyList_New(0);
			if(result)
			{
				int nItem = 0;
				for(list<CELoadItem*>::iterator itr = ItemList.begin();itr != ItemList.end();++itr)
				{
					PyObject* value = PyString_FromString((*itr)->GetName().c_str());
					if(!value)
					{
						Py_DECREF(result);
						return Py_ReturnNone();
					}
					PyList_Append(result , value);
				}

				return result;
			}
		}
	}

	return Py_ReturnNone();
}

struct MatchELoadItemName : binary_function<CELoadItem* , string , bool>
{
	bool operator()(const CELoadItem* lhs , const string& rhs) const
	{
		return (lhs->valid() && (rhs == lhs->GetName()));
	}
};

/**
	@brief	item의 property를 구한다.

	@author BHK	

	@date 2009-06-15 오전 9:48:05	

	@param	

	@return		
*/
static PyObject* func_GetELoadItemProp(PyObject* self, PyObject* args)
{
	string rValue;

	CStringList strList;
	if(args)
	{
		ParsePyArg(args , strList);
		if(4 == strList.GetSize())
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			const string rTypeString = strList.GetAt(pos);
			pos = strList.FindIndex(1);
			const string rName = strList.GetAt(pos);
			pos = strList.FindIndex(2);
			const string rCategory = strList.GetAt(pos);
			pos = strList.FindIndex(3);
			const string rKey = strList.GetAt(pos);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			list<CELoadItem*> ItemList;
			docData.GetELoadItemListOf(ItemList , rTypeString);
			list<CELoadItem*>::iterator where = find_if(ItemList.begin() , ItemList.end() , bind2nd(MatchELoadItemName() , rName));
			if(where != ItemList.end()) rValue = (*where)->prop()->GetValue(rCategory , rKey);
		}

		Py_DECREF(args);
	}

	return Py_BuildValue("s", rValue.c_str());
}

/**
	@brief	item의 property를 설정한다.

	@author BHK	

	@date 2009-06-15 오전 9:57:22	

	@param	

	@return		
*/
static PyObject* func_SetELoadItemProp(PyObject* self, PyObject* args)
{
	string rValue;

	CStringList strList;
	if(args)
	{
		ParsePyArg(args , strList);
		if(5 == strList.GetSize())
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			const string rTypeString = strList.GetAt(pos);
			pos = strList.FindIndex(1);
			const string rName = strList.GetAt(pos);
			pos = strList.FindIndex(2);
			const string rCategory = strList.GetAt(pos);
			pos = strList.FindIndex(3);
			const string rKey = strList.GetAt(pos);
			pos = strList.FindIndex(4);
			const string rValue = strList.GetAt(pos);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			list<CELoadItem*> ItemList;
			docData.GetELoadItemListOf(ItemList , rTypeString);
			list<CELoadItem*>::iterator where = find_if(ItemList.begin() , ItemList.end() , bind2nd(MatchELoadItemName() , rName));
			if(where != ItemList.end()) (*where)->prop()->SetValue(rCategory , rKey , rValue);
		}

		Py_DECREF(args);
	}

	return Py_ReturnNone();
}


/**
	@brief	PDB BUS NAME LIST를 리턴한다.

	@param

	@author	KHS
*/
static PyObject* func_GetPBDBusNameList(PyObject* self, PyObject* args)
{
	string rValue;

	vector<CBusItem*> BusItemEntry;
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		list<CELoadItem*> BusGroupList;
		docData.GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
		//if(pBusGroupList)
		{
			for(list<CELoadItem*>::iterator itr = BusGroupList.begin();itr != BusGroupList.end();++itr)
			{
				CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);
				if(pBusGroup)
				{
					if(_T("PDB") == pBusGroup->GetName())
					{
						for(vector<CBusItem*>::iterator jtr = pBusGroup->m_BusRefEntry.begin(); jtr != pBusGroup->m_BusRefEntry.end();++jtr)
						{
							///! 삭제가 안된 BUS만 넘겨준다.
							if(!(*jtr)->IsDeleted())
							{
								///BusItemEntry.insert(BusItemEntry.end() , (*itr)->m_BusRefEntry.begin() , (*itr)->m_BusRefEntry.end());
								BusItemEntry.push_back((*jtr));
							}
						}
					}
				}
			}
		}
	}

	PyObject* result = PyList_New(0);
	if(result)
	{
		int nItem = 0;
		for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
		{
                        if((*itr)->IsDeleted()) continue;
			PyObject* value = PyString_FromString((*itr)->GetName().c_str());
			if(!value)
			{
				Py_DECREF(result);
				return Py_ReturnNone();
			}
			PyList_Append(result , value);
		}
	}

	return result;
}


/**
	@brief	BUS NAME LIST를 리턴한다.

	@param

	@author	BHK
*/
static PyObject* func_GetBusNameList(PyObject* self, PyObject* args)
{
	string rValue;
	if(args)
	{
		CStringList strList;
		ParsePyArg(args , strList);
		Py_DECREF(args);

		if(1 == strList.GetSize())
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			const string rBusType = strList.GetAt(pos);
		
			vector<CBusItem*> BusItemEntry;
			{
				CELoadDocData& docData = CELoadDocData::GetInstance();
				list<CELoadItem*> BusGroupList;
				docData.GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
				for(list<CELoadItem*>::iterator itr = BusGroupList.begin();itr != BusGroupList.end();++itr)
				{
					CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);
					if(pBusGroup)
					{
						if((_T("ALL") == rBusType) || 
							((_T("BUS") == rBusType) && (SUBBUS != pBusGroup->GetName())) ||
							((_T("PDB") == rBusType) && (SUBBUS == pBusGroup->GetName())))
						{
							for(vector<CBusItem*>::iterator jtr = pBusGroup->m_BusRefEntry.begin(); jtr != pBusGroup->m_BusRefEntry.end();++jtr)
							{
								///! 삭제가 안된 BUS만 넘겨준다.
								if(!(*jtr)->IsDeleted())
								{
									BusItemEntry.push_back((*jtr));
								}
							}
						}
					}
				}
			}

			PyObject* result = PyList_New(0);
			if(result)
			{
				int nItem = 0;
				for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
				{
					if((*itr)->IsDeleted()) continue;
					PyObject* value = PyString_FromString((*itr)->GetName().c_str());
					if(!value)
					{
						Py_DECREF(result);
						return Py_ReturnNone();
					}
					PyList_Append(result , value);
				}
				return result;
			}
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@param	args[0]에 BUS GROUP NAME

	@author	BHK
*/
static PyObject* func_GetBusNameListInBusGroup(PyObject* self, PyObject* args)
{
	PyObject* result = NULL;

	if(args)
	{
		CStringList strList;
		ParsePyArg(args , strList);
		Py_DECREF(args);

		if(1 == strList.GetSize())
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			string rBusGroupName = strList.GetAt(pos);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CBusGroupItem* pBusGroupItem = docData.FindBusGroupItemByName(rBusGroupName);
			if(pBusGroupItem)
			{
				vector<CBusItem*> BusItemEntry;
				BusItemEntry.insert(BusItemEntry.end() , pBusGroupItem->m_BusRefEntry.begin() , pBusGroupItem->m_BusRefEntry.end());
				
				result = PyList_New(0);
				if(result)
				{
					int nItem = 0;
					for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
					{
                                                if((*itr)->IsDeleted()) continue;
						PyObject* value = PyString_FromString((*itr)->GetName().c_str());
						if(!value)
						{
							Py_DECREF(result);
							return Py_ReturnNone();
						}
						PyList_Append(result , value);
					}

					return result;
				}
			}
		}
	}
	
	return Py_ReturnNone();
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_GetTransformerNameList(PyObject* self, PyObject* args)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> TrList;
	docData.GetELoadItemListOf(TrList , CTransformerItem::TypeString());
	//if(pList)
	{
		PyObject* result = PyList_New(0);
		if(result)
		{
			int nItem = 0;
			for(list<CELoadItem*>::iterator itr = TrList.begin();itr != TrList.end();++itr)
			{
				if((*itr)->IsDeleted()) continue;
				PyObject* value = PyString_FromString((*itr)->GetName().c_str());
				if(!value)
				{
					Py_DECREF(result);
					return Py_ReturnNone();
				}
				PyList_Append(result , value);
			}

			return result;
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@param	args에 Transformer의 name,category,key값으로 3개의 값을 가진다.

	@author	BHK
*/
static PyObject* func_GetTransformerProp(PyObject* self, PyObject* args)
{
	string rValue;

	CStringList strList;
	if(args)
	{
		ParsePyArg(args , strList);
		Py_DECREF(args);
		if(3 == strList.GetSize())
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			string rName = strList.GetAt(pos);
			pos = strList.FindIndex(1);
			string rCategory = strList.GetAt(pos);
			pos = strList.FindIndex(2);
			string rKey      = strList.GetAt(pos);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CTransformerItem* pTrItem = docData.FindTransformerItemByName(rName);
			if(pTrItem)
			{
				rValue = pTrItem->prop()->GetValue(rCategory , rKey);
				return Py_BuildValue("s", rValue.c_str());
			}
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@author BHK	

	@date 2009-05-15 오후 6:52:04	

	@param	

	@return		
*/
static PyObject* func_GetLoadSummaryCalcDataOfTr(PyObject* self, PyObject* args)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CStringList strList;
	ParsePyArg(args , strList);
	Py_DECREF(args);
	if(1 == strList.GetSize())
	{
		int nPos = 0;
		POSITION pos = strList.FindIndex(0);
		const string rTrID = strList.GetAt(pos);

		CTransformerItem* pTr = docData.FindTransformerItemByName(rTrID);
		if(pTr)
		{
			double nFactors[3] = {0.f , 0.f , 0.f};
			double nKW[3] = {0.f , 0.f , 0.f} , nKVAR[3] = {0.f , 0.f , 0.f};

			pTr->GetLoadSummaryCalcData(nFactors , nKW , nKVAR);
			return Py_BuildValue("[d,d,d,d,d,d,d,d,d]" , 
				nFactors[0] , nFactors[1] , nFactors[3] ,
				nKW[0] , nKVAR[0] , 
				nKW[1] , nKVAR[1] , 
				nKW[3] , nKVAR[2]);
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	Tr에 연결된 Bus Name들을 구한다.

	@author BHK	

	@date 2009-05-19 오전 10:20:07	

	@param	

	@return		
*/
static PyObject* func_GetConnectedBusNameListOfTr(PyObject* self, PyObject* args)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CStringList strList;
	ParsePyArg(args , strList);
	Py_DECREF(args);
	if(1 == strList.GetSize())
	{
		int nPos = 0;
		POSITION pos = strList.FindIndex(0);
		const string rTrID = strList.GetAt(pos);

		CTransformerItem* pTr = docData.FindTransformerItemByName(rTrID);
		if(pTr)
		{
			vector<CBusItem*> BusItemEntry;
			pTr->GetConnectedBusItemEntry(BusItemEntry);

			//! BUS NAME을 PYTHON에 넘겨 주기위해 LIST에 담는다.
			PyObject* result = PyList_New(0);
			if(result)
			{
				int nItem = 0;
				for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
				{
					PyObject* value = PyString_FromString((*itr)->GetName().c_str());
					if(!value)
					{
						Py_DECREF(result);
						return Py_ReturnNone();
					}
					PyList_Append(result , value);
				}

				return result;
			}
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@param	args에 Transformer의 name,category,key,value값으로 4개의 값을 가진다.

	@author	BHK
*/
static PyObject* func_SetTransformerProp(PyObject* self, PyObject* args)
{
	string rValue;

	CStringList strList;
	if(args)
	{
		ParsePyArg(args , strList);
		Py_DECREF(args);
		if(4 == strList.GetSize())
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			string rName = strList.GetAt(pos);
			pos = strList.FindIndex(1);
			string rCategory = strList.GetAt(pos);
			pos = strList.FindIndex(2);
			string rKey      = strList.GetAt(pos);
			pos = strList.FindIndex(3);
			string rValue    = strList.GetAt(pos);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CTransformerItem* pTrItem = docData.FindTransformerItemByName(rName);
			if(pTrItem)
			{
				pTrItem->prop()->SetValue(rCategory , rKey , rValue);
			}
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@param	args에 Bus의 name,category,key값으로 3개의 값을 가진다.

	@author	BHK
*/
static PyObject* func_GetBusProp(PyObject* self, PyObject* args)
{
	string rValue;

	CStringList strList;
	if(args)
	{
		ParsePyArg(args , strList);
		Py_DECREF(args);
		if(3 == strList.GetSize())
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			string rName = strList.GetAt(pos);
			pos = strList.FindIndex(1);
			string rCategory = strList.GetAt(pos);
			pos = strList.FindIndex(2);
			string rKey      = strList.GetAt(pos);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CBusItem* pBusItem = docData.FindBusItemByName(rName);
			if(pBusItem)
			{
				rValue = pBusItem->prop()->GetValue(rCategory , rKey);
				return Py_BuildValue("s", rValue.c_str());
			}
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@author BHK	

	@date 2009-04-01 오후 6:57:33	

	@param	

	@return		
*/
static PyObject* func_GetLoadSummaryInBus(PyObject* self, PyObject* args)
{
	string rValue;

	CStringList strList;
	if(args)
	{
		ParsePyArg(args , strList);
		Py_DECREF(args);
		if(1 == strList.GetSize())
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			const string rBusID = strList.GetAt(pos);

			double nKW = 0. , nKVAR = 0.;
			CELoadDocData& docData = CELoadDocData::GetInstance();
			CBusItem* pBusItem = docData.FindBusItemByName(rBusID);
			if(pBusItem) pBusItem->GetLoadSummaryResultData(nKW , nKVAR);
			return Py_BuildValue("[d,d]", nKW , nKVAR);
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@param	args에 Bus의 name

	@author	BHK
*/
static PyObject* func_GetLoadNameList(PyObject* self, PyObject* args)
{
	//! LOAD NAME을 PYTHON에 넘겨 주기위해 LIST에 담는다.
	PyObject* result = PyList_New(0);
	if(result)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		int nItem = 0;
		list<CELoadItem*> LoadItemList;
		docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
		if(!LoadItemList.empty())
		{
			for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
			{
				CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);

				PyObject* value = PyString_FromString(pLoad->GetName().c_str());
				if(!value)
				{
					Py_DECREF(result);
					return Py_ReturnNone();
				}
				PyList_Append(result , value);
			}
		}
	}

	return result;
}

/**
	@brief	

	@param	args에 Bus의 name

	@author	BHK
*/
static PyObject* func_GetLoadNameListInBus(PyObject* self, PyObject* args)
{
	string rValue;

	vector<CLoadItem*> LoadItemEntry;

	CStringList strList;
	ParsePyArg(args , strList);
	Py_DECREF(args);
	if(1 == strList.GetSize())
	{
		int nPos = 0;
		POSITION pos = strList.FindIndex(0);
		string rName = strList.GetAt(pos);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		list<CELoadItem*> LoadItemList;
		docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
		if(!LoadItemList.empty())
		{
			for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
			{
				CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);
				if(rName == pLoad->GetBusId()) LoadItemEntry.push_back(pLoad);
			}
		}
	}

	//! LOAD NAME을 PYTHON에 넘겨 주기위해 LIST에 담는다.
	PyObject* result = PyList_New(0);
	if(result)
	{
		int nItem = 0;
		for(vector<CLoadItem*>::iterator itr = LoadItemEntry.begin();itr != LoadItemEntry.end();++itr)
		{
			PyObject* value = PyString_FromString((*itr)->GetName().c_str());
			if(!value)
			{
				Py_DECREF(result);
				return Py_ReturnNone();
			}
			PyList_Append(result , value);
		}
	}

	return result;
}

/**
	@brief	BUS에 연결된 Transformer들을 구한다.

	@author BHK	

	@date 2009-05-15 오후 1:28:51	

	@param	

	@return		
*/
static PyObject* func_GetTrNameListInBus(PyObject* self, PyObject* args)
{
	vector<CTransformerItem*> TrItemEntry;

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CStringList strList;
	ParsePyArg(args , strList);
	Py_DECREF(args);
	if(1 == strList.GetSize())
	{
		int nPos = 0;
		POSITION pos = strList.FindIndex(0);
		const string rBusID = strList.GetAt(pos);

		CBusItem* pBusItem = docData.FindBusItemByName(rBusID);
		if(pBusItem) pBusItem->GetConnectedTrItemEntry(TrItemEntry);
	}

	//! TRANSFORMER NAME을 PYTHON에 넘겨 주기위해 LIST에 담는다.
	PyObject* result = PyList_New(0);
	if(result)
	{
		int nItem = 0;
		for(vector<CTransformerItem*>::iterator itr = TrItemEntry.begin();itr != TrItemEntry.end();++itr)
		{
			PyObject* value = PyString_FromString((*itr)->GetName().c_str());
			if(!value)
			{
				Py_DECREF(result);
				return Py_ReturnNone();
			}
			PyList_Append(result , value);
		}
	}

	return result;
}

/**
	@brief	

	@param	args에 Rating Capacity,volt , Poles, LF 의 4개의 값을 가진다.

	@author	BHK
*/
static PyObject* func_SearchLoadTable(PyObject* self, PyObject* args)
{
	string rValue;

	CStringList strList;
	if(args)
	{
		ParsePyArg(args , strList);
		Py_DECREF(args);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		string* pValue = pSettingTable->GetFieldValue(_T("C_HERTZ"));
		const int nHertz = ((NULL != pValue) && !pValue->empty()) ? atoi(pValue->c_str()) : 50;

		if(4 == strList.GetSize())
		{
			CLoadSummaryRefTable& refTable = CLoadSummaryRefTable::GetInstance();

			int nPos = 0;

			POSITION pos = strList.FindIndex(0);
			string rRatingCapacity = strList.GetAt(pos);

			pos = strList.FindIndex(1);
			string rVolt = strList.GetAt(pos);

			pos = strList.FindIndex(2);
			string rPoles = strList.GetAt(pos);

			pos = strList.FindIndex(3);
			string rLf = strList.GetAt(pos);

			LoadSearchResult res;
			if(ERROR_SUCCESS == refTable.FindValues(res , atof(rRatingCapacity.c_str()) , nHertz , atof(rVolt.c_str()) , atoi(rPoles.c_str()) , atof(rLf.c_str())))
			{ 
				//! 결과값을 리스트 형식으로 PYTHON에게 넘겨준다.
				return Py_BuildValue("[d,d,d,d,d,d,d,d,d,d]" , res.PF100 , res.PF75 , res.PF50 , res.EFF100 , res.EFF75 , res.EFF50 , res.PF , res.EFF ,
					res.LRC , res.PFlr);
			}
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@param	args에 name,category,key값으로 3개의 값을 가진다.

	@author	BHK
*/
static PyObject* func_GetLoadProp(PyObject* self, PyObject* args)
{
	string rValue;

	CStringList strList;
	if(args)
	{
		ParsePyArg(args , strList);
		if(3 == strList.GetSize())
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			string rName = strList.GetAt(pos);
			pos = strList.FindIndex(1);
			string rCategory = strList.GetAt(pos);
			pos = strList.FindIndex(2);
			string rKey      = strList.GetAt(pos);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CLoadItem* pLoadItem = docData.FindLoadItemByName(rName);
			if(pLoadItem)
			{
				rValue = pLoadItem->prop()->GetValue(rCategory , rKey);
			}
		}

		Py_DECREF(args);
	}

	return Py_BuildValue("s", rValue.c_str());
}

/**
	@brief	Cable Size용 FLC값을 구합니다.

	@author BHK	

	@date 2009-04-24 오후 2:48:34	

	@param	

	@return		
*/
static PyObject* func_GetFLCForCable(PyObject* self, PyObject* args)
{
	double nFLC = 0.f;

	CStringList strList;
	if(args)
	{
		ParsePyArg(args , strList);
		if(1 == strList.GetSize())
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			string rLoadID = strList.GetAt(pos);
			
			CELoadDocData& docData = CELoadDocData::GetInstance();
			CLoadItem* pLoadItem = docData.FindLoadItemByName(rLoadID);
			if(pLoadItem) nFLC = pLoadItem->GetFLCForPowerCableCreation();
		}

		Py_DECREF(args);
	}

	return Py_BuildValue("d", nFLC);
}

/**
	@brief	property의 LOCK상태 여부를 리턴한다.

	@param	args에 name,category,key값으로 3개의 값을 가진다.

	@author	BHK
*/
static PyObject* func_IsLoadPropLock(PyObject* self, PyObject* args)
{
	CStringList strList;
	if(args)
	{
		ParsePyArg(args , strList);
		Py_DECREF(args);
		if(3 == strList.GetSize())
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			string rName = strList.GetAt(pos);
			pos = strList.FindIndex(1);
			string rCategory = strList.GetAt(pos);
			pos = strList.FindIndex(2);
			string rKey      = strList.GetAt(pos);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CLoadItem* pLoadItem = docData.FindLoadItemByName(rName);
			if(pLoadItem)
			{
				CItemPropCategory* pCategory = pLoadItem->prop()->GetCategoryByName(rCategory);
				if(pCategory)
				{
					return Py_BuildValue("s" , (true == pCategory->IsLocked(rKey)) ? _T("TRUE") : _T("FALSE"));
				}
			}
		}
	}

	return Py_BuildValue("s", _T("FALSE"));
}
/**
	@brief	Load property에 설정한 unit값을 가져온다.

	@param	args에 name,category,key값으로 3개의 값을 가진다.

	@author	BHK
*/
static PyObject* func_GetLoadPropUnitStr(PyObject* self, PyObject* args)
{
	string rValue;

	CStringList strList;
	if(args)
	{
		ParsePyArg(args , strList);
		if(3 == strList.GetSize())
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			string rName = strList.GetAt(pos);
			pos = strList.FindIndex(1);
			string rCategory = strList.GetAt(pos);
			pos = strList.FindIndex(2);
			string rKey      = strList.GetAt(pos);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CLoadItem* pLoadItem = docData.FindLoadItemByName(rName);
			if(pLoadItem)
			{
				rValue = pLoadItem->prop()->GetUnitStr(rCategory , rKey);
			}
		}

		Py_DECREF(args);
	}

	return Py_BuildValue("s", rValue.c_str());
}

/**
	@brief	Load property에 값을 설정한다.

	@param	args에 name,category,key값으로 3개의 값을 가진다.

	@author	BHK
*/
static PyObject* func_SetLoadProp(PyObject* self, PyObject* args)
{
	CStringList strList;

	if( args )
	{
		ParsePyArg(args , strList);
		if(4 == (strList.GetSize()))
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			string rName = strList.GetAt(pos);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CLoadItem* pLoadItem = docData.FindLoadItemByName(rName);
			
			if(pLoadItem)
			{
				pos = strList.FindIndex(1);
				string rCategory = strList.GetAt(pos);
				pos = strList.FindIndex(2);
				string rKey = strList.GetAt(pos);
				pos = strList.FindIndex(3);
				string rValue = strList.GetAt(pos);
			
				if((_T("Characteristic") == rCategory) && (_T("PF LOCK") == rKey))
				{
					int d = 1;
				}

				pLoadItem->prop()->SetValue(rCategory , rKey , rValue);
			}
		}
		Py_DECREF(args);
	}
	
	return Py_ReturnNone();
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_SetLoadOriginalProp(PyObject* self, PyObject* args)
{
	CStringList strList;

	if( args )
	{
		ParsePyArg(args , strList);
		if(4 == (strList.GetSize()))
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			string rName = strList.GetAt(pos);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CLoadItem* pLoadItem = docData.FindLoadItemByName(rName);
			if(pLoadItem)
			{
				pos = strList.FindIndex(1);
				string rCategory = strList.GetAt(pos);
				pos = strList.FindIndex(2);
				string rKey = strList.GetAt(pos);
				pos = strList.FindIndex(3);
				string rValue = strList.GetAt(pos);
				
				pLoadItem->prop()->SetOriginalValue(rCategory , rKey , rValue);
			}
		}
		Py_DECREF(args);
	}
	
	return Py_ReturnNone();
}

/**
	@brief	get rating capacity of load. 만일 연결된 Bus의 Rating Capacity를 사용한다면 계산후 리턴한다.

	@author BHK	

	@date 2009-04-06 오후 3:02:07	

	@param	

	@return	rating capacity
*/
static PyObject* func_GetLoadRatingCapacity(PyObject* self, PyObject* args)
{
	CStringList strList;

	if( args )
	{
		ParsePyArg(args , strList);
		Py_DECREF(args);
		if(1 == (strList.GetSize()))
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			string rLoadId = strList.GetAt(pos);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CLoadItem* pLoadItem = docData.FindLoadItemByName(rLoadId);
			if(pLoadItem)
			{
				double nRatingCapacity = 0.f;
				pLoadItem->CalcRatingCapacity(nRatingCapacity);
				
				if(0.f != nRatingCapacity)
				{
					ostringstream oss;
					oss << nRatingCapacity;

					return Py_BuildValue(_T("s") , oss.str().c_str());
				}
			}
		}
		
	}
	
	return Py_ReturnNone();
}

/**
	@brief	프로젝트 세팅 테이블에서 키값에 해당하는 값을 리턴한다.

	@author	BHK
*/
static PyObject* func_GetFieldValueInProjectSettingTable(PyObject* self, PyObject* args)
{
	CStringList strList;

	if( args )
	{
		ParsePyArg(args , strList);
		Py_DECREF(args);
		if(1 == (strList.GetSize()))
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			string rName = strList.GetAt(pos);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
			string* pValue = pSettingTable->GetFieldValue(rName);
			return Py_BuildValue("s", pValue->c_str());
		}
	}
	
	return Py_ReturnNone();
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_GetStartingVoltageDrop2(PyObject* self, PyObject* args)
{
	if(args) Py_DECREF(args);
	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

	CString rValue = CCableSizeSettingTable::GetInstance().GetStartingVoltageDrop2(rDataName);
	return Py_BuildValue("s", rValue.operator LPCSTR());
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_GetStartingPowerFactor(PyObject* self, PyObject* args)
{
	if(args) Py_DECREF(args);
	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

	CString rValue = CCableSizeSettingTable::GetInstance().GetStartingPowerFactor(rDataName);
	return Py_BuildValue("s", rValue.operator LPCSTR());
}

/**
	@brief	BUS의 LoadSummary 결과를 리턴한다.

	@author BHK	

	@date 2009-05-19 오전 10:25:27	

	@param	

	@return		
*/
static PyObject* func_GetLoadSummaryCalcDataOfBus(PyObject* self, PyObject* args)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CStringList strList;
	ParsePyArg(args , strList);
	Py_DECREF(args);
	if(1 == strList.GetSize())
	{
		int nPos = 0;
		POSITION pos = strList.FindIndex(0);
		const string rBusID = strList.GetAt(pos);

		CBusItem* pBusItem = docData.FindBusItemByName(rBusID);
		if(pBusItem)
		{
			double nFactors[3] = {0.f , 0.f , 0.f};
			double nKW[3] = {0.f , 0.f , 0.f} , nKVAR[3] = {0.f , 0.f , 0.f};

			pBusItem->GetLoadSummaryCalcData(nFactors , nKW , nKVAR);
			return Py_BuildValue("[d,d,d,d,d,d,d,d,d]" , 
				nFactors[0] , nFactors[1] , nFactors[2] ,
				nKW[0] , nKVAR[0] , 
				nKW[1] , nKVAR[1] , 
				nKW[2] , nKVAR[2]);
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	UPS/DC의 LoadSummary 결과를 리턴한다.

	@author BHK	

	@date 2009-06-15 오후 7:16:34	

	@param	

	@return		
*/
static PyObject* func_GetLoadSummaryCalcDataOfUPSDC(PyObject* self, PyObject* args)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CStringList strList;
	ParsePyArg(args , strList);
	Py_DECREF(args);
	if(2 == strList.GetSize())
	{
		int nPos = 0;
		POSITION pos = strList.FindIndex(0);
		const string rUPSDCID = strList.GetAt(pos);
		pos = strList.FindIndex(1);
		const string rBusID = strList.GetAt(pos);

		list<CELoadItem*> ItemList;
		docData.GetELoadItemListOf(ItemList , _T("UPSDC"));
		list<CELoadItem*>::iterator where = find_if(ItemList.begin() , ItemList.end() , bind2nd(MatchItemName() , rUPSDCID));
		if(where != ItemList.end())
		{
			double nFactors[3] = {0.f , 0.f , 0.f};
			double nKW[3] = {0.f , 0.f , 0.f} , nKVAR[3] = {0.f , 0.f , 0.f};

			CUPSDCItem* pUPSDCItem = static_cast<CUPSDCItem*>(*where);
			pUPSDCItem->GetLoadSummaryCalcData(nFactors , nKW , nKVAR , rBusID);
			return Py_BuildValue("[d,d,d,d,d,d,d,d,d]" , 
				nFactors[0] , nFactors[1] , nFactors[2] ,
				nKW[0] , nKVAR[0] , 
				nKW[1] , nKVAR[1] , 
				nKW[2] , nKVAR[2]);
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	CF값을 넘겨준다.

	@author	BHK
static PyObject* func_GetCF(PyObject* self, PyObject* args)
{
	if(args) Py_DECREF(args);
	CELoadDocData& docData = CELoadDocData::GetInstance();
        //! 결과값을 리스트 형식으로 PYTHON에게 넘겨준다.
        return Py_BuildValue("[d,d,d]" , docData.m_LoadSummaryOptionData.m_dContinuous , docData.m_LoadSummaryOptionData.m_dIntermittent , docData.m_LoadSummaryOptionData.m_dStandby );
}
*/

/**
	@brief	

	@author	HumKyung

	@date	????.??.??

	@param	self
	@param	args

	@return	PyObject*
*/
static PyObject* func_GetCableLengthDataSheetRowCount(PyObject* self, PyObject* args)
{
	if(args) Py_DECREF(args);
	CELoadDocData& docData = CELoadDocData::GetInstance();
	//! integer형으로 넘긴다.
        return Py_BuildValue("i" , docData.m_CableLengthDataSheet.GetRowCount());
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_GetCableLengthDataSheetColCount(PyObject* self, PyObject* args)
{
	if(args) Py_DECREF(args);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	//! integer형으로 넘긴다.
        return Py_BuildValue("i" , docData.m_CableLengthDataSheet.GetColCount());
}

/**
        @brief  

        @author BHK
*/
static PyObject* func_GetCableDataList(PyObject* self, PyObject* args)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	CStringList strList;
	ParsePyArg(args , strList);
	Py_DECREF(args);

	vector<string> CableTypeList;
	docData.m_ProjectCableDataTable.GetCableTypeKeyList(CableTypeList);

	if(!CableTypeList.empty() && 2 == strList.GetSize())
	{
		int nPos = 0;
		POSITION pos = strList.FindIndex(0);
		const CString rHertz = strList.GetAt(pos);
		pos = strList.FindIndex(1);
		const string rCableType = strList.GetAt(pos);

		PyObject* pResult = PyList_New(0);

		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(rCableType);
		if(pCableDataByType)
		{
		vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz(atoi(rHertz));
		if(pRecordList)
		{
			for(vector<CCableDataByType::Record*>::iterator jtr = pRecordList->begin();jtr != pRecordList->end();++jtr)
			{
				PyObject* pRowList = PyList_New(16);

				///PyObject* pValue = PyString_FromString((*jtr)->m_FieldMap[_TNO.c_str());
				///if(pValue) PyList_SetItem(pRowList , 0 , pValue);

				PyObject* pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_HERTZ")].c_str());
				if(pValue) PyList_SetItem(pRowList , 0 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_CONDUCTOR")].c_str());
				if(pValue) PyList_SetItem(pRowList , 1 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_VOLT")].c_str());
				if(pValue) PyList_SetItem(pRowList , 2 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
				if(pValue) PyList_SetItem(pRowList , 3 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_CORE")].c_str());
				if(pValue) PyList_SetItem(pRowList , 4 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_MPR")].c_str());
				if(pValue) PyList_SetItem(pRowList , 5 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_UG_BASE_TEMP")].c_str());
				if(pValue) PyList_SetItem(pRowList , 6 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_AG_BASE_TEMP")].c_str());
				if(pValue) PyList_SetItem(pRowList , 7 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_MAX_CONDUCTOR_TEMP")].c_str());
				if(pValue) PyList_SetItem(pRowList , 8 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_SIZE")].c_str());
				if(pValue) PyList_SetItem(pRowList , 9 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_AMP_G")].c_str());
				if(pValue) PyList_SetItem(pRowList , 10 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_AMP_A")].c_str());
				if(pValue) PyList_SetItem(pRowList , 11 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_R")].c_str());
				if(pValue) PyList_SetItem(pRowList , 12 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_X")].c_str());
				if(pValue) PyList_SetItem(pRowList , 13 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_DIA")].c_str());
				if(pValue) PyList_SetItem(pRowList , 14 , pValue);

				pValue = PyString_FromString((*jtr)->m_FieldMap[_T("C_CABLE_WEIGHT")].c_str());
				if(pValue) PyList_SetItem(pRowList , 15, pValue);

				///pValue = PyString_FromString((*jtr)->C_X_50.c_str());
				///if(pValue) PyList_SetItem(pRowList , 8 , pValue);

				PyList_Append(pResult , pRowList);
			}
		}

		return pResult;
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_GetCableDataSizeList(PyObject* self, PyObject* args)
{
	if(args) Py_DECREF(args);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

	CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
	CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(
		CableSizeSettingTable.m_RecordMap[rDataName].m_rCableType);
	vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
	if(pRecordList)
	{
		PyObject* pResult = PyList_New(pRecordList->size());
		if(pResult)
		{
			int i = 0;
			for(vector<CCableDataByType::Record*>::iterator itr = pRecordList->begin();itr != pRecordList->end();++itr)
			{
				PyObject* pValue = PyString_FromString((*itr)->m_FieldMap[_T("C_SIZE")].c_str());
				if(pValue) PyList_SetItem(pResult , i++ , pValue);				
			}

			return pResult;
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_GetCableDataDiaList(PyObject* self, PyObject* args)
{
	if(args) Py_DECREF(args);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

	CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
	CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(
		CableSizeSettingTable.m_RecordMap[rDataName].m_rCableType);
	vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
	if(pRecordList)
	{
		PyObject* pResult = PyList_New(pRecordList->size());
		if(pResult)
		{
			int i = 0;
			for(vector<CCableDataByType::Record*>::iterator itr = pRecordList->begin();itr != pRecordList->end();++itr)
			{
				PyObject* pValue = PyString_FromString((*itr)->m_FieldMap[_T("C_DIA")].c_str());
				if(pValue) PyList_SetItem(pResult , i++ , pValue);				
			}

			return pResult;
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_GetCableDataAllowanceCurrentList(PyObject* self, PyObject* args)
{
	if(args) Py_DECREF(args);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

	CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
	CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(
		CableSizeSettingTable.m_RecordMap[rDataName].m_rCableType);
	vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
	if(pRecordList)
	{
		PyObject* pResult = PyList_New(pRecordList->size());
		if(pResult)
		{
			const bool bAirInsulation = (_T("AIR") == CableSizeSettingTable.m_RecordMap[rDataName].m_rInsulationMethod) ? true : false;

			int i = 0;
			for(vector<CCableDataByType::Record*>::iterator itr = pRecordList->begin();itr != pRecordList->end();++itr)
			{
				PyObject* pValue = NULL;
				pValue = (bAirInsulation) ? PyString_FromString((*itr)->m_FieldMap[_T("C_AMP_A")].c_str()) : 
					PyString_FromString((*itr)->m_FieldMap[_T("C_AMP_G")].c_str());
				if(pValue) PyList_SetItem(pResult , i++ , pValue);
			}

			return pResult;
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_GetCableDataRegistanceList(PyObject* self, PyObject* args)
{
	if(args) Py_DECREF(args);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

	CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
	CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(
		CableSizeSettingTable.m_RecordMap[rDataName].m_rCableType);
	vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
	if(pRecordList)
	{
		PyObject* pResult = PyList_New(pRecordList->size());
		if(pResult)
		{
			int i = 0;
			for(vector<CCableDataByType::Record*>::iterator itr = pRecordList->begin();itr != pRecordList->end();++itr)
			{
				PyObject* pValue = PyString_FromString((*itr)->m_FieldMap[_T("C_R")].c_str());
				if(pValue) PyList_SetItem(pResult , i++ , pValue);				
			}

			return pResult;
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_GetCableDataReactanceList(PyObject* self, PyObject* args)
{
	if(args) Py_DECREF(args);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

	CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
	CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(
		CableSizeSettingTable.m_RecordMap[rDataName].m_rCableType);
	vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
	if(pRecordList)
	{
		PyObject* pResult = PyList_New(pRecordList->size());
		if(pResult)
		{
			CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
			string* pValue = pSettingTable->GetFieldValue(_T("C_HERTZ"));
			const string rHertz = ((NULL != pValue) && !pValue->empty()) ? pValue->c_str() : _T("50");

			int i = 0;
			for(vector<CCableDataByType::Record*>::iterator itr = pRecordList->begin();itr != pRecordList->end();++itr)
			{

				PyObject* pValue = PyString_FromString((*itr)->m_FieldMap[_T("C_X")].c_str());			
				if(pValue) PyList_SetItem(pResult , i++ , pValue);				
			}

			return pResult;
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	Cable Length 계산에 사용한 CONDUIT LIST를 리턴한다.

	@author	BHK
*/
static PyObject* func_GetCableLengthConduitValueList(PyObject* self, PyObject* args)
{
	if(args) Py_DECREF(args);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	PyObject* pResult = PyList_New(docData.m_CableLengthDataSheet.m_ConduitSizeList.size());
	if(pResult)
	{
		int i = 0;
		for(vector<CCableLengthDataSheet::ConduitSizeData>::iterator itr = docData.m_CableLengthDataSheet.m_ConduitSizeList.begin();itr != docData.m_CableLengthDataSheet.m_ConduitSizeList.end();++itr)
		{
			stringstream oss;
			oss << (itr->ConduitSize);
			PyObject* pValue = PyString_FromString(oss.str().c_str());
			if(pValue) PyList_SetItem(pResult , i++ , pValue);
		}
		
		return pResult;
	}

	return Py_ReturnNone();
}

/**
	@brief	rating capacity list를 구한다.

	@author	BHK
*/
static PyObject* func_GetCableLengthKWList(PyObject* self, PyObject* args)
{
	if(args) Py_DECREF(args);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const int nSize = docData.m_CableLengthDataSheet.m_RatingCapacityEntry.size();
	PyObject* pResult = PyList_New(nSize);
	if(pResult)
	{
		int i = 0;
		for(vector<double>::iterator itr = docData.m_CableLengthDataSheet.m_RatingCapacityEntry.begin();itr != docData.m_CableLengthDataSheet.m_RatingCapacityEntry.end();++itr)
		{
			PyObject* pValue = PyFloat_FromDouble(*itr);
			if(pValue) PyList_SetItem(pResult , i++ , pValue);
		}

		return pResult;
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_GetCableLengthFLCList(PyObject* self, PyObject* args)
{
	if(args) Py_DECREF(args);

	CStringList strList;
	
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const int nSize = docData.m_CableLengthDataSheet.m_FLCEntry.size();
	PyObject* pResult = PyList_New(nSize);
	if(pResult)
	{
		int i = 0;
		for(vector<double>::iterator itr = docData.m_CableLengthDataSheet.m_FLCEntry.begin();itr != docData.m_CableLengthDataSheet.m_FLCEntry.end();++itr)
		{
			PyObject* pValue = PyFloat_FromDouble(*itr);
			if(pValue) PyList_SetItem(pResult , i++ , pValue);
		}

		return pResult;
	}

	return Py_ReturnNone();
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_GetCableLengthPFList(PyObject* self, PyObject* args)
{
	if(args) Py_DECREF(args);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const int nSize = docData.m_CableLengthDataSheet.m_PFEntry.size();
	PyObject* pResult = PyList_New(nSize);
	if(pResult)
	{
		int i = 0;
		for(vector<double>::iterator itr = docData.m_CableLengthDataSheet.m_PFEntry.begin();itr != docData.m_CableLengthDataSheet.m_PFEntry.end();++itr)
		{
			PyObject* pValue = PyFloat_FromDouble(*itr);
			if(pValue) PyList_SetItem(pResult , i++ , pValue);
		}

		return pResult;
	}	

	return Py_ReturnNone();
}

/**
	@brief	

	@author	BHK
*/
static PyObject* func_GetCableLengthDataSheetValue(PyObject* self, PyObject* args)
{
	CStringList strList;

	if( args )
	{
		ParsePyArg(args , strList);
		Py_DECREF(args);
		if(2 == (strList.GetSize()))
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			const int nRow = atoi(strList.GetAt(pos));
			pos = strList.FindIndex(1);
			const int nCol = atoi(strList.GetAt(pos));

			CELoadDocData& docData = CELoadDocData::GetInstance();
			string rValue = docData.m_CableLengthDataSheet.GetValue(nRow , nCol);
			return Py_BuildValue("s", rValue.c_str());
		}
	}

	return Py_ReturnNone();
}

/**
	@brief	log message를 outputwnd에 남긴다.

	@author	BHK
*/
static PyObject* func_AppendLogToOutputWnd(PyObject* self, PyObject* args)
{
	CStringList strList;

	if( args )
	{
		ParsePyArg(args , strList);
		Py_DECREF(args);
		if(1 == (strList.GetSize()))
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			string rLog  = strList.GetAt(pos);

			CMainFrame* pMainFrm = ELOADGetMainWnd();
			if(pMainFrm)
			{
				pMainFrm->m_wndOutput.AppendLogMsg(rLog);
			}
		}
	}
	
	return Py_ReturnNone();
}

/**
	@brief	

	@author BHK	

	@date 2009-03-31 오전 10:34:39	

	@param	

	@return		
*/
static PyObject* func_ELOAD_LOG4CXX(PyObject* self, PyObject* args)
{
	CStringList strList;

	if( args )
	{
		ParsePyArg(args , strList);
		Py_DECREF(args);
		if(3 == (strList.GetSize()))
		{
			int nPos = 0;
			POSITION pos = strList.FindIndex(0);
			const string rErrorType  = strList.GetAt(pos);
			pos = strList.FindIndex(1);
			const string rItemId = strList.GetAt(pos);
			pos = strList.FindIndex(2);
			const int nErrorCode = atoi(strList.GetAt(pos));

			if(_T("INFO") == rErrorType)
			{
				ELOAD_LOG4CXX_INFO(mylogger , rItemId , nErrorCode);
			}
			else if(_T("WARN") == rErrorType)
			{
				ELOAD_LOG4CXX_WARN(mylogger , rItemId , nErrorCode);
			}
			else if(_T("ERROR") == rErrorType)
			{
				ELOAD_LOG4CXX_ERROR(mylogger , rItemId , nErrorCode);
			}
			else if(_T("FATAL") == rErrorType)
			{
				ELOAD_LOG4CXX_FATAL(mylogger , rItemId , nErrorCode);
			}
		}
	}
	
	return Py_ReturnNone();
}

/**
	@brief	outputwnd를 clear한다.

	@author	BHK
*/
static PyObject* func_ClearOutputWnd(PyObject* self, PyObject* args)
{
	CStringList strList;

	if( args )
	{
		ParsePyArg(args , strList);
		Py_DECREF(args);

		CMainFrame* pMainFrm = ELOADGetMainWnd();
		if(pMainFrm) pMainFrm->m_wndOutput.ClearLogWnd();
	}
	
	return Py_ReturnNone();
}

// Definition der Methoden, die cfrompy exportiert
PyMethodDef ELoadApp_methods[] =
{   
	{ "GetExecPath"				, func_GetExecPath				, METH_VARARGS , NULL},
	{ "GetProjectNo"			, func_GetProjectNo				, METH_VARARGS , NULL},
	{ "GetProjectName"			, func_GetProjectName				, METH_VARARGS , NULL},
	{ "GetProjectDesc"			, func_GetProjectDesc				, METH_VARARGS , NULL},
	{ "GetProjectClient"			, func_GetProjectClient				, METH_VARARGS , NULL},
	{ "GetProjectFolder"			, func_GetProjectFolderPath			, METH_VARARGS , NULL},
	{ "GetDocumentNo"			, func_GetDocumentNo				, METH_VARARGS , NULL},
	{ "GetFieldValueInProjectSettingTable"	, func_GetFieldValueInProjectSettingTable	, METH_VARARGS , NULL},

	{ "CreateBus"				, func_CreateBus				, METH_VARARGS , NULL},
	{ "GetBusGroupNameList"			, func_GetBusGroupNameList			, METH_VARARGS , NULL},
	{ "GetBusGroupNameHasBus"		, func_GetBusGroupNameHasBus			, METH_VARARGS , NULL},
	{ "GetBusNameList"			, func_GetBusNameList				, METH_VARARGS , NULL},
	{ "GetPDBBusNameList"			, func_GetPBDBusNameList			, METH_VARARGS , NULL},
	{ "GetBusNameListInBusGroup"		, func_GetBusNameListInBusGroup			, METH_VARARGS , NULL},
	{ "GetBusProp"				, func_GetBusProp				, METH_VARARGS , NULL},
	{ "GetLoadSummaryInBus"			, func_GetLoadSummaryInBus			, METH_VARARGS , NULL},

	{ "GetTransformerNameList"		, func_GetTransformerNameList			, METH_VARARGS , NULL},
	{ "SetTransformerProp"			, func_SetTransformerProp			, METH_VARARGS , NULL},
	{ "GetTransformerProp"			, func_GetTransformerProp			, METH_VARARGS , NULL},
	{ "GetLoadSummaryCalcDataOfTr"		, func_GetLoadSummaryCalcDataOfTr		, METH_VARARGS , NULL},
	{ "GetConnectedBusNameListOfTr"		, func_GetConnectedBusNameListOfTr		, METH_VARARGS , NULL},
	
	{ "GetLoadNameList"			, func_GetLoadNameList				, METH_VARARGS , NULL},
	{ "GetLoadNameListInBus"		, func_GetLoadNameListInBus			, METH_VARARGS , NULL},
	{ "GetTrNameListInBus"			, func_GetTrNameListInBus			, METH_VARARGS , NULL},
	{ "SearchLoadTable"			, func_SearchLoadTable				, METH_VARARGS , NULL},
	{ "GetLoadProp"				, func_GetLoadProp				, METH_VARARGS , NULL},
	{ "GetFLCForCable"			, func_GetFLCForCable				, METH_VARARGS , NULL},
	{ "IsLoadPropLock"			, func_IsLoadPropLock				, METH_VARARGS , NULL},
	{ "GetLoadPropUnitStr"			, func_GetLoadPropUnitStr			, METH_VARARGS , NULL},
	{ "SetLoadProp"				, func_SetLoadProp				, METH_VARARGS , NULL},
	{ "SetLoadOriginalProp"			, func_SetLoadOriginalProp			, METH_VARARGS , NULL},
	{ "GetLoadRatingCapacity"		, func_GetLoadRatingCapacity			, METH_VARARGS , NULL},
	{ "GetStartingVoltageDrop2"		, func_GetStartingVoltageDrop2			, METH_VARARGS , NULL},
	{ "GetStartingPowerFactor"		, func_GetStartingPowerFactor			, METH_VARARGS , NULL},
		
	{ "GetCableDataList"			, func_GetCableDataList				, METH_VARARGS , NULL},
	{ "GetCableDataSizeList"		, func_GetCableDataSizeList			, METH_VARARGS , NULL},
	{ "GetCableDataDiaList"			, func_GetCableDataDiaList			, METH_VARARGS , NULL},
	{ "GetCableDataAllowanceCurrentList"	, func_GetCableDataAllowanceCurrentList		, METH_VARARGS , NULL},
	{ "GetCableDataRegistanceList"		, func_GetCableDataRegistanceList		, METH_VARARGS , NULL},
	{ "GetCableDataReactanceList"		, func_GetCableDataReactanceList		, METH_VARARGS , NULL},

	{ "GetCableLengthConduitValueList"	, func_GetCableLengthConduitValueList		, METH_VARARGS , NULL},
	{ "GetCableLengthKWList"		, func_GetCableLengthKWList			, METH_VARARGS , NULL},
	{ "GetCableLengthPFList"		, func_GetCableLengthPFList			, METH_VARARGS , NULL},
	{ "GetCableLengthFLCList"		, func_GetCableLengthFLCList			, METH_VARARGS , NULL},
	{ "GetCableLengthDataSheetRowCount"	, func_GetCableLengthDataSheetRowCount		, METH_VARARGS , NULL},
	{ "GetCableLengthDataSheetColCount"	, func_GetCableLengthDataSheetColCount		, METH_VARARGS , NULL},
	{ "GetCableLengthDataSheetValue"	, func_GetCableLengthDataSheetValue		, METH_VARARGS , NULL},

	{ "GetRouteNameList"			, func_GetRouteNameList				, METH_VARARGS , NULL},
	
	{ "GetELoadItemListOf"			, func_GetELoadItemListOf			, METH_VARARGS , NULL},
	{ "GetELoadItemProp"			, func_GetELoadItemProp				, METH_VARARGS , NULL},
	{ "SetELoadItemProp"			, func_SetELoadItemProp				, METH_VARARGS , NULL},
	{ "GetLoadSummaryCalcDataOfBus"		, func_GetLoadSummaryCalcDataOfBus		, METH_VARARGS , NULL},
	{ "GetLoadSummaryCalcDataOfUPSDC"	, func_GetLoadSummaryCalcDataOfUPSDC		, METH_VARARGS , NULL},
	
	{ "ELOAD_LOG4CXX"			, func_ELOAD_LOG4CXX				, METH_VARARGS , NULL},
	{ "AppendLogToOutputWnd"		, func_AppendLogToOutputWnd			, METH_VARARGS , NULL},
	{ "ClearOutputWnd"			, func_ClearOutputWnd				, METH_VARARGS , NULL},

	{ NULL, NULL, 0, 0 } /* end of list */
};