package serverpkg;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

public class ServerCommunicator implements Runnable {
    private Thread t;
    private ServerMain main;
    private ServerSocket serverSocket;

    public ServerCommunicator(){
        main = ServerMain.main();
    }

    @Override
    public void run(){
        try {
            serverSocket = new ServerSocket(ServerValues.port);
            while(true){
                Socket clientSocket = serverSocket.accept();
                System.out.println("accepted");

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
            writer.println("accepted");
            writer.flush();

            BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            String stdID = reader.readLine();

            main.verifyStudent(clientSocket, stdID);
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

}
