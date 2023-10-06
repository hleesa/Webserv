## configuration file
### class config 
  - class server block
    - int port
    - std::string host
    - std::string name (필수 x)
    - std::map<int, std::string> error_page
      - int 상태코드
      - std::string 에러 페이지
    - std::string root
    - std::vector\<std::string> index
    - long limit client body size
    - std::map<std::string, Location> locations
      - key - path
      - class Location
        - std::vector\<std::string> HTTP redirection
        - std::string root
        - std::vector\<std::string> index
        - bool autoindex
    - class cgi location : public location
      - std::vector\<std::string> cgi_path
      - std::vector\<std::string> cgi_ext



