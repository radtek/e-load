#! -*- Encoding: Latin-1 -*-

import ELoadApp
import sys
import os
import locale
import math
import win32com.client
import time
import traceback

from math import *

locale.setlocale(locale.LC_ALL, "")

def SAFE_ROUND(value,ndigits):
	return round(float(value),ndigits)

# brief		float로 형변환을 한다. 형 변환시에 에러가 발생하면 0를 리턴한다.
# author	BHK
def SAFE_FLOAT(arg):
	try:
		res = float(arg)
		return res
	except ValueError:
		return 0

	return 0


# brief 	CABLE ROUTE의 CABLE LIST를 엑셀파일로 저장한다.
# author	KHS
# param		args[0] = src excel file , args[1] = dest excel file , args[2] = continuous cf , args[3] = Intermittent cf , args[4] = stand-by cf
def PyWriteRouoteCableListToExcel(args):
	if type(args) != type([]) or len(args) != 2:
		ELoadApp.AppendLogToOutputWnd('arguments error')
		return

	xl = win32com.client.gencache.EnsureDispatch ('Excel.Application')
	wb = xl.Workbooks.Open(args[0])
	ws = wb.ActiveSheet
	xl.Visible = False	# 보이지 않게 한다.

	# TOTAL DICTIONARY
	TOTAL = {}
	PROJECT_FOLDER = ELoadApp.GetProjectFolder()
	projectno = ELoadApp.GetProjectNo()
	projectname = ELoadApp.GetProjectName()
	documentno = ELoadApp.GetDocumentNo()

	# 먼저 BUS의 이름들을 가져온다.
	RouteNameList = ELoadApp.GetRouteNameList()
	for Route in RouteNameList:
		# bus 이름으로 SHEET를 만들어 맨 마지막에 붙인다.
		try:
			wb.Worksheets('ROUTE').Copy(Before=wb.Worksheets(wb.Worksheets.Count))
			ws = wb.ActiveSheet
			ws.Name = Route
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
	
		try:
			row = 11
			routefile = open(PROJECT_FOLDER + 'Working\\' + Route + '.txt', 'r')
		
			for line in routefile.readlines():
				list = line.split('\t')
				col = 1
				for item in list:
					ws.Cells(row,col).Value = item
					col = col + 1
				row = row + 1
				if  line.find('TOTAL') == -1 and line != '\n':
					ws.Range('A'+str(row) +':R'+str(row)).Insert()

			ws.Cells(5,5).Value = Route
			ws.Cells(1,1).Value = projectno
			ws.Cells(2,1).Value = projectname
			ws.Cells(3,1).Value = ELoadApp.GetProjectClient()
			# 현재 날짜를 표기한다.
			now = time.localtime()
			ws.Cells(2,9).Value = '%d' % now.tm_year + '.' + '%d' % now.tm_mon + '.' + '%d' % now.tm_mday
			ws.Cells(3,9).Value = documentno

			ws.Columns('A:A').EntireColumn.AutoFit()
			routefile.close()


		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	try:
		# TEMPLATE SHEET를 삭제합니다.
		xl.DisplayAlerts = False
	        wb.Worksheets("ROUTE").Delete()

		# SAVE AND 엑셀 APP 종료.	
		wb.SaveAs(args[1])
		xl.Visible = True
		wb.Close()
		xl.DisplayAlerts = True
	#	xl.Quit()

		os.startfile(args[1])
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	return 'SUCCESS'

