# electric load database management #

## 프로젝트 목표 ##
  * Electrical Load 관련 Data를 통합관리하므로써, 관련 산출물의 자동화 구현을 통한 M/H 절감 및 산출물의 정확도 향상

## 프로젝트 기간 ##
  * 수행기간은 2008년 10월 01일부터 2009년 5월 30일까지이다.

## 개발 범위 ##
  1. Main User Interface 개발
    * 사용자 편리성 최우선으로 고려
    * Excel Import / Export 기능
    * Project별 관리
    * Output Data AutoCAD 에 Write 기능
  1. Electrical Load DB 구축
    * 주요 데이터 DB 구축
    * ELECT. LOAD LIST TABLE
    * CABLE INFORM. TABLE, SEQUENCE TYPE TABLE…
    * IO TABLE…, 기타 REFERENCE TABLE…
  1. 각종 Report 출력
    * LOAD LIST, CABLE SCHEDULE, BM SUMMARY….
    * GENERATOR 용량 계산서….

## required modules ##

  1. python 2.6.msi
  1. pywin32-214.win32-py2.6.exe
  1. py2exe-0.6.9.win32-py2.6.exe
  1. [WinGraphviz\_v1.02.24.msi](http://wingraphviz.sourceforge.net/pub/wingraphviz/WinGraphviz_v1.02.24.msi)
  1. log4cxx-0.9.7
  1. Spread 7.0