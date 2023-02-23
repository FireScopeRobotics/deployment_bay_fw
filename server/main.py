from server import Server

hostName = ""
serverPort = 42069

if __name__ == "__main__":        
    webServer = Server(hostName, serverPort)
    webServer.server_start()

