#! -*- Encoding: Latin-1 -*-

import ELoadApp    
import sys
import os
import locale
import math
import re
import win32com.client
import LoadSummary
import BusDiagram
import CableSizing
import CableCreation
import Generator
import CableRoute
import traceback

from math import *
try:
	from LoadSummary import *
	from BusDiagram import *
	from CableSizing import *
	from CableCreation import *
	from Generator import *
	from CableRoute import *
	from TransformerSizing import *
except:
	traceback.print_exc(file=open('c:\\test.at','a'))


locale.setlocale(locale.LC_ALL, "")

# brief		¸ðµâÀ» »õ·Ó°Ô ·ÎµùÇÕ´Ï´Ù.
# author	BHK
# date		2009-06-28 00:22:14
def reloadall(): 
	try:
		import sys 
		modsreloaded = {} 
		for sym, obj in globals().items(): 
			if hasattr(obj, '__module__'): 
				mod = obj.__module__ 
				if mod.startswith('__'): 
					continue 
				if mod not in modsreloaded: 
					modsreloaded[mod] = reload(sys.modules[obj.__module__]) 
				globals()[sym] = getattr(modsreloaded[mod], sym)
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
	ELoadApp.AppendLogToOutputWnd('format_exc()')

reloadall()

#try:
#	reloadall()
#except:
#	traceback.print_exc(file=open('c:\\test.at','a'))

# brief		float·Î Çüº¯È¯À» ÇÑ´Ù. Çü º¯È¯½Ã¿¡ ¿¡·¯°¡ ¹ß»ýÇÏ¸é 0¸¦ ¸®ÅÏÇÑ´Ù.
# author	BHK
def SAFE_FLOAT(arg):
	try:
		res = float(arg)
		return res
	except ValueError:
		return 0
	except TypeError:
		ELoadApp.AppendLogToOutputWnd('TypeError = ' + str(arg))
		return 0

	return 0

# brief		»ç¿ëÀÚ°¡ BHP°ªÀ» ÀÔ·ÂÇÏÁö ¾Ê¾Ò´Ù¸é ÀÚµ¿À¸·Î BHP°ªÀ» ±¸ÇÑ´Ù.
# author	BHK
# return	BHP°ª
def PyBHPAutoCalc(BHP,Load_Category,Rating_Capacity):
	if '' == BHP:
		# »ç¿ëÀÚ°¡ °ªÀ» ÀÔ·ÂÇÏÁö ¾Ê¾Ò´Ù¸é ÀÚµ¿À¸·Î °è»êÇÑ´Ù.
		if 'MOTOR' == Load_Category:
			if Rating_Capacity <= 19:
				return (Rating_Capacity/1.25)
			elif (Rating_Capacity > 19) and (Rating_Capacity < 75):
				return (Rating_Capacity/1.15)
			else:
				return (Rating_Capacity/1.1)
		else:
			LF = 0.9	# ±âº»°ªÀ» 0.9·Î µÐ´Ù.
			if Load_Category == 'LTG':
				LF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_LTG_LF'))
			elif Load_Category == 'WELDING':
				LF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_WELDING_LF'))
			elif Load_Category == 'INST.':
				LF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_INST_LF'))
			elif Load_Category == 'COMM.':
				LF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_COMM_LF'))
			elif Load_Category == 'OTHERS':
				LF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_OTHERS_LF'))

			return Rating_Capacity*LF
	else:
		return BHP


# brief		LoadTable¿¡¼­ PF,EFFÀÇ °ªÀ» ±¸ÇÑ´Ù.
# author	BHK
# return 	when success return list of PF,EFF otherwise return empty list.
def PySearchLoadTablePFandEFF(Rating_Capacity , Rated_Voltage , Poles, LF):
	res = ELoadApp.SearchLoadTable(Rating_Capacity,Rated_Voltage,Poles, LF)

	# TEST
	if type(res) is type([]) and (len(res) == 10):
		# ¡Æ¨£A¡í A¢´¡íoAuA¢¬¡¤I A¡Ì¨úO¢¥U¢¬e ¢¬¢ç¨ö¨¬¨¡¢ç¢¯¢® ¢¥a¨ú¨¡ ©øN¡ÆUA¨ª¢¥U.
		try:
			PF100  = SAFE_FLOAT(res[0])
			PF100  = SAFE_ROUND(PF100 , 3)
			PF75   = SAFE_FLOAT(res[1])
			PF75   = SAFE_ROUND(PF75 , 3)
			PF50   = SAFE_FLOAT(res[2])
			PF50   = SAFE_ROUND(PF50 , 3)
			EFF100 = SAFE_FLOAT(res[3])
			EFF100 = SAFE_ROUND(EFF100 , 3)
			EFF75  = SAFE_FLOAT(res[4])
			EFF75  = SAFE_ROUND(EFF75 , 3)
			EFF50  = SAFE_FLOAT(res[5])
			EFF50  = SAFE_ROUND(EFF50 , 3)			
			PF     = SAFE_FLOAT(res[6])
			PF     = SAFE_ROUND(PF , 3)
			EFF    = SAFE_FLOAT(res[7])
			EFF    = SAFE_ROUND(EFF, 3)
			LRC    = SAFE_FLOAT(res[8])
			LRC    = SAFE_ROUND(LRC , 3)
			PFlr   = SAFE_FLOAT(res[9])
			PFlr   = SAFE_ROUND(PFlr , 3)

			return [PF100,PF75,PF50,EFF100,EFF75,EFF50,PF,EFF,LRC,PFlr]
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

	return [0,0,0,0,0,0,0,0,0,0]

