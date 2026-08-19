# Boost.Asio Chat + Ollama MUD 과제 가이드

## 0. 과제 목적

이번 과제의 목적은 완성도 높은 채팅 프로그램이나 거대한 게임 서버를 만드는 것이 아닙니다.

핵심 질문은 다음입니다.

> 비동기 작업이 아직 끝나지 않았을 때, 객체와 버퍼 데이터는 누가 살아 있게 만드는가?

먼저 Boost.Asio를 사용해 간단한 TCP 채팅 서버/클라이언트를 만듭니다.
그 다음 이 구조를 확장해서 Ollama와 연결되는 간단한 텍스트 기반 MUD 게임을 만듭니다.

MUD는 다음을 의미합니다.

```text
Multi-User Dungeon
```

이번 과제에서는 여러 사용자가 접속할 수 있는 텍스트 기반 던전 게임 정도로 이해하면 됩니다.

## 1. 개발 환경

권장 환경:

- Windows 10 이상
- Visual Studio 2022
- C++17 이상
- Boost.Asio
- Ollama

수업에서는 Visual Studio 2022가 이미 설치되어 있다고 가정합니다.

먼저 확인할 것:

- Visual Studio 2022에서 C++ 개발 도구가 설치되어 있는가?
- `vcpkg`가 설치되어 있는가?
- Boost.Asio가 `vcpkg`로 설치되어 있는가?
- Ollama가 설치되어 있는가?

## 2. 자동 설치 스크립트 사용

수업에서 제공된 프로젝트 폴더에 다음 파일이 있으면 자동 설치를 먼저 시도합니다.

```text
scripts\setup_boost_asio.ps1
```

`Developer PowerShell for VS 2022`를 실행한 뒤, 프로젝트 폴더로 이동합니다.

예:

```powershell
cd C:\0CurrentWorkspace\BoostAsioChat
```

그 다음 실행합니다.

```powershell
.\scripts\setup_boost_asio.ps1
```

이 스크립트는 다음 작업을 자동으로 수행합니다.

- Visual Studio C++ 컴파일러 확인
- Git 설치 확인
- `C:\vcpkg`에 vcpkg가 있는지 확인
- vcpkg가 없으면 다운로드 및 설치
- vcpkg를 Visual Studio와 연결
- `boost-asio:x64-windows` 설치 여부 확인
- Boost.Asio가 없으면 설치

PowerShell 실행 정책 때문에 스크립트 실행이 막히면 다음 명령으로 현재 터미널에서만 허용할 수 있습니다.

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

그 다음 다시 실행합니다.

```powershell
.\scripts\setup_boost_asio.ps1
```

자동 설치가 성공하면 8번 "Boost.Asio 빌드 테스트"로 이동합니다.

자동 설치가 실패하면 아래 수동 확인 절차를 따라갑니다.

## 3. 자동 설치 스크립트가 하는 일

`scripts\setup_boost_asio.ps1`는 Boost.Asio 실습 환경을 준비하기 위한 PowerShell 스크립트입니다.

스크립트는 다음 순서로 동작합니다.

1. Visual Studio C++ 컴파일러 확인

   ```powershell
   cl
   ```

   `Developer PowerShell for VS 2022`에서 실행 중인지 확인합니다.
   일반 PowerShell에서 실행하면 `cl`을 찾지 못할 수 있습니다.

2. Git 설치 확인

   ```powershell
   git
   ```

   vcpkg가 없는 경우 GitHub에서 vcpkg를 받아와야 하므로 Git이 필요합니다.

3. vcpkg 설치 확인

   ```text
   C:\vcpkg\vcpkg.exe
   ```

   이 파일이 있으면 기존 vcpkg를 그대로 사용합니다.

4. vcpkg가 없으면 설치

   ```powershell
   git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
   C:\vcpkg\bootstrap-vcpkg.bat
   ```

   즉, vcpkg가 없을 때만 `C:\vcpkg`에 새로 설치합니다.

5. vcpkg와 Visual Studio 연결

   ```powershell
   C:\vcpkg\vcpkg integrate install
   ```

   Visual Studio 프로젝트가 vcpkg로 설치한 라이브러리를 찾을 수 있게 설정합니다.

6. Boost.Asio 설치 확인

   ```powershell
   C:\vcpkg\vcpkg list
   ```

   `boost-asio:x64-windows`가 설치되어 있는지 확인합니다.

