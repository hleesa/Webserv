#!/usr/bin/env python

import os

# HTTP 헤더 및 HTML 문서의 시작 부분 출력
print("Content-type: text/html\n")
print("<html><head><title>CGI Python Script</title></head><body>")

for param in os.environ.keys():
    print("%s: %s" % (param, os.environ[param]))

# HTML 문서의 끝 부분 출력
print("</body></html>")
