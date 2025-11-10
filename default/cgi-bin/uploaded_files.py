#!/usr/bin/env python3
import os
from os import listdir
from os.path import isfile, join

print("Content-Type: text/html\n")

print("""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="color-scheme" content="light dark">
    <link
      rel="stylesheet"
      href="https://cdn.jsdelivr.net/npm/@picocss/pico@2/css/pico.min.css"
    >
    <title>Directory Listing</title>
    <style>
        ul {
            list-style: none;
            padding: 0;
        }
        li {
            display: flex;
            align-items: center;
            justify-content: space-between;
            background: var(--pico-background-color);
            border: 1px solid var(--pico-muted-border-color);
            border-radius: 0.25rem;
            padding: 0.5rem 1rem;
            margin-bottom: 0.5rem;
        }
        a {
            text-decoration: none;
        }
        button {
            font-size: 0.8rem;
            padding: 0.25rem 0.5rem;
            background-color: red;
            font-weight: bold;
            color: white;
            border: none;
            border-radius: 0.25rem;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <main class="container">
        <h1>Directory Listing</h1>
        <ul>
""")

current =  os.getcwd()
mypath = current + "/default/uploads"
onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]

for file in onlyfiles:
    print(f"""
        <li>
            <a href="/uploads/{file}">{file}</a>
            <button onclick="deleteFile('{file}')">DELETE</button>
        </li>
    """)

print("""
        </ul>
    </main>

    <script>
    async function deleteFile(filename) {
        if (!confirm(`Supprimer ${filename} ?`)) return;

        try {
            const response = await fetch(`/uploads/${filename}`, {
                method: 'DELETE'
            });
			const text = await response.text();
            if (response.ok) {
                alert(`${filename} supprimé avec succès`);
                location.reload();
            } else {
                document.open();
				document.write(text);
				document.close();
            }
        } catch (error) {
            alert('Erreur réseau : ' + error);
        }
    }
    </script>

</body>
</html>
""")
