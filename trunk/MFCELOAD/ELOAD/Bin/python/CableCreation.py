#! -*- Encoding: Latin-1 -*-

import ELoadApp
import sys
import os
import locale
import math
import win32com.client
import time
import traceback
import string


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

# brief		check load property before power cable creation.
# author	BHK
# return	'SUCCESS' or 'FAIL'
def PyCheckLoadPropForPowerCableCreation(argv):
	res = 'SUCCESS'

	if (type(argv) == type([])) and (1 == len(argv)):
		LoadCategory = ELoadApp.GetLoadProp(argv[0] , 'Type' , 'Load Category')
		if ('' == LoadCategory) or (('MOTOR' != LoadCategory) and ('LTG' != LoadCategory) and ('WELDING' != LoadCategory) and ('INST.' != LoadCategory) and ('COMM.' != LoadCategory) and ('OTHERS' != LoadCategory) and ('SubBUS' != LoadCategory)):
			ELoadApp.ELOAD_LOG4CXX(['ERROR' , argv[0] , 10])
			return 'FAIL'

		value = ELoadApp.GetLoadProp(argv[0] , 'Cable Route' , 'Design Length')
		if '' == value:
			ELoadApp.ELOAD_LOG4CXX(['ERROR' , argv[0] , 16])
			#res = 'FAIL'
			return 'FAIL'

		UseRealKVA = ELoadApp.GetLoadProp(argv[0] , 'Panel' , 'Real KVA For Power Cable')
		if ('SubBUS' == LoadCategory) and ('YES' == UseRealKVA.upper()):
			# Load Category°¡ SubBUSÀÌ°í Real KVA For Power CableÀÇ °ªÀÌ YESÀÏ¶§¿¡´Â Connect Bus°ªÀÌ ¼³Á¤µÇ¾î¾ß ÇÑ´Ù.
			value = ELoadApp.GetLoadProp(argv[0] , 'Panel' , 'Connect Bus')
			if '' == value:
				ELoadApp.ELOAD_LOG4CXX(['ERROR' , argv[0] , 24])
				#res = 'FAIL'
				return 'FAIL'
		else:
			# Load Category°¡ SubBUS°¡ ¾Æ´Ï°Å³ª Real KVA For Power CableÀÇ °ªÀÌ YES°¡ ¾Æ´Ò¶§¿¡´Â FLC°ªÀÌ ÀÖ¾î¾ß ÇÑ´Ù.
			value = ELoadApp.GetLoadProp(argv[0] , 'Rating' , 'FLC')
			if ('' == value) or ('0' == value):
				ELoadApp.ELOAD_LOG4CXX(['ERROR' , argv[0] , 17])
#				res = 'FAIL'
				return 'FAIL'

		#return 'SUCCESS'
	return 'SUCCESS'

# brief		Minimum Power Cable Size¸¦ Ã£½À´Ï´Ù.
# date		2009-04-23 13:24:48 
#		2009-08-11 10:45:26 : ÀÎÀÚ ¼öÁ¤.
# author	BHK
def PyFindMinPowerCableSize(argv):
	if (type(argv) == type([])) and (7 == len(argv)):
		Install = argv[0]
		TotalDF = SAFE_FLOAT(argv[1])
		FLC = SAFE_FLOAT(argv[2])
		SF = SAFE_FLOAT(argv[3])
		UGCableAmbTemp = argv[4]
		AGCableAmbTemp = argv[5]
		NoOfCable = SAFE_FLOAT(argv[6])
		CableAmbTemp = 0	

		try:
			if 'A/G' == Install:
				CableAmbTemp = SAFE_FLOAT(AGCableAmbTemp)
			elif 'U/G' == Install:
				CableAmbTemp = SAFE_FLOAT(UGCableAmbTemp)
			else:
				ELoadApp.AppendLogToOutputWnd(loadname + ': Install Value Error')
				return 'FAIL'

			if (FLC*SF) < (CableAmbTemp*TotalDF*NoOfCable):
				return 'SUCCESS'
			else:
				return 'FAIL'
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
	
	return 'FAIL'