# args[0]	= load name
# author	BHK
# return	list
def PyLoadSummary(args):
	try:
		Load_Category= ELoadApp.GetLoadProp(args[0],'Type','Load Category')
		Load_Category = Load_Category.upper()
		# SUBBUS´Â °è»êÇÏÁö ¾Ê´Â´Ù.
		if Load_Category == 'SUBBUS':
			return 'SUCCESS'

		Rating_Capacity= ELoadApp.GetLoadRatingCapacity(args[0])
		if type(Rating_Capacity) != type(''):
			ELoadApp.ELOAD_LOG4CXX(['WARN' , args[0] , 52])
			return 'ERROR'

		Rating_Capacity=SAFE_FLOAT(Rating_Capacity)
	#except ValueError:
	#	ELoadApp.AppendLogToOutputWnd('Rating Capacity Value Error')
	#	return 'Rating Capacity Value Error'

		BHP = ELoadApp.GetLoadProp(args[0],'Load','BHP <1>')
		BHP = PyBHPAutoCalc(BHP,Load_Category,Rating_Capacity)
		try:
			BHP = SAFE_ROUND(BHP,3)
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

		# BHP°ªÀ» ÀúÀåÇÑ´Ù.
		ELoadApp.SetLoadProp(args[0],'Load','BHP <1>','%.3f' % (BHP))
		Load_Type = ELoadApp.GetLoadProp(args[0],'Type','Load Type')
		Poles= ELoadApp.GetLoadProp(args[0],'Rating','Poles')

		PFlock  = ELoadApp.GetLoadProp(args[0],'Characteristic','PF LOCK')
		PFlock  = PFlock.upper()
		EFFlock = ELoadApp.GetLoadProp(args[0],'Characteristic','EFF LOCK')
		EFFlock = EFFlock.upper()
		if 'MOTOR' == Load_Category:
			# MOTORÀÏ °æ¿ì¿¡´Â °è»êÀ» ¼öÇàÇÑ´Ù.
			try:
				LF = SAFE_FLOAT(BHP) / SAFE_FLOAT(Rating_Capacity)
			except ZeroDivisionError:
				LF = 0
			except:
				ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
			LF = SAFE_ROUND(LF,3)

			if LF < 0 or LF > 1:
            			ELoadApp.AppendLogToOutputWnd('LF range error')
				return 'LF range error'

			#LF´Â ¼Ò¼öÁ¡ 3ÀÚ¸®·Î
			param = [args[0],'Characteristic','LF','%.3f' % (LF)]
			ELoadApp.SetLoadProp(param)

			# Reference table¿¡¼­ PF,EFF¸¦ Ã£´Â´Ù.
			Rated_Voltage= ELoadApp.GetLoadProp(args[0],'Rating','Rated Voltage')
			PFandEFF = PySearchLoadTablePFandEFF(Rating_Capacity , Rated_Voltage , Poles, LF)

			# LRC , PFlrÀÇ °ªÀ» ¼³Á¤ÇÑ´Ù.
			LockStatus = ELoadApp.IsLoadPropLock(args[0] , 'Characteristic' , 'LRC')
			if 'FALSE' == LockStatus:
				param[2] = 'LRC'
				LRC = round(PFandEFF[8],0)
				param[3] = '%.0f' % LRC
				if 0 == LRC:
					param[3] = ''

				ELoadApp.SetLoadProp(param)

			LockStatus = ELoadApp.IsLoadPropLock(args[0] , 'Characteristic' , 'PF at Starting')
			if 'FALSE' == LockStatus:
				param[2] = 'PF at Starting'
				PFatStarting = round(PFandEFF[9],2)
				param[3] = '%.2f' % PFatStarting
				if 0 == PFatStarting:
					param[3] = ''

				ELoadApp.SetLoadProp(param)		
		
			try:
				if PFlock == 'NO':
					if type(PFandEFF) is type([]) and (len(PFandEFF) == 10):
						PF = PFandEFF[6]

						# 100%,75%,50%ÀÇ °ªÀ» ±âÀÔÇÑ´Ù.(¼Ò¼öÁ¡ 3ÀÚ¸®·Î)
						# °ªÀÌ 0ÀÏ °æ¿ì¿¡´Â ''À¸·Î ´ëÀÔÇÑ´Ù.
						param = [args[0],'Characteristic','PF(100% Load)','%.3f' % PFandEFF[0]]
						if 0 == PFandEFF[0]:
							param[3] = ''
						ELoadApp.SetLoadProp(param)
						param[2] = 'PF(75% Load)'
						param[3] = '%.3f' % PFandEFF[1]
						if 0 == PFandEFF[1]:
							param[3] = ''
						ELoadApp.SetLoadProp(param)
						param[2] = 'PF(50% Load)'
						param[3] = '%.3f' % PFandEFF[2]
						if 0 == PFandEFF[2]:
							param[3] = ''
						ELoadApp.SetLoadProp(param)
					else:
						ELoadApp.ELOAD_LOG4CXX(['WARN' , args[0] , 12])
						return 'ERROR IN SEARCH REF LOAD TABLE FROM PYTHON.'
				elif PFlock == 'YES':
					PF = PyGetPFIfLockIsYes(args[0],PFandEFF,LF)
				else:
					ELoadApp.ELOAD_LOG4CXX(['WARN' , args[0] , 19])
					return 'PFlock type ERROR'
			except:
				ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

			try:
				if EFFlock == 'NO':
					if type(PFandEFF) is type([]) and (len(PFandEFF) == 10):
						EFF= PFandEFF[7]

						# 100%,75%,50%ÀÇ °ªÀ» ±âÀÔÇÑ´Ù.(¼Ò¼öÁ¡ 3ÀÚ¸®·Î)
						# °ªÀÌ 0ÀÏ °æ¿ì¿¡´Â ''À¸·Î ´ëÀÔÇÑ´Ù.
						param = [args[0],'Characteristic','EFF(100% Load)','%.3f' % PFandEFF[3]]
						if 0 == PFandEFF[3]:
							param[3] = ''
						ELoadApp.SetLoadProp(param)
						param[2] = 'EFF(75% Load)'
						param[3] = '%.3f' % PFandEFF[4]
						if 0 == PFandEFF[4]:
							param[3] = ''
						ELoadApp.SetLoadProp(param)
						param[2] = 'EFF(50% Load)'
						param[3] = '%.3f' % PFandEFF[5]
						if 0 == PFandEFF[5]:
							param[3] = ''
						ELoadApp.SetLoadProp(param)				
					else:
						ELoadApp.ELOAD_LOG4CXX(['WARN' , args[0] , 12])
						return 'ERROR IN SEARCH REF LOAD TABLE FROM PYTHON.'
				elif EFFlock == 'YES':
					EFF = PyGetEFFIfLockIsYes(args[0],PFandEFF,LF)
				else:
					ELoadApp.ELOAD_LOG4CXX(['WARN' , args[0] , 20])
					return 'EFFlock type ERROR'
			except:
				ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
		else:
			try:
				LF = SAFE_FLOAT(BHP) / SAFE_FLOAT(Rating_Capacity)
			except ZeroDivisionError:
				LF = 0
			except:
				ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

			LF = SAFE_ROUND(LF,2)
			if LF < 0 or LF > 1:
	            		ELoadApp.ELOAD_LOG4CXX(['WARN' , args[0] , 21])
				return 'LF range error'

			#LF´Â ¼Ò¼öÁ¡ 2ÀÚ¸®·Î
			param = [args[0],'Characteristic','LF','%.2f' % (LF)]
			ELoadApp.SetLoadProp(param)
	
			# Reference table¿¡¼­ PF,EFF¸¦ Ã£´Â´Ù.
			Rated_Voltage= ELoadApp.GetLoadProp(args[0],'Rating','Rated Voltage')
			#PFandEFF = PySearchLoadTablePFandEFF(Rating_Capacity , Rated_Voltage , Poles, LF)

			# LRC , PFlrÀÇ °ªÀ» ¼³Á¤ÇÑ´Ù.
			#param[2] = 'LRC'
			#param[3] = '%.3f' % PFandEFF[8]
			#ELoadApp.SetLoadProp(param)
			#param[2] = 'PF at Starting'
			#param[3] = '%.3f' % PFandEFF[9]
			#ELoadApp.SetLoadProp(param)

			# TABLE¿¡¼­ Load Category¿¡ µû¸¥ PF,EFF°ªÀ» °¡Á®¿Â´Ù.
			if 'NO' == PFlock.upper():
				if Load_Category == 'LTG':
					PF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_LTG_PF'))
				elif Load_Category == 'WELDING':
					PF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_WELDING_PF'))
				elif Load_Category == 'INST.':
					PF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_INST_PF'))
				elif Load_Category == 'COMM.':
					PF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_COMM_PF'))
				elif Load_Category == 'OTHERS':
					PF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_OTHERS_PF'))
				else:
			        	ELoadApp.ELOAD_LOG4CXX(['ERROR' , args[0] , 10])
				        return 'LoadType is unset'
	
				param[1] = 'Characteristic'
				param[2] = 'PF(100% Load)'
				param[3] = '%.3f' % (PF)
				ELoadApp.SetLoadProp(param)
			elif 'YES' == PFlock.upper():
				try:
					PFandEFF = PySearchLoadTablePFandEFF(Rating_Capacity , Rated_Voltage , Poles, LF)
					if type(PFandEFF) is type([]) and (len(PFandEFF) == 10):
						PF = PyGetPFIfLockIsYes(args[0],PFandEFF,LF)
					else:
						ELoadApp.ELOAD_LOG4CXX(['WARN' , args[0] , 11])
						return 'SearchLoadTable is fail.'
				except:
					ELoadApp.AppendLogToOutputWnd(args[0] + ' : ' + traceback.format_exc())
					return (args[0] + ' : ' + traceback.format_exc())
			else:
				ELoadApp.ELOAD_LOG4CXX(['WARN' , args[0] , 19])
				return 'PFlock type ERROR'

			if 'NO' == EFFlock.upper():
				if Load_Category == 'LTG':
					EFF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_LTG_EFF'))
				elif Load_Category == 'WELDING':
					EFF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_WELDING_EFF'))
				elif Load_Category == 'INST.':
					EFF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_INST_EFF'))
				elif Load_Category == 'COMM.':
					EFF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_COMM_EFF'))
				elif Load_Category == 'OTHERS':
					EFF = SAFE_FLOAT(ELoadApp.GetFieldValueInProjectSettingTable('C_OTHERS_EFF'))
				else:
		        		ELoadApp.ELOAD_LOG4CXX(['ERROR' , args[0] , 10])
				        return 'LoadType is unset'
	
				param[1] = 'Characteristic'
				param[2] = 'EFF(100% Load)'
				param[3] = '%.3f' % (EFF)
				ELoadApp.SetLoadProp(param)	
	
			elif 'YES' == EFFlock.upper():
				try:
					PFandEFF = PySearchLoadTablePFandEFF(Rating_Capacity , Rated_Voltage , Poles, LF)
					if type(PFandEFF) is type([]) and (len(PFandEFF) == 10):
						EFF = PyGetEFFIfLockIsYes(args[0],PFandEFF,LF)
					else:
						ELoadApp.ELOAD_LOG4CXX(['WARN' , args[0] , 11])
						return 'SearchLoadTable is fail.'
				except:	
					ELoadApp.AppendLogToOutputWnd(args[0] + ' : ' + traceback.format_exc())
					return (args[0] + ' : ' + traceback.format_exc())
			else:
				ELoadApp.ELOAD_LOG4CXX(['WARN' , args[0] , 20])
				return 'EFFlock type ERROR'
	
			PF = SAFE_ROUND(PF , 3)
			EFF= SAFE_ROUND(EFF, 3)

		Rating_Capacity_Unit = ELoadApp.GetLoadPropUnitStr(args[0],'Rating','Rating Capacity')
		if 'KVA' == Rating_Capacity_Unit:
			## 'KVA'ÀÏ °æ¿ìÀÇ °è»ê½Ä
			KW   = SAFE_FLOAT(BHP)*SAFE_FLOAT(PF)
		else:
			## 'KVA'ÀÌ¿ÜÀÇ °æ¿ìÀÇ °è»ê½Ä
			KW   = SAFE_FLOAT(BHP)/SAFE_FLOAT(EFF)
	
		KW   = SAFE_ROUND(KW , 2)
		try:	
			KVA  = SAFE_FLOAT(KW)/SAFE_FLOAT(PF)
			KVA  = SAFE_ROUND(KVA, 2)
			KVAR = math.sqrt(math.pow(KVA,2) - math.pow(KW,2))
			KVAR = SAFE_ROUND(KVAR , 2)
		except:	
			ELoadApp.AppendLogToOutputWnd(args[0] + ' : ' + traceback.format_exc())
	
		param = [args[0] , 'Load Summary' , 'KW' , '%.2f' % (KW)]
		ELoadApp.SetLoadProp(param)

		param[2] = 'KVA'
		param[3] = '%.2f' % (KVA)
		ELoadApp.SetLoadProp(param)
		
		param[2] = 'KVAR'
		param[3] = '%.2f' % (KVAR)
		ELoadApp.SetLoadProp(param)

		param[2] = 'PF'
		param[3] = '%.3f' % (PF)
		ELoadApp.SetLoadProp(param)

		param[2] = 'EFF'
		param[3] = '%.3f' % EFF
		ELoadApp.SetLoadProp(param)

		# FLC°ªÀ» °è»êÇÏ¿© ÀúÀåÇÑ´Ù.
		param[1] = 'Rating'
		param[2] = 'FLC'
		param[3] = PyCalculateFLC(args[0])
		ELoadApp.SetLoadProp(param)

		return 'SUCCESS'
	except:
		ELoadApp.ELOAD_LOG4CXX(['ERROR' , args[0] + ' : ' + traceback.format_exc() , 1])
		ELoadApp.AppendLogToOutputWnd(args[0] + ' : ' + traceback.format_exc())

