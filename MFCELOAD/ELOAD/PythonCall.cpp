#include "stdafx.h"

#ifdef _DEBUG
#undef _DEBUG
#include <python.h>
#define _DEBUG
#else
#include <python.h>
#endif

#include "pyfromc.h"
#include "PythonCall.h"
extern int ParsePyArg(PyObject* arg , CStringList& list);

PyObject* Py_ReturnNone();

PythonCall::PythonCall( PyObject* pModule/*Python* pThis*/, LPCTSTR lpszName, DWORD dwArgs )
    :   m_pyFunc( NULL ), m_pyArgs( NULL ) , m_dwArgs(dwArgs)
        
{
	m_strName = lpszName;
	m_GIL = PyGILState_Ensure();
	m_pyFunc = PyDict_GetItemString(PyModule_GetDict(pModule/*pThis->m_pyMain*/), lpszName);
	/* pFun: Borrowed reference */

	if(m_pyFunc && PyCallable_Check(m_pyFunc))
	{
		m_pyArgs = PyTuple_New(m_dwArgs);
	}
	else m_pyFunc = NULL;
}

PythonCall::~PythonCall()
{   
    if( m_pyArgs )
    {
        Py_DECREF(m_pyArgs);
        m_pyArgs = NULL;
    }

    PyGILState_Release(m_GIL);

    /*
    PyObject* sysModules = PySys_GetObject("modules");
    if(NULL != sysModules) 
    {
	    PyObject* pName = PyString_FromString("ELoadExtension");
	    int ret = PyDict_DelItem(sysModules, pName);
	    // ..俊矾贸府 内靛 积帆..
    }
    */
}

void PythonCall::set( DWORD dwIndex, DWORD dwValue )
{
	if(m_pyFunc && PyCallable_Check(m_pyFunc) && (NULL == m_pyArgs))
	{
		m_pyArgs = PyTuple_New(m_dwArgs);
	}

	if( m_pyArgs )
	{
		// todo: handle LONG/ULONG situations ...
		PyTuple_SetItem(m_pyArgs, dwIndex, PyInt_FromLong(dwValue));
	}
}

void PythonCall::set( DWORD dwIndex, CStringList& source )
{
	if(m_pyFunc && PyCallable_Check(m_pyFunc) && (NULL == m_pyArgs))
	{
		m_pyArgs = PyTuple_New(m_dwArgs);
	}

	if( m_pyArgs )
	{
		PyObject* l = PyList_New( source.GetCount() );
		if( l != NULL )
		{
			POSITION pos = source.GetHeadPosition();
			int nItem = 0;
			while( pos )
			{
				PyList_SET_ITEM( l, nItem++, PyString_FromString( source.GetNext( pos ) ) );
			}

			PyTuple_SetItem(m_pyArgs, dwIndex, l);
		}
	}
}

void PythonCall::set( DWORD dwIndex, LPBYTE lpbMemory, DWORD dwSize )
{
	if(m_pyFunc && PyCallable_Check(m_pyFunc) && (NULL == m_pyArgs))
	{
		m_pyArgs = PyTuple_New(m_dwArgs);
	}

	if( m_pyArgs )
	{
		// todo: handle LONG/ULONG situations ...
		PyTuple_SetItem(m_pyArgs, dwIndex, PyString_FromStringAndSize((const char*) lpbMemory, dwSize));
	}
}

void PythonCall::set( DWORD dwIndex, LPCSTR lpszText )
{
	if(m_pyFunc && PyCallable_Check(m_pyFunc) && (NULL == m_pyArgs))
	{
		m_pyArgs = PyTuple_New(m_dwArgs);
	}

	if( m_pyArgs )
	{
		if( lpszText == NULL )
		{
			PyTuple_SetItem(m_pyArgs, dwIndex, Py_ReturnNone());
		}
		else
		{
			PyTuple_SetItem(m_pyArgs, dwIndex, PyString_FromString(lpszText));
		}
	}
}