# brief		Á¶°Ç¿¡ ¸Â´Â Power Cable Size¸¦ Ã£½À´Ï´Ù.
# date		2009-04-23 13:27:34 
# author	BHK
def PyFindPowerCableSize(argv):
	if (type(argv) == type([])) and (9 == len(argv)):
		ID            = argv[0]
		Phase         = argv[1]
		L             = SAFE_FLOAT(argv[2])
		R             = SAFE_FLOAT(argv[3])
		X             = SAFE_FLOAT(argv[4])
		PF            = argv[5]
		I             = round(SAFE_FLOAT(argv[6]),2)
		MotorVD       = SAFE_FLOAT(argv[7])
		MULTIPLY      = SAFE_FLOAT(argv[8])

		try:
			COS = SAFE_FLOAT(PF)
			ELoadApp.AppendLogToOutputWnd('COS : ' + str(COS))
			#Phase = ELoadApp.GetLoadProp(loadname , 'Rating' , 'Phase')
			#L   = SAFE_FLOAT(ELoadApp.GetLoadProp(loadname , 'Cable Route'  , 'Design Length'))
			#I   = SAFE_FLOAT(I) #ELoadApp.GetLoadProp(loadname , 'Voltage Drop' , 'SF For FLC'))
			#R   = SAFE_FLOAT(R)
			#X   = SAFE_FLOAT(X)
			if '3' == Phase:	# 3PH MOTOR or 3PH FEEDER
				SIN = math.sqrt(1 - math.pow(COS,2))
				VD = (L*math.sqrt(3)*I*(R*COS + X*SIN))/1000.
			elif '1' == Phase:	# 1PH FEEDER or 1PH MOTOR
				SIN = math.sqrt(1 - math.pow(COS,2))		
				VD = (L*2*I*(R*COS + X*SIN))/1000.
			elif 'DC' == Phase:	# DC FEEDER
				VD = (L*2*I*R)/1000.
			else:
				ELoadApp.AppendLogToOutputWnd(ID + ': Phase Value Error')
				return ['FAIL',0.]
	
			VD = VD/MULTIPLY
			if VD <= (MotorVD):
				VD = round(VD,2)
				return ['SUCCESS',VD]
			else:
				VD = round(VD,2)
				return ['FAIL',VD]
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

	return ['FAIL',0.]

# brief		Control cable result ¸¦ Excel¿¡ ¾´´Ù.
# date		2009-06-10 09:55:34 
# author	KHS
# param		args[0] = template exel file path, args[1] = output excel file path, args[2] = FolderPath
def PyWriteControlCableResultToExcel(args):
	if type(args) != type([]) or len(args) != 3:
		ELoadApp.AppendLogToOutputWnd('arguments error')
		return

	xl = win32com.client.gencache.EnsureDispatch ('Excel.Application')
	wb = xl.Workbooks.Open(args[0])
	ws = wb.ActiveSheet
	xl.Visible = False	# º¸ÀÌÁö ¾Ê°Ô ÇÑ´Ù.

	FolderPath = args[2]
	# TOTAL DICTIONARY
	TOTAL = {}
	PROJECT_FOLDER = ELoadApp.GetProjectFolder()
	projectno = ELoadApp.GetProjectNo()
	projectname = ELoadApp.GetProjectName()
	documentno = ELoadApp.GetDocumentNo()

	BusNameList = ELoadApp.GetBusNameList('ALL')

	for bus in BusNameList:
		busgroup = ELoadApp.GetBusProp(bus,'General','Bus Group')
		#if busgroup == 'INCOMING':
		#	continue
		try:
			wb.Worksheets('CONTROL_CABLE').Copy(Before=wb.Worksheets(wb.Worksheets.Count))
			ws = wb.ActiveSheet
			ws.Name = bus
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
	
		try:
			row = 11
			controlcablefile = open(FolderPath + '\\' + bus + '.BUS', 'r')
		
			buffer = controlcablefile.readlines()
			for index in range(1,len(buffer)):
				ws.Range('A'+str(row + 1) +':N'+str(row + 1)).Insert()
				row = row + 1
			
			row = 11
			for line in buffer:
				list = line.rstrip('\n').split('\t')
				col = 1
				for item in list:
					if (len(item) >= 1) and ('*' == item[len(item) - 1]):
						# it is modified item. so filled with red.
						ws.Cells(row,col).Value = item[0:len(item)-1]
						ws.Cells(row,col).Interior.ColorIndex = 44
					else:
						ws.Cells(row,col).Value = item
					col = col + 1
				row = row + 1
			
			BusVolt = ELoadApp.GetBusProp(bus,'General','Bus Voltage')
			ws.Cells(1,4).Value = projectno
			ws.Cells(2,4).Value = projectname
			ws.Cells(3,4).Value = ELoadApp.GetProjectClient()
			ws.Cells(5,4).Value = bus
			ws.Cells(5,6).Value = BusVolt
			# ÇöÀç ³¯Â¥¸¦ Ç¥±âÇÑ´Ù.
			now = time.localtime()
			ws.Cells(2,9).Value = '%d' % now.tm_year + '.' + '%d' % now.tm_mon + '.' + '%d' % now.tm_mday
			ws.Cells(3,9).Value = documentno
			ws.Columns('A:A').EntireColumn.AutoFit()
			controlcablefile.close()


		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	try:
		# TEMPLATE SHEET¸¦ »èÁ¦ÇÕ´Ï´Ù.
		xl.DisplayAlerts = False
		wb.Worksheets("CONTROL_CABLE").Delete()

		# SAVE AND ¿¢¼¿ APP Á¾·á.	
		wb.SaveAs(args[1])
		xl.Visible = True
		wb.Close()
		xl.DisplayAlerts = True
		#xl.Quit()

		os.startfile(args[1])
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	return 'SUCCESS'

