1.1.3.363: 2014.03.24
	1. 프로젝트 생성시 Voltage 저장 오류 수정
	2. Cable Data Import File Dialog 확장자에 .xlsx 추가

1.1.3.362: 2014.03.18
	1. #12:Load Voltage 표기 방법 수정

1.1.3.360: 2012.09.03
	1. Bus To Bus Diagram 오류 수정

1.1.3.359: 2012.07.10
	1. BusToBus 기능에서 사용자 실수를 유발할 수 있는 부분이 존재하여 From, To를 동일 Bus를 지정하지 못하도록 프로그램 수정
	2. 엑셀 템플릿 수정

1.1.2.358: 2012.01.17
	1. Load Summary 결과값에서 Remark란에 Location-Area가 나오던것을 Remark가 나오도록 수정

1.1.1.357: 2012.01.13
	1. 매뉴얼 수정(이윤영)

1.1.1.355: 2011.10.31
	1. MS Office2010 지원

1.1.1.353: 2011.07.18
	1. TEMPLATE_LOAD_SUMMARY_PDB , TEMPLATE_LOAD_SUMMARY template excel file 변경(이윤영)

1.1.1.352: 2011.07.18
	1. BHP , LF의 자릿수를 소수점 2째에서 3째로 변경
	2. Transformer Sizing Report시 Transformer Item의 Capacity 를 계산하도록 함.

1.1.0.348: 2011.07.14
	1. 매뉴얼 수정(이윤영)

1.1.0.347: 2011.05.16
	1. Revision 저장 오류 수정

1.1.0.346: 2011.02.25
	1. Transformer를 생성한 후 바로 To Bus를 선택시 List에 표기되지 않는 현상 수정
	2. 메뉴얼 수정 - 이윤영
	3. 배포판 설치 프로젝트 변경 - 이윤영

1.1.0.345: 2011.02.24
	1. LV Cable Sizng Table 계산시 결과물에 Sheet가 나타나지 않고 있음.

1.1.0.344: 2011.02.24
	1. Transformer Property Dialog에서 연결된 Bus를 제대로 보여주지 못함
	2. 기본 Conduit 테이블이 화면에 표시되나 실제 Conduit 테이블이 로딩되지 않아 계산에서 오류 발생
	3. Transformer Property Dialog Tab Order 변경

1.1.0.343: 2011.02.23
	1. Load Summary 결과 오류 수정(SubBus)
	2. 처음으로 3PH Motor 실행하려면 하면 OK활성화가 안됨
	3. 3PH MOTOR를 제외한 나머지 Starting VD, Staring PF를 N/A로 표현했는데 화면 결과에도 N/A표기요
	4. 모든 계산 작성시 COND_IEC 기본 선택으로 할 것
	5. 일부 값 변경후 재 계산시 화면에 모두 clear하고 계산결과을 표기하도록

1.0.9.342: 2011.02.18
	1. TR.에서 To Bus 처음 지정시 아래 List에 반영이 되는데 변경시는 반영이 않됨.
	2. 000001 : TR. Property시 에러
	3. 000000 : Load Summary-SubBus 이상함.
	4. SEELECT MOTOR TYPE를 SELECT LOAD TYPE으로 이름 변경, 빨간색 글자+진하게 (COMBO도 적용요)
	5. 3PH MOTOR제외한 나머지는 STARTING VD, STARTING PF 비활성화 (계산에 사용되지 않음)
	6. 모든 계산작성시 COND_IEC default 선택으로 할 것
	7. 용량부분 사용자가 입력한 값이 SORTING되어 계산에 적용되어야 함.
	8. 3PH MOTOR는 용량부분은 KVA가 아닌 KW로 표기
	9. 3PH MOTOR를 제외한 나머지는 STARTING VD, STARING PF비적용이므로 N/A로 표기
	10. 사용자가 입력한 값이 SORTING되어 나타나도록
	11. COMMON_LIB 파일적용

1.0.8.341: 2011.02.12
	1. PyCalculateCableLength 함수 수정

