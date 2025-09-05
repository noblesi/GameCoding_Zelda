#pragma once

// 서버를 외부에서 안전하게 종료하기 위한 요청 함수를 선언한 헤더
// RequestShutdown을 분리하여 신호 처리나 관리자 명령 등 다양한 곳에서 재사용할 수 있도록 함
void RequestShutdown();