# brief		Control cable SettingÀ» Excel¿¡ ¾´´Ù. 
# author	KHS
def PyWriteControlCableSettingToExcel(args):
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


	try:
		wb.Worksheets('CONTROL_CABLE_SETTING').Copy(Before=wb.Worksheets(wb.Worksheets.Count))
		ws = wb.ActiveSheet
		ws.Name = 'CONTROL CABLE SETTING'
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
	
	try:
		row = 7
		controlcablefile = open(PROJECT_FOLDER + 'Working\\CONTROL_CABLE_SETTING.TXT', 'r')
		
		for line in controlcablefile.readlines():
			ws.Range('A'+str(row+1) +':G'+str(row+1)).Insert()
			list = line.split('\t')
			col = 1
			for item in list:
				if -1 != item.find('@'):
					item = item[0:len(item) - 1]
					ws.Cells(row,col).Interior.ColorIndex = 6
				ws.Cells(row,col).Value = item
				col = col + 1
			row = row + 1

		ws.Columns('A:A').EntireColumn.AutoFit()
		controlcablefile.close()


	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	try:
		# TEMPLATE SHEET¸¦ »èÁ¦ÇÕ´Ï´Ù.
		xl.DisplayAlerts = False
		wb.Worksheets("CONTROL_CABLE_SETTING").Delete()

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


# brief		data fileÀÇ ³»¿ëÀ» template excel¿¡ ¾´´Ù. 
# author	khs,bhk
def PyWriteCableScheduleResultToExcel(args):
	if type(args) != type([]) or len(args) != 2:
		ELoadApp.AppendLogToOutputWnd('arguments error')
		return

	xl = win32com.client.gencache.EnsureDispatch ('Excel.Application')
	wb = xl.Workbooks.Open(args[0])
	ws = wb.ActiveSheet
	xl.Visible = False	# º¸ÀÌÁö ¾Ê°Ô ÇÑ´Ù.

	# total dictionary
	total = {}
	PROJECT_FOLDER = ELoadApp.GetProjectFolder()
	projectno = ELoadApp.GetProjectNo()
	projectname = ELoadApp.GetProjectName()
	documentno = ELoadApp.GetDocumentNo()
	
	try:
		wb.Worksheets('CABLE').Copy(Before=wb.Worksheets(wb.Worksheets.Count))
		ws = wb.ActiveSheet
		ws.Name = 'Cable Schedule'
		
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
	try:
		datafile = open(PROJECT_FOLDER + 'Cable Schedule Result\\CableScheduleResult.txt', 'r')
		buffer = datafile.readlines()
		row = 12
		
		for index in range(1,len(buffer) - 1):
			ws.Range('A'+str(row + 1) +':Q'+str(row + 1)).Insert()
			row = row + 1
		
		row = 11
		for line in buffer:
			list = line.rstrip('\n').split('\t')
			col = 2
			for item in list:
				if (len(item) >= 1) and ('*' == item[len(item) - 1]):
					# it is modified item. so filled with red.
					ws.Cells(row,col).Value = item[0:len(item)-1]
					ws.Cells(row,col).Interior.ColorIndex = 44
				else:
					ws.Cells(row,col).Value = item
				col = col + 1
			row = row + 1
		
		ws.Cells(3,1).Value = 'PROJECT : ' + projectno
		ws.Cells(4,1).Value = 'PROJECT NAME : ' + projectname
		# ÇöÀç ³¯Â¥¸¦ Ç¥±âÇÑ´Ù.
		now = time.localtime()
		ws.Cells(4,16).Value = 'DATE : ' + '%d' % now.tm_year + '.' + '%d' % now.tm_mon + '.' + '%d' % now.tm_mday
		
		datafile.close()

		ws.Columns('A:A').EntireColumn.AutoFit()

	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	try:
		xl.DisplayAlerts = False
		wb.Worksheets("CABLE").Delete()

		# save and ¿¢¼¿ app Á¾·á.	
		wb.SaveAs(args[1])
		xl.Visible = True
		wb.Close()
		xl.DisplayAlerts = True
		#xl.Quit()

		os.startfile(args[1])
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	return 'success'

