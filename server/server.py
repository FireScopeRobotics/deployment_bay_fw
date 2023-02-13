from http.server import BaseHTTPRequestHandler, HTTPServer
import time

class Request_handler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        self.wfile.write(bytes("<html><head><title>https://pythonbasics.org</title></head>", "utf-8"))
        self.wfile.write(bytes("<p>Request: %s</p>" % self.path, "utf-8"))
        self.wfile.write(bytes("<body>", "utf-8"))
        self.wfile.write(bytes("<p>This is an example web server.</p>", "utf-8"))
        self.wfile.write(bytes("</body></html>", "utf-8"))
        print("got path", self.path)

class Server():
    def __init__(self, hostName, serverPort):
        self.webServer = HTTPServer((hostName, serverPort), Request_handler)
        print("Server started http://%s:%s" % (hostName, serverPort))
    
    def server_start(self):
        try:
            self.webServer.serve_forever()
        except KeyboardInterrupt:
            pass
        self.webServer.server_close()
        print("Server stopped.")
