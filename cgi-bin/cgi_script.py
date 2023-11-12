#!/usr/bin/env python

import os

query_string = os.environ.get("QUERY_STRING", "")
query_params = query_string.split('=')
input_value = query_params[1] if len(query_params) > 1 else ""

response_body = "<html><head><title>CGI Python Script</title></head><body><h1>%s</h1></body></html>" % input_value
content_length = len(response_body)

# HTTP 응답 헤더 출력
print("HTTP/1.1 200 OK")  # 상태 코드 200은 "OK"를 나타냅니다.
print("Content-type: text/html")
print("Content-Length: %d" % content_length)
print("\n")  # 헤더와 본문 사이에 빈 줄을 추가

# HTTP 응답 본문 출력
print(response_body)