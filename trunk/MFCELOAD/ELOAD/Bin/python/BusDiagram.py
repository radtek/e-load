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

# brief		read a dot file and convert to jpg.
# author	BHK
def PyConvertDOTToJPEG(dotfile,jpegfile):
	# NOTICE : below code happens error when it is called in thread.
	dot = win32com.client.gencache.EnsureDispatch('Wingraphviz.dot')

	dotfile = open(dotfile,'r')
	lines = dotfile.readlines()
	dotfile.close()

	totalines = ''
	for x in lines:
		totalines = totalines + x
	
#	str = dot.ToSvg(totalines)
#	f = open('d:\\a.svg' , 'w')
#	f.write(str)
#	f.close()

	img = dot.ToJPEG(totalines)
	img.Save(jpegfile)
	del img

# brief		diagram¿¡¼­ ½ÃÀÛÀÌ µÇ´Â busµéÀ» ±¸ÇÑ´Ù.(INCOMING BUS GROUP¿¡ ¼ÓÇÑ BUSµé.)
# author	BHK 
# date		2009-05-29 10:53:19 
# return	bus name list
def PyGetStartBus():
	res = ELoadApp.GetBusNameListInBusGroup('INCOMING')
	return res

# brief		bus¿¡ ¿¬°áµÈ UPS/DC NameÀ» ±¸ÇÑ´Ù.
# author	BHK 
# date		2009-06-15 10:02:51  
# return	UPS/DC name list
def PyGetUPSDCNameConnectedToBus(bus):
	res = []
	UPSDCItemList = ELoadApp.GetELoadItemListOf('UPSDC')
	for upsdc in UPSDCItemList:
		BusID1 = ELoadApp.GetELoadItemProp('UPSDC' , upsdc , 'From1' , 'Bus ID')
		BusID2 = ELoadApp.GetELoadItemProp('UPSDC' , upsdc , 'From2' , 'Bus ID')
		if (bus == BusID1) or (bus == BusID2):
			res.append(upsdc)

	return res

# brief		bus¿¡ ¿¬°áµÈ Transformer NameÀ» ±¸ÇÑ´Ù.
# author	BHK 
# date		2009-05-29 09:57:34 
# return	transformer name list
def PyGetTrNameConnectedToBus(bus):
	res = []
	TrNameList = ELoadApp.GetTransformerNameList()
	for tr in TrNameList:
		ConnectedBusID = ELoadApp.GetTransformerProp(tr , 'From' , 'Bus Id')
		if bus == ConnectedBusID:
			res.append(tr)

	return res

# brief		bus¿¡ ¿¬°áµÈ SubBUS BUS NameÀ» ±¸ÇÑ´Ù.
# author	BHK 
# date		2009-06-01 11:35:24 
# return	SubBUS bus name list
def PyGetPDBNameConnectedToBus(bus):
	res = []
	LoadNameList = ELoadApp.GetLoadNameListInBus(bus)
	for load in LoadNameList:
		LoadCategory = ELoadApp.GetLoadProp(load , 'Type' , 'Load Category')
		ConnectedBusID = ELoadApp.GetLoadProp(load , 'Panel' , 'Connect Bus')
		if ('SubBUS' == LoadCategory) and (ConnectedBusID != ''):
			res.append(ConnectedBusID)

	return res

