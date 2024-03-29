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

# brief		PF의 값을 구한다.
# author	BHK
# return 	PF을 리턴한다..
def PyFindPF(PFS , LF):
	COEF= [0.5,0.75,1.0]

	# 50% , 75%의 값이 모두 0이면 100%의 값을 선정한다.
	if (0. == PFS[0]) and (0. == PFS[1]):
		PF = PFS[2]
	else:
		index = 0
		PREV  = 0.0
		for x in COEF:
			if LF == x:
				PF  = PFS[index]
				break
			elif LF < x and index > 0:
				df  = (LF - PREV) / (x - PREV)
				PF  = PFS[index - 1] + (PFS[index] -  PFS[index-1]) * df
				break
			elif LF < x and index == 0:
				df  = (LF - PREV) / (x - PREV)
				PF  = (PFS[index] - 0) * df
				break
			
			PREV  = x
			index = index + 1

	PF = SAFE_ROUND(PF , 3)
	return PF
	
# brief		EFF의 값을 구한다.
# author	BHK
# return 	EFF을 리턴한다.
def PyFindEFF(EFFS , LF):
	COEF= [0.5,0.75,1.0]

	# 50% , 75%의 값이 모두 0이면 100%의 값을 선정한다.
	if (0. == EFFS[0]) and (0. == EFFS[1]):
		EFF = EFFS[2]
	else:
		index = 0
		PREV  = 0.0
		for x in COEF:
			if LF == x:
				EFF = EFFS[index]
				break
			elif LF < x and index > 0:
				df  = (LF - PREV) / (x - PREV)
				EFF = EFFS[index- 1] + (EFFS[index] - EFFS[index-1]) * df
				break
			elif LF < x and index == 0:
				df  = (LF - PREV) / (x - PREV)
				EFF = (EFFS[index] - 0) * df
				break
			
			PREV  = x
			index = index + 1

	EFF= SAFE_ROUND(EFF, 3)	
	return EFF

# author	BHK
# return	PF
def PyGetPFIfLockIsYes(loadname,PFandEFF,LF):
	# item에서 PF,EFF 값을 가져온다.(LF값을 기준으로 찾아온다.)
	PFS = [0,0,0]
	PFS[0]  = SAFE_FLOAT(ELoadApp.GetLoadProp(loadname,'Characteristic','PF(50% Load)'))
	PFS[1]  = SAFE_FLOAT(ELoadApp.GetLoadProp(loadname,'Characteristic','PF(75% Load)'))
	PFS[2]  = SAFE_FLOAT(ELoadApp.GetLoadProp(loadname,'Characteristic','PF(100% Load)'))

	# 100%의 값이 없을 경우에는 테이블에서 구한다.
	if 0. == PFS[2]:
		if type(PFandEFF) is type([]) and (len(PFandEFF) == 10):
			PFS[0] = PFandEFF[2]
			PFS[1] = PFandEFF[1]
			PFS[2] = PFandEFF[0]

			# 구한 값을 저장한다.(100%,75%,50%)
			param = [loadname,'Characteristic','PF(100% Load)','%.3f' % PFS[2]]
			ELoadApp.SetLoadProp(param)
			param[2] = 'PF(75% Load)'
			param[3] = '%.3f' % PFS[1]
			ELoadApp.SetLoadProp(param)
			param[2] = 'PF(50% Load)'
			param[3] = '%.3f' % PFS[0]
			ELoadApp.SetLoadProp(param)

	return PyFindPF(PFS , LF)

