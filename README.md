# Webserv 프로젝트

## 소개

Webserv 프로젝트는 HTTP 서버를 직접 구현하는 것을 목표로 합니다. HTTP는 인터넷에서 가장 널리 사용되는 프로토콜 중 하나로, 웹 페이지의 저장, 처리 및 전달을 담당합니다. 이 프로젝트를 통해 HTTP의 기본 원리를 이해하고, 실제 브라우저와의 테스트를 통해 서버를 검증할 수 있습니다. HTTP 서버의 개발은 웹 사이트 개발뿐만 아니라 다양한 네트워크 애플리케이션에서도 중요한 역할을 합니다.

## 일반 규칙

- 프로그램은 어떠한 상황에서도 크래시나 예기치 않은 종료가 발생하지 않아야 합니다.
- 소스 파일을 컴파일할 수 있는 Makefile을 제출해야 하며, 재링크는 허용되지 않습니다.
- Makefile에는 최소한 $(NAME), all, clean, fclean, re 규칙이 포함되어야 합니다.
- 프로그램은 C++98 표준을 준수하여 컴파일되어야 하며, -Wall -Wextra -Werror 플래그를 사용해야 합니다.
- 외부 라이브러리와 Boost 라이브러리는 사용이 금지됩니다.

## 필수 요구 사항

- 프로그램 이름: `webserv`
- 제출 파일: Makefile, *.{h, hpp}, *.cpp, *.tpp, *.ipp, 설정 파일
- Makefile 규칙: $(NAME), all, clean, fclean, re
- 실행 인자: [설정 파일]
- 외부 함수: 모든 C++98 함수 및 `execve`, `dup`, `dup2`, `pipe`, `strerror`, `gai_strerror`, `errno`, `dup`, `dup2`, `fork`, `socketpair`, `htons`, `htonl`, `ntohs`, `ntohl`, `select`, `poll`, `epoll`, `kqueue`, `socket`, `accept`, `listen`, `send`, `recv`, `chdir`, `bind`, `connect`, `getaddrinfo`, `freeaddrinfo`, `setsockopt`, `getsockname`, `getprotobyname`, `fcntl`, `close`, `read`, `write`, `waitpid`, `kill`, `signal`, `access`, `stat`, `open`, `opendir`, `readdir`, `closedir`.

### 프로그램 실행

```bash
git clone https://github.com/hleesa/Webserv.git
```
```bash
cd Webserv
make
./webserv ConfigFile/default.conf
```

### 요구 사항

- 프로그램은 설정 파일을 인자로 받아야 하며, 기본 경로를 사용할 수 있어야 합니다.
- 다른 웹 서버를 execve 할 수 없습니다.
- 서버는 절대로 블록되지 않아야 하며, 필요한 경우 클라이언트를 적절히 처리할 수 있어야 합니다.
- 서버는 비블로킹 모드로 작동하며, 모든 I/O 작업(리스닝 포함)에 대해 하나의 poll() (또는 이에 상응하는 함수)만 사용해야 합니다.
- poll() (또는 이에 상응하는 함수)는 읽기 및 쓰기를 동시에 체크해야 합니다.
- 설정 파일을 읽기 전에는 poll() (또는 이에 상응하는 함수)을 사용하지 않아도 됩니다.
- errno 값을 읽기 또는 쓰기 작업 후에 체크하는 것은 엄격히 금지됩니다.
- 서버는 여러 포트를 청취할 수 있어야 합니다.

## MacOS 전용 규칙

- MacOS는 다른 Unix OS와 다르게 write()를 구현하므로, fcntl()을 사용할 수 있습니다.
- 비블로킹 모드를 사용하여 파일 디스크립터를 설정해야 하며, 허용되는 플래그는 F_SETFL, O_NONBLOCK, FD_CLOEXEC만 사용 가능합니다.

## 설정 파일

NGINX 설정 파일의 'server' 부분을 참고할 수 있습니다. 설정 파일에서는 다음과 같은 항목을 설정할 수 있어야 합니다.

- 각 '서버'의 포트 및 호스트 선택
- 서버 이름 설정
- 기본 오류 페이지 설정
- 클라이언트 본문 크기 제한
- 라우트 설정 (HTTP 메서드, 리디렉션, 디렉토리/파일 경로, 디렉토리 목록 표시 여부, 기본 파일 설정, CGI 실행 등)

## 결론

Webserv 프로젝트는 HTTP 서버를 직접 구현하여 네트워크 프로그래밍 및 소켓 프로그래밍에 대한 깊은 이해를 돕습니다. 이를 통해 웹 서버의 기본 작동 원리와 HTTP 프로토콜의 중요성을 학습할 수 있습니다. 프로젝트를 성공적으로 완료하면 다양한 네트워크 애플리케이션에서 활용할 수 있는 중요한 기술을 습득하게 될 것입니다.
