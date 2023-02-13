from server import Server

hostName = "localhost"
serverPort = 42069

if __name__ == "__main__":        
    webServer = Server(hostName, serverPort)
    webServer.server_start()