# brief		Total Drum fileÀÇ ³»¿ëÀ» template Excel¿¡ ¾´´Ù. 
# author	KHS,BHK
def PyWriteTotalDrumScheduleListToExcel(wb, FolderPath):

	PROJECT_FOLDER = ELoadApp.GetProjectFolder()
	projectno = ELoadApp.GetProjectNo()
	projectname = ELoadApp.GetProjectName()
	documentno = ELoadApp.GetDocumentNo()
	try:
		wb.Worksheets('DRUM TOTAL TEMP').Copy(Before=wb.Worksheets(wb.Worksheets.Count))
		ws = wb.ActiveSheet
		ws.Name = 'TOTAL-DRUM'
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())	
	try:
		drumscheduleresultfile = open(FolderPath + '\\Total_DrumScheduleResult.txt', 'r')
		buffer = drumscheduleresultfile.readlines()

		row = 10
		for index in range(1,len(buffer) - 1):
			ws.Range('A'+str(row + 1) +':I'+str(row + 1)).Insert()
			row = row + 1
			
		row = 9
		for line in buffer:
			list = line.rstrip('\n').split('\t')
			col = 1
			for item in list:
				ws.Cells(row,col).Value = item
				col = col + 1
			row = row + 1
		

		ws.Cells(3,1).Value = 'PROJECT : ' + projectno
		ws.Cells(4,1).Value = 'PROJECT NAME : ' + projectname
		# ÇöÀç ³¯Â¥¸¦ Ç¥±âÇÑ´Ù.
		now = time.localtime()
		ws.Cells(4,9).Value = 'DATE : ' + '%d' % now.tm_year + '.' + '%d' % now.tm_mon + '.' + '%d' % now.tm_mday
		
		ws.Columns('A:A').EntireColumn.AutoFit()
		
		drumscheduleresultfile.close()

		ELoadApp.AppendLogToOutputWnd('Total book End')

	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

	return 'SUCCESS'

# brief		Total Drum fileÀÇ ³»¿ëÀ» template Excel¿¡ ¾´´Ù. 
# author	KHS,BHK
def PyWriteDrumListToExcel(wb, FolderPath):

	PROJECT_FOLDER = ELoadApp.GetProjectFolder()
	projectno = ELoadApp.GetProjectNo()
	projectname = ELoadApp.GetProjectName()
	documentno = ELoadApp.GetDocumentNo()

	try:
		wb.Worksheets('DRUM TEMP').Copy(Before=wb.Worksheets(wb.Worksheets.Count))
		ws = wb.ActiveSheet
		ws.Name = 'DRUM'
		
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())	
	try:
		drumscheduleresultfile = open(FolderPath + '\\DrumScheduleResult.txt', 'r')
		buffer = drumscheduleresultfile.readlines()

		row = 10
		for index in range(1,len(buffer) - 1):
			ws.Range('A'+str(row + 1) +':M'+str(row + 1)).Insert()
			row = row + 1
			
		row = 9
		for line in buffer:
			list = line.rstrip('\n').split('\t')
			col = 1
			for item in list:
				ws.Cells(row,col).Value = item
				col = col + 1
			row = row + 1

		ws.Cells(3,1).Value = 'PROJECT : ' + projectno
		ws.Cells(4,1).Value = 'PROJECT NAME : ' + projectname
	
		# ÇöÀç ³¯Â¥¸¦ Ç¥±âÇÑ´Ù.
		now = time.localtime()
		ws.Cells(4,12).Value = 'DATE : ' + '%d' % now.tm_year + '.' + '%d' % now.tm_mon + '.' + '%d' % now.tm_mday

		ws.Columns('A:A').EntireColumn.AutoFit()
		drumscheduleresultfile.close()

	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

	

	return 'SUCCESS'

