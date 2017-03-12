package clientpkg;

import javafx.application.Platform;
import javafx.scene.control.Alert;
import javafx.stage.DirectoryChooser;
import javafx.stage.FileChooser;

import javax.swing.*;
import java.io.*;
import java.net.Socket;
import java.util.AbstractCollection;
import java.util.ArrayList;
import java.util.Scanner;
import java.util.StringTokenizer;

public class ClientCommunicator implements Runnable {
    private Thread t;
    private ClientMain main;
    private Socket s;
    private String serverIP;
    private int port;
    private String stdID;
    private boolean downloadPermitted;

    private PrintWriter writer;
    private BufferedReader reader;
    private InputStream fileDownloader;
    private OutputStream fileUploader;


    public ClientCommunicator(ClientMain main, String serverIP, int port, String stdID){
        this.main = main;
        this.serverIP = serverIP;
        this.port = port;
        this.stdID = stdID;
    }

    @Override
    public void run(){
        try {
            s = new Socket(serverIP, port); //sets up the connection
            reader = new BufferedReader(new InputStreamReader(s.getInputStream()));
            String ack = reader.readLine(); //receives the acknowledgement

            if(!ack.startsWith("CONNECTED")) return;
            StringTokenizer tok = new StringTokenizer(ack, ":");
            String str = tok.nextToken(); //first one is discarded, cause it is the message connected itself


            ArrayList<String> examNames = new ArrayList<>();
            while(tok.hasMoreTokens()){
                str = tok.nextToken();
                if(!str.equals("")){
                    examNames.add(str);
                }
            }
            main.setExamList(examNames);




             writer = new PrintWriter(new OutputStreamWriter(s.getOutputStream()));
            fileDownloader = s.getInputStream();
            fileUploader = s.getOutputStream();
            createReader();
            Platform.runLater(()->main.showHomePage());
        }
        catch (Exception e){
            e.printStackTrace();
        }

    }

    private void createReader(){
        new Thread(){
            @Override
            public void run(){
                try {
                    while (true) {
                        String msg = reader.readLine();
                        if (msg == null || msg.equals("")) continue;
                        System.out.println(msg);
                        msg = msg.replace('#', '\n');


                        StringTokenizer tok = new StringTokenizer(msg, ":");

                        if(!tok.hasMoreTokens()) continue;
                           String id = tok.nextToken();

                        if (!id.startsWith(stdID)) {
                            continue;
                        }

                        String myExamName = main.getMyExamName();
                        String examName = tok.nextToken();
                        if (!examName.startsWith(myExamName)) continue;
                        String opcode = tok.nextToken();

                        if (opcode.startsWith("UPDATE")) {
                            String update = tok.nextToken();
                            if (update.startsWith("EXAM START")) {
                                Platform.runLater(()->main.log("Exam started")); ;

                            } else if (update.startsWith("EXAM END")) {
                                   Platform.runLater(()->main.log("Exam ended")); ;
                            }
                            else if(update.startsWith("CORRECTION")){
                                Platform.runLater(()->{main.updateCorrection(tok.nextToken());main.log("Corrections updated");}

                                );
                                ;
                            }
                            else if(update.startsWith("WARNING")){
                                Platform.runLater(()->main.log("Warning:"+tok.nextToken()));
                            }


                        }
                        else if(opcode.startsWith("RECEIVE FILE")){
                            downloadFile(Integer.parseInt(tok.nextToken()));

                        }
                        else if(opcode.startsWith("SEND FILE")){
                            new Thread(){
                                @Override
                                public void run(){
                                    sendFile("RECEIVE FILE:"+examName+":"+stdID, ClientValues.filePath());
                                }

                            }.start();
                        }

                        else if (opcode.startsWith("REJECTED")) {
                                String message = tok.nextToken();
                                Platform.runLater(()->main.log(message));
                            } else if (opcode.startsWith("ACCEPTED")) {
                                String details = tok.nextToken();

                                Platform.runLater(()->main.log("registration successful"));;

                                Platform.runLater(() -> main.showExamPage(myExamName, details));
                            }
                        else if(opcode.startsWith("ACCEPTED_RECONNECT")){
                            String details = tok.nextToken();
                            Platform.runLater(()->main.log("Reconnected"));
                            Platform.runLater(()->main.showExamPage(myExamName,details));

                        }


                    }

                }catch (Exception e){
                    e.printStackTrace();
                }
            }
        }.start();

    }



    public void tryRegistration(String examName){
        sendMessage("REGISTER:"+examName+":"+stdID);

    }


    public void start(){
        t = new Thread(this);
        t.start();
    }

    private void downloadFile(int size){

        try {



            BufferedOutputStream bos = new BufferedOutputStream(
                    new FileOutputStream(ClientValues.filePath())
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

        Platform.runLater(()->main.log("Question received"));;

    }


    public void sendMessage(String message){
        writer.println(message.replace('\n','#'));
        writer.flush();
    }

    public void sendFile(String message, String filePath) {

        try
        {
            File file = new File(filePath);
            FileInputStream fis = new FileInputStream(file);
            BufferedInputStream bis = new BufferedInputStream(fis);

            int size = (int) file.length();
            byte[] data  = new byte [size];
            sendMessage(message+":"+(size));

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
            Platform.runLater(()->main.log("Backup sent"));
        }
        catch(Exception e)
        {
            System.err.println("Could not transfer file.");
        }
        //    writer.println("Downloaded.");
        //  writer.flush();

    }


    public void setStdID(String stdID) {
        this.stdID = stdID;
    }

    public void tryReconnection(String examName) {
        sendMessage("RECONNECT:"+examName+":"+stdID);
    }
}
