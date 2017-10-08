package client;

import javafx.application.Platform;

import java.io.*;
import java.net.Socket;
import java.util.Scanner;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Client {
    static Socket socket;
    PrintWriter writer;
    BufferedReader reader;
    InputStream fileDownloader;
    OutputStream fileUploader;
    Scanner scn;
    String stdid;
    int maximum_size;

    String receiver_stdid;
    String filepath;
    Thread readThread;
    Lock lock;
    boolean ready;

    public Client() {

        lock = new ReentrantLock();
        ready = false;

        try {
            socket = new Socket("127.0.0.1", 34343);
            Thread.sleep(1000);
            reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            writer = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));
            fileDownloader = socket.getInputStream();
            fileUploader = socket.getOutputStream();


            scn = new Scanner(System.in);
            System.out.print("Enter student_id: ");
            stdid = scn.nextInt() + "";


            String msg = "stdid:" + stdid;
            writeSync(msg);

            startReading(); ///new thread----

            Thread.sleep(200);

            System.out.println("Enter receiver's student_id and file no (0 0 if no file): ");
            scn = new Scanner(System.in);
            receiver_stdid = scn.nextInt() + "";
            int file_no = scn.nextInt();
            if (file_no >= 1 && file_no <= 3){
                sendFile(file_no);
            }




        }catch (Exception e){
            e.printStackTrace();
        }

    }

    public void sendFile(int file_no){
        enterCritical();
        filepath = "/src/file_" + file_no;
        try
        {
            File file = new File(filepath);
            FileInputStream fis = new FileInputStream(file);
            BufferedInputStream bis = new BufferedInputStream(fis);

            int size = (int) file.length();
            byte[] data  = new byte [size];
            writeAsync("wants_to_send:"+receiver_stdid+":"+size);

            while(!ready){
                Thread.sleep(500);
            }
            ready = false;

            int total = 0;
            while(total<size){

                int byteRead = bis.read(data);
                if(byteRead<=0) break;
                fileUploader.write(data, total, byteRead);
                total+= byteRead;
                fileUploader.flush();
                System.out.println("Sending file ..");
            }

            bis.close();
            System.out.println("File sent successfully!");
        }
        catch(Exception e)
        {
            System.err.println("Could not transfer file.");
        }
        finally {
            exitCritical();
        }
        //    writer.println("Downloaded.");
        //  writer.flush();


    }

    private void enterCritical(){
        lock.lock();
    }
    private void exitCritical(){
        lock.unlock();
    }

    public void writeSync(String msg){
        enterCritical();
        writer.println(msg);
        writer.flush();
        exitCritical();
    }

    ///caution----- use it carefully
    private void writeAsync(String msg){
        writer.println(msg);
        writer.flush();
    }

    public void startReading(){
        readThread = new Thread(){
            @Override
            public void run(){
                try {

                    while (true) {
                        String msg = reader.readLine();
                        String[] tokens = msg.split(":");
                        if(msg.startsWith("maximum_size")){
                            maximum_size = Integer.parseInt(tokens[1]);
                            System.out.println("Maximum file size: "+maximum_size);
                        }

                    }
                }catch (Exception e){
                    e.printStackTrace();
                }
            }
        };
        readThread.start();
    }
}