# author	BHK
# return	EFF
def PyGetEFFIfLockIsYes(loadname,PFandEFF,LF):
	EFFS= [0,0,0]
	EFFS[0] = SAFE_FLOAT(ELoadApp.GetLoadProp(loadname,'Characteristic','EFF(50% Load)'))
	EFFS[1] = SAFE_FLOAT(ELoadApp.GetLoadProp(loadname,'Characteristic','EFF(75% Load)'))
	EFFS[2] = SAFE_FLOAT(ELoadApp.GetLoadProp(loadname,'Characteristic','EFF(100% Load)'))
	if 0. == EFFS[2]:
		# 100%의 값이 없을 경우에는 테이블에서 구한다.
		if type(PFandEFF) is type([]) and (len(PFandEFF) == 10):
			EFFS[0] = PFandEFF[5]
			EFFS[1] = PFandEFF[4]
			EFFS[2] = PFandEFF[3]

			# 구한 값을 저장한다.(100%,75%,50%)
			param = [loadname,'Characteristic','EFF(100% Load)','%.3f' % (EFFS[2])]
			ELoadApp.SetLoadProp(param)
			param[2] = 'EFF(75% Load)'
			param[3] = '%.3f' % EFFS[1]
			ELoadApp.SetLoadProp(param)			
			param[2] = 'EFF(50% Load)'
			param[3] = '%.3f' % EFFS[0]
			ELoadApp.SetLoadProp(param)

	return PyFindPF(EFFS , LF)

# brief		float로 형변환을 한다. 형 변환시에 에러가 발생하면 0를 리턴한다.
# author	BHK
def SAFE_FLOAT(arg):
	try:
		res = float(arg)
		return res
	except ValueError:
		return 0

	return 0

# brief	
# author	BHK
# param		type = 'BUS' or 'PDB'
def PyWriteTotalLoadSummaryToExcel(wb,type,FolderPath):
	projectno = ELoadApp.GetProjectNo()
	projectname = ELoadApp.GetProjectName()
	documentno = ELoadApp.GetDocumentNo()
	try:
	#	ELoadApp.AppendLogToOutputWnd('bus')
		wb.Worksheets('TOTAL').Copy(Before=wb.Worksheets('TOTAL'))
		ws = wb.ActiveSheet
		ws.Name = 'TOTAL_' + type
	
		PROJECT_FOLDER = ELoadApp.GetProjectFolder()

		busfile = open(FolderPath + '\\Total' + type + '.bus', 'r')
		row = 10
		no = 1
		buffer = busfile.readlines()
		for index in range(1,len(buffer)-1):
			ws.Range('A'+str(row + 1) +':X'+str(row + 1)).Insert()
			ws.Cells(row,2).Value = str(no)
			row = row + 1
			no = no + 1

		row = 10
		for line in buffer:
			list = line.rstrip('\n').split('\t')
			col = 3
			for item in list:
				if (len(item) >= 1) and ('*' == item[len(item) - 1]):
					# it is modified item. so filled with red.
					ws.Cells(row,col).Value = item[0:len(item)-1]
					ws.Cells(row,col).Interior.ColorIndex = 44
				else:
					ws.Cells(row,col).Value = item
				col = col + 1
			row = row + 1
			#if  line.find('TOTAL') == -1 and line != '\n':
			#	ws.Range('A'+str(row) +':X'+str(row)).Insert()
	

		ws.Cells(3,1).Value = 'PROJECT : ' + projectno
		ws.Cells(4,1).Value = 'PROJECT NAME : ' + projectname
		#ws.Cells(3,4).Value = ELoadApp.GetProjectClient()
		# 현재 날짜를 표기한다.
		now = time.localtime()
		ws.Cells(4,19).Value = 'DATE : ' + '%d' % now.tm_year + '.' + '%d' % now.tm_mon + '.' + '%d' % now.tm_mday
		#ws.Cells(3,8).Value = documentno

		ws.Columns('A:A').EntireColumn.AutoFit()
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

