# GameCoding Zelda 서버/클라이언트 개요

## 프로젝트 소개
`GameCoding_Zelda`는 C++로 작성된 네트워크 게임 서버와 예제 클라이언트(더미 클라이언트)를 포함한 학습용 프로젝트입니다. IOCP 기반의 네트워크 구조와 Google Protobuf를 활용한 패킷 직렬화를 통해 간단한 방(Room) 기반 게임 로직을 구현합니다.

## 개발 환경 및 의존성
- **Visual Studio 2022** (v143 툴셋)
- **Windows 10 SDK**
- **Google Protobuf** 라이브러리 (프로젝트 내 `Server/Libraries/Include/google/protobuf`)

## 빌드 방법
1. Visual Studio 2022에서 솔루션 열기
   - 서버: `Server/Server.sln`
   - 클라이언트(더미): `Server/Client/GameCoding.sln` 또는 `Server/DummyClient/DummyClient.vcxproj`
2. 원하는 구성(Debug/Release, x64)으로 빌드합니다.
3. 빌드 결과 실행 파일은 각 프로젝트의 `Debug` 또는 `Release` 폴더에 생성됩니다.

## 실행 예시
### 서버 실행
```cmd
> Server.exe
```
- 기본적으로 `127.0.0.1:7777` 포트에서 대기합니다.
- 종료 시에는 콘솔에서 `Ctrl+C`를 눌러 안전하게 종료합니다.

### 클라이언트 실행
```cmd
> DummyClient.exe
```
- 실행 후 자동으로 `127.0.0.1:7777` 서버에 접속합니다.
- 서버가 먼저 실행되어 있어야 하며, 방화벽 또는 포트 충돌에 주의합니다.

## 주의사항
- 서버와 클라이언트 모두 Windows 환경을 전제로 합니다.
- 실행 전 포트 `7777`이 다른 프로그램에서 사용 중이지 않은지 확인하세요.
- 서버 로그는 실행 디렉터리의 `server.log`에서 확인할 수 있습니다.