# brief		LoadSummary °è»ê °á°ú¸¦ ÅØ½ºÆ® ÆÄÀÏ·Î ÀúÀåÇÑ´Ù.
# author	BHK
def WriteTotalLoadSummaryToDataFileA(BusNameList,TOTAL,rApplyTr):
	PROJECT_FOLDER = ELoadApp.GetProjectFolder()

	keylist    = TOTAL.keys()
	keylistlen = len(keylist)

	C_SUM = [0,0]
	D_SUM = [0,0]
	E_SUM = [0,0]
	F_SUM = [0,0]
	G_SUM = [0,0]
	H_SUM = [0,0]
	I_SUM = [0,0]
	J_SUM = [0,0]
	K_SUM = [0,0]

	if False == os.path.exists(PROJECT_FOLDER + 'Load Summary Result'):
		os.mkdir(PROJECT_FOLDER + 'Load Summary Result')

	TotalBUSFile = open(PROJECT_FOLDER + 'Load Summary Result\\TotalBUS.bus','w')
	TotalSubBUSFile = open(PROJECT_FOLDER + 'Load Summary Result\\TotalSubBUS.bus','w')
	for busname in BusNameList:
		BusGroup = ELoadApp.GetBusGroupNameHasBus(busname)
		BusGroup = BusGroup.upper()
		if ('INCOMING' == busname):
			continue

		if 'SUBBUS' != BusGroup:
			f = TotalBUSFile
		else:
			f = TotalSubBUSFile

		try:
			BUS_ID = ELoadApp.GetBusProp(busname,'General','Bus ID')
			f.write(busname + '\t')	# skip Bus Equip. No.
			DESC   = ELoadApp.GetBusProp(busname,'General','Description')
			f.write(DESC + '\t')
			
			BUS_VOLT = ELoadApp.GetBusProp(busname, 'General', 'Bus Voltage')
			f.write(BUS_VOLT + '\t')

			LOAD_VOLT = ELoadApp.GetBusProp(busname, 'General', 'Load Voltage')
			f.write(LOAD_VOLT + '\t')

			valist = TOTAL.get(busname)

			Continuous = ELoadApp.GetBusProp(busname,'General','Continuous')
			Intermittent = ELoadApp.GetBusProp(busname,'General','Intermittent')
			Standby = ELoadApp.GetBusProp(busname,'General','Stand-by')		

			C = SAFE_FLOAT(Continuous)*0.01*valist[0] + SAFE_FLOAT(Intermittent)*0.01*valist[2] + SAFE_FLOAT(Standby)*0.01*valist[4]
			if 'SUBBUS' != BusGroup:
				C_SUM[0] = C_SUM[0] + C
			else:
				C_SUM[1] = C_SUM[1] + C
			D = SAFE_FLOAT(Continuous)*0.01*valist[1] + SAFE_FLOAT(Intermittent)*0.01*valist[3] + SAFE_FLOAT(Standby)*0.01*valist[5]
			if 'SUBBUS' != BusGroup:
				D_SUM[0] = D_SUM[0] + D
			else:
				D_SUM[1] = D_SUM[1] + D
			if 0. != C:
				f.write(str('%.2f' % C) + '\t')
			else:
				f.write('\t')
			if 0. != D:
				f.write(str('%.2f' %  D) + '\t')
			else:
				f.write('\t')
			E = SAFE_ROUND(math.sqrt(C*C + D*D),2)
			if 0. != E:
				f.write(str('%.2f' % E) + '\t')
			else:
				f.write('\t')
			if 'SUBBUS' != BusGroup:	
				E_SUM[0] = E_SUM[0] + E
			else:
				E_SUM[1] = E_SUM[1] + E
			
			if '0' != Continuous:
				if Continuous == '100':
					Continuous = '1'
				else:
					Continuous = str(SAFE_FLOAT(Continuous) * 0.01)
			if '0' != Intermittent:
				if '100' == Intermittent:
					Intermittent = '1'
				else:
					Intermittent = str(SAFE_FLOAT(Intermittent) * 0.01)
			if '0' != Standby:
				if '100' == Standby:
					Standby = '1'
				else:
					Standby = str(SAFE_FLOAT(Standby) * 0.01)

			f.write(str(Continuous) + '\t')
			f.write(str(Intermittent) + '\t')
			f.write(str(Standby) + '\t')

			# 0. ÀÎ°ÍÀº »Ñ¸®Áö ¾Ê´Â´Ù.
			if 0. != valist[0]:
				f.write(str('%.2f' % valist[0]) + '\t')
			else:
				f.write('\t')
			if 'SUBBUS' != BusGroup:
				F_SUM[0] = F_SUM[0] + valist[0]
			else:
				F_SUM[1] = F_SUM[1] + valist[0]
			if 0. != valist[1]:
				f.write(str('%.2f' % valist[1]) + '\t')
			else:	
				f.write('\t')
			if 'SUBBUS' != BusGroup:
				G_SUM[0] = G_SUM[0] + valist[1]
			else:
				G_SUM[1] = G_SUM[1] + valist[1]

			if 0. != valist[2]:
				f.write(str('%.2f' % valist[2]) + '\t')
			else:
				f.write('\t')
			if 'SUBBUS' != BusGroup:
				H_SUM[0] = H_SUM[0] + valist[2]
			else:
				H_SUM[1] = H_SUM[1] + valist[2]
			if 0. != valist[3]:
				f.write(str('%.2f' % valist[3]) + '\t')
			else:
				f.write('\t')
			if 'SUBBUS' != BusGroup:
				I_SUM[0] = I_SUM[0] + valist[3]
			else:
				I_SUM[1] = I_SUM[1] + valist[3]

			if 0. != valist[4]:
				f.write(str('%.2f' % valist[4]) + '\t')
			else:
				f.write('\t')
			if 'SUBBUS' != BusGroup:
				J_SUM[0] = J_SUM[0] + valist[4]
			else:
				J_SUM[1] = J_SUM[1] + valist[4]
			if 0. != valist[5]:
				f.write(str('%.2f' % valist[5]))
			if 'SUBBUS' != BusGroup:
				K_SUM[0] = K_SUM[0] + valist[5]
			else:
				K_SUM[1] = K_SUM[1] + valist[5]

			f.write('\n')
		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

	for i in range(0,2):
		if 0 == i:
			f = TotalBUSFile
		else:
			f = TotalSubBUSFile
		
		f.write('\n')

		if rApplyTr == 'TRUE':
			f.write('All Bus Capacity has been accumulated by the Sub-Bus Capacity (Down-Stream Bus Capacity).')
		else:
			f.write('TOTAL' + '\t\t\t\t')
			f.write(str('%.2f' % C_SUM[i]) + '\t')
			f.write(str('%.2f' % D_SUM[i]) + '\t')
			f.write(str('%.2f' % E_SUM[i]) + '\t')
			f.write('\t\t\t')
			if 0. != F_SUM[i]:
				f.write(str('%.2f' % F_SUM[i]) + '\t')
			else:
				f.write('\t')
			if 0. != G_SUM[i]:
				f.write(str('%.2f' % G_SUM[i]) + '\t')
			else:
				f.write('\t')
			if 0. != H_SUM[i]:
				f.write(str('%.2f' % H_SUM[i]) + '\t')
			else:
				f.write('\t')
			if 0. != I_SUM[i]:
				f.write(str('%.2f' % I_SUM[i]) + '\t')
			else:
				f.write('\t')
			if 0. != J_SUM[i]:
				f.write(str('%.2f' % J_SUM[i]) + '\t')
			else:
				f.write('\t')
			if 0. != K_SUM[i]:
				f.write(str('%.2f' % K_SUM[i]))
	
		f.close()