7. Boost.Asio가 없으면 설치

   ```powershell
   C:\vcpkg\vcpkg install boost-asio:x64-windows
   ```

   이미 설치되어 있으면 다시 설치하지 않습니다.

정리하면 다음과 같습니다.

```text
vcpkg 있음      -> 그대로 사용
vcpkg 없음      -> C:\vcpkg에 설치

Boost.Asio 있음 -> 그대로 사용
Boost.Asio 없음 -> boost-asio:x64-windows 설치
```

스크립트가 하지 않는 일:

- Visual Studio를 설치하지 않습니다.
- Git이 없을 때 Git을 자동 설치하지 않습니다.
- Ollama를 설치하지 않습니다.
- 기존 vcpkg 폴더를 삭제하지 않습니다.
- 기존 프로젝트 코드를 수정하지 않습니다.
- 관리자 권한을 요구하지 않는 방식으로 작성되어 있습니다.

스크립트가 실패하는 흔한 경우:

- 일반 PowerShell에서 실행해서 `cl`을 찾지 못한 경우
- Git이 설치되어 있지 않은 경우
- 학교/회사 네트워크에서 GitHub 접속이 막힌 경우
- `C:\vcpkg`에 쓸 권한이 없는 경우
- 디스크 공간이 부족한 경우

이 경우에는 오류 메시지를 확인한 뒤 아래 수동 확인 절차를 따라갑니다.

## 4. Visual Studio C++ 환경 확인

시작 메뉴에서 다음 터미널을 실행합니다.

```text
Developer PowerShell for VS 2022
```

이 터미널은 Visual Studio C++ 컴파일러 경로를 자동으로 알고 있으므로 C++ 개발에 편리합니다.

터미널에서 다음 명령을 실행합니다.

```powershell
cl
```

정상이라면 Microsoft C/C++ Compiler 버전 정보가 출력됩니다.

만약 `cl`을 찾을 수 없다고 나오면 Visual Studio Installer에서 다음 워크로드가 설치되어 있는지 확인합니다.

```text
Desktop development with C++
```

필요한 구성 요소:

- MSVC C++ compiler
- Windows SDK
- CMake tools for Windows

Visual Studio 다운로드가 필요한 경우:

<https://visualstudio.microsoft.com/downloads/>

## 5. vcpkg 설치 확인

`vcpkg`는 C/C++ 라이브러리 패키지 관리자입니다.
Boost를 직접 다운로드하고 include/lib 경로를 수동으로 잡는 것보다 훨씬 편합니다.

먼저 `vcpkg`가 이미 설치되어 있는지 확인합니다.

`Developer PowerShell for VS 2022`에서 실행:

```powershell
vcpkg version
```

정상이라면 다음과 비슷한 버전 정보가 출력됩니다.

```text
vcpkg package management program version ...
```

명령을 찾을 수 없다고 나오면 PATH에 등록되지 않았을 수 있습니다.
대표 설치 경로를 확인합니다.

```powershell
Test-Path C:\vcpkg\vcpkg.exe
```

결과가 `True`이면 vcpkg가 설치되어 있지만 PATH에 잡혀 있지 않은 상태입니다.
이 경우 다음처럼 전체 경로로 실행할 수 있습니다.

```powershell
C:\vcpkg\vcpkg version
```

결과가 `False`이면 vcpkg가 설치되어 있지 않은 것입니다.

## 6. vcpkg가 없을 때 설치

이 단계는 `vcpkg version` 또는 `C:\vcpkg\vcpkg version`이 실패한 경우에만 진행합니다.

`Developer PowerShell for VS 2022`를 열고 다음 명령을 실행합니다.

```powershell
cd C:\
git clone https://github.com/microsoft/vcpkg.git
cd C:\vcpkg
.\bootstrap-vcpkg.bat
```

설치 후 다시 확인합니다.

```powershell
C:\vcpkg\vcpkg version
```

## 7. vcpkg와 Visual Studio 연결

```powershell
C:\vcpkg\vcpkg integrate install
```

성공하면 Visual Studio 프로젝트에서 vcpkg로 설치한 라이브러리를 자동으로 찾을 수 있습니다.

이미 연결되어 있는 경우에도 다시 실행해도 괜찮습니다.

## 8. Boost.Asio 설치 확인

이번 과제에서는 Boost.Asio를 사용합니다.

먼저 설치되어 있는지 확인합니다.

```powershell
C:\vcpkg\vcpkg list boost-asio
```

설치되어 있다면 `boost-asio` 항목이 출력됩니다.

아무것도 출력되지 않으면 설치합니다.