# brief 	LoadSummary의 내용을 엑셀파일로 저장한다.
# author	BHK
# param		args[0] = src excel file , args[1] = dest excel file , argv[2] = folder contains files.
def PyWriteLoadSummaryToExcel(args):
	if type(args) != type([]) or len(args) != 3:
		ELoadApp.AppendLogToOutputWnd('arguments error')
		return

	xl = win32com.client.gencache.EnsureDispatch ('Excel.Application')
	wb = xl.Workbooks.Open(args[0])
	ws = wb.ActiveSheet
	xl.Visible = False	# 보이지 않게 한다.

	FolderPath = args[2]
	# TOTAL DICTIONARY
	TOTAL = {}
	PROJECT_FOLDER = ELoadApp.GetProjectFolder()
	projectno = ELoadApp.GetProjectNo()
	projectname = ELoadApp.GetProjectName()
	documentno = ELoadApp.GetDocumentNo()

	# 먼저 BUS의 이름들을 가져온다.
	BusNameList = ELoadApp.GetBusNameList('BUS')
	for bus in BusNameList:
		# bus 이름으로 SHEET를 만들어 맨 마지막에 붙인다.
		try:
			wb.Worksheets('BUS').Copy(Before=wb.Worksheets(wb.Worksheets.Count))
			ws = wb.ActiveSheet
			ws.Name = bus
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

		#if False == TOTAL.has_key(bus):
		TOTAL[bus] = [0,0,0,0,0,0]	# CONTINUOUS(KW,KVAR) , INTERMITTENT(KW,KVAR) , STAND-BY(KW,KVAR)
		
		busvolt = ELoadApp.GetBusProp(bus,'General','Bus Voltage')
		loadvolt = ELoadApp.GetBusProp(bus, 'General', 'Load Voltage')
		LoadSummary = ELoadApp.GetLoadSummaryInBus(bus);
		KW = str('%.2f'% LoadSummary[0])
		KVAR = str('%.2f'% LoadSummary[1])
		KVA = SAFE_ROUND(math.sqrt(LoadSummary[0]*LoadSummary[0] + LoadSummary[1]*LoadSummary[1]),2)
		try:
			row = 12
			busfile = open(FolderPath + '\\' + bus + '.bus', 'r')
			ELoadApp.AppendLogToOutputWnd(FolderPath + '\\' + bus + '.bus')
			
			buffer = busfile.readlines()
			for index in range(1,len(buffer)-1):
				ws.Range('A'+str(row + 1) +':X'+str(row + 1)).Insert()
				row = row + 1
			
			isLoad = True
			row = 12
			for line in buffer:
				list = line.rstrip('\n').split('\t')
				col = 3
				for item in list:
					if (len(item) >= 1) and ('*' == item[len(item) - 1]):
						# it is modified item. so filled with red.
						ws.Cells(row,col).Value = item[0:len(item)-1]
						ws.Cells(row,col).Interior.ColorIndex = 44
					else:
						ws.Cells(row,col).Value = item

					col = col + 1
				if '' == list[0]:
					isLoad = False

				# Load에만 no.를 표기한다.
				if (True == isLoad) and ((row - 11) < len(buffer)):
					ws.Cells(row,2).Value = str(row - 11)
				row = row + 1

			ws.Cells(5,4).Value = bus
			ws.Cells(5,5).Value = busvolt
			ws.Cells(3,1).Value = 'PROJECT : ' + projectno
			ws.Cells(4,1).Value = 'PROJECT NAME : ' + projectname
			#ws.Cells(3,4).Value = ELoadApp.GetProjectClient()
			# 현재 날짜를 표기한다.
			now = time.localtime()
			ws.Cells(4,19).Value = 'DATE : ' + '%d' % now.tm_year + '.' + '%d' % now.tm_mon + '.' + '%d' % now.tm_mday
			#ws.Cells(3,7).Value = documentno

			ws.Cells(row + 2,11).Value = str(SAFE_FLOAT(ELoadApp.GetELoadItemProp('BUS' , bus , 'General' , 'Continuous')) / 100)
			ws.Cells(row + 2,21).Value = loadvolt
			ws.Cells(row + 2, 15).Value = KW
			ws.Cells(row + 3,11).Value = str(SAFE_FLOAT(ELoadApp.GetELoadItemProp('BUS' , bus , 'General' , 'Intermittent')) / 100)
			ws.Cells(row + 3, 15).Value = KVAR
			ws.Cells(row + 4,11).Value = str(SAFE_FLOAT(ELoadApp.GetELoadItemProp('BUS' , bus , 'General' , 'Stand-by')) / 100)
			ws.Cells(row + 4, 15).Value = KVA
			
			ws.Columns('A:A').EntireColumn.AutoFit()
			busfile.close()


		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

	# TOTAL을 기입한다.
	PyWriteTotalLoadSummaryToExcel(wb,'BUS',FolderPath)
	try:
		# TEMPLATE SHEET를 삭제합니다.
		xl.DisplayAlerts = False
	        wb.Worksheets("TOTAL").Delete()
		wb.Worksheets("BUS").Delete()

		# SAVE AND 엑셀 APP 종료.	
		wb.SaveAs(args[1])
		xl.Visible = True
		wb.Close()
		xl.DisplayAlerts = True
	#	xl.Quit():

	
		os.startfile(args[1])
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	return 'SUCCESS'

