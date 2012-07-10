from distutils.core import setup
import sys, os, py2exe

sys.argv.extend(['--packages','win32com'])

setup(
# 	console=['LogMsgToELOAD.py','ELOAD.py'],
	scripts=['ELOAD.py','LoadSummary.py','BusDiagram.py','CableSizing.py','CableCreation.py','Generator.py','CableRoute.py','TransformerSizing.py'],
	)
