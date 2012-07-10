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

# brief		FLC°ªÀ» °è»êÇÑ´Ù. ¼Ò¼öÁ¡ 2ÀÚ¸®±îÁö Ç¥ÇöÇÑ´Ù.
# author	BHK
def CalculateFLC(Capacity , Motor_Voltage , PF , MotorType):
	try:
		if '3PH MOTOR' == MotorType:
			FLC = Capacity / (math.sqrt(3) * Motor_Voltage * PF) * 1000
		elif '3PH FEEDER' == MotorType:
			KVA = Capacity
			FLC = KVA / (math.sqrt(3) * Motor_Voltage) * 1000
		elif '1PH FEEDER' == MotorType:
			KVA = Capacity
			FLC = KVA / (Motor_Voltage) * 1000
		else: # 'DC FEEDER'
			KVA = Capacity 
			FLC = KVA / (Motor_Voltage) * 1000

		FLC = round(FLC,2)
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
		return 0

	return FLC

# brief		calculate cable length
# author	BHK
def PyCalculateCableLength(argv):
	L = -1

	#ELoadApp.ClearOutputWnd()
	if type(argv) == type([]) and (9 == len(argv)):
		Rating_Capacity = SAFE_FLOAT(argv[0])
		COS1 = SAFE_FLOAT(argv[1])	# PF
		Motor_Voltage = SAFE_FLOAT(argv[2])
		VD1  	  = SAFE_FLOAT(argv[3])	# Voltage Drop
		R    	  = SAFE_FLOAT(argv[4])
		X    	  = SAFE_FLOAT(argv[5])
		VD2  	  = SAFE_FLOAT(argv[6])
		COS2 	  = SAFE_FLOAT(argv[7])
		MotorType = argv[8]

		I    = CalculateFLC(Rating_Capacity , Motor_Voltage , COS1 , MotorType)
		if 0 == I:
			return [L,I]

		try:
			SIN= math.sqrt(1 - math.pow(COS1,2))
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
			return [L,I]

		try:
			if ('3PH MOTOR' == MotorType) or ('3PH FEEDER' == MotorType):
				L1 = (VD1*1000)/(math.sqrt(3)*I*(R*COS1 + X*SIN))
			elif '1PH FEEDER' == MotorType:
				L1 = (VD1*1000)/(2*I*(R*COS1 + X*SIN))
			elif 'DC FEEDER' == MotorType:
				L1 = (VD1*1000)/(2*I*R)
			else:
				ELoadApp.AppendLogToOutputWnd('expected motor type=' + MotorType)
				return [-1,-1]
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
			L1 = 0
			return [L,I]

		#VD2 = float(ELoadApp.GetStartingVoltageDrop2())
		#ELoadApp.AppendLogToOutputWnd(str(VD2))
		#COS2= float(ELoadApp.GetStartingPowerFactor())
		#ELoadApp.AppendLogToOutputWnd(str(COS2))

		try:	
			SIN= math.sqrt(1 - math.pow(COS2,2))
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
			return [L,I]

		try:
			L2 = (VD2*1000)/(math.sqrt(3)*6*I*(R*COS2 + X*SIN))	# 6À» °öÇÔ - 2011.02.14 modified by HumKyung
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
			L2 = 0

		# '3PH MOTOR'ÀÏ °æ¿ì 2°³ÀÇ ±æÀÌÁß ÂªÀº °ÍÀ» ¼±ÅÃÇÑ´Ù. - 2011.02.14 modified by HumKyung
		# ±× ¿ÜÀÇ °æ¿ì¿¡´Â L1À» ¼±ÅÃÇÑ´Ù.
		# ¼Ò¼öÁ¡ Ã¹Â° ÀÚ¸®¿¡¼­ ¹Ý¿Ã¸²ÇÑ´Ù.
		if '3PH MOTOR' == MotorType:
			if L1 < L2:
				L = round(L1,1)
			else:
				L = round(L2,1)
		else:
			L = L1

	return [L,I]

