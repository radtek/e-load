#! -*- Encoding: Latin-1 -*-

import ELoadApp
import sys
import os
import locale
import math
import win32com.client
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

# brief		PG1°ªÀ» °è»êÇÑ´Ù.
# author	BHK
def PyCalculatePG1(argv):
	try:
		WL = SAFE_FLOAT(argv[0])
		L  = SAFE_FLOAT(argv[1])
		COS= SAFE_FLOAT(argv[2])

		PG1 = round((WL * L) / COS , 2)
		return str(PG1)
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
		return str('')

	return str('')

# brief		PG2°ªÀ» °è»êÇÑ´Ù.
# author	BHK
def PyCalculatePG2(argv):
	try:
		PLmax = SAFE_FLOAT(argv[0])
		V  = SAFE_FLOAT(argv[1])
		Xd1= SAFE_FLOAT(argv[2])
		Xd2= SAFE_FLOAT(argv[3])

		PG2 = round(PLmax * ((Xd1 + Xd2) / 2.) * ((1 - V) / V) , 2)
		return str(PG2)
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
		return str('')

	return str('')

# brief		PG3°ªÀ» °è»êÇÑ´Ù.
# author	BHK
def PyCalculatePG3(argv):
	try:
		W0    = SAFE_FLOAT(argv[0])
		Qlmax = SAFE_FLOAT(argv[1])
		COSQl = SAFE_FLOAT(argv[2])
		K     = SAFE_FLOAT(argv[3])
		COSQg = SAFE_FLOAT(argv[4])

		PG3 = round((W0 + (Qlmax * COSQl)) / (K * COSQg),2)
		return str(PG3)
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
		return str('')

	return str('')
