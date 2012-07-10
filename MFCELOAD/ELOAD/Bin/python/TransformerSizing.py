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

# brief		float·Î Çüº¯È¯À» ÇÑ´Ù. Çü º¯È¯½Ã¿¡ ¿¡·¯°¡ ¹ß»ýÇÏ¸é 0¸¦ ¸®ÅÏÇÑ´Ù.
# author	BHK
def SAFE_FLOAT(arg):
	try:
		res = float(arg)
		return res
	except ValueError:
		return 0

	return 0


# brief		write Transformer Sizing Result To Excel 
# author	HumKyung.BAEK
#date		2010.08.04
# param		args[0] = temlate excel file path, args[1] = output excel file path, args[2] = FilePath
def PyWriteTransformerSizingResultToExcel(args):
	if type(args) != type([]) or len(args) != 3:
		ELoadApp.AppendLogToOutputWnd('arguments error')
		return

	xl = win32com.client.gencache.EnsureDispatch ('Excel.Application')
	wb = xl.Workbooks.Open(args[0])
	ws = wb.ActiveSheet
	xl.Visible = False	# º¸ÀÌÁö ¾Ê°Ô ÇÑ´Ù.

	FilePath = args[2]
	# TOTAL DICTIONARY
	TOTAL = {}
	PROJECT_FOLDER = ELoadApp.GetProjectFolder()
	projectno = ELoadApp.GetProjectNo()
	projectname = ELoadApp.GetProjectName()

	try:
		row = 10
		ifile = open(FilePath , 'r')

		buffer = ifile.readlines()
		for index in range(1,len(buffer)):
			ws.Range('A'+str(row + 1) +':AD'+str(row + 1)).Insert()
			row = row + 1
		
		tr_no = 1
		row = 10
		for line in buffer:
			list = line.rstrip('\n').split('\t')
			col = 3

			if '' != list[0]:
				ws.Cells(row,col - 1).Value = str(tr_no)
				tr_no = tr_no + 1

			for item in list:
				if (len(item) >= 1) and ('*' == item[len(item) - 1]):
					# it is modified item. so filled with red.
					ws.Cells(row,col).Value = item[0:len(item)-1]
					ws.Cells(row,col).Interior.ColorIndex = 44 # ?
				else:
					ws.Cells(row,col).Value = item
				col = col + 1
			row = row + 1

		ws.Cells(3,1).Value = '\' PROJECT :' + projectno
		ws.Cells(4,1).Value = '\' PROJECT NAME :' + projectname
		# ÇöÀç ³¯Â¥¸¦ Ç¥±âÇÑ´Ù.
		now = time.localtime()
		ws.Cells(4,15).Value = '%d' % now.tm_year + '.' + '%d' % now.tm_mon + '.' + '%d' % now.tm_mday
			
		ws.Columns('A:A').EntireColumn.AutoFit()
		ifile.close()

	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

	try:
		# TEMPLATE SHEET¸¦ »èÁ¦ÇÕ´Ï´Ù.
		xl.DisplayAlerts = False

		# SAVE AND ¿¢¼¿ APP Á¾·á.	
		wb.SaveAs(args[1])
		xl.Visible = True
		#wb.Close()
		xl.DisplayAlerts = True
	#	xl.Quit()

	#	os.startfile(args[1])
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	return 'SUCCESS'