# brief		delete all bus files
# author	BHK
def DeleteAllBusFiles(prjFolder):
	for filename in os.listdir(prjFolder):
		result = re.search('[.]*\.bus',filename)
		if result:
			os.remove(os.path.join(prjFolder,filename))
	
# brief		
# author	BHK
# param		
# date		2009-06-15 19:34:17
def WriteLoadSummaryDataOfSpecialItem(busfile , typeString , id , param , LoadSummaryCalcData):
	busfile.write(id + '\t')
	if typeString == 'TRANSFORMER':
		busfile.write('Connected BUS' + '\t')
		busfile.write('\t\t\t\t\t\t')
	elif typeString == 'UPSDC':
		SelectedCapacity = ELoadApp.GetELoadItemProp('UPSDC' , id , 'General' , 'SELECTED CAPACITY')
		if SelectedCapacity == 'UPS Capacity':
			busfile.write('Connected UPS' + '\t')
			LoadVolt = ELoadApp.GetELoadItemProp('UPSDC' , id , 'UPS Capacity' , 'Input Load Volt')
			busfile.write(LoadVolt + '\t')
			KVA = ELoadApp.GetELoadItemProp('UPSDC' , id , 'UPS Capacity' , 'KVA')
			busfile.write('%.2f' % SAFE_FLOAT(KVA) + '\t')
			busfile.write('KVA' + '\t')
			BHP = ELoadApp.GetELoadItemProp('UPSDC' , id , 'UPS Capacity' , 'BHP')
			busfile.write('%.2f' % SAFE_FLOAT(BHP) + '\t')
		else:
			busfile.write('Connected DC' + '\t')
			LoadVolt = ELoadApp.GetELoadItemProp('UPSDC' , id , 'DC Capacity' , 'AC Rating Load Volt')
			busfile.write(LoadVolt + '\t')
			KVA = ELoadApp.GetELoadItemProp('UPSDC' , id , 'DC Capacity' , 'AC Rating KVA')
			busfile.write('%.2f' % SAFE_FLOAT(KVA) + '\t')
			busfile.write('KVA' + '\t')
			BHP = ELoadApp.GetELoadItemProp('UPSDC' , id , 'DC Capacity' , 'AC Rating BHP')
			busfile.write('%.2f' % SAFE_FLOAT(BHP) + '\t')
		busfile.write('\t') # TYPEÀº °ø¹éÀ¸·Î µÐ´Ù.
		OP = ELoadApp.GetELoadItemProp('UPSDC' , id , param , 'Operation Mode')
		if OP == 'CONTINUOUS':
			busfile.write('C' + '\t')
		elif OP == 'INTERMITTENT':
			busfile.write('I' + '\t')
		elif OP == 'STAND-BY':
			busfile.write('S' + '\t')
		else:
			busfile.write('\t')

	# LF
	if ('' != LoadSummaryCalcData[0]) and (0 != LoadSummaryCalcData[0]):
		busfile.write('%.2f' % LoadSummaryCalcData[0] + '\t')
	else:
		busfile.write('\t')
	# PF
	if ('' != LoadSummaryCalcData[1]) and (0 != LoadSummaryCalcData[1]):
		busfile.write('%.3f' % LoadSummaryCalcData[1] + '\t')
	else:
		busfile.write('\t')
	# EFF
	if ('' != LoadSummaryCalcData[2]) and (0 != LoadSummaryCalcData[2]):
		busfile.write('%.3f' % LoadSummaryCalcData[2] + '\t')
	else:
		busfile.write('\t')
	if ('' != LoadSummaryCalcData[3]) and (0 != LoadSummaryCalcData[3]):
		busfile.write('%.2f' % LoadSummaryCalcData[3] + '\t')
	else:
		busfile.write('\t')
	if ('' != LoadSummaryCalcData[4]) and (0 != LoadSummaryCalcData[4]):
		busfile.write('%.2f' % LoadSummaryCalcData[4] + '\t')
	else:
		busfile.write('\t')
	if ('' != LoadSummaryCalcData[5]) and (0 != LoadSummaryCalcData[5]):
		busfile.write('%.2f' % LoadSummaryCalcData[5] + '\t')
	else:
		busfile.write('\t')
	if ('' != LoadSummaryCalcData[6]) and (0 != LoadSummaryCalcData[6]):
		busfile.write('%.2f' % LoadSummaryCalcData[6] + '\t')
	else:
		busfile.write('\t')
	if ('' != LoadSummaryCalcData[7]) and (0 != LoadSummaryCalcData[7]):
		busfile.write('%.2f' % LoadSummaryCalcData[7] + '\t')
	else:
		busfile.write('\t')
	if ('' != LoadSummaryCalcData[8]) and (0 != LoadSummaryCalcData[8]):
		busfile.write('%.2f' % LoadSummaryCalcData[8] + '\t')
	else:
		busfile.write('\t')
	busfile.write('\t\n')