# brief 	LoadSummary의 내용을 엑셀파일로 저장한다.
# author	KHS
def PyWritePDBLoadSummaryToExcel(args):
	if type(args) != type([]) or len(args) != 3:
		ELoadApp.AppendLogToOutputWnd('arguments error')
		return

	xl = win32com.client.gencache.EnsureDispatch ('Excel.Application')
	wb = xl.Workbooks.Open(args[0])
	ws = wb.ActiveSheet
	xl.Visible = False	# 보이지 않게 한다.

	FolderPath = args[2]
	# TOTAL DICTIONARY
	TOTAL = {}
	PROJECT_FOLDER = ELoadApp.GetProjectFolder()
	projectno = ELoadApp.GetProjectNo()
	projectname = ELoadApp.GetProjectName()
	documentno = ELoadApp.GetDocumentNo()

	# 먼저 PDB의 이름들을 가져온다.
	PdbNameList = ELoadApp.GetBusNameList('PDB')
	for bus in PdbNameList:
		# bus 이름으로 SHEET를 만들어 맨 마지막에 붙인다.
		try:
			wb.Worksheets('BUS').Copy(Before=wb.Worksheets(wb.Worksheets.Count))
			ws = wb.ActiveSheet
			ws.Name = bus
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

		#if False == TOTAL.has_key(bus):
		TOTAL[bus] = [0,0,0,0,0,0]	# CONTINUOUS(KW,KVAR) , INTERMITTENT(KW,KVAR) , STAND-BY(KW,KVAR)
		
		busvolt = ELoadApp.GetBusProp(bus,'General','Bus Voltage')
		loadvolt = ELoadApp.GetBusProp(bus, 'General', 'Load Voltage')
		LoadSummary = ELoadApp.GetLoadSummaryInBus(bus);
		KW = str('%.2f' % LoadSummary[0])
		KVAR = str('%.2f' % LoadSummary[1])
		KVA = SAFE_ROUND(math.sqrt(LoadSummary[0]*LoadSummary[0] + LoadSummary[1]*LoadSummary[1]),2)
		
		try:
			row = 12
			busfile = open(FolderPath + '\\' + bus + '.bus', 'r')
		
			buffer = busfile.readlines()
			for index in range(1,len(buffer)-1):
				ws.Range('A'+str(row + 1) +':X'+str(row + 1)).Insert()
				row = row + 1

			isLoad = True
			row = 12
			for line in buffer:
				list = line.rstrip('\n').split('\t')
				col = 3
				for item in list:
					if (len(item) >= 1) and ('*' == item[len(item) - 1]):
						# it is modified item. so filled with red.
						ws.Cells(row,col).Value = item[0:len(item)-1]
						ws.Cells(row,col).Interior.ColorIndex = 44 # ?
					else:
						ws.Cells(row,col).Value = item
					col = col + 1
				if '' == list[0]:
					isLoad = False

				if (True == isLoad) and ((row - 11) < len(buffer)):
					ws.Cells(row,2).Value = str(row - 11)
				row = row + 1

			ws.Cells(5,4).Value = bus
			ws.Cells(5,5).Value = busvolt
			ws.Cells(3,1).Value = 'PROJECT : ' + projectno
			ws.Cells(4,1).Value = 'PROJECT NAME : ' + projectname
			#ws.Cells(3,4).Value = ELoadApp.GetProjectClient()
			# 현재 날짜를 표기한다.
			now = time.localtime()
			ws.Cells(4,19).Value = 'DATE : ' + '%d' % now.tm_year + '.' + '%d' % now.tm_mon + '.' + '%d' % now.tm_mday

			#ws.Cells(row + 2,21).Value = loadvolt
			ws.Cells(row + 2,11).Value = str(SAFE_FLOAT(ELoadApp.GetELoadItemProp('BUS' , bus , 'General' , 'Continuous')) / 100)
			ws.Cells(row + 2, 15).Value = KW
			ws.Cells(row + 3,11).Value = str(SAFE_FLOAT(ELoadApp.GetELoadItemProp('BUS' , bus , 'General' , 'Intermittent')) / 100)
			ws.Cells(row + 3, 15).Value = KVAR
			ws.Cells(row + 4,11).Value = str(SAFE_FLOAT(ELoadApp.GetELoadItemProp('BUS' , bus , 'General' , 'Stand-by')) / 100)
			ws.Cells(row + 4, 15).Value = KVA
			
			ws.Columns('A:A').EntireColumn.AutoFit()
			busfile.close()


		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

	# TOTAL을 기입한다.
	PyWriteTotalLoadSummaryToExcel(wb,'SubBUS',FolderPath)
	try:
		# TEMPLATE SHEET를 삭제합니다.
		xl.DisplayAlerts = False
	        wb.Worksheets("TOTAL").Delete()
		wb.Worksheets("BUS").Delete()

		# SAVE AND 엑셀 APP 종료.	
		wb.SaveAs(args[1])
		xl.Visible = True
		wb.Close()
		xl.DisplayAlerts = True
	#	xl.Quit():

	
		os.startfile(args[1])
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	return 'SUCCESS'

