package server;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;

public class Server {
    static ServerSocket serverSocket;
    static HashMap<String, ServerThread> threads;
    static int maximum_size;

    public Server(){
        threads = new HashMap<String, ServerThread>();
        maximum_size = 10000;
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