```powershell
C:\vcpkg\vcpkg install boost-asio:x64-windows
```

Visual Studio 프로젝트도 `x64`로 맞추는 것을 권장합니다.

만약 프로젝트를 `x86`으로 만들었다면 다음 명령을 사용해야 합니다.

```powershell
C:\vcpkg\vcpkg install boost-asio:x86-windows
```

하지만 수업에서는 혼동을 줄이기 위해 `x64`를 기준으로 진행합니다.

## 9. Boost.Asio 빌드 테스트

Visual Studio에서 C++ 콘솔 프로젝트를 하나 만들고 다음 코드를 빌드해 봅니다.

```cpp
#include <boost/asio.hpp>
#include <iostream>

namespace asio = boost::asio;

int main()
{
    asio::io_context ioContext;
    std::cout << "Boost.Asio is ready.\n";
    return 0;
}
```

실행 결과:

```text
Boost.Asio is ready.
```

이 문장이 출력되면 Boost.Asio 사용 준비가 된 것입니다.

## 10. Boost include 오류가 날 때

다음을 확인합니다.

- `vcpkg integrate install`을 실행했는가?
- Visual Studio 프로젝트가 `x64`로 설정되어 있는가?
- `boost-asio:x64-windows`를 설치했는가?
- Visual Studio를 다시 실행해 보았는가?
- 일반 C++ 콘솔 프로젝트에서 테스트하고 있는가?

그래도 안 되면 Visual Studio 프로젝트 속성에서 include 경로를 직접 추가할 수 있습니다.

```text
C:\vcpkg\installed\x64-windows\include
```

다만 보통은 `vcpkg integrate install` 이후 자동으로 잡힙니다.

## 11. Boost 수동 설치 대안

vcpkg를 사용할 수 없는 경우에만 이 방법을 사용합니다.

1. Boost 공식 다운로드 페이지에서 Windows용 압축 파일을 받습니다.

   <https://www.boost.org/users/download/>

2. 압축을 단순한 경로에 풉니다.

   예:

   ```text
   C:\local\boost_1_xx_x
   ```

3. Visual Studio 프로젝트 속성에서 다음 항목에 include 경로를 추가합니다.

   ```text
   C/C++ > General > Additional Include Directories
   ```

   예:

   ```text
   C:\local\boost_1_xx_x
   ```

이번 과제의 Boost.Asio TCP 예제는 대부분 header 기반으로 사용할 수 있으므로, 단순한 예제에서는 Boost 전체 빌드가 필요하지 않을 수 있습니다.
하지만 수업 환경 통일을 위해 vcpkg 방식을 권장합니다.

## 12. 1단계 과제: Boost.Asio 채팅 예제

C++17과 Boost.Asio를 사용하여 콘솔 기반 TCP 채팅 서버와 클라이언트를 만듭니다.

Visual Studio 2022에서 하나의 솔루션을 만들고, 그 안에 두 개의 C++ 콘솔 프로젝트를 추가합니다.

필수 솔루션/프로젝트 구조:

```text
BoostAsioChat.sln

ChatServer/
  ChatServer.vcxproj
  main.cpp
  ChatServer.h
  ChatServer.cpp
  ChatSession.h
  ChatSession.cpp

ChatClient/
  ChatClient.vcxproj
  main.cpp
  ChatClient.h
  ChatClient.cpp

README.md
```

생성되어야 하는 실행 파일:

```text
ChatServer.exe
ChatClient.exe
```

권장 Visual Studio 설정:

- `x64 Debug`
- C++17 이상
- Console application
- Boost.Asio는 vcpkg 연동으로 include

서버 요구사항:

- TCP 포트 `7777`에서 대기합니다.
- 여러 클라이언트 접속을 허용합니다.
- 클라이언트 하나당 `ChatSession` 객체 하나를 만듭니다.
- 접속 중인 세션은 `std::shared_ptr<ChatSession>` 컬렉션으로 관리합니다.
- 한 클라이언트가 보낸 한 줄 메시지를 모든 클라이언트에게 broadcast합니다.
- 클라이언트가 종료되면 해당 세션을 제거합니다.
- 한 클라이언트가 종료되어도 서버는 계속 실행되어야 합니다.

클라이언트 요구사항:

- 기본 접속 주소는 `127.0.0.1:7777`입니다.
- `std::getline`으로 콘솔 입력을 받습니다.
- 입력한 한 줄을 서버로 전송합니다.
- 서버에서 오는 메시지를 비동기로 받아 출력합니다.
- 여러 클라이언트를 실행해 서로 채팅할 수 있어야 합니다.