# brief		write cable length
# author	BHK
def PyWriteCableLength(argv):
	
	# ELoadApp.ClearOutputWnd()
	ELoadApp.AppendLogToOutputWnd('ARGV = ' + argv[0])
	PROJECT_FOLDER = (ELoadApp.GetProjectFolder() + 'Output_CableSizing.xls')
	ELoadApp.AppendLogToOutputWnd('PROJECT FOLDER=' + PROJECT_FOLDER)
	try:
		nRowCount = ELoadApp.GetCableLengthDataSheetRowCount()
		ELoadApp.AppendLogToOutputWnd('Row Count=' + str(nRowCount))
		nColCount = ELoadApp.GetCableLengthDataSheetColCount()
		ELoadApp.AppendLogToOutputWnd('Col Count=' + str(nColCount))

		# Excel Application Open
		xl = win32com.client.gencache.EnsureDispatch('Excel.Application')
		wb = xl.Workbooks.Open(argv[0])
		ws = wb.ActiveSheet
			
		conduit = ELoadApp.GetCableLengthConduitValueList()
		ELoadApp.AppendLogToOutputWnd('conduit='  + str(conduit))

		for c in range(nColCount):
			if len(conduit) > c:
				ws.Cells(17, c + 6).Value = conduit[c];

		# KWL
		kwl = ELoadApp.GetCableLengthKWList()
		ELoadApp.AppendLogToOutputWnd('kwl=' + str(kwl))

		# PF
		pf = ELoadApp.GetCableLengthPFList()
		ELoadApp.AppendLogToOutputWnd('PF='+ str(pf))

		# FLC
		flc = ELoadApp.GetCableLengthFLCList()
		ELoadApp.AppendLogToOutputWnd('FLC=' + str(flc))
		
		# CABLE DATA SIZE
		cable_data_size = ELoadApp.GetCableDataSizeList()
		ELoadApp.AppendLogToOutputWnd('SIZE=' + str(cable_data_size))

		# CABLE DIA
		dia = ELoadApp.GetCableDataDiaList()
		ELoadApp.AppendLogToOutputWnd('DIA=' + str(dia))

		# ALLOWANCE CURRENT
		AllowanceCurrent = ELoadApp.GetCableDataAllowanceCurrentList()

		# REGISTANCE
		registance = ELoadApp.GetCableDataRegistanceList()
		ELoadApp.AppendLogToOutputWnd('REGISTANCE=' + str(registance))

		# REACTANCE
		reactance = ELoadApp.GetCableDataReactanceList()
		ELoadApp.AppendLogToOutputWnd('REACTANCE=' + str(reactance))

		if nRowCount > 24:
			for r in range(nRowCount - 24):
				ws.Rows(str(r + 41) + ':' + str(r + 41)).Insert()
		if nColCount > 17:
			for c in range(nColCount - 17):
				ws.Columns(chr(ord('V') + c) + ':' + chr(ord('V') + c)).Insert()

		ws.Cells(6 , 1).Value = 'DERATING FACTOR               : ' + argv[1]

		for c in range(nColCount):
			ws.Cells( 9, 6 + c).Value = cable_data_size[c]
			ws.Cells(10, 6 + c).Value = dia[c]
			ws.Cells(11, 6 + c).Value = AllowanceCurrent[c]
			ws.Cells(12, 6 + c).Value = str(SAFE_FLOAT(argv[1])*SAFE_FLOAT(AllowanceCurrent[c]))
			ws.Cells(13, 6 + c).Value = registance[c]
			ws.Cells(14, 6 + c).Value = reactance[c]

		for r in range(nRowCount):
			ws.Cells(r + 18, 1).Value = kwl[r]
			ws.Cells(r + 18, 2).Value = flc[r]
			ws.Cells(r + 18, 3).Value = pf[r]

		for c in range(nColCount):
			for r in range(nRowCount):
				value = ELoadApp.GetCableLengthDataSheetValue(r,c)
				ELoadApp.AppendLogToOutputWnd('VALUE=' + str(value))
				ws.Cells(r + 18, c + 6).Value = str(value)
		
		ws.Columns('A:' + chr(ord('A') + nColCount + 5)).ColumnWidth = 8
				# TEST
				# ELoadApp.AppendLogToOutputWnd('(' + str(r) + ',' + str(c) + ')=' + value)
		
		xl.DisplayAlerts = False

		wb.SaveAs(PROJECT_FOLDER)
		wb.Close()
	#	xl.Quit()

	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

	return 'SUCCESS'

# brief 	convert string - > int
# author 	KHS

def safe_int(object):
	try:
		retval = int(object)
	except ValueError:
		retval = 'could not convert non-number to int'
	return retval

# brief		write cable sizing data
# author	KHS

