#!/usr/bin/env python3
import cgi

print("Content-Type: text/html")
print()

form = cgi.FieldStorage()
name = form.getvalue("prenom", "inconnu")

print(f"""
<html data-theme="light">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="color-scheme" content="light dark">
    <link
      rel="stylesheet"
      href="https://cdn.jsdelivr.net/npm/@picocss/pico@2/css/pico.min.css"
    >
    <title>Bonjour</title>
  </head>
  <body>
    <main class="container">
      <section>
        <h3>Bonjour, {name} !</h3>
        <p>Ce message a été généré dynamiquement par le script Python sur le serveur.</p>
        <a href="/script.html" role="button">↩ Retour aux scripts</a>
      </section>
    </main>
  </body>
</html>
""")