#brief LRC와 FLC을 가지고 Starting Amp값을 계산합니다.
#author BHK
#date 2009-04-24 12:16:26 
def PyAutoCalculateStartingAmp(args):
	LRC = ELoadApp.GetLoadProp(args[0] , 'Characteristic' , 'LRC')
	FLC = ELoadApp.GetLoadProp(args[0] , 'Rating' , 'FLC')
	if (LRC != '') and (FLC != ''):
		StartingAmp = SAFE_FLOAT(LRC) * SAFE_FLOAT(FLC) * 0.01
		return ('%.2f' % StartingAmp)

	return ''

#brief LoadSummary를 계산하고 난 뒤의 후속 작업.
#author BHK
#
def PyPostLoadSummary(args):
	try:
		# Staring Amp값을 계산하여 채웁니다.
		loadnamelist = ELoadApp.GetLoadNameList()
		for loadname in loadnamelist:
			StartingAmp = PyAutoCalculateStartingAmp([loadname])
			param = [loadname , 'Rating' , 'Starting Amp' , StartingAmp]
			ELoadApp.SetLoadProp(param)

		TrNameList = ELoadApp.GetTransformerNameList()
		for TrName in TrNameList:
			Connection = ELoadApp.GetTransformerProp(TrName,'Capacity','Connection')

			TotalRatingCapacity = 0.
			if 'BUS' == Connection:
				BusId = ELoadApp.GetTransformerProp(TrName,'To','Bus Id')
				LoadSummary = ELoadApp.GetLoadSummaryInBus(BusId)
				TotalRatingCapacity = math.sqrt(LoadSummary[0]*LoadSummary[0] + LoadSummary[1]*LoadSummary[1])
				TotalRatingCapacity = round(TotalRatingCapacity,2)
			elif 'BUS GROUP' == Connection:
				BusId = ELoadApp.GetTransformerProp(TrName,'To','Bus Id')
				BusGroupName = ELoadApp.GetBusGroupNameHasBus(BusId)
				BusNameList = ELoadApp.GetBusNameListInBusGroup(BusGroupName)
				for BusName in BusNameList:
					LoadSummary = ELoadApp.GetLoadSummaryInBus(BusName)
					TotalRatingCapacity = TotalRatingCapacity + math.sqrt(LoadSummary[0]*LoadSummary[0] + LoadSummary[1]*LoadSummary[1])
				TotalRatingCapacity = round(TotalRatingCapacity,2)					

			if 0. != TotalRatingCapacity:
				ELoadApp.SetTransformerProp(TrName , 'Capacity' , 'Calculated Capacity' , str(TotalRatingCapacity))
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

