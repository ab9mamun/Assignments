package serverpkg;

import clientpkg.ClientValues;
import javafx.application.Platform;

import java.io.*;
import java.net.Socket;
import java.util.StringTokenizer;

public class ServerMessenger {
    ServerMain main;
    Socket socket;
    String ip;

    OutputStream fileUploader;
    InputStream fileDownloader;
    BufferedReader reader;
    PrintWriter writer;






    public ServerMessenger(ServerMain main, Socket socket){
        this.main = main;
        this.socket = socket;
        ip = socket.getInetAddress().getHostAddress();

        try {


            reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            writer = new PrintWriter(socket.getOutputStream());

            fileUploader = socket.getOutputStream();
            fileDownloader = socket.getInputStream();

        }catch (Exception e ){
            e.printStackTrace();
        }

        createReader();

    }

    public void sendMessage(String message){
        writer.println(message);
        writer.flush();
    }


    private void createReader(){
        new Thread(){
            @Override
            public void run(){
                try {
                    while (true) {
                        String msg = reader.readLine();
                        System.out.println(msg);
                        StringTokenizer tok = new StringTokenizer(msg, ":");

                        String opcode = tok.nextToken();
                        if(opcode.startsWith("REGISTER")){
                            String examName = tok.nextToken();
                            String studentId = tok.nextToken();
                            String message = main.registerForExam(examName, studentId, socket);
                            writer.println(message);
                            writer.flush();
                        }

                        else if(opcode.startsWith("RECEIVE FILE")) {
                            String examName = tok.nextToken();
                            String sender = tok.nextToken();
                            String fileSize = tok.nextToken();
                            downloadFile(examName, sender, Integer.parseInt(fileSize));
                           // System.out.println("file downloaded");
                        }
                    }


                }catch (Exception e){
                   e.printStackTrace();
                }
            }
        }.start();

    }
    private void downloadFile(String examName, String stdId, int size){

        try {
            BufferedOutputStream bos = new BufferedOutputStream(
                    new FileOutputStream(main.getAnswerPath(examName, stdId, ip))
            );
            int total = 0;

            byte[] contents = new byte[size];
            while (total<size)    //loop is continued until received byte=totalfilesize
            {

                int bytesRead = fileDownloader.read(contents, total, size-total);
                if(bytesRead<=0) break;
                bos.write(contents, total, bytesRead);
                total+= bytesRead;
                bos.flush();
                System.out.println("file reading");
            }

            System.out.println("file reading end");
            bos.close();
        }
        catch (Exception e){
            e.printStackTrace();
        }

        Platform.runLater(()->main.log("Backup received"));


    }

    public void sendFile(String message, String filePath) {

        try
        {
            File file = new File(filePath);
            FileInputStream fis = new FileInputStream(file);
            BufferedInputStream bis = new BufferedInputStream(fis);

            int size = (int) file.length();

            sendMessage(message+":"+size);

            int total = 0;
            while(total<size){

                int datasize = Math.min(100000,size-total);
                byte[] data = new byte[datasize];

                int byteRead = bis.read(data, total, datasize);
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
    //    writer.println("Downloaded.");
      //  writer.flush();

    }
}
