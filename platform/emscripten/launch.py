import subprocess
import time
import webbrowser
import sys
import os

# --- Configuration ---
PORT = 8000
HOST = 'localhost'

# --- Get arguments from CMake ---
# The directory where the server should run
working_dir = sys.argv[1]
# The HTML file to open
html_file = sys.argv[2]

# --- Change to the correct directory ---
os.chdir(working_dir)

# --- Start the server ---
# We run the server as a separate process
server_process = subprocess.Popen(
    ['python', '-m', 'http.server', str(PORT)],
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE
)

url = f"http://{HOST}:{PORT}/{html_file}"
print(f"Python server starting in background at {url}")

# Give the server a moment to initialize
time.sleep(1)

# --- Launch the web browser ---
print(f"Opening browser...")
webbrowser.open(url)

# --- Keep the script alive to monitor the server ---
# You can close the server by pressing Ctrl+C in the terminal
try:
    # Print server output so you can see requests
    for line in iter(server_process.stdout.readline, b''):
        print(f"[Server]: {line.decode().strip()}")
    server_process.wait()
except KeyboardInterrupt:
    print("\nStopping server...")
    server_process.terminate()
    print("Server stopped.")