void PythonCall::invoke(CStringList& list)
{
	if( m_pyFunc && m_pyArgs )
	{
		PyObject* pyValue;

		try
		{
			PyGILState_STATE s = PyGILState_Ensure();
			/* Perform Python actions here.  */
			pyValue = PyObject_CallObject(m_pyFunc, m_pyArgs);
			PyGILState_Release(s);

			Py_DECREF(m_pyArgs);
			m_pyArgs = NULL;
			if( pyValue )
			{
				ParsePyArg(pyValue , list);
				
				Py_DECREF(pyValue);
				pyValue = NULL;
			}
		}
		catch(...)
		{
			AfxMessageBox("ddd");
		}
	}
}

CString PythonCall::invokeString()
{
	CString result;
	if( m_pyFunc && m_pyArgs )
	{
		PyObject* pyValue;

		PyGILState_STATE s = PyGILState_Ensure();
		/* Perform Python actions here.  */
		pyValue = PyObject_CallObject(m_pyFunc, m_pyArgs);
		PyGILState_Release(s);

		Py_DECREF(m_pyArgs);
		m_pyArgs = NULL;
		if( pyValue )
		{
			if( PyString_Check(pyValue) )
				result = PyString_AsString(pyValue);
			Py_DECREF(pyValue);
		}
	}
	return result;
}

int PythonCall::invokeInt()
{
    int result = -1;
    if( m_pyFunc && m_pyArgs )
    {
        PyObject* pyValue;

        PyGILState_STATE s = PyGILState_Ensure();
        /* Perform Python actions here.  */
        pyValue = PyObject_CallObject(m_pyFunc, m_pyArgs);
        PyGILState_Release(s);

        
        Py_DECREF(m_pyArgs);
        m_pyArgs = NULL;
        if( pyValue )
        {
            if( PyInt_Check(pyValue) )
                result = PyInt_AsLong(pyValue);
            Py_DECREF(pyValue);
        }
    }
    return result;
}

double PythonCall::invokeFloat()
{
    double result = 0;
    if( m_pyFunc && m_pyArgs )
    {
        PyObject* pyValue;

        PyGILState_STATE s = PyGILState_Ensure();
        /* Perform Python actions here.  */
        pyValue = PyObject_CallObject(m_pyFunc, m_pyArgs);
        PyGILState_Release(s);

        
        Py_DECREF(m_pyArgs);
        m_pyArgs = NULL;
        if( pyValue )
        {
            if( PyFloat_Check(pyValue) )
                result = PyFloat_AsDouble(pyValue);
            Py_DECREF(pyValue);
        }
    }
    return result;
}

bool PythonCall::invokeBool()
{
    bool result = false;
    if( m_pyFunc && m_pyArgs )
    {
        PyObject* pyValue;

        PyGILState_STATE s = PyGILState_Ensure();
        /* Perform Python actions here.  */
        pyValue = PyObject_CallObject(m_pyFunc, m_pyArgs);
        PyGILState_Release(s);

        Py_DECREF(m_pyArgs);
        m_pyArgs = NULL;
        if( pyValue )
        {
            result = PyObject_IsTrue(pyValue) ? true : false;
            Py_DECREF(pyValue);
        }
    }
    return result;
}

void PythonCall::invoke()
{
    if( m_pyFunc && m_pyArgs )
    {
        PyObject* pyValue = PyObject_CallObject(m_pyFunc, m_pyArgs);
        Py_DECREF(m_pyArgs);
        m_pyArgs = NULL;
        if (pyValue != NULL) {
            Py_DECREF(pyValue);
        }
    }
}

PyObject* PythonCall::invokeObject()
{
    if( m_pyFunc && m_pyArgs )
    {
        PyObject* pyValue = PyObject_CallObject(m_pyFunc, m_pyArgs);
        Py_DECREF(m_pyArgs);
        m_pyArgs = NULL;
        return pyValue;
    }
    return NULL;
}