def PyExportCableSizingExcelFile(argv):
	
	# ELoadApp.ClearOutputWnd()
	ELoadApp.AppendLogToOutputWnd('ARGV = ' + argv[0])
	PROJECT_FOLDER = (ELoadApp.GetExecPath() + 'BackUp\TEMPLATE_CABEL_DATA.xls')
	ELoadApp.AppendLogToOutputWnd('PROJECT FOLDER=' + PROJECT_FOLDER)
	try:

		lResult = ELoadApp.GetCableDataList(argv[1], argv[2])
		
		# ½ºÆ®¸µÇü NO¸¦ Á¤¼öÇüÀ¸·Î º¯È¯
	#	for r in range(len(lResult)):
	#		lResult[r][0] = safe_int(lResult[r][0])

		#lResult.sort()

		# Excel Application Open
		xl = win32com.client.gencache.EnsureDispatch('Excel.Application')
		wb = xl.Workbooks.Open(PROJECT_FOLDER)
		ws = wb.ActiveSheet
		xl.Visible = False
		
		for c in range(len(lResult) - 1):
			ws.Range('A3:P3').Insert()
		
		ELoadApp.AppendLogToOutputWnd('INSERT SUCCESS')
		# EXCEL WRITE
		#ws.Cells(1, 1).Value = 'NO'
		#ws.Cells(1, 2).Value = 'c_ID'
		#ws.Cells(1, 3).Value = 'C_SIZE'
		#ws.Cells(1, 4).Value = 'C_DIA'
		#ws.Cells(1, 5).Value = 'C_AMP_G'
		#ws.Cells(1, 6).Value = 'C_AMP_A'
		#ws.Cells(1, 7).Value = 'C_R_OHM'
		#ws.Cells(1, 8).Value = 'C_X_60'
		#ws.Cells(1, 9).Value = 'C_X_50'	
		
		#for c in range(len(lResult[0])):
		#	ws.Cells(1, c + 1).Font.Size = 10

		for r in range(len(lResult)):
			lRowList = lResult[r]
			for c in range(len(lRowList)):
				 ws.Cells(r + 2, c + 1).Value = lRowList[c]
				 #ws.Cells(r + 2, c + 1).Font.Size = 10

		ELoadApp.AppendLogToOutputWnd('Write SUCCESS')
		ws.Columns('A:P').EntireColumn.AutoFit()

		xl.DisplayAlerts = False

		wb.SaveAs(argv[0])
		wb.Close()
		xl.DisplayAlerts = True

		os.startfile(argv[0])
	#	xl.Quit()

	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

	return 'SUCCESS'


