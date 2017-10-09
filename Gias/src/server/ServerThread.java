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
    int max_chunk;
    boolean ready;

     PrintWriter writer;
     BufferedReader reader;
     InputStream fileDownloader;
     OutputStream fileUploader;
     String receiver_stdid;
     byte[] contents;

    public ServerThread(Socket sock){
        t = new Thread(this);
        t.start();
        socket = sock;
    }

    @Override
    public void run(){

        lock = new ReentrantLock();
        ready = false;
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

                    ServerThread thread = Server.threads.get(receiver_stdid);
                    if(thread!=null){
                        new Thread(){
                            @Override
                            public void run(){
                                thread.sendFile(contents);
                            }
                        }.start();
                    }
                }
                else if(msg.startsWith("ready")){
                    ready = true;
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
                    new FileOutputStream("src/downloaded_by_server.txt")
            );
            int total = 0;

            contents = new byte[size];
            max_chunk = Math.abs(Server.random.nextInt())%5 +1;

            writeAsync("ready:"+max_chunk);

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




    public void sendFile(byte[] data){
        enterCritical();

        try
        {

            int size = (int) data.length;
            System.out.println("File length is : "+size);
            System.out.println("Trying to send file to Student "+stdid);
            writeAsync("wants_to_send:"+size);

            while(!ready){
                Thread.sleep(50);
            }
            ready = false;
            max_chunk = 5;
            System.out.println("Student "+stdid +" is ready to receive");

            int total = 0;
            while(total<size){

                int byteRead = Math.min(max_chunk, size-total);
                if(byteRead<=0) break;
                fileUploader.write(data, total, byteRead);
                total+= byteRead;
                fileUploader.flush();
                System.out.println("Sending file ..");
            }

            System.out.println("File sent successfully!");
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
        finally {
            exitCritical();
        }
        //    writer.println("Downloaded.");
        //  writer.flush();


    }
}
