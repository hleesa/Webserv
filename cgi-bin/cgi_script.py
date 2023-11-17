#!/usr/bin/env python

import os
import sys

# query_string = os.environ.get("QUERY_STRING", "")
input_string = sys.stdin.read()
params = input_string.split('=')
param = params[1] if len(params) > 1 else ""

response_body = f"""
    <!DOCTYPE html>
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