# brief 	LoadSummaryÀÇ ³»¿ëÀ» ÅØ½ºÆ® ÆÄÀÏ·Î ÀúÀåÇÑ´Ù.
# author	BHK
# param		args[0] = src excel file , args[1] = dest excel file
def WriteLoadSummaryToDataFileA(args):
	rApplyTr = 'TRUE'
	if type(args) is type([]) and (len(args) == 1):
		rApplyTr = args[0]

	# TOTAL DICTIONARY
	TOTAL = {}

	try:

		PROJECT_FOLDER =  ELoadApp.GetProjectFolder()
		if False == os.path.exists(PROJECT_FOLDER + 'Load Summary Result'):
			os.mkdir(PROJECT_FOLDER + 'Load Summary Result')
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
		return

	DeleteAllBusFiles(PROJECT_FOLDER + 'Load Summary Result')	
	

	# ¸ÕÀú BUSÀÇ ÀÌ¸§µéÀ» °¡Á®¿Â´Ù.
	BusNameList = ELoadApp.GetBusNameList('ALL')

	for bus in BusNameList:
		BusGroup = ELoadApp.GetBusGroupNameHasBus(bus)
		if ('INCOMING' == bus):
			continue

		try:

			#if False == TOTAL.has_key(bus):
			TOTAL[bus] = [0,0,0,0,0,0]	# CONTINUOUS(KW,KVAR) , INTERMITTENT(KW,KVAR) , STAND-BY(KW,KVAR)
			C_SUM = 0
			D_SUM = 0
			H_SUM = 0
			I_SUM = 0
			J_SUM = 0
			K_SUM = 0
			L_SUM = 0
			M_SUM = 0
			N_SUM = 0
			O_SUM = 0
			P_SUM = 0
			Q_SUM = 0

			row = 1
			# BUSº° Load Summary ÆÄÀÏÀ» ³²±ä´Ù.
			busfile = open(PROJECT_FOLDER + 'Load Summary Result\\' + bus + '.bus','w')
			LoadNameList = ELoadApp.GetLoadNameListInBus(bus)
			LoadNameList.sort()

			LoadCount = len(LoadNameList)
			for loadname in LoadNameList:
				NAME = ELoadApp.GetLoadProp(loadname,'General','ITEM ID')
				busfile.write(NAME + '\t')

				DESC = ELoadApp.GetLoadProp(loadname,'General','Description')
				busfile.write(DESC + '\t')

				LoadCategory = ELoadApp.GetLoadProp(loadname , 'Type' , 'Load Category')
				LoadCategory = LoadCategory.upper()
				ConnectedBusID = ELoadApp.GetLoadProp(loadname , 'Panel' , 'Connect Bus')
				if (ConnectedBusID != '') and ('SUBBUS' != LoadCategory):
					ELoadApp.ELOAD_LOG4CXX(['WARN' , loadname , 22])
				
				KW   = ['','','']
				KVAR = ['','','']
				KVA  = ''
				PF   = ''
				EFF  = ''

				OP   = ELoadApp.GetLoadProp(loadname,'Type','Operation Mode')

				if (('SUBBUS' == LoadCategory) and ('' != ConnectedBusID)):
					LoadSummaryCalcData = ELoadApp.GetLoadSummaryCalcDataOfBus(ConnectedBusID)
					PF      = str(LoadSummaryCalcData[1])
					EFF     = str(LoadSummaryCalcData[2])
					KW[0]   = str(LoadSummaryCalcData[3])
					KVAR[0] = str(LoadSummaryCalcData[4])
					KW[1]   = str(LoadSummaryCalcData[5])
					KVAR[1] = str(LoadSummaryCalcData[6])
					KW[2]   = str(LoadSummaryCalcData[7])
					KVAR[2] = str(LoadSummaryCalcData[8])
				else:
					if OP == 'CONTINUOUS':
						KW[0]  = ELoadApp.GetLoadProp(loadname,'Load Summary','KW').strip()
						KVAR[0]= ELoadApp.GetLoadProp(loadname,'Load Summary','KVAR').strip()
						KVA    = ELoadApp.GetLoadProp(loadname,'Load Summary','KVA').strip()
					elif OP == 'INTERMITTENT':
						KW[1]  = ELoadApp.GetLoadProp(loadname,'Load Summary','KW').strip()
						KVAR[1]= ELoadApp.GetLoadProp(loadname,'Load Summary','KVAR').strip()
						KVA    = ELoadApp.GetLoadProp(loadname,'Load Summary','KVA').strip()
					else:
						KW[2]  = ELoadApp.GetLoadProp(loadname,'Load Summary','KW').strip()
						KVAR[2]= ELoadApp.GetLoadProp(loadname,'Load Summary','KVAR').strip()
						KVA    = ELoadApp.GetLoadProp(loadname,'Load Summary','KVA').strip()


				if ('SUBBUS' != LoadCategory):
					RATED_VOLTS = ELoadApp.GetLoadProp(loadname, 'Rating','Rated Voltage')
					busfile.write(('%.2f' % SAFE_FLOAT(RATED_VOLTS)) + '\t')

					Rating_Capacity = ELoadApp.GetLoadRatingCapacity(loadname)
					if type(Rating_Capacity) == type(''):
						D_SUM = D_SUM + SAFE_FLOAT(Rating_Capacity)
					busfile.write(('%.2f' % SAFE_FLOAT(Rating_Capacity)) + '\t')

					Rating_Capacity_Unit = ELoadApp.GetLoadPropUnitStr(loadname,'Rating','Rating Capacity')
					busfile.write(Rating_Capacity_Unit + '\t')
					
					BHP = ELoadApp.GetLoadProp(loadname,'Load','BHP <1>')
					if ('' != BHP) and ('0' != BHP):
						busfile.write(BHP + '\t')
						Q_SUM = Q_SUM + SAFE_FLOAT(BHP)
					else:
						busfile.write('\t')
					
					TYPE = ELoadApp.GetLoadProp(loadname,'Type', 'Load Category')
					TYPE = TYPE.upper()
					if TYPE == 'MOTOR':
						busfile.write('M\t')
					elif TYPE == 'LTG':
						busfile.write('L\t')
					elif TYPE == 'WELDING':
						busfile.write('W\t')
					elif TYPE == 'INST.':
						busfile.write('I\t')
					elif TYPE == 'COMM.':
						busfile.write('C\t')
					elif TYPE == 'OTHERS':
						busfile.write('O\t')
					elif TYPE == 'SUBBUS':
						busfile.write('P\t')

					if OP == 'CONTINUOUS':
						busfile.write('C\t')
					elif OP == 'INTERMITTENT':
						busfile.write('I\t')
					else:
						busfile.write('S\t')

					LF   = ELoadApp.GetLoadProp(loadname,'Characteristic','LF')
					H_SUM = H_SUM +SAFE_FLOAT(LF)
					busfile.write(LF + '\t')
	
					PF   = ELoadApp.GetLoadProp(loadname,'Load Summary','PF')
					I_SUM = I_SUM + SAFE_FLOAT(PF)
					busfile.write(PF + '\t')

					EFF  = ELoadApp.GetLoadProp(loadname,'Load Summary','EFF')
					J_SUM = J_SUM + SAFE_FLOAT(EFF)
					busfile.write(EFF + '\t')
				else:
					# SUBBUSÀÏ °æ¿ì.(Rated Volt.,Rating Capacity,UNIT,Load,Type,Duty,LF ´Â °ø¹éÀ¸·Î ¾´´Ù.)
					busfile.write('\t\t\t\t\t\t\t')
					busfile.write(PF + '\t')
					I_SUM = I_SUM + SAFE_FLOAT(PF)
					busfile.write(EFF + '\t')
					J_SUM = J_SUM + SAFE_FLOAT(EFF)

			
				if ('' != KW[0]) and ('0' != KW[0]):
					busfile.write(KW[0] + '\t')
					K_SUM = K_SUM + SAFE_FLOAT(KW[0])
				else:
					busfile.write('\t')
				if ('' != KVAR[0]) and ('0' != KVAR[0]):
					busfile.write(KVAR[0] + '\t')
					L_SUM = L_SUM + SAFE_FLOAT(KVAR[0])
				else:
					busfile.write('\t')
				if ('' != KW[1]) and ('0' != KW[1]):
					busfile.write(KW[1] + '\t')
					M_SUM = M_SUM + SAFE_FLOAT(KW[1])
				else:
					busfile.write('\t')
				if ('' != KVAR[1]) and ('0' != KVAR[1]):
					busfile.write(KVAR[1] + '\t')
					N_SUM = N_SUM + SAFE_FLOAT(KVAR[1])
				else:
					busfile.write('\t')
				if ('' != KW[2]) and ('0' != KW[2]):
					busfile.write(KW[2] + '\t')
					O_SUM = O_SUM + SAFE_FLOAT(KW[2])
				else:
					busfile.write('\t')
				if ('' != KVAR[2]) and ('0' != KVAR[2]):
					busfile.write(KVAR[2] + '\t')
					P_SUM = P_SUM + SAFE_FLOAT(KVAR[2])
				else:
					busfile.write('\t')

				# 2010.02.11
				remark = ELoadApp.GetLoadProp(loadname,'Remark','REMARK')
				busfile.write(remark)
				# 

				TOTAL[bus][0] = TOTAL[bus][0] + SAFE_FLOAT(KW[0])
				TOTAL[bus][1] = TOTAL[bus][1] + SAFE_FLOAT(KVAR[0])
				TOTAL[bus][2] = TOTAL[bus][2] + SAFE_FLOAT(KW[1])
				TOTAL[bus][3] = TOTAL[bus][3] + SAFE_FLOAT(KVAR[1])
				TOTAL[bus][4] = TOTAL[bus][4] + SAFE_FLOAT(KW[2])
				TOTAL[bus][5] = TOTAL[bus][5] + SAFE_FLOAT(KVAR[2])

				row = row + 1
				busfile.write('\n')

			busfile.write('\n')

			####################################################################################################
			UPSDCCount = 0
			## ¿¬°áµÈ UPS/DC°¡ ÀÖÀ¸¸é...
			UPSDCItemList = ELoadApp.GetELoadItemListOf('UPSDC')
			for upsdc in UPSDCItemList:
				BusID1 = ELoadApp.GetELoadItemProp('UPSDC' , upsdc , 'From1' , 'Bus ID')
				BusID2 = ELoadApp.GetELoadItemProp('UPSDC' , upsdc , 'From2' , 'Bus ID')
				if (bus == BusID1) or (bus == BusID2):
					LoadSummaryCalcData = ELoadApp.GetLoadSummaryCalcDataOfUPSDC(upsdc , bus)
					if bus == BusID1:
						WriteLoadSummaryDataOfSpecialItem(busfile , 'UPSDC' , upsdc , 'From1' , LoadSummaryCalcData)
					else:
						WriteLoadSummaryDataOfSpecialItem(busfile , 'UPSDC' , upsdc , 'From2' , LoadSummaryCalcData)
					SelectedCapacity = ELoadApp.GetELoadItemProp('UPSDC' , upsdc , 'General' , 'SELECTED CAPACITY')
					if SelectedCapacity == 'UPS Capacity':
						KVA = SAFE_FLOAT(ELoadApp.GetELoadItemProp('UPSDC' , upsdc , 'UPS Capacity' , 'KVA'))
						D_SUM = D_SUM + KVA
						BHP = SAFE_FLOAT(ELoadApp.GetELoadItemProp('UPSDC' , upsdc , 'UPS Capacity' , 'BHP'))
						Q_SUM = Q_SUM + BHP
					else:
						KVA = SAFE_FLOAT(ELoadApp.GetELoadItemProp('UPSDC' , upsdc , 'DC Capacity' , 'AC Rating KVA'))
						D_SUM = D_SUM + KVA						
						BHP = SAFE_FLOAT(ELoadApp.GetELoadItemProp('UPSDC' , upsdc , 'DC Capacity' , 'AC Rating BHP'))
						Q_SUM = Q_SUM + BHP						

					H_SUM = H_SUM + SAFE_FLOAT(LoadSummaryCalcData[0])
					I_SUM = I_SUM + SAFE_FLOAT(LoadSummaryCalcData[1])
					J_SUM = J_SUM + SAFE_FLOAT(LoadSummaryCalcData[2])
					TOTAL[bus][0] = TOTAL[bus][0] + SAFE_FLOAT(LoadSummaryCalcData[3])
					K_SUM = K_SUM + SAFE_FLOAT(LoadSummaryCalcData[3])
					TOTAL[bus][1] = TOTAL[bus][1] + SAFE_FLOAT(LoadSummaryCalcData[4])
					L_SUM = L_SUM + SAFE_FLOAT(LoadSummaryCalcData[4])
					TOTAL[bus][2] = TOTAL[bus][2] + SAFE_FLOAT(LoadSummaryCalcData[5])
					M_SUM = M_SUM + SAFE_FLOAT(LoadSummaryCalcData[5])
					TOTAL[bus][3] = TOTAL[bus][3] + SAFE_FLOAT(LoadSummaryCalcData[6])
					N_SUM = N_SUM + SAFE_FLOAT(LoadSummaryCalcData[6])
					TOTAL[bus][4] = TOTAL[bus][4] + SAFE_FLOAT(LoadSummaryCalcData[7])
					O_SUM = O_SUM + SAFE_FLOAT(LoadSummaryCalcData[7])
					TOTAL[bus][5] = TOTAL[bus][5] + SAFE_FLOAT(LoadSummaryCalcData[8])
					P_SUM = P_SUM + SAFE_FLOAT(LoadSummaryCalcData[8])
					UPSDCCount = UPSDCCount + 1

			if UPSDCCount > 0:
				busfile.write('\n')
				LoadCount = LoadCount + UPSDCCount

			####################################################################################################
			TrCount = 0
			## TransformerÂÊÀ» Æ÷ÇÔÇÑ´Ù¸é...
			if rApplyTr == 'TRUE':
				## BUS¿¡ ¿¬°áµÈ Transformer¸¦ ±¸ÇÑ ÈÄ Transformer¿¡ ¿¬°áµÈ BUSÀÇ Load Summary°ªÀ» ÇÕÇÑ´Ù.
				TrNameList = ELoadApp.GetTrNameListInBus(bus)
				TrNameList.sort()
				for tr in TrNameList:
					trbus = ELoadApp.GetTransformerProp(tr , 'To' , 'Bus Id')
					#TrBusNameList = ELoadApp.GetConnectedBusNameListOfTr(tr)
					#for trbus in TrBusNameList:
					LoadSummaryCalcData = ELoadApp.GetLoadSummaryCalcDataOfBus(trbus)
					WriteLoadSummaryDataOfSpecialItem(busfile , 'TRANSFORMER' , trbus , '' , LoadSummaryCalcData)
					H_SUM = H_SUM + SAFE_FLOAT(LoadSummaryCalcData[0])
					I_SUM = I_SUM + SAFE_FLOAT(LoadSummaryCalcData[1])
					J_SUM = J_SUM + SAFE_FLOAT(LoadSummaryCalcData[2])
					TOTAL[bus][0] = TOTAL[bus][0] + SAFE_FLOAT(LoadSummaryCalcData[3])
					K_SUM = K_SUM + SAFE_FLOAT(LoadSummaryCalcData[3])
					TOTAL[bus][1] = TOTAL[bus][1] + SAFE_FLOAT(LoadSummaryCalcData[4])
					L_SUM = L_SUM + SAFE_FLOAT(LoadSummaryCalcData[4])
					TOTAL[bus][2] = TOTAL[bus][2] + SAFE_FLOAT(LoadSummaryCalcData[5])
					M_SUM = M_SUM + SAFE_FLOAT(LoadSummaryCalcData[5])
					TOTAL[bus][3] = TOTAL[bus][3] + SAFE_FLOAT(LoadSummaryCalcData[6])
					N_SUM = N_SUM + SAFE_FLOAT(LoadSummaryCalcData[6])
					TOTAL[bus][4] = TOTAL[bus][4] + SAFE_FLOAT(LoadSummaryCalcData[7])
					O_SUM = O_SUM + SAFE_FLOAT(LoadSummaryCalcData[7])
					TOTAL[bus][5] = TOTAL[bus][5] + SAFE_FLOAT(LoadSummaryCalcData[8])
					P_SUM = P_SUM + SAFE_FLOAT(LoadSummaryCalcData[8])
