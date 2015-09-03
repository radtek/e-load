#win32com.client error

# Introduction #

PyImport\_Import시 win32com.client error 발생.


# Details #
  * 설치한 python과 예전에 project 폴더에 만들어 두었던 python 실행 모듈의 호환성이 맞지 않아 에러 발생
  * project 폴드에 있던 python 관련 실행 모듈을 제거해서 error 제거
  * 배포 build시 python 실행 모듈을 생성하고 배포 패키지에 포함하도록 한다.