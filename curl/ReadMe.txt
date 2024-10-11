Include안의 내용들 넣을 위치

..\MiniDungeon\Server\Libraries\Include\curl

Libs안의 내용들 넣을 위치
..\MiniDungeon\Server\Libraries\Libs\curls

Binary안의 내용들 넣을 위치
..\MiniDungeon\Server\Binary\Debug

.sln 설정
GameServer 속성 -> 구성 속성 -> C/C++ -> 전처리기에 CURL_STATICLIB 추가

ServerCore 속성 -> 구성 속성 -> C/C++ -> 전처리기에 _WINSOCK_DEPRECATED_NO_WARNINGS
_CRT_SECURE_NO_WARNINGS 추가