# brief		data fileÀÇ ³»¿ëÀ» template Excel¿¡ ¾´´Ù. 
# author	KHS,BHK
def PyWriteCableListDrumScheduleToExcel(args):
	if type(args) != type([]) or len(args) != 3:
		ELoadApp.AppendLogToOutputWnd('arguments error')
		return
	
	xl = win32com.client.gencache.EnsureDispatch ('Excel.Application')
	wb = xl.Workbooks.Open(args[0])
	ws = wb.ActiveSheet
	xl.Visible = False	# º¸ÀÌÁö ¾Ê°Ô ÇÑ´Ù.

	FolderPath = args[2]
	# TOTAL DICTIONARY
	TOTAL = {}
	PROJECT_FOLDER = ELoadApp.GetProjectFolder()
	projectno = ELoadApp.GetProjectNo()
	projectname = ELoadApp.GetProjectName()
	documentno = ELoadApp.GetDocumentNo()

	try:
		wb.Worksheets('CABLE TOTAL TEMP').Copy(Before=wb.Worksheets(wb.Worksheets.Count))
		ws = wb.ActiveSheet
		ws.Name = 'TOTAL-CABLE'
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())	
	try:
		drumscheduleresultfile = open(FolderPath + '\\Total_Drum_Cable_Result.txt', 'r')
		buffer = drumscheduleresultfile.readlines()

		row = 10
		for index in range(1,len(buffer) - 1):
			ws.Range('A'+str(row + 1) +':H'+str(row + 1)).Insert()
			row = row + 1
			
		row = 9
		for line in buffer:
			list = line.rstrip('\n').split('\t')
			col = 1
			for item in list:
				ws.Cells(row,col).Value = item
				col = col + 1
			row = row + 1
		

		ws.Cells(3,1).Value = 'PROJECT : ' + projectno
		ws.Cells(4,1).Value = 'PROJECT NAME : ' + projectname
		# ÇöÀç ³¯Â¥¸¦ Ç¥±âÇÑ´Ù.
		now = time.localtime()
		ws.Cells(4,8).Value = 'DATE : ' + '%d' % now.tm_year + '.' + '%d' % now.tm_mon + '.' + '%d' % now.tm_mday
		
		ws.Columns('A:A').EntireColumn.AutoFit()
		
		drumscheduleresultfile.close()

		# TOTAL DRUM
		PyWriteTotalDrumScheduleListToExcel(wb, FolderPath)
		PyWriteDrumListToExcel(wb, FolderPath)
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

	try:
		xl.DisplayAlerts = False
		wb.Worksheets("DRUM TEMP").Delete()
		wb.Worksheets("DRUM TOTAL TEMP").Delete()
		wb.Worksheets("CABLE TOTAL TEMP").Delete()

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


# brief		data fileÀÇ ³»¿ëÀ» template excel¿¡ ¾´´Ù. 
# author	khs,bhk
def pywritedatafiletoexcel(args):
	if type(args) != type([]) or len(args) != 7:
		eloadapp.appendlogtooutputwnd('arguments error')
		return

	xl = win32com.client.gencache.ensuredispatch ('excel.application')
	wb = xl.workbooks.open(args[0])
	ws = wb.activesheet
	xl.visible = false	# º¸ÀÌÁö ¾Ê°Ô ÇÑ´Ù.

	# total dictionary
	total = {}
	project_folder = eloadapp.getprojectfolder()
	projectno = eloadapp.getprojectno()
	projectname = eloadapp.getprojectname()
	documentno = eloadapp.getdocumentno()

	
	try:
		row = int(args[1])
		datafile = open(args[6] , 'r')
		
		for line in datafile.readlines():
			ws.range('a'+str(row+1) +':z'+str(row+1)).insert()
			list = line.rstrip('\n').split('\t')
			col = 1
			for item in list:
				ws.cells(row,col).value = item
				col = col + 1
			row = row + 1
		
		ws.range(args[2]).value = projectno
		ws.range(args[3]).value = projectname
		ws.range(args[4]).value = eloadapp.getprojectclient()
		# ÇöÀç ³¯Â¥¸¦ Ç¥±âÇÑ´Ù.pywritedrumschedulelisttoexcel
		now = time.localtime()
		ws.range(args[5]).value = '%d' % now.tm_year + '.' + '%d' % now.tm_mon + '.' + '%d' % now.tm_mday
		#ws.columns('a:a').entirecolumn.autofit()
		datafile.close()

	except:
		eloadapp.appendlogtooutputwnd(traceback.format_exc())


	try:
		xl.displayalerts = false

		# save and ¿¢¼¿ app Á¾·á.	
		wb.save()
		xl.visible = true
		wb.close()
		xl.displayalerts = true
	#	xl.quit()

		os.startfile(args[0])
	except:
		eloadapp.appendlogtooutputwnd(traceback.format_exc())


	return 'success'

