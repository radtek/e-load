#! -*- Encoding: Latin-1 -*-

import ELoadApp
import sys
import os
import locale
import math
import traceback

locale.setlocale(locale.LC_ALL, "")

#def reloadall(): 
#	import sys 
#	modsreloaded = {} 
#	for sym, obj in globals().items(): 
#		if hasattr(obj, '__module__'): 
#			mod = obj.__module__ 
#			if mod.startswith('__'): 
#				continue 
#			if mod not in modsreloaded: 
#				modsreloaded[mod] = reload(sys.modules[obj.__module__]) 
#				print "Reloaded %s module." % mod 
#			globals()[sym] = getattr(modsreloaded[mod], sym)

#try:
#	reloadall()
#except:
#	traceback.print_exc(file=open('c:\\test.at','a'))

# brief	
# author	BHK
def PyLogMsgToELOAD(msg):
	try:
		ELoadApp.AppendLogToOutputWnd('bwhy this is not run!!!')
	except:
		traceback.print_exc(file=open('c:\\test.at','a'))