# brief		bus¿¡ ¿¬°áµÈ diagramÀ» ±×¸°´Ù.
# author	BHK
# date		2009-05-29 10:57:46
# return	none
def PyGenerateSubDiagramOf(contents , bus , drawed_bus_list):
	# Bus¿¡ ¿¬°áµÈ UPS/DCµéÀ» ±¸ÇÑ´Ù.
	UPSDCNameList = PyGetUPSDCNameConnectedToBus(bus)
	if type(UPSDCNameList) == type([]) and len(UPSDCNameList) > 0:
		for upsdc in UPSDCNameList:
			contents = contents + '"' + str(upsdc) +'"[label = "' + str(upsdc) + '",style=filled,fillcolor=green,shape=doubleoctagon];' + '\n'
			contents = contents + '"' + str(bus) + '"'+ '->' + '"' + str(upsdc) + '"' + '\n'

			# UPS/DC¿¡ ¿¬°áµÈ BUS¸¦ ±×¸°´Ù.
			ConnectedBusID = ELoadApp.GetELoadItemProp('UPSDC' , upsdc , 'To' , 'Bus ID')
			if (ConnectedBusID != ''):
				bFound = False
				for upsdcbus in drawed_bus_list:
					if upsdcbus == ConnectedBusID:
						bFound = True
						break
				# ÀÌ¹Ì ±×·È´Ù¸é »ý·«.
				if bFound == False:
					contents = contents + '"' + str(ConnectedBusID) + '"[label="'+ str(ConnectedBusID) + '",shape=box];' + '\n'
					contents = contents + '"' + str(upsdc) + '"'+ '->' + '"' + str(ConnectedBusID) + '"' + '\n'
					drawed_bus_list.append(str(ConnectedBusID))

					contents = PyGenerateSubDiagramOf(contents , ConnectedBusID , drawed_bus_list)
				
	# Bus¿¡ ¿¬°áµÈ TransformerµéÀ» ±¸ÇÑ´Ù.
	TrNameList = PyGetTrNameConnectedToBus(bus)
	if type(TrNameList) == type([]) and len(TrNameList) > 0:
		for tr in TrNameList:
			pos = tr.find('DEL ')
			if 0 != pos:
				contents = contents + '"' + str(tr) +'"[label = "' + str(tr) + '",style=filled,fillcolor=yellow,shape=ellipse];' + '\n'
				contents = contents + '"' + str(bus) + '"'+ '->' + '"' + str(tr) + '"' + '\n'

				ConnectedBusID = ELoadApp.GetTransformerProp(tr , 'To' , 'Bus Id')
				if ConnectedBusID != '':
					contents = contents + '"' + str(ConnectedBusID) + '"[label="'+ str(ConnectedBusID) + '",shape=box];' + '\n'
					contents = contents + '"' + str(tr) + '"'+ '->' + '"' + str(ConnectedBusID) + '"' + '\n'
					drawed_bus_list.append(str(ConnectedBusID))

					contents = PyGenerateSubDiagramOf(contents , ConnectedBusID , drawed_bus_list)
			else:
				# don't display virtual transformer. display directly bus to bus. - 2011.01.06 added by HumKyung
				ConnectedBusID = ELoadApp.GetTransformerProp(tr , 'To' , 'Bus Id')
				if ConnectedBusID != '':
					contents = contents + '"' + str(ConnectedBusID) + '"[label="'+ str(ConnectedBusID) + '",shape=box];' + '\n'
					contents = contents + '"' + str(bus) + '"'+ '->' + '"' + str(ConnectedBusID) + '"' + '\n'
					drawed_bus_list.append(str(ConnectedBusID))

					contents = PyGenerateSubDiagramOf(contents , ConnectedBusID , drawed_bus_list)
	
	# Bus¿¡ ¿¬°áµÈ PDB BUSµéÀ» ±¸ÇÑ´Ù.
	PDBBusNameList = PyGetPDBNameConnectedToBus(bus)
	if type(PDBBusNameList) == type([]) and len(PDBBusNameList) > 0:
		for pdb in PDBBusNameList:
			contents = contents + '"' + str(pdb) +'"[label = "' + str(pdb) + '",shape=box];' + '\n'
			contents = contents + '"' + str(bus) + '"'+ '->' + '"' + str(pdb) + '"' + '\n'

			drawed_bus_list.append(str(pdb))

			contents = PyGenerateSubDiagramOf(contents , pdb , drawed_bus_list)

	return contents

# brief		generate a dot file which contains bus diagram.
# author	BHK
# return	none
def PyGenerateBusDiagram(argv):
	try:
		strExecPath = ELoadApp.GetExecPath()
		if '\\' != strExecPath[len(strExecPath) - 1]:
			strExecPath = strExecPath + '\\'
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())
	
	try:
		dotfile = open(strExecPath + 'temp\BusDiagram.dot' , 'w')
		contents = 'digraph G {' + '\n'
		#contents = contents + 'graph[\n rankdir = "LR"\n ];\n'
		contents = contents + 'node [shape = record,height=.1,fontsize=10, shape = box, fontname="Arial"];\n'
		#contents = contents + 'BusDiagram\n'

		drawed_bus_list = []
		StartBusNameList = PyGetStartBus()	# get all start bus name
		for bus in StartBusNameList:
			#if 'Not Assigned' == busgroup:
			#	continue

			contents = contents + '"' + str(bus) + '"[label="'+ str(bus) + '",shape=box];' + '\n'
			drawed_bus_list.append(str(bus))

			contents = PyGenerateSubDiagramOf(contents , bus , drawed_bus_list)
			
		#* EQUIPMENT¿Í ¿¬°áµÇÁö ¾ÊÀº BUSµéÀ» ±×¸°´Ù.
		BusNameList = ELoadApp.GetBusNameList('ALL')
		label = ''
		for bus in BusNameList:
			drawed = False
			for drawed_bus in drawed_bus_list:
				if drawed_bus == bus:
					drawed = True
					break
			if False == drawed:
				contents = contents + '"' + str(bus) + '"[label="'+ str(bus) + '",shape=box];' + '\n'
				drawed_bus_list.append(str(bus))

				contents = PyGenerateSubDiagramOf(contents , bus , drawed_bus_list)
				#label = label + str(bus) + '|'
		#label = label[0:len(label)-1]
		#if label != '':
		#	contents = contents + 'node [shape=record];'
		#	contents = contents + 'struct1 [label="' + label + '",style=filled,fillcolor=gray];'

		contents = contents + '}' + '\n'
		
		dotfile.write(contents)
		dotfile.close()

		PyConvertDOTToJPEG(strExecPath + 'temp\BusDiagram.dot' , strExecPath + 'temp\BusDiagram.jpg')
	except:
		ELoadApp.AppendLogToOutputWnd(traceback.format_exc())