# brief 	Load LIST - SLDy의 내용을 엑셀파일로 저장한다.
# author	KHS	
# param		args[0] = src excel file , args[1] = dest excel file 
def PyWriteLoadListSldToExcel(args):
	if type(args) != type([]) or len(args) != 3:
		ELoadApp.AppendLogToOutputWnd('arguments error')
		return
	ELoadApp.AppendLogToOutputWnd('Before Excel Open')
	xl = win32com.client.gencache.EnsureDispatch ('Excel.Application')
	wb = xl.Workbooks.Open(args[0])
	ws = wb.ActiveSheet
	xl.Visible = False	# 보이지 않게 한다.

	
	PROJECT_FOLDER = ELoadApp.GetProjectFolder()
	projectno = ELoadApp.GetProjectNo()
	projectname = ELoadApp.GetProjectName()
	documentno = ELoadApp.GetDocumentNo()
	
	FolderPath = args[2]

	# 먼저 BUS의 이름들을 가져온다.
	BusNameList = ELoadApp.GetBusNameList('ALL')
	ELoadApp.AppendLogToOutputWnd('Get All Bus')
	for bus in BusNameList:
		# bus 이름으로 SHEET를 만들어 맨 마지막에 붙인다.
		try:
			wb.Worksheets('BUS').Copy(Before=wb.Worksheets(wb.Worksheets.Count))
			ws = wb.ActiveSheet
			ws.Name = bus
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

		try:
			row = 5
			busfile = open(FolderPath + '\\' + bus + '.txt', 'r')
			ELoadApp.AppendLogToOutputWnd(FolderPath + '\\' + bus + '.txt')
			
			buffer = busfile.readlines()
			for index in range(1,len(buffer)-1):
				ws.Range('A'+str(row + 1) +':L'+str(row + 1)).Insert()
				row = row + 1
			
			row = 5
			for line in buffer:
				list = line.rstrip('\n').split('\t')
				ELoadApp.AppendLogToOutputWnd(list)
				col = 1
				for item in list:
					ws.Cells(row,col).Value = item
					col = col + 1
				row = row + 1

			
			ws.Columns('A:A').EntireColumn.AutoFit()
			busfile.close()
			ws.Cells(2,1).Value = bus


		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	try:
		# TEMPLATE SHEET를 삭제합니다.
		xl.DisplayAlerts = False
		wb.Worksheets("BUS").Delete()

		# SAVE AND 엑셀 APP 종료.	
		wb.SaveAs(args[1])
		xl.Visible = True
		wb.Close()
		xl.DisplayAlerts = True
	#	xl.Quit():

	
		os.startfile(args[1])
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	return 'SUCCESS'

