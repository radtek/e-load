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

# brief		diagram에서 시작이 되는 bus들을 구한다.(INCOMING BUS GROUP에 속한 BUS들.)
# author	BHK 
# date		2009-05-29 10:53:19 
# return	bus name list
def PyGetStartBus():
	res = ELoadApp.GetBusNameListInBusGroup('INCOMING')
	return res

# brief		bus에 연결된 UPS/DC Name을 구한다.
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

# brief		bus에 연결된 Transformer Name을 구한다.
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

# brief		bus에 연결된 SubBUS BUS Name을 구한다.
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

# brief		connection이 이미 그려졌는지 검사한다.
# author	humkyung
# date		2012-09-03 16:31:12 
# return	bool
def IsAlreadyDrawn(connection , drawed_bus_list):
	bFound = False

	for drawn in drawed_bus_list:
		if drawn == connection:
			bFound = True
			break
	
	return bFound

# brief		bus에 연결된 diagram을 그린다.
# author	BHK
# date		2009-05-29 10:57:46
# return	none
def PyGenerateSubDiagramOf(contents , bus , drawed_bus_list):
	# Bus에 연결된 UPS/DC들을 구한다.
	UPSDCNameList = PyGetUPSDCNameConnectedToBus(bus)
	if type(UPSDCNameList) == type([]) and len(UPSDCNameList) > 0:
		for upsdc in UPSDCNameList:
			if False == IsAlreadyDrawn(bus + '->' + upsdc , drawed_bus_list):
				contents = contents + '"' + str(upsdc) +'"[label = "' + str(upsdc) + '",style=filled,fillcolor=green,shape=doubleoctagon];' + '\n'
				contents = contents + '"' + str(bus) + '"'+ '->' + '"' + str(upsdc) + '"' + '\n'
				drawed_bus_list.append(str(bus) + '->' + str(upsdc))

			# UPS/DC에 연결된 BUS를 그린다.
			ConnectedBusID = ELoadApp.GetELoadItemProp('UPSDC' , upsdc , 'To' , 'Bus ID')
			if (ConnectedBusID != ''):
				if False == IsAlreadyDrawn(upsdc + '->' + ConnectedBusID , drawed_bus_list):
					contents = contents + '"' + str(ConnectedBusID) + '"[label="'+ str(ConnectedBusID) + '",shape=box];' + '\n'
					contents = contents + '"' + str(upsdc) + '"'+ '->' + '"' + str(ConnectedBusID) + '"' + '\n'
					drawed_bus_list.append(str(upsdc) + '->' + str(ConnectedBusID))

					contents = PyGenerateSubDiagramOf(contents , ConnectedBusID , drawed_bus_list)
				
	# Bus에 연결된 Transformer들을 구한다.
	TrNameList = PyGetTrNameConnectedToBus(bus)
	if type(TrNameList) == type([]) and len(TrNameList) > 0:
		for tr in TrNameList:
			pos = tr.find('DEL ')
			if 0 != pos:
				if False == IsAlreadyDrawn(bus + '->' + tr , drawed_bus_list):
					contents = contents + '"' + str(tr) +'"[label = "' + str(tr) + '",style=filled,fillcolor=yellow,shape=ellipse];' + '\n'
					contents = contents + '"' + str(bus) + '"'+ '->' + '"' + str(tr) + '"' + '\n'
					drawed_bus_list.append(str(bus) + '->' + str(tr))

				ConnectedBusID = ELoadApp.GetTransformerProp(tr , 'To' , 'Bus Id')
				if ConnectedBusID != '':
					if False == IsAlreadyDrawn(tr + '->' + ConnectedBusID , drawed_bus_list):
						contents = contents + '"' + str(ConnectedBusID) + '"[label="'+ str(ConnectedBusID) + '",shape=box];' + '\n'
						contents = contents + '"' + str(tr) + '"'+ '->' + '"' + str(ConnectedBusID) + '"' + '\n'
						drawed_bus_list.append(str(tr) + '->' + str(ConnectedBusID))

						contents = PyGenerateSubDiagramOf(contents , ConnectedBusID , drawed_bus_list)
			else:
				# don't display virtual transformer. display directly bus to bus. - 2011.01.06 added by HumKyung
				ConnectedBusID = ELoadApp.GetTransformerProp(tr , 'To' , 'Bus Id')
				if ConnectedBusID != '':
					if False == IsAlreadyDrawn(bus + '->' + ConnectedBusID , drawed_bus_list):
						contents = contents + '"' + str(ConnectedBusID) + '"[label="'+ str(ConnectedBusID) + '",shape=box];' + '\n'
						contents = contents + '"' + str(bus) + '"'+ '->' + '"' + str(ConnectedBusID) + '"' + '\n'
						drawed_bus_list.append(str(bus) + '->' + str(ConnectedBusID))

						contents = PyGenerateSubDiagramOf(contents , ConnectedBusID , drawed_bus_list)
	
	# Bus에 연결된 PDB BUS들을 구한다.
	PDBBusNameList = PyGetPDBNameConnectedToBus(bus)
	if type(PDBBusNameList) == type([]) and len(PDBBusNameList) > 0:
		for pdb in PDBBusNameList:
			if False == IsAlreadyDrawn(bus + '->' + pdb , drawed_bus_list):
				contents = contents + '"' + str(pdb) +'"[label = "' + str(pdb) + '",shape=box];' + '\n'
				contents = contents + '"' + str(bus) + '"'+ '->' + '"' + str(pdb) + '"' + '\n'
				drawed_bus_list.append(str(bus) + '->' + str(pdb))

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
			
		#* EQUIPMENT와 연결되지 않은 BUS들을 그린다.
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