1.0.8.340: 2011.02.12
	1. Excel import화면에서 파일 선택의 확장자에 xls, xlxs가 동시에 나타나게
	2. 기존 생성 Project에 Bus Gr에 등록되어 있던 Bus List 가지고 오기 (TR. Capacity에서)
	3. 기존 생성Project에 SubBUS가 삭제가 안되는데 어떻게 삭제하는지?
	4. Bus Gr. 삭제 불가능하도록 수정요
	5. TR.에서 From Bus 선택 및 수정시 아래 Bus List에 바로 반영이 되도록
	6. TR. 변경사항은 Bus Diagram에 바로 반영이 되도록 수정요
	7. TR. Sizing Report에 DEL-TR 삭제 요망
	8. TR. Sizing Report는 Bus Gr.이 없으므로 TR. Capcity를 위한 Bus List에 등록 Bus 기준으로 계산한다

1.0.7.336: 2011.02.10
	1. Excel import화면에서 select start row 기능이 적용되지 않고 있음 .(첨부 파일로 테스트요)
	2. Excel import화면에서 파일 선택의 확장자에 xlsx도 포함
	3. Bus Gr.  기본 생성 : HV, LV, HV, LV가 중복되어 있음(INCOMING, HV, MV, LV, SubBus 4가지로 고정해 주세요.)
	4. 생성한 Bus가 Bus Diagram, TR. From, To에 나타나지 않음
	5. Project Voltage화면에 [kV]를 표시요

1.0.6.334: 2011.02.09
	1. Tr. Capacity Property Dialog의 폭 넓힘
	2. Uninstall시 Shortcut 메뉴가 제거되지 않는 현상 수정

1.0.6.333: 2011.02.08
	1. Tr. Capacity에 Bus Gr. 개념 삭제
	2. Bus Group을 생성하지 못하도록 한다.
	3. Bus Gr.은 현재처럼 Incoming, HV, LV, SubBus로 고정하도록 한다.

1.0.5.330: 2011.01.28
	1. VC++ 2008 재배포판 설치 문제 수정

1.0.5.329: 2011.01.28
	1. 삭제된 Bus가 Load Summary 계산 결과에 Tab으로 표현되는 현상 수정
	2. Load Summary 계산 결과 Tab이 Bus Order순으로 표시되지 않는 현상 수정
	3. Install에 VC++ 2008 재배포판 추가
	4. Bus Diagram과 Bus To Bus를 한 화면으로 통합

1.0.5.312: 2011.01.25
	1. Bus To Bus화면에서 Save 버튼을 이동시킬 것
	2. Assign Load To Bus화면에서 Load Voltage 항목을 ListCtrl에 추가할 것
	3. Assign Load To Bus화면에서 오른쪽 ListCtrl에 Assign했을때 Load Voltage값이 적용되지 않음

1.0.5.305: 2011.01.10
	1. Bus To Bus 폼에서 Bus ComboBox의 내용을 소팅
	2. 매뉴얼 수정
	3. LOAD.PROP 수정(Load Category에 SubBUS추가) - 이윤영

1.0.5.297: 2011.01.06
	1. Delete 오류 수정 - Bus To Bus
	2. Bus Diagram에서 가상 transformer 삭제
	3. Bus To Bus의 Grid column 넓힘

1.0.5.294:
	1. Bus To Bus 설정 모듈을 그리드형식으로 변경

1.0.5.260:
	1. Bus To Bus 설정 모듈 추가

1.0.4.248:
	1. cable table importing 오류 수정
	2. e-LOAD manual 추가(F1 , Manual 폴더)

1.0.4.244:
	1. Bus의 Load Summary 값과 Transformer Sizing에의 값이 상이한 오류 수정

1.0.4.239:
	1. BusDiagram 이미지를 오픈하는 기능 추가
	2. 좌측 트리 아이템 변경 - 이윤영
	3. LoadSummary의 SubBUS의 total이 엑셀에 출력되지 않는 오류 수정

1.0.4.235:
	1. PG2 계산 오류 수정
	2. 설치 시 기존의 ELOAD.MDB 파일이 있으면 ELOAD-BACKUP.MDB 파일로 변경한다.

1.0.4.231:
	1. MAC address를 구하는 루틴 수정

1.0.4.223:
	1. Transformer Data 엑셀 파일 출력 오류 수정
	2. LoadSummary의 SubBUS 데이터 리비젼 처리 오류 수정

