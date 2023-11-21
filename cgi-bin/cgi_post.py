#!/usr/bin/env python

import os
import sys

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
        <h1>Hello {param}!! this is an awesome HTML file!</h1>
        <p><h2>This message is Response of "POST CGI" with message body</h2></p>
    </body>
    </html>
    """

print(response_body)
