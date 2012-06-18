#pragma once

#ifdef _DEBUG
#undef _DEBUG
#include <python.h>
#define _DEBUG
#else
#include <python.h>
#endif
#include "pyfromc.h"

class PythonCall
{
public:
        PythonCall( /*Python* pThis*/ PyObject* pModule, LPCTSTR lpszName, DWORD dwArgs );
        ~PythonCall();

        // Aufrufargumente setzen
        void set( DWORD dwIndex, DWORD dwValue );
        void set( DWORD dwIndex, LPBYTE lpbMemory, DWORD dwSize );
        void set( DWORD dwIndex, LPCSTR lpszText );
        void set( DWORD dwIndex, CStringList& list );

        // Aufrufe durchf?ren
	double invokeFloat();
        int invokeInt();
        bool invokeBool();
        void invoke();
        void invoke(CStringList& list);
        PyObject* invokeObject();
        CString invokeString();

    protected:
        CString m_strName;
        PyObject* m_pyFunc;
        PyObject* m_pyArgs;
        Python* m_pThis;
        PyGILState_STATE m_GIL;
	int m_dwArgs;
};