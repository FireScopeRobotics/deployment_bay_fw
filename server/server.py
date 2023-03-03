from http.server import BaseHTTPRequestHandler, HTTPServer
import time
import uuid

class DockStatuses():
    IDLE = 0
    GATE_OPERATING = 1
    OFFLINE = 2
        

class Dock():
    # request commands
    OPEN_DOOR = "open_door"
    CLOSE_DOOR = "close_door"
    LIGHT_CHANGE = "light_change"
    def __init__(self):
        self.statuses = DockStatuses()
        self.id = str(uuid.uuid4())
        self.requests = []
        self.status =  self.statuses.IDLE
    def _check_requests(self):
        if (not len(self.requests) == 0) and self.status == self.statuses.IDLE:
            return self.requests.pop(0)
        else:
            return None
    def queue_request(self, request):
        self.requests.append(request)
    
    def process_request(self, params):
        command = params[0]
        if command == "heartbeat":
            #reset timer
            request = self._check_requests()
            if request != None:
                return request
            else:
                return None




class Request_handler(BaseHTTPRequestHandler):
    # def __init__(self):
    docks = {}

    def request_init(self, params):
        dock = Dock()
        self.docks[dock.id] = dock
        return dock.id

    def do_GET(self):
        self.send_response(200)
        tokens = self.path.split("/")
        response = None
        if tokens[1] == "request-init":
            response = self.request_init(tokens[2:])
            self.send_header("Content-type", "text/html")
            self.end_headers()  
            if not response is None:
                self.wfile.write(bytes(response, "utf-8"))
        elif tokens[1] in self.docks.keys():
            response = self.docks[tokens[1]].process_request(tokens[2:])
            print("received heartbeat")
            self.send_header("Content-type", "text/html")
            self.end_headers()  
            if not response is None:
                self.wfile.write(bytes(response, "utf-8"))
            self.wfile.write(bytes(" ", "utf-8"))
        elif tokens[1] == "sudo":
            self.process_sudo_request(tokens[2:])
        print(self.docks.keys())
        print(tokens[1] in self.docks.keys())
        
        print("got path", self.path)
    
    def process_sudo_request(self, params):
        self.send_header("Content-type", "text/html")
        self.end_headers()  
        if not params[0] in self.docks.keys() and not params[0] == 'all':

            self.wfile.write(bytes("<body>could not locate key</body>", "utf-8"))
            return
        
        self.wfile.write(bytes("<body>success</body>", "utf-8"))
        if params[1] == "req":
            if params[0] == "all":
                for i in self.docks:
                    self.docks[i].queue_request(params[2])
            else:
                self.docks[params[0]].queue_request(params[2])
            self.send_header("Content-type", "text/html")
            self.end_headers()  


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
