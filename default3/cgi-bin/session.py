#!/usr/bin/env python3
import os, random

print("Content-Type: text/html")

cookie = os.environ.get("HTTP_COOKIE")

if not cookie or "session_id3=" not in cookie:
    session_id = str(random.randint(1000, 9999))
    print(f"Set-Cookie: session_id3={session_id}; Path=/; Max-Age=600")
    print()  # ligne vide obligatoire entre en-têtes et contenu
    print(f"<html><body>New session created: {session_id}</body></html>")
else:
    session_id = cookie.split("session_id3=")[1].split(";")[0]
    print()  # ligne vide obligatoire
    print(f"<html><body>Welcome back! Your session ID: {session_id}</body></html>")