#
#					busfile.write(trbus + '\t')
#					busfile.write('\t\t\t\t\t\t\t')
#					if ('' != LoadSummaryCalcData[0]) and ('0' != LoadSummaryCalcData[0]):
#						busfile.write(str(LoadSummaryCalcData[0]) + '\t')
#						H_SUM = H_SUM + SAFE_FLOAT(LoadSummaryCalcData[0])
#					else:
#						busfile.write('\t')
#					if ('' != LoadSummaryCalcData[1]) and ('0' != LoadSummaryCalcData[1]):
#						busfile.write(str(LoadSummaryCalcData[1]) + '\t')
#						I_SUM = I_SUM + SAFE_FLOAT(LoadSummaryCalcData[1])
#					else:
#						busfile.write('\t')
#					if ('' != LoadSummaryCalcData[2]) and ('0' != LoadSummaryCalcData[2]):
#						busfile.write(str(LoadSummaryCalcData[2]) + '\t')
#						J_SUM = J_SUM + SAFE_FLOAT(LoadSummaryCalcData[2])
#					else:
#						busfile.write('\t')
#					if ('' != LoadSummaryCalcData[3]) and ('0' != LoadSummaryCalcData[3]):
#						busfile.write(str(LoadSummaryCalcData[3]) + '\t')
#						TOTAL[bus][0] = TOTAL[bus][0] + SAFE_FLOAT(LoadSummaryCalcData[3])
#						K_SUM = K_SUM + SAFE_FLOAT(LoadSummaryCalcData[3])
#					else:
#						busfile.write('\t')
#					if ('' != LoadSummaryCalcData[4]) and ('0' != LoadSummaryCalcData[4]):
#						busfile.write(str(LoadSummaryCalcData[4]) + '\t')
#						TOTAL[bus][1] = TOTAL[bus][1] + SAFE_FLOAT(LoadSummaryCalcData[4])
#						L_SUM = L_SUM + SAFE_FLOAT(LoadSummaryCalcData[4])
#					else:
#						busfile.write('\t')
#					if ('' != LoadSummaryCalcData[5]) and ('0' != LoadSummaryCalcData[5]):
#						busfile.write(str(LoadSummaryCalcData[5]) + '\t')
#						TOTAL[bus][2] = TOTAL[bus][2] + SAFE_FLOAT(LoadSummaryCalcData[5])
#						M_SUM = M_SUM + SAFE_FLOAT(LoadSummaryCalcData[5])
##					else:
#						busfile.write('\t')
#					if ('' != LoadSummaryCalcData[6]) and ('0' != LoadSummaryCalcData[6]):
#						busfile.write(str(LoadSummaryCalcData[6]) + '\t')
#						TOTAL[bus][3] = TOTAL[bus][3] + SAFE_FLOAT(LoadSummaryCalcData[6])
#						N_SUM = N_SUM + SAFE_FLOAT(LoadSummaryCalcData[6])
#					else:
#						busfile.write('\t')
#					if ('' != LoadSummaryCalcData[7]) and ('0' != LoadSummaryCalcData[7]):
#						busfile.write(str(LoadSummaryCalcData[7]) + '\t')
#						TOTAL[bus][4] = TOTAL[bus][4] + SAFE_FLOAT(LoadSummaryCalcData[7])
#						O_SUM = O_SUM + SAFE_FLOAT(LoadSummaryCalcData[7])
#					else:
#						busfile.write('\t')
#					if ('' != LoadSummaryCalcData[8]) and ('0' != LoadSummaryCalcData[8]):
#						busfile.write(str(LoadSummaryCalcData[8]) + '\t')
#						TOTAL[bus][5] = TOTAL[bus][5] + SAFE_FLOAT(LoadSummaryCalcData[8])
#						P_SUM = P_SUM + SAFE_FLOAT(LoadSummaryCalcData[8])
#					else:
#						busfile.write('\t')
#					busfile.write('\t\n')
					TrCount = TrCount + 1
				if TrCount > 0:
					busfile.write('\n')
					LoadCount = LoadCount + TrCount
			####################################################################################################
			
			busfile.write('TOTAL' + '\t\t')
			#if .0 != C_SUM:
			#	busfile.write(str('%.2f' % C_SUM) + '\t')
			#else:
			#	busfile.write('\t')
			busfile.write('\t') # UNIT
			if .0 != D_SUM:
				busfile.write(str('%.2f' % D_SUM) + '\t')
			else:
				busfile.write('\t')
			busfile.write('\t')

			if .0 != Q_SUM:
				busfile.write(str('%.2f' % Q_SUM) + '\t')
			else:
				busfile.write('\t')

			busfile.write('\t\t')

			# Æò±Õ°ªÀ» WRITEÇÑ´Ù.
			if .0 != H_SUM:
				busfile.write(str('%.2f' % (H_SUM / LoadCount)) + '\t')
			else:
				busfile.write('\t')
			if .0 != I_SUM:
				busfile.write(str('%.3f' % (I_SUM / LoadCount)) + '\t')
			else:
				busfile.write('\t')
			if .0 != J_SUM:
				busfile.write(str('%.3f' % (J_SUM / LoadCount)) + '\t')
			else:
				busfile.write('\t')
			
			if .0 != K_SUM:
				busfile.write(str('%.2f' % K_SUM) + '\t')
			else:
				busfile.write('\t')
			if .0 != L_SUM:
				busfile.write(str('%.2f' % L_SUM) + '\t')
			else:
				busfile.write('\t')
			if .0 != M_SUM:
				busfile.write(str('%.2f' % M_SUM) + '\t')
			else:
				busfile.write('\t')
			if .0 != N_SUM:
				busfile.write(str('%.2f' % N_SUM) + '\t')
			else:
				busfile.write('\t')
			if .0 != O_SUM:
				busfile.write(str('%.2f' % O_SUM) + '\t')
			else:
				busfile.write('\t')
			if .0 != P_SUM:
				busfile.write(str('%.2f' % P_SUM) + '\t')
			else:
				busfile.write('\t')

		except:
			ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
		
		busfile.close()

	# TOTALÀ» ±âÀÔÇÑ´Ù.
	WriteTotalLoadSummaryToDataFileA(BusNameList,TOTAL,rApplyTr)

	return "SUCCESS"