# brief		Write Cable Sizing Data in Txt file.
# author	KHS
def PyWriteCableSizingToDataFile(argv):
	# ELoadApp.ClearOutputWnd()
	PROJECT_FOLDER = ELoadApp.GetProjectFolder()
	
	try:
		MotorType = argv[17]

		# create CableSizing folder if CableSizing folder not exists.
		if False == os.path.exists(PROJECT_FOLDER + 'Cable Sizing Table'):
			os.mkdir(PROJECT_FOLDER + 'Cable Sizing Table')

		# remove file if already exists.
		filename = PROJECT_FOLDER + 'Cable Sizing Table\\' + argv[0] + '.txt'
		if True == os.path.isfile(filename):
			os.remove(filename)

		ofile = open(filename,'w')
		
		ofile.write('[CABLE_SIZE_DATA]' + '\n')
		
		nRowCount = ELoadApp.GetCableLengthDataSheetRowCount()
		ELoadApp.AppendLogToOutputWnd('Row Count=' + str(nRowCount))
		ofile.write('ROWCOUNT=' + str(nRowCount) + '\n')
		nColCount = ELoadApp.GetCableLengthDataSheetColCount()
		ELoadApp.AppendLogToOutputWnd('Col Count=' + str(nColCount))
		ofile.write('COLCOUNT=' + str(nColCount) + '\n')

		# CONDUIT
		conduit = ELoadApp.GetCableLengthConduitValueList()
		ELoadApp.AppendLogToOutputWnd('CONDUIT=' + str(conduit))
		ofile.write('CONDUIT=')
		for c in range(len(conduit)):
			ofile.write(str(conduit[c]) + ',')
		ofile.write('\n')

		# KWL
		kwl = ELoadApp.GetCableLengthKWList()
		ELoadApp.AppendLogToOutputWnd('KWL=' + str(kwl))
		ELoadApp.AppendLogToOutputWnd( 'LEN=' + str(len(kwl)))
		ofile.write('KWL=')
		for r in range(len(kwl)):
			ofile.write(str(kwl[r]) + ',')
		ofile.write('\n')

		# PF
		pf = ELoadApp.GetCableLengthPFList()
		ELoadApp.AppendLogToOutputWnd('PF=' + str(pf))
		ELoadApp.AppendLogToOutputWnd( 'LEN=' + str(len(pf)))
		ofile.write('PF=')
		if 'DC FEEDER' != MotorType:
			for r in range(len(pf)):
				ofile.write(str(pf[r]) + ',')
		else:
			for r in range(len(pf)):
				ofile.write(str(' ') + ',')
		ofile.write('\n')

		# FLC
		flc = ELoadApp.GetCableLengthFLCList()
		ELoadApp.AppendLogToOutputWnd('FLC=' + str(flc))
		ELoadApp.AppendLogToOutputWnd( 'LEN=' + str(len(flc)))
		ofile.write('FLC=')
		for r in range(len(flc)):
			ofile.write(str(flc[r]) + ',')
		ofile.write('\n')
		
		# CABLE DATA SIZE
		cable_data_size = ELoadApp.GetCableDataSizeList()
		ofile.write('CABLESIZE=')
		for c in range(len(cable_data_size)):
			ofile.write(str(cable_data_size[c]) + ',')
		ofile.write('\n')

		# CABLE DIA
		dia = ELoadApp.GetCableDataDiaList()
		ofile.write('DIA=')
		for c in range(len(dia)):
			ofile.write(str(dia[c]) + ',')
		ofile.write('\n')

		# ALLOWANCE CURRENT
		AllowanceCurrent = ELoadApp.GetCableDataAllowanceCurrentList()
		ofile.write('ALLOWANCE=')
		for c in range(len(AllowanceCurrent)):
			ofile.write(str(AllowanceCurrent[c]) + ',')
		ofile.write('\n')
		
		# SAFETY CURRENT
		ofile.write('SAFETY=')
		for c in range(len(AllowanceCurrent)):
			safety = SAFE_FLOAT(AllowanceCurrent[c]) * SAFE_FLOAT(argv[1])
			ofile.write(str(safety) + ',')
		ofile.write('\n')

		# REGISTANCE
		registance = ELoadApp.GetCableDataRegistanceList()
		ofile.write('REGISTANCE=')
		for c in range(len(registance)):
			ofile.write(str(registance[c]) + ',')
		ofile.write('\n')


		# REACTANCE
		reactance = ELoadApp.GetCableDataReactanceList()
		ofile.write('REACTANCE=')
		for c in range(len(reactance)):
			ofile.write(str(reactance[c]) + ',')
		ofile.write('\n')

		for r in range(nRowCount):
			ofile.write('CELL' + str(r) + '=')
			for c in range(nColCount):
				value = ELoadApp.GetCableLengthDataSheetValue(r, c)
				value = int(round(SAFE_FLOAT(value),0))
				ofile.write(str(value) + ',')

			ofile.write('\n')

		ofile.write('DERATING FACTOR=' + argv[1] + '\n')
		ofile.write('CABLE DATA TABLE=' + argv[2] + '\n')
		ofile.write('SELECT BUS=' + argv[3] + '\n')
		ofile.write('MOTOR TYPE=' + argv[4] + '\n')
		ofile.write('BUS VOLTAGE=' + argv[5] + '\n')
		ofile.write('MOTOR VOLTAGE=' + argv[6] + '\n')
		ofile.write('VOLTAGE DROP %=' + argv[7] + '\n')
		ofile.write('VOLTAGE DROP V=' + argv[8] + '\n')
		ofile.write('PF FOR FEEDER=' + argv[9] + '\n')
		ofile.write('STARTING VOLTAGE DROP %=' + argv[10] + '\n')
		ofile.write('STARTING VOLTAGE DROP V=' + argv[11] + '\n')
		ofile.write('STARTING POWER FACTOR=' + argv[12] + '\n')
		ofile.write('INSULATION METHOD=' + argv[13] + '\n')
		ofile.write('CABLE TYPE=' + argv[14] + '\n')
		ofile.write('CONDUIT TABLE=' + argv[15] + '\n')
		ofile.write('CONDUIT DESC=' + argv[16] + '\n')
		ofile.write('MOTOR TYPE=' + argv[17] + '\n')
	
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
	
	ofile.close()

	return 'SUCCESS'