실행 방법:

1. `ChatServer.exe`를 먼저 실행합니다.
2. `ChatClient.exe`를 두 개 실행합니다.
3. 한 클라이언트에서 메시지를 입력합니다.
4. 두 클라이언트 모두 메시지를 받는지 확인합니다.

Visual Studio에서는 시작 프로젝트를 하나만 지정하면 하나의 프로그램만 실행됩니다.
서버와 클라이언트를 함께 테스트하려면 다음 중 하나를 사용합니다.

- `ChatServer`를 Visual Studio에서 실행하고, `ChatClient.exe`는 터미널에서 두 번 실행
- 빌드 후 `ChatServer.exe`, `ChatClient.exe`, `ChatClient.exe`를 각각 터미널에서 실행
- Solution Properties에서 Multiple startup projects를 설정한 뒤, 부족한 클라이언트 하나를 추가 실행

## 13. 반드시 드러나야 하는 C++ 개념

코드에서 다음 개념이 분명히 보여야 합니다.

- 비동기 네트워크 처리
- lambda callback
- `std::shared_ptr`
- `std::enable_shared_from_this`
- 객체 lifetime
- 비동기 write buffer lifetime
- 소유 관계와 비소유 참조의 구분

`ChatSession`은 반드시 다음을 상속합니다.

```cpp
std::enable_shared_from_this<ChatSession>
```

비동기 콜백에서 세션 객체가 필요하면 다음 형태를 사용합니다.

```cpp
auto self = shared_from_this();
```

그리고 콜백에서 `self`를 캡처합니다.

다음 방식에만 의존하면 안 됩니다.

```cpp
[this]
```

이유:

```text
this는 객체의 주소일 뿐입니다.
객체의 수명을 연장하지 않습니다.
```

## 14. async_write 버퍼 lifetime

다음 코드는 위험할 수 있습니다.

```cpp
std::string message = "Hello\n";
asio::async_write(socket, asio::buffer(message), callback);
```

`async_write()`가 끝나기 전에 `message`가 사라질 수 있기 때문입니다.

비동기 write에 넘긴 버퍼 데이터는 write callback이 호출될 때까지 살아 있어야 합니다.

교육용으로 이해하기 쉬운 방법:

```cpp
auto message = std::make_shared<std::string>("Hello\n");

asio::async_write(
    socket,
    asio::buffer(*message),
    [self, message](boost::system::error_code ec, std::size_t length)
    {
        // message는 이 callback이 끝날 때까지 살아 있습니다.
    });
```

## 15. 채팅 예제 동작 확인

터미널 3개를 실행합니다.

Terminal 1:

```text
ChatServer.exe
```

Terminal 2:

```text
ChatClient.exe
> Hello
```

Terminal 3:

```text
ChatClient.exe
```

기대 결과:

- Terminal 2에 `Hello`가 출력됩니다.
- Terminal 3에도 `Hello`가 출력됩니다.
- 클라이언트 하나를 종료해도 서버는 종료되지 않습니다.
- 남은 클라이언트는 계속 채팅할 수 있습니다.

## 16. Ollama 설치

Ollama는 로컬 컴퓨터에서 LLM을 실행할 수 있게 해주는 도구입니다.

Windows용 Ollama를 다운로드합니다.

<https://ollama.com/download/windows>

설치 후 PowerShell에서 확인합니다.

```powershell
ollama --version
```

Ollama는 기본적으로 다음 주소에서 로컬 HTTP API를 제공합니다.

```text
http://localhost:11434
```

## 17. Ollama 모델 다운로드

처음에는 작은 모델로 테스트합니다.

```powershell
ollama pull llama3.2
```

실행 테스트:

```powershell
ollama run llama3.2
```

다음처럼 입력해 봅니다.

```text
Say hello in one sentence.
```

종료:

```text
/bye
```

## 18. Ollama HTTP API 테스트

PowerShell에서 다음을 실행합니다.

```powershell
$body = @{
    model = "llama3.2"
    prompt = "Describe a dark fantasy dungeon room in one sentence."
    stream = $false
} | ConvertTo-Json

Invoke-RestMethod `
    -Method Post `
    -Uri "http://localhost:11434/api/generate" `
    -Body $body `
    -ContentType "application/json"
```

정상 동작하면 생성된 문장이 포함된 JSON 응답을 받습니다.

## 19. 2단계 과제: Ollama 연결 MUD 게임

