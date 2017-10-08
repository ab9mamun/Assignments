package server;

import java.io.*;
import java.net.Socket;

public class ServerThread implements Runnable {
    Thread t;
    Socket socket;
    String stdid;

     PrintWriter writer;
     BufferedReader reader;
     InputStream fileDownloader;
     OutputStream fileUploader;
     String receiver_stdid;

    public ServerThread(Socket sock){
        t = new Thread(this);
        t.start();
        socket = sock;
    }

    @Override
    public void run(){


        try{
            reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            writer = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));
            fileDownloader = socket.getInputStream();
            fileUploader = socket.getOutputStream();

            while(true){
                String msg = reader.readLine();
                String[] tokens = msg.split(":");
                if(msg.startsWith("stdid")){
                    stdid = tokens[1];
                    Server.threads.put(stdid, this);
                    System.out.println("Student "+stdid+" connected");

                    writeMessage("maximum_size:"+Server.maximum_size);
                }
                else if(msg.startsWith("wants_to_send")){
                    receiver_stdid = tokens[1];
                    int file_size = Integer.parseInt(tokens[2]);
                    System.out.println("File request: "+tokens[1]);
                }
            }


        }
        catch (Exception e){
            e.printStackTrace();
            System.out.println("Student "+stdid+" disconnected");
        }
    }
    public void writeMessage(String msg){
        writer.println(msg);
        writer.flush();
    }
}