#brief		write cable length To Excel read TXT
#author		HumKyung
#date		????.??.??
#param		argv
def PyWriteCableLengthReadTxtToExcel(argv):
	
	# ELoadApp.ClearOutputWnd()
	PROJECT_FOLDER = (ELoadApp.GetProjectFolder() + 'Working\\Output_CableSizing.xls')

	try:
		# Excel Application Open
		xl = win32com.client.gencache.EnsureDispatch('Excel.Application')
		wb = xl.Workbooks.Open(argv[0])
		ws = wb.ActiveSheet
		
		xl.Visible = False
		
		projectno = ELoadApp.GetProjectNo()

		ofile = open(ELoadApp.GetProjectFolder() + 'Working\\CABLE_SIZING_DATA.txt', 'r')

		aLine = ofile.readline()
		cable_data_size = aLine.split(',')

		aLine = ofile.readline()
		dia = aLine.split(',')

		aLine = ofile.readline()
		AllowanceCurrent = aLine.split(',')

		aLine = ofile.readline()
		safety = aLine.split(',')

		aLine = ofile.readline()
		registance = aLine.split(',')

		aLine = ofile.readline()
		reactance = aLine.split(',')

		aLine = ofile.readline()
		kwl = aLine.split(',')

		aLine = ofile.readline()
		flc = aLine.split(',')

		aLine = ofile.readline()
		pf = aLine.split(',')

		aLine = ofile.readline()
		conduit = aLine.split(',')
		
		cell_data_list = []
		cell_data = []
	#	ELoadApp.AppendLogToOutputWnd('cell_data_list=' + str(cell_data))
	
		for r in range(len(kwl) - 2):
			aLine = ofile.readline()
			cell_data = aLine.split(',')
			ELoadApp.AppendLogToOutputWnd('cell_data_list=' + str(cell_data))
			cell_data_list.insert(r, cell_data)

		ofile.close()

		if len(kwl) - 2 > 24:
			for r in range(len(kwl) - 2 - 24):
				ws.Rows(str(r + 41) + ':' + str(r + 41)).Insert()
		if len(cable_data_size) -7 > 17:
			for c in range( len(cable_data_size) -7 - 17):
				ws.Columns(chr(ord('V') + c) + ':' + chr(ord('V') + c)).Insert()

		#ws.Cells(6 , 1).Value = 'DERATING FACTOR               : ' + argv[1]

		for c in range(len(cable_data_size) - 2):
			ws.Cells(10, 4 + c).Value = str(cable_data_size[c + 1])
			ws.Cells(11, 4 + c).Value = str(dia[c + 1])
			ws.Cells(12, 4 + c).Value = str(AllowanceCurrent[c + 1])
			ws.Cells(13, 4 + c).Value = str(safety[c + 1])
			ws.Cells(14, 4 + c).Value = str(registance[c + 1])
			ws.Cells(15, 4 + c).Value = str(reactance[c + 1])

		for r in range(len(kwl) - 2):
			ws.Cells(r + 19, 1).Value = str(kwl[r + 1])
			ws.Cells(r + 19, 2).Value = str(flc[r + 1])
			ws.Cells(r + 19, 3).Value = str(pf[r + 1])

		for c in range(len(conduit) - 2):
			if len(conduit) > c:
				ws.Cells(18, c + 4).Value = str(conduit[c + 1])

	#	ELoadApp.AppendLogToOutputWnd('len(cable_data_size=' + str(len(cable_data_size)))
	#	ELoadApp.AppendLogToOutputWnd(cell_data_list)
		
		busvoltage = ''
		loadvoltage = ''
		deratingfactor = ''
		voltagedrop = ''
		startingvoltagedrop = ''
		startingpowerfactor = ''
		installationmethod = ''
		cabletype = ''
		motortype = ''
		for c in range(len(cable_data_size) - 2):
			for r in range(len(kwl) -2):
				if (len(kwl) -7) > r:
					ws.Cells(r + 19, c + 4).Value = cell_data_list[r][c + 1]

				if c == 0 and r == len(kwl) - 7:
					deratingfactor = cell_data_list[r][c + 1]
					ELoadApp.AppendLogToOutputWnd('deratingfactor=' + deratingfactor )
					list = deratingfactor.split(':')
					deratingfactor = list[1]
				elif c == 0 and r == len(kwl) - 6:
					busvoltage = cell_data_list[r][c + 1]
					list = busvoltage.split(':')
					busvoltage = list[1]
				elif c == 4 and r == len(kwl) - 6:
					loadvoltage =  cell_data_list[r][c + 1]
					list = loadvoltage.split(':')
					loadvoltage = list[1]
				elif c == 12 and r == len(kwl) - 6:
					voltagedrop = cell_data_list[r][c + 1]
					list = voltagedrop.split(':')
					voltagedrop = list[1]
				elif c == 8 and r == len(kwl) - 5:
					startingvoltagedrop = cell_data_list[r][c + 1]
					list = startingvoltagedrop.split(':')
					startingvoltagedrop = list[1]
				elif c == 12 and r == len(kwl) - 5:
					startingpowerfactor = cell_data_list[r][c + 1]
					list = startingpowerfactor.split(':')
					startingpowerfactor = list[1]
				elif c == 0 and r == len(kwl) - 4:
					installationmethod =  cell_data_list[r][c + 1]
					list = installationmethod.split(':')
					installationmethod = list[1]
				elif c == 0 and r == len(kwl) - 3:
					cabletype = cell_data_list[r][c + 1]
					list = cabletype.split(':')
					cabletype = list[1]
				elif c == 12 and r == len(kwl) - 7:
					motortype = cell_data_list[r][c + 1]
					list = motortype.split(':')
					motortype = list[1].strip()

		
		cablemotorunit = '' #kwl[0]
		if '3PH MOTOR' == motortype:
			cablemotorunit = 'KW'
			startingvoltagedrop = startingvoltagedrop + '[V]'	# 2011.02.17 added by HumKyung
			startingpowerfactor = startingpowerfactor		# 2011.02.17 added by HumKyung
		else:
			cablemotorunit = 'KVA'
			startingvoltagedrop = 'N/A'				# 2011.02.17 added by HumKyung
			startingpowerfactor = 'N/A'				# 2011.02.17 added by HumKyung


		#ws.Columns('A:' + chr(ord('A') + len(cable_data_size) - 2 + 5)).ColumnWidth = 8
		
		ws.Cells(3, 1).Value = motortype
		ws.Cells(5,3).Value = projectno
		ws.Cells(6,3).Value = busvoltage + '[V]' + ',' + loadvoltage + '[V]'
		ws.Cells(7,3).Value = deratingfactor
		ws.Cells(8,3).Value = voltagedrop + '[V]'
		ws.Cells(5,17).Value = startingvoltagedrop
		ws.Cells(6,17).Value = startingpowerfactor
		ws.Cells(7,17).Value = installationmethod
		ws.Cells(8,17).Value = cabletype
		ws.Cells(16 ,1).Value = motortype
		ws.Cells(17, 1).Value = cablemotorunit

		xl.DisplayAlerts = False

		wb.SaveAs(PROJECT_FOLDER)
		xl.Visible = True
		wb.Close()
		xl.DisplayAlerts = True
	#	xl.Quit()
		os.startfile(PROJECT_FOLDER)

	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

	return 'SUCCESS'

# brief		write cable Sizing Result To Excel (Power Cable Result)
# author	KHS
# param		args[0] = temlate excel file path, args[1] = output excel file path, args[2] = FolderPath
def PyWriteCableSizingResultToExcel(args):
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
			wb.Worksheets('POWER_CABLE').Copy(Before=wb.Worksheets(wb.Worksheets.Count))
			ws = wb.ActiveSheet
			ws.Name = bus
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
	
		try:
			row = 11
			ifile = open(FolderPath + '\\' + bus + '.BUS', 'r')
			ELoadApp.AppendLogToOutputWnd(FolderPath + '\\' + bus + '.BUS')

			buffer = ifile.readlines()
			for index in range(1,len(buffer)):
				ws.Range('A'+str(row + 1) +':AD'+str(row + 1)).Insert()
				row = row + 1
			
			row = 11
			for line in buffer:
				list = line.rstrip('\n').split('\t')
				col = 1
				for item in list:
					if (len(item) >= 1) and ('*' == item[len(item) - 1]):
						# it is modified item. so filled with red.
						ws.Cells(row,col).Value = item[0:len(item)-1]
						ws.Cells(row,col).Interior.ColorIndex = 44 # ?
					else:
						ws.Cells(row,col).Value = item
					col = col + 1
				row = row + 1

			BusVolt = ELoadApp.GetBusProp(bus,'General','Bus Voltage')
			ws.Cells(1,4).Value = projectno
			ws.Cells(2,4).Value = projectname
			ws.Cells(3,4).Value = ELoadApp.GetProjectClient()
			ws.Cells(5,4).Value = bus
			ws.Cells(5,8).Value = BusVolt
			# ÇöÀç ³¯Â¥¸¦ Ç¥±âÇÑ´Ù.
			now = time.localtime()
			ws.Cells(2,12).Value = '%d' % now.tm_year + '.' + '%d' % now.tm_mon + '.' + '%d' % now.tm_mday
			ws.Cells(3,12).Value = documentno
			
			ws.Columns('A:A').EntireColumn.AutoFit()
			ifile.close()


		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

	try:
		# TEMPLATE SHEET¸¦ »èÁ¦ÇÕ´Ï´Ù.
		xl.DisplayAlerts = False
	        wb.Worksheets("POWER_CABLE").Delete()

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