채팅 예제를 완성한 뒤, 같은 네트워크 구조를 확장하여 간단한 MUD 게임을 만듭니다.

기본 방향:

- 텍스트 기반 게임
- TCP 서버
- 여러 클라이언트 접속
- 콘솔 클라이언트에서 명령어 입력
- 서버가 게임 월드 상태 관리
- Ollama는 설명 문장이나 NPC 응답 생성에 사용

## 20. MUD 최소 기능

MUD 서버는 최소한 다음 기능을 지원해야 합니다.

- 여러 플레이어 접속
- 플레이어 입장/퇴장 메시지
- 최소 3개의 방
- 이동 명령어:
  - `north`
  - `south`
  - `east`
  - `west`
- `look` 명령어
- `say <message>` 명령어
- 같은 방에 있는 플레이어에게만 메시지 broadcast
- 서버가 직접 관리하는 간단한 게임 상태

예:

```text
> look
You are in the Old Gate. Paths lead north and east.

> north
You moved to the Broken Hall.

> say hello
You say: hello
```

## 21. Ollama 연동 요구사항

Ollama는 다음 중 하나 이상에 사용합니다.

- 방 설명 생성
- NPC 대사 생성
- 단순한 설명을 분위기 있는 문장으로 변환
- 이벤트 메시지 생성
- 게임 마스터처럼 상황 설명 생성

예:

```text
플레이어 명령:
look

서버가 Ollama에 요청:
Describe this room in 2 sentences. Room: Broken Hall. Style: fantasy MUD.

서버가 결과를 플레이어에게 전송.
```

Ollama 연동은 단순하게 유지합니다.

권장 설계:

- C++ 서버가 게임 상태를 소유합니다.
- Ollama는 텍스트 생성만 담당합니다.
- 방 연결, 플레이어 위치, 명령어 처리 규칙은 C++ 코드가 결정합니다.

## 22. MUD 설계 원칙

LLM이 게임 엔진 전체가 되면 안 됩니다.

좋은 설계:

```text
C++ Server
    owns players, rooms, commands, rules

Ollama
    generates descriptions and dialogue
```

나쁜 설계:

```text
Ollama
    decides all rules, player locations, inventory, and combat state
```

이번 과제의 목적은 네트워크 프로그래밍, 객체 lifetime, ownership 설계를 연습하는 것입니다.
프롬프트 작성만 연습하는 과제가 아닙니다.

## 23. MUD 동작 확인

다음을 실행합니다.

```text
MudServer.exe
MudClient.exe
MudClient.exe
```

기대 결과:

- 두 클라이언트가 접속할 수 있습니다.
- 각 플레이어가 `look`을 사용할 수 있습니다.
- 각 플레이어가 방 사이를 이동할 수 있습니다.
- `say` 메시지는 같은 방의 플레이어에게 보입니다.
- 최소 하나의 명령어가 Ollama 생성 텍스트를 사용합니다.
- 한 플레이어가 종료되어도 서버는 계속 실행됩니다.

## 24. 제출 체크리스트

제출물:

- 소스 코드
- `README.md`
- 빌드/실행 방법
- 채팅 테스트 화면 또는 콘솔 로그
- MUD 테스트 화면 또는 콘솔 로그
- `shared_ptr`를 어디에 사용했고 왜 사용했는지 설명
- `shared_from_this()`를 어디에 사용했고 왜 사용했는지 설명
- async write buffer lifetime을 어떻게 보호했는지 설명
- Mermaid 클래스 다이어그램 또는 ownership 다이어그램

## 25. README 권장 구조

`README.md`에는 다음 내용을 포함하는 것을 권장합니다.

```text
1. 프로젝트 목표
2. 빌드 방법
3. 채팅 서버/클라이언트 실행 방법
4. MUD 서버/클라이언트 실행 방법
5. 객체 lifetime 설명
6. ownership 다이어그램
7. Ollama 연동 설계
8. 테스트 결과
```

## 26. 참고 링크

- Boost 다운로드: <https://www.boost.org/users/download/>
- Boost Getting Started: <https://docs.cppalliance.org/user-guide/getting-started.html>
- vcpkg Boost.Asio 패키지: <https://vcpkg.io/en/package/boost-asio.html>
- Visual Studio 다운로드: <https://visualstudio.microsoft.com/downloads/>
- Ollama Windows 문서: <https://docs.ollama.com/windows>
- Ollama Windows 다운로드: <https://ollama.com/download/windows>