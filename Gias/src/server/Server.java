package server;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;

public class Server {
    static ServerSocket serverSocket;
    static HashMap<String, ServerThread> threads;

    public Server(){
        threads = new HashMap<String, ServerThread>();
        try {
            serverSocket = new ServerSocket(34343);
            while(true){
                Socket s = serverSocket.accept();
                new ServerThread(s);

            }

        }catch (Exception e){
            e.printStackTrace();
        }
    }
}