# brief		interface function
# author	BHK
#param		args
def PyWriteLoadSummaryToDataFile(args):
	WriteLoadSummaryToDataFileA(args)

# brief		excel¿¡¼­ ·Îµå µ¥ÀÌÅÍ¸¦ IMPORTÇÑ ÈÄÀÇ ÀÛ¾÷.
# author	BHK
#param		args
def PyPostImportLoadDataFromExcel(args):
	if type(args) is type([]) and (len(args) == 1):
		LoadID = args[0]
		
		loadCategory = ELoadApp.GetLoadProp(LoadID,'Type','Load Category')
		loadCategory = loadCategory.upper()
		if 'L' == loadCategory:
			ELoadApp.SetLoadProp(LoadID,'Type','Load Category' , 'LTG')
			ELoadApp.SetLoadOriginalProp(LoadID,'Type','Load Category' , 'LTG')
		elif 'M' == loadCategory:
			ELoadApp.SetLoadProp(LoadID,'Type','Load Category' , 'MOTOR')
			ELoadApp.SetLoadOriginalProp(LoadID,'Type','Load Category' , 'MOTOR')
		elif 'O' == loadCategory:
			ELoadApp.SetLoadProp(LoadID,'Type','Load Category' , 'OTHERS')
			ELoadApp.SetLoadOriginalProp(LoadID,'Type','Load Category' , 'OTHERS')
		elif 'W' == loadCategory:
			ELoadApp.SetLoadProp(LoadID,'Type','Load Category' , 'WELDING')
			ELoadApp.SetLoadOriginalProp(LoadID,'Type','Load Category' , 'WELDING')
		elif 'I' == loadCategory:
			ELoadApp.SetLoadProp(LoadID,'Type','Load Category' , 'INST.')
			ELoadApp.SetLoadOriginalProp(LoadID,'Type','Load Category' , 'INST.')	
		elif 'C' == loadCategory:
			ELoadApp.SetLoadProp(LoadID,'Type','Load Category' , 'COMM.')
			ELoadApp.SetLoadOriginalProp(LoadID,'Type','Load Category' , 'COMM.')			
		elif 'S' == loadCategory:
			ELoadApp.SetLoadProp(LoadID,'Type','Load Category' , 'SubBus')
			ELoadApp.SetLoadOriginalProp(LoadID,'Type','Load Category' , 'SubBus')

		operMode = ELoadApp.GetLoadProp(LoadID,'Type','Operation Mode')
		operMode = operMode.upper()
		if 'C' == operMode:
			ELoadApp.SetLoadProp(LoadID,'Type','Operation Mode','CONTINUOUS')
			ELoadApp.SetLoadOriginalProp(LoadID,'Type','Operation Mode','CONTINUOUS')
		elif 'I' == operMode:
			ELoadApp.SetLoadProp(LoadID,'Type','Operation Mode','INTERMITTENT')
			ELoadApp.SetLoadOriginalProp(LoadID,'Type','Operation Mode','INTERMITTENT')
		elif 'S' == operMode:
			ELoadApp.SetLoadProp(LoadID,'Type','Operation Mode','STAND-BY')
			ELoadApp.SetLoadOriginalProp(LoadID,'Type','Operation Mode','STAND-BY')

		PF100  = SAFE_FLOAT(ELoadApp.GetLoadProp(LoadID,'Characteristic','PF(100% Load)'))
		if (0. != PF100):
			ELoadApp.SetLoadProp(LoadID,'Characteristic','PF LOCK','Yes')
			ELoadApp.SetLoadOriginalProp(LoadID,'Characteristic','PF LOCK','Yes')
		else:
			ELoadApp.SetLoadProp(LoadID,'Characteristic','PF LOCK','No')
			ELoadApp.SetLoadOriginalProp(LoadID,'Characteristic','PF LOCK','No')
	
		EFF100 = SAFE_FLOAT(ELoadApp.GetLoadProp(LoadID,'Characteristic','EFF(100% Load)'))
		if (0. != EFF100):
			ELoadApp.SetLoadProp(LoadID,'Characteristic','EFF LOCK','Yes')
			ELoadApp.SetLoadOriginalProp(LoadID,'Characteristic','EFF LOCK','Yes')
		else:
			ELoadApp.SetLoadProp(LoadID,'Characteristic','EFF LOCK','No')
			ELoadApp.SetLoadOriginalProp(LoadID,'Characteristic','EFF LOCK','No')

		busname = ELoadApp.GetLoadProp(LoadID , 'Panel' , 'Bus ID')
		if '' != busname:
			# BUS°¡ ¼³Á¤µÇ¾î ÀÖÀ¸¸é BUSÀÇ Load Voltage¿Í PhaseÀÇ °ªÀ» °¡Á®¿Í¼­ °¢°¢ Rated Voltage,Phase¿¡ ¼³Á¤ÇÑ´Ù.
			LoadVoltage = ELoadApp.GetBusProp(busname , 'General', 'Load Voltage')
			Phase = ELoadApp.GetBusProp(busname , 'General', 'Phase')
			ELoadApp.SetLoadProp(LoadID,'Rating','Rated Voltage',LoadVoltage)
			ELoadApp.SetLoadOriginalProp(LoadID,'Rating','Rated Voltage',LoadVoltage)
			ELoadApp.SetLoadProp(LoadID,'Rating','Phase',Phase)
			ELoadApp.SetLoadOriginalProp(LoadID,'Rating','Phase',Phase)

		# Rating Capacity´Â ¼Ò¼öÁ¡ 2 BHPÀÇ °ªÀ» ¼Ò¼öÁ¡ 3ÀÚ¸®±îÁö Ç¥ÇöÇÑ´Ù.
		RatingCapacity = SAFE_FLOAT(ELoadApp.GetLoadRatingCapacity(LoadID))
		ELoadApp.SetLoadProp(LoadID , 'Rating','Rating Capacity' , '%.2f' % RatingCapacity)
		ELoadApp.SetLoadOriginalProp(LoadID , 'Rating','Rating Capacity' , '%.2f' % RatingCapacity)
		BHP = ELoadApp.GetLoadProp(LoadID,'Load','BHP <1>')
		if '' != BHP:
			BHP = SAFE_FLOAT(BHP)
			ELoadApp.SetLoadProp(LoadID,'Load','BHP <1>' , '%.3f' % BHP)
			ELoadApp.SetLoadOriginalProp(LoadID,'Load','BHP <1>' , '%.3f' % BHP)

		ELoadApp.SetLoadProp(LoadID,'Circuit Breaker','LOCK','No')
		ELoadApp.SetLoadOriginalProp(LoadID,'Circuit Breaker','LOCK','No')		
		ELoadApp.SetLoadProp(LoadID,'Control','LOCK','No')
		ELoadApp.SetLoadOriginalProp(LoadID,'Control','LOCK','No')		
		ELoadApp.SetLoadProp(LoadID,'Control Cable','LOCK','No')
		ELoadApp.SetLoadOriginalProp(LoadID,'Control Cable','LOCK','No')		
	
		return 'SUCCESS'

	return 'FAIL'

