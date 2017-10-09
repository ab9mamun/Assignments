package server;

import java.io.*;
import java.net.Socket;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class ServerThread implements Runnable {
    Thread t;
    Socket socket;
    String stdid;
    Lock lock;

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

        lock = new ReentrantLock();
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

                    writeSync("maximum_size:"+Server.maximum_size);
                }
                else if(msg.startsWith("wants_to_send")){
                    receiver_stdid = tokens[1];
                    int file_size = Integer.parseInt(tokens[2]);
                    System.out.println("Student "+stdid+" wants to send file");
                    downloadFile(file_size);
                }
            }


        }
        catch (Exception e){
            e.printStackTrace();
            System.out.println("Student "+stdid+" disconnected");
        }
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


    private void downloadFile(int size){

        try {



            BufferedOutputStream bos = new BufferedOutputStream(
                    new FileOutputStream("src/downloaded.txt")
            );
            int total = 0;

            byte[] contents = new byte[size];

            writeAsync("ready");

            while (total<size)    //loop is continued until received byte=totalfilesize
            {
                int bytesRead = fileDownloader.read(contents, total, size-total);
                if(bytesRead<=0) break;
                bos.write(contents, total, bytesRead);
                total+= bytesRead;
                bos.flush();
                System.out.println("file receiving");
            }

            System.out.println("file reading end");
            bos.close();
        }
        catch (Exception e){
            e.printStackTrace();
        }


    }
}