1.0.4.217:
	1. Default Server Folder 설정	
	2. Report의No.는 TR.ID 기준으로만 기입되도록 (첨부파일 참조)
	3. Calculation Result 항목들을 클릭시 마우스 모래시계 나타나도록 반영요
	4. 수정한 파일을 첨부하오니 설치본에 Template 파일을 변경

1.0.4.x:
	1. PDB를 SubBUS로 변경
	2. L3에서 PDB를 SubBUS로 변경하고 SubBUS관련 BUS를 읽어 오도록 한다.
	3. 삭제한 프로젝트의 폴더명을 'DELETED - 프로젝트 명'으로 한다.
	4. LoadSummary StartUp Dialog에서 Continous,Intermittent,Stand-By의 값을 설정후 저장되지 않음.
	5. LoadSummary후 SubBUS의 내용을 볼수 없음.
	6. TR SIZING RESULT 생성 루틴 수정
	7. Export Load Data 기능 추가
	8. Export Load Data ICON을 리본바에 표시
	9. Login Dialog 수정(ADMIN/USER , ADMIN일 경우에 암호를 입력하도록 함.)
	10.Property Window가 겹쳐지는 현상 수정
	11.Excel Export 에러 수정
	12.Transformer,Capacity,Generator,UPC/DC 관련 UI 수정
	13.New Project시 INCOMING, SubBus만 Default로 생성되어 있는데
           Incoming , HV , LV, SubBus 모두 Default로 생성해 주세요 (순서도 맞게)
	14.Transformer Sizing 결과 report excel 생성기능

1.0.3.x:
	1. Title Bar에 표기방법 수정
	2. UPS/DC 속성 창에서 BHP값을 공백을 입력하면 KVA값을 가지도록 수정.
	3. Bus의 Phase 수정시 Bus에 속한 Load들의 Phase와 FLC , Starting Amp 값 변경.
	4. Load Summary의 Remark란에 Area 기입.
	5. Rating값이 없을시 오류 발생 수정.
	6. FORMAT_LOAD_IMPORT.xls 파일 변경.

1.0.2.x:
	1. Cable Sizing Table 엑셀 출력시 오류 수정.
	2. LoadSummary의 BUS,PDB에서 SaveToDatabase할때 BUS,PDB를 동시에 Database에 저장하도록 함.
	3. Cable Sizing Table 계산시 오류 수정.
	4. LoadSummary의 BUS,PDB에서 SaveToDatabase할때 BUS,PDB 둘중 하나의 Tree만 갱신되는 문제 수정.
	5. Power/Control Cable의 Revision 보는 방식 수정(Tree Node를 클릭했을때 최근의 리비젼과 비교하여 보여주도록 함)
	6. Drum Schedule 생성시 일부 Cable이 생략되던 문제 수정.
	7. LoadSummary Compare 기능 수정(삭제된 Item 표기시) - 2010.01.27
	8. ICON 수정 - 2010.01.28
	9. Excel Import시 Operation Mode를 대소문자 구별하지 않도록 함. - 2010.01.28
	10.Excel Import시 (관심있는)모든 필드의 값이 없을때 루프를 중단,Import Error Check(EFF,PF값이 1보다 클때 에러) 수정 - 2010.01.29
	11.Revision Data가 비어 있을때 오류 발생 수정 - 2010.02.02
	12.CAPACITOR DEG,UPS/DC - NEW 생성시 "Cable Data 생성되지 않았습니다." 오류 메시지 처리 - 2010.02.05
	13.LoadSummary - Rating 0일 경우 에러로 카운터 할 것. - 2010.02.05
	14.Load Assign에서 Bus 변경시 Bus가 가지고 있는 Load Volt, Phase 정보를 각 Load에 기입해 줘야함. - 2010.02.08
	15.Virtual Assign에서 Transformer의 값을 소수점 이하는 버려서 표시 - 2010.02.08

0.0.2.2:
	1. Bus Diagram 생성 루틴 수정.
	2. Load Import 부분 오류 수정.
	3. Cable Table Tab 삭제시 마지막 하나 남은 Sheet의 Title이 보이도록 수정 - revised.

0.0.2.1:
	1. Cable Setting 저장시 Total Derating Factor 저장 문제 수정.(PowerCable 생성시 오류)
	2. Cable Table Tab 삭제시 마지막 하나 남은 Sheet의 Title이 보이도록 수정.

