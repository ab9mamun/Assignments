package serverpkg;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

public class ServerCommunicator implements Runnable {
    private Thread t;
    private ServerMain main;
    private ServerSocket serverSocket;

    public ServerCommunicator(ServerMain main){
        this.main = main;
    }

    @Override
    public void run(){
        try {
            serverSocket = new ServerSocket(main.getPort());
            while(true){
                Socket clientSocket = serverSocket.accept();
                System.out.println("CONNECTED");

                new Thread(){
                    public void run() {
                        doAfterConnect(clientSocket);
                    }
                }.start();


            }
        }catch(Exception e){
            System.out.println(e);
        }

    }

    public void start(){
        t = new Thread(this);
        t.start();
    }



    private void doAfterConnect(Socket clientSocket){
        try {
            PrintWriter writer = new PrintWriter(new OutputStreamWriter(clientSocket.getOutputStream()));

            writer.println("CONNECTED"+main.getAllExamNames());
            writer.flush();

            main.createMessenger(clientSocket);
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

}
