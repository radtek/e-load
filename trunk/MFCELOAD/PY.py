import win32com.client
xl = win32com.client.gencache.EnsureDispatch ('Excel.Application')
wb = xl.Workbooks.Open('D:\\Project\\SolutionWare\\Projects\\2009.01.e-LOAD\\MFCELOAD\\PY.XLS')
ws = wb.ActiveSheet
xl.Visible = True
wb.Worksheets('BUS_TEMPLATE').Copy(Before=wb.Worksheets(wb.Worksheets.Count))
ws.Cells(1,1).Value='aaa'
ws.Cells(1, 1).Font.Size = 10
ws.Cells(1,1).Interior.ColorIndex = 6
dot = win32com.client.gencache.EnsureDispatch('Wingraphviz.dot')

dotfile = open('.\\PY.DOT','r')
lines = dotfile.readlines()
dotfile.close()

totalines = ''
for x in lines:
	totalines = totalines + x

img = dot.ToJPEG(totalines)
img.Save('.\\PY.JPG')
del img