# brief		Calculate FLC
# author	BHK
# param		loadname
def PyCalculateFLC(loadname):
	res = '0'

	Phase = ELoadApp.GetLoadProp(loadname,'Rating','Phase')
	Rating_Capacity = SAFE_FLOAT(ELoadApp.GetLoadRatingCapacity(loadname))
	Rated_Voltage   = SAFE_FLOAT(ELoadApp.GetLoadProp(loadname,'Rating','Rated Voltage'))
	PF100  = SAFE_FLOAT(ELoadApp.GetLoadProp(loadname,'Characteristic','PF(100% Load)'))
	EFF100 = SAFE_FLOAT(ELoadApp.GetLoadProp(loadname,'Characteristic','EFF(100% Load)'))
	try:
		Rating_Capacity_Unit = ELoadApp.GetLoadPropUnitStr(loadname,'Rating','Rating Capacity')
		if '3' == Phase:
			if 'KVA' == Rating_Capacity_Unit:
				res = (Rating_Capacity / (math.sqrt(3)*Rated_Voltage))
			else:
				res = (Rating_Capacity / (math.sqrt(3)*Rated_Voltage*PF100*EFF100))
				
			res = str(round(res,2))

		elif '1' == Phase:
			if 'KVA' == Rating_Capacity_Unit:
				res = (Rating_Capacity / (Rated_Voltage))
			else:
				res = (Rating_Capacity / (Rated_Voltage*PF100*EFF100))			
				
			res = str(round(res,2))
		elif 'DC' == Phase:	# DC´Â PF°¡ ¾ø´Ù.
			if 'KVA' == Rating_Capacity_Unit:
				res = (Rating_Capacity / (Rated_Voltage))				
			else:
				res = (Rating_Capacity / (Rated_Voltage*EFF100))
				
			res = str(round(res,2))
	except:
		pass

	return res

def selftest():
    print calcLoadSummary(["ddd",11,0.5,11])

if __name__ == "__main__":
    selftest()