0.0.2.0:
	1. add Load List View and Report
	2. modify load summary revision module.
	3. 선택된 Power Cable Item을 생성시 로그를 저장하도록 함.
	4. Power Cable을 생성하는 루틴 수정.
	5. Control Cable을 생성하는 루틴 수정.
	6. terminate program if can't connect internet.
	7. add function to create new project.
	8. add drum total.
	9. Cable Size 생성시 오류 수정.
	10.LoadSummary Compare View에서 Bus 순서대로 탭을 생성.

0.0.1.8:
	1. modify revision module.
	2. add drum schedule function.
	3. add funtion to export revision data to excel.(for load summary & power cable)

0.0.1.7:
	1. UPS 2차측의 FLC 계산식 수정
	2. Equipment Power Cable 생성시 Area 입력 가능하도록 UI 및 코드 수정.
	3. 새로운 Project 생성시 COMMON_LIBRARY.MDB 복사/테이블 에러 수정.
	4. Bus을 생성할때 Bus Voltage,Load Voltage,Phase,Desc를 Bus Group에서 가져온다.
	5. Cable Schedule Result 폴더를 생성하지 못해 Cable Schedule 생성시 오류 수정.(#2)
	6. Ribbon Menu 수정.
	7. Cable Schedule에서 Cable이 누락되는 현상 수정. / Cable Schedule Report 수정.
	8. Cable Sizing Table 삭제 기능 추가.
	9. UPS/DC 에서 2차측 Cable은 생성하지 않도록 함.
	10.Power Cable생성시 Load Volt를 가져오도록 함.
	11.Cable List Control에서 Load순으로 Sorting하도록 함.
	12.All Power/Control Cable 생성시 message 창으로 사용자에게 실행 여부를 확인함.

0.0.1.6:
	1. Load Excel Import시 Bus ID가 없는 Load가 후처리가 되지 않는 현상 수정.
	2. Transformer에서 Power Cable 생성 결과중 CABLE SPEC 표기 오류 수정.
	3. Power Cable Result View에서 INCOMING BUS도 표시하도록 수정.
	4. UPS/DC Item 속성 편집창 오류 수정.
	5. Capacitor용 Power Cable 생성 루틴 수정.
	6. MAC address를 확인하여 프로그램 실행 여부 결정.
	7. UPS/DC에서 Power Cable을 생성하는 부분 수정.
	8. Capacitor Property Dialog 수정.
	9. Cable Schedule Setting에서 Cable을 가지지 않는 User Bus를 표기하지 않도록 함.
	10.Capacitor,UPS/DC 계산식 수정.
	11.Cable Schedule 기능 추가.

0.0.1.5:
	1. Cable B/M Excel Report 구현
	2. Equipment Type의 Item의 Power Cable 생성
	3. User Cable Import 부분 수정
	4. Bus Group의 이름 수정시 Bus Group에 속하는 Bus의 Bus Group 항목도 같이 수정하도록 함.
	5. Load Summary에서 Error가 발생한 Item의 Load Summary 계산 결과를 저장하는 항목을 초기화 시킴.
	6. Load Summary Error Log를 볼수 있도록 함.
	7. New Project 기능 수정.
	8. Transformer측 2차 PowerCable 생성 루틴 수정.
	9. Power Cable / Control Cable 생성시 이전에 생성된 텍스트 파일을 삭제함.
	10.Power Cable / Control Cable 를 Spread에 표시할때 INCOMING BUS의 것도 표시하도록 함.

0.0.1.4:
	1. Power Cable View를 BUS별로 구분하여 보여주도록 함.
	2. Control Cable View를 BUS별로 구분하여 보여주도록 함.
	3. Power Cable Report Template 변경.
	4. Cable List Ctrl에 나타나는 메뉴에 Delete All 아이템 추가.
	5. Contro Cable Report에서 TYPE란 기입.
	6. Power Cable , Control Cable 생성후 에러난 갯수를 표시함.
	7. Power Cable , Control Cable , User Cable 생성 후 Cable List Control에 생성된  Cable을 표기함.
	8. PowerCable 생성시 PDB TYPE인 Load에서 오류가 발생하는 부분 수정.
	9. LoadSummary시 UPS/DC Item Report시 오류 수정
	10.Power Cable , Control Cable Report시 BUS Name과 Bus Volt 표기하도록 함.
	11.Power Cable 생성 루틴 수정.
	12.User Cable Import시 Spec 항목을 채워줌.
	13.Power Cable 생성시 PF,EFF 값이 없을때에 에러 발생을 막기 위해 1.로 가정함.
	14.Control Cable시 To항목을 LCS/LCP/DCS 중 하나로 설정하도록 함.
	15.R3 다이얼로그에서 Save 버튼 클릭시 다이얼로그가 종료하지 않도록 함.
	16.Cable Route Result View에서 Cable Size가 누락된것을 수정함.
	17.Excel Report Format을 설정하는 UI 추가(Tools/Make Excel Report Format)

0.0.1.3:
	1. LoadSummary Excel Template 변경
	2. LoadSummary 계산 수정.
	3. Generator 계산식 수정.
	4. Cable B/M 수정.(Sorting , Text 정렬)
	5. Virtual Load Summary 부분 수정.
	6. User Cable Excel Import 부분 추가.
	7. LoadSummary 계산시 Doc No.를 기억하도록 함.
	8. LoadSummary Excel Export시 BUS의 Continuous , Intermittent , Stand-by 값이 잘못 뿌려지는 오류 수정.
	9. C4 UI 개선
	10.Excel Import후 Item에 Lock이 걸리는 현상 수정.
	11.LoadSummary Start-Up에서 Apply 부분을 기억하도록 한다.
	12.C3 Report 부분 개선

0.0.1.2:
	1. LoadSummary 계산시 UPS/DC Item 포함.
	2. BusDiagram에 UPS/DC Item 포함.
	3. C2에서 Schematic Type입력후 TAB으로 이동 가능하도록 함.
	4. C3에서 Copy후 ScrollBar이동 오류 수정.
	5. LoadSummary Sheet의 헤더 부분 수정.
	6. C3-Check UI 수정
	7. Excel Export시 Office 2007에서 테스트. 
	8. Power cable result Excel Export
	9. C3 - Excel Export 기능 추가
	10.ImportExcelDlg에서 Import Option에 따라 메세지 처리.
	11.Load Summary  PDB, BUS 따로 Excel Export
	12.Control Cable Creation Setting Report 수정
	13.Excel Export 시 다른 Excel 닫지 않기.
	14.C3 에서 Check시 Error가 없으면 OK 메시지 박스
	15.Power Cable Excel Export
	16.Report 파일들을 Working으로 이동
	17.BUS ITEM의 이름 변경시 EQUIPMENT에 사용된 BUS 이름도 변경한다.
	18.NOT ASSIGNED LOAD를 위한 NONE BUS ITEM TREE NODE를 생성.
	19.LoadSummary Excel 파일에 Date , Doc. No 정보 기입
	20.Revision Data를 Delete Key로 삭제 가능
	21.Cable B/M Summary UI 작성
	22.Excel File Import시 Import된 Item의 갯수를 표시함.Excel Import후 ClassTreeCtrl 갱신함.

0.0.1.1:
	1. By Area별로 Load를 표시하는 기능 추가.
	2. BusDiagram 표기 방법 수정.
	3. Load View에서 Ctrl-F로 찾기 기능 추가.
	4. BusDiagram Print 기능 추가.(View에서 오른쪽 마우스 클릭을 통해서)
	5. Excel Import시 New Load Import / New Load Update 옵션 제공.
	6. UPS/DC Item 추가.

0.0.1.0:
	1. BusTree에서 방향키로 Item을 선택했을 때 PropertyWindow에 반영.
	2. CableItem인 경우에 하단 리스트에 이름뿐만 아니라 Load ID도 함께 표시.
	3. 하단 리스트에서 Schematic , IO , LCS Item을 더블 클릭시 해당 다이얼로그 표시.
	4. Voltage Setting시 Volt. 값 중복 체크를 해당 구분(HV,LV1PH,LV3PH,DC)에서 하도록 수정.
	5. Cable Route UI 작성.
	6. Load Item의 Remark란에 사용자가 기록을 할수 있도록 함.(날짜,Remark)
	7. Capacitor에 연결에 연결된 Bus List를 표시함.
	8. LoadSummary 계산시 옵션에 따라 Transformer 값을 적용하도록 함.

0.0.0.9:
	1. LoadSummary 계산 후 Starting Amp 값을 채우도록 함.
	2. Excel Import 한후 저장시 Load Item이 중복되어 저장되는 현상 수정.

0.0.0.8:
	1. MCCB SIZE AUTO SELECTION DIALOG 제작.
	2. CABLE ITEM PROPERTY WINDOW에서 CORE,SIZE,가닥수 변경시 자동으로 VD를 계산.
	3. Control Cable 관련 UI 제작.
	4. LoadCategory에 PDB 옵션 추가.
	5. Ribbon Menu 제작.
	6. Wizard Dialog 제작.

0.0.0.7:
	1. Excel Import후 저장시 에러나는 현상 수정.
	2. 'PDB' Bus Group을 항상 보여주게 한다.(LoadSummary 결과에는 보여주지 않도록 한다.)
	3. 각 Load마다 Bus의 용량를 사용할것인지 여부를 설정하는 Property추가.
	4. Virtual LoadSummary 기능 추가.
	5. Main Menu 수정.
	6. LoadSummary시 BUS,PDB별 Factor를 설정할수 있도록 함.
	7. Assign Load To Bus UI 수정
	8. 3번의 결과치를 표시하는 부분 수정.
	9. Total DF는 item의 Amb Temp DF와 Others DF의 곱으로 표현(Others DF의 Lock기능 적용)
	
0.0.0.6:
	1. LoadSummary계산시 FLC를 자동 계산하도록 함.
	2. T_RPOJECT_SETTING에서 ID 필드 삭제
	3. Voltage Setting,Power Cable Creation과 Power Cable Naming Rule 관련 UI 수정
	4. LoadSummary관련 요구사항 반영(LF,PF,EFF는 평균값으로 출력/Project Setting에서 Load Type별로 LF값을 설정할수 있도록 함...)
	5. ComboItem일 경우에도 Lock을 걸수 있도록 함.
	6. Transformer UI 추가 / New , Copy , Delete 기능 추가
	7. Bus Item을 Bus Group사이로 이동할때의 문제 수정.
	8. property 값의 변경 상태를 확인하는 루틴에서 숫자 형식일때는 숫자 값을 비교하여 변경 여부를 확인하도록 함.
	9. Power Cable Creation , Cable Result View 기능 제공
	10. Transformer DataSheet 제공
	11. Error 출력창 제공
	12. LoadSummary 계산에서 KW,KVA일 경우 계산식 수정 , FLC 계산식 수정

0.0.0.5:
	1.  database loading시 BHP가 값이 ''이면 ''으로 그대로 둔다.
	2.  LoadSummary의 Total Sheet에서 bus별 CF값들을 표시하는 COLUMN을 추가.
	3.  Characterstic이 LOCK일 경우에 PF(100% Load)또는 EFF(100% Load)의 값이 없을 경우에 
            PF(100% Load)또는 EFF(100% Load)의 값을 테이블에서 가져오지 못하는 문제 수정.
	4.  LOCK ITEM의 lock상태를 저장하도록 함.
	5.  항목이 LOCK상태일 경우에는 LoadSummary 계산결과를 반영하지 않는다.
	6.  LoadSummary 계산후 자동으로 PropertyWindow의 내용을 변경하고 , Data가 변경되었으면 DocData의 ModifiedFlag를 True로 설정한다.
	7.  Voltage Setting Dialog 추가.
	8.  Bus항목에서 Bus Voltage와 Load Voltage는 Voltage Setting에서 설정한 값을 선택하도록 수정.
	9.  Power Cable Creation Dialog 작성
	10. Power Cable Naming Rule Dialog 작성
	11. CB TABLE을 PROJECT MDB에서 COMM. LIBRARY MDB로 옮김.
	12. LoadSummary결과를 뿌려줄때 PF(100% Load),PF(75% Load),PF(50% Load),
            EFF(100% Load),EFF(75% Load),EFF(50% Load),PF at Starting,LRC의 값이 0일때는 공백으로 설정한다.
	13. Bus별 LoadSummary 결과를 뿌려줄때 BHP와 Rating Capacity 사이에 UNIT를 뿌려주도록 함.
	14. Excel Import후 Save시 Error나는 부분 수정.
