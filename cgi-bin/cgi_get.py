#!/usr/bin/env python

import os

query_string = os.environ.get("QUERY_STRING", "")
query_params = query_string.split('=')
param = query_params[1] if len(query_params) > 1 else ""

response_body = f"""Status: 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Awesome HTML</title>
</head>
<body>
	<h1>Hello, this is an awesome HTML file!</h1>
	<p>Received value from query string: <strong>{param}</strong></p>
</body>
</html>
"""

print(response_body)