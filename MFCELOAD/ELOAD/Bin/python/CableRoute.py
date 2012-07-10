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

# brief		float·Î Çüº¯È¯À» ÇÑ´Ù. Çü º¯È¯½Ã¿¡ ¿¡·¯°¡ ¹ß»ýÇÏ¸é 0¸¦ ¸®ÅÏÇÑ´Ù.
# author	BHK
def SAFE_FLOAT(arg):
	try:
		res = float(arg)
		return res
	except ValueError:
		return 0

	return 0


# brief 	CABLE ROUTEÀÇ CABLE LIST¸¦ ¿¢¼¿ÆÄÀÏ·Î ÀúÀåÇÑ´Ù.
# author	KHS
# param		args[0] = src excel file , args[1] = dest excel file , args[2] = continuous cf , args[3] = Intermittent cf , args[4] = stand-by cf
def PyWriteRouoteCableListToExcel(args):
	if type(args) != type([]) or len(args) != 2:
		ELoadApp.AppendLogToOutputWnd('arguments error')
		return

	xl = win32com.client.gencache.EnsureDispatch ('Excel.Application')
	wb = xl.Workbooks.Open(args[0])
	ws = wb.ActiveSheet
	xl.Visible = False	# º¸ÀÌÁö ¾Ê°Ô ÇÑ´Ù.

	# TOTAL DICTIONARY
	TOTAL = {}
	PROJECT_FOLDER = ELoadApp.GetProjectFolder()
	projectno = ELoadApp.GetProjectNo()
	projectname = ELoadApp.GetProjectName()
	documentno = ELoadApp.GetDocumentNo()

	# ¸ÕÀú BUSÀÇ ÀÌ¸§µéÀ» °¡Á®¿Â´Ù.
	RouteNameList = ELoadApp.GetRouteNameList()
	for Route in RouteNameList:
		# bus ÀÌ¸§À¸·Î SHEET¸¦ ¸¸µé¾î ¸Ç ¸¶Áö¸·¿¡ ºÙÀÎ´Ù.
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
			# ÇöÀç ³¯Â¥¸¦ Ç¥±âÇÑ´Ù.
			now = time.localtime()
			ws.Cells(2,9).Value = '%d' % now.tm_year + '.' + '%d' % now.tm_mon + '.' + '%d' % now.tm_mday
			ws.Cells(3,9).Value = documentno

			ws.Columns('A:A').EntireColumn.AutoFit()
			routefile.close()


		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	try:
		# TEMPLATE SHEET¸¦ »èÁ¦ÇÕ´Ï´Ù.
		xl.DisplayAlerts = False
	        wb.Worksheets("ROUTE").Delete()

		# SAVE AND ¿¢¼¿ APP Á¾·á.	
		wb.SaveAs(args[1])
		xl.Visible = True
		wb.Close()
		xl.DisplayAlerts = True
	#	xl.Quit()

		os.startfile(args[1])
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	return 'SUCCESS'