# brief		write Cable BM Summary Result To Excel
# author	KHS,BHK
def PyWriteCableBMSummaryToExcel(args):
	if type(args) != type([]) or len(args) != 2:
		ELoadApp.AppendLogToOutputWnd('arguments error')
		return

	xl = win32com.client.gencache.EnsureDispatch ('Excel.Application')
	wb = xl.Workbooks.Open(args[0])
	ws = wb.ActiveSheet
	xl.Visible = False	# º¸ÀÌÁö ¾Ê°Ô ÇÑ´Ù.

	# TOTAL DICTIONARY
	TOTAL = {}
	TOTAL['CONTROL'] = 0;TOTAL['HV'] = 0;TOTAL['LV'] = 0
	AREA = {}
	PROJECT_FOLDER = ELoadApp.GetProjectFolder()
	projectno = ELoadApp.GetProjectNo()
	projectname = ELoadApp.GetProjectName()
	documentno = ELoadApp.GetDocumentNo()

	try:
		wb.Worksheets('Cable_BM').Copy(Before=wb.Worksheets(wb.Worksheets.Count))
		ws = wb.ActiveSheet
		ws.Name = 'Cable BM'
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
	
	try:
		Total = 0
		row = 33
		ifile = open(PROJECT_FOLDER + '\\Working\\CABLE_BM_SUMMARY_CONTROL_CABLE.txt', 'r')
		buffer = ifile.readlines()

		for index in range(len(buffer) - 1):
			ws.Range('A'+str(row + 1) +':IV'+str(row + 1)).Insert()
			
		sum = 0
		row = 33;col = 2;index = 0
		for line in buffer:
			line = line.rstrip('\n')
			if 0 == index:	# areaºÎºÐÀ» writeÇÑ´Ù.
				cols = line.rstrip('\n').split('\t')

				col = 7
				for item in cols:
					ws.Cells(row - 1,col).Value = item
					AREA[col] = 0
					col = col + 1
				index = index + 1
				continue

			col = 2
			cols = line.rstrip('\n').split('\t')
			for item in cols:
				ws.Cells(row,col).Value = item
				if (col >= 7) and ('' != item):	# store area length
					AREA[col] = AREA[col] + SAFE_FLOAT(item)
				col = col + 1
			sum = sum + SAFE_FLOAT(cols[4])
			TOTAL['CONTROL'] = TOTAL['CONTROL'] + SAFE_FLOAT(cols[4])
			row = row + 1
			index = index + 1
		ws.Cells(row + 2 , 6).Value = str(TOTAL['CONTROL'])
		# °¢ AREAº° length¸¦ ±âÀÔÇÑ´Ù.
		dickeys = AREA.keys()
		for key in dickeys:
			ws.Cells(row + 2,key).Value = str(AREA[key])
		ifile.close()

		row = 25
		ifile = open(PROJECT_FOLDER + '\\Working\\CABLE_BM_SUMMARY_LV_POWER_CABLE.txt', 'r')
		buffer = ifile.readlines()

		# ±¸ÇÑ row °¹¼ö ¸¸Å­ insertÇÑ´Ù.
		for index in range(len(buffer) - 1):
			ws.Range('A'+str(row + 1) +':IV'+str(row + 1)).Insert()
			
		row = 25;col = -4;index = 0
		for line in buffer:
			line = line.rstrip('\n')
			if 0 == index:	# areaºÎºÐÀ» writeÇÑ´Ù.
				cols = line.rstrip('\n').split('\t')

				col = 7
				for item in cols:
					ws.Cells(row - 1,col).Value = item
					AREA[col] = 0
					col = col + 1
				index = index + 1
				continue

			col = 2
			cols = line.rstrip('\n').split('\t')
			for item in cols:
				ws.Cells(row,col).Value = item
				if (col >= 7) and ('' != item):	# store area length
					AREA[col] = AREA[col] + SAFE_FLOAT(item)
				col = col + 1
			sum = sum + SAFE_FLOAT(cols[4])
			TOTAL['LV'] = TOTAL['LV'] + SAFE_FLOAT(cols[4])

			row = row + 1
			index = index + 1
		ws.Cells(row + 2 , 6).Value = str(TOTAL['LV'])
		# °¢ AREAº° length¸¦ ±âÀÔÇÑ´Ù.
		dickeys = AREA.keys()
		for key in dickeys:
			ws.Cells(row + 2,key).Value = str(AREA[key])
		ifile.close()

		row = 17
		ifile = open(PROJECT_FOLDER + '\\Working\\CABLE_BM_SUMMARY_HV_POWER_CABLE.txt', 'r')
		buffer = ifile.readlines()

		# ±¸ÇÑ row °¹¼ö ¸¸Å­ insertÇÑ´Ù.
		for index in range(len(buffer) - 1):
			ws.Range('A'+str(row + 1) +':IV'+str(row + 1)).Insert()
			
		row = 17;col = -4;index = 0
		for line in buffer:
			line = line.rstrip('\n')
			if 0 == index:	# areaºÎºÐÀ» writeÇÑ´Ù.
				cols = line.rstrip('\n').split('\t')

				col = 7
				for item in cols:
					ws.Cells(row - 1,col).Value = item
					AREA[col] = 0
					col = col + 1
				index = index + 1
				continue

			col = 2
			cols = line.rstrip('\n').split('\t')
			for item in cols:
				ws.Cells(row,col).Value = item
				if (col >= 7) and ('' != item):	# store area length
					AREA[col] = AREA[col] + SAFE_FLOAT(item)
				col = col + 1
			sum = sum + SAFE_FLOAT(cols[4])
			TOTAL['HV'] = TOTAL['HV'] + SAFE_FLOAT(cols[4])
			row = row + 1
			index = index + 1
		ws.Cells(row + 2 , 6).Value = str(TOTAL['HV'])
		# °¢ AREAº° length¸¦ ±âÀÔÇÑ´Ù.
		dickeys = AREA.keys()
		for key in dickeys:
			ws.Cells(row + 2,key).Value = str(AREA[key])	
		ifile.close()

		ws.Cells(11,6).Value = str(Total)
		ws.Cells(1,3).Value = projectno
		ws.Cells(2,3).Value = projectname
		ws.Cells(3,3).Value = ELoadApp.GetProjectClient()

		ws.Cells(8,6).Value = str(TOTAL['HV'])
		ws.Cells(9,6).Value = str(TOTAL['LV'])
		ws.Cells(10,6).Value = str(TOTAL['CONTROL'])
		ws.Cells(11,6).Value = str(TOTAL['CONTROL']+TOTAL['HV']+TOTAL['LV'])

		# ÇöÀç ³¯Â¥¸¦ Ç¥±âÇÑ´Ù.
		now = time.localtime()
		ws.Cells(2,8).Value = '%d' % now.tm_year + '.' + '%d' % now.tm_mon + '.' + '%d' % now.tm_mday
		#ws.Cells(3,12).Value = documentno
			
		ws.Columns('A:F').EntireColumn.AutoFit()


	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

	try:
		# TEMPLATE SHEET¸¦ »èÁ¦ÇÕ´Ï´Ù.
		xl.DisplayAlerts = False
	        wb.Worksheets("Cable_BM").Delete()

		# SAVE AND ¿¢¼¿ APP Á¾·á.	
		wb.SaveAs(args[1])
		xl.Visible = True
		#wb.Close()
		xl.DisplayAlerts = True
	#	xl.Quit()

		#os.startfile(args[1])
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())


	return 'SUCCESS'
