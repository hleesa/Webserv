## configuration file
### class config 
  - class server block
    - int port = 80
    - std::string host = ""
    - std::string name (필수 x) = ""
    - std::map<int, std::string> error_page, duplicated ok
      - int 상태코드
      - std::string 에러 페이지
    - std::string root = "html"
    - std::vector\<std::string> index = "index.html"
    - long limit client body size = 1e6
    - std::map<std::string, Location> locations
      - key - path
      - class Location
	  	- std::vector\<std::string> HTTP method; directive = limit_except
        - std::vector\<std::string> HTTP redirection
        - std::string root = "html"
        - std::vector\<std::string> index = "index.html"
        - bool autoindex = false
    - class cgi location : public location
      - std::vector\<std::string> cgi_path
      - std::vector\<std::string> cgi_ext


| Rule 
- server_name = http request message의 host
- first server = defult server


| Config 객체
- 한 문장의 끝에 다음 세 개(;{}) 중 하나씩만 오는지 확인
- 한 문장 끝의 ; 은 떼서 보냄
- std::string 벡터의 벡터(2차원 벡터)를 server 객체로 보내는데, 이때 1차원벡터가 한 문장

| Server 객체
- Config와 동일하게 2차원 std::string vector 를 Location 객체로 넘겨줌
