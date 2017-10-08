package client;

import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class Client {
    static Socket socket;
    PrintWriter writer;
    BufferedReader reader;
    InputStream fileDownloader;
    OutputStream fileUploader;
    Scanner scn;
    String stdid;

    public Client(){

        try {
            socket = new Socket("127.0.0.1", 34343);
            Thread.sleep(1000);
            reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            writer = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));
            fileDownloader = socket.getInputStream();
            fileUploader = socket.getOutputStream();


            scn = new Scanner(System.in);
            System.out.println("Input student id:");
            stdid = scn.nextInt()+"";

            String msg = "stdid:"+stdid;

            writeMessage(msg);

            try {
                while (true) {
                    msg = reader.readLine();


                }
            }catch (Exception e){

            }




        }catch (Exception e){
            e.printStackTrace();
        }

    }

    public void writeMessage(String msg){
        writer.println(msg);
        writer.flush();
    }
}
