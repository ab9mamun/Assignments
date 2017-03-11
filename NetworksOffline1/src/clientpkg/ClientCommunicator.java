package clientpkg;

import javafx.application.Platform;
import javafx.scene.control.Alert;

import javax.swing.*;
import java.io.*;
import java.net.Socket;
import java.util.AbstractCollection;
import java.util.ArrayList;
import java.util.StringTokenizer;

public class ClientCommunicator implements Runnable {
    private Thread t;
    private ClientMain main;
    private Socket s;
    private String serverIP;
    private int port;
    private String stdID;

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
            System.out.println(e);
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
                        StringTokenizer tok = new StringTokenizer(msg, ":");

                        String id = tok.nextToken();
                        if (!id.startsWith(stdID)) {
                            continue;
                        }

                        String myExamName = main.getMyExamName();
                        String opcode = tok.nextToken();

                        if (opcode.startsWith("UPDATE")) {
                            String update = tok.nextToken();
                            if (update.startsWith("EXAM START")) {
                                Platform.runLater(() ->
                                        {
                                            Alert alert = new Alert(Alert.AlertType.INFORMATION);
                                            alert.setTitle("Message for examinee");
                                            alert.setHeaderText(null);
                                            alert.setContentText("Exam started");

                                            alert.show();
                                        }
                                );

                            } else if (update.startsWith("EXAM END")) {
                                if (update.startsWith("EXAM START")) {
                                    Platform.runLater(() ->
                                            {
                                                Alert alert = new Alert(Alert.AlertType.INFORMATION);
                                                alert.setTitle("Message for Examinee");
                                                alert.setHeaderText(null);
                                                alert.setContentText("Exam ended");

                                                alert.show();
                                            }
                                    );
                                }
                            }
                        } else {
                            String examName = opcode;
                            if (!examName.startsWith(myExamName)) continue;
                            String decision = tok.nextToken();
                            if (decision.startsWith("REJECTED")) {
                                String message = tok.nextToken();

                                Platform.runLater(() -> {
                                    Alert alert = new Alert(Alert.AlertType.ERROR);
                                    alert.setTitle("Oops");
                                    alert.setHeaderText("Error message for examinee");
                                    alert.setContentText(message);

                                    alert.show();
                                });
                            } else if (decision.startsWith("ACCEPTED")) {
                                String details = tok.nextToken();

                                Platform.runLater(() -> {
                                    Alert alert = new Alert(Alert.AlertType.INFORMATION);
                                    alert.setTitle("Message for examinee");
                                    alert.setHeaderText(null);
                                    alert.setContentText("Registration Successful");

                                    alert.show();
                                });

                                Platform.runLater(() -> main.showExamPage(myExamName, details));
                            }


                        }
                    }


                }catch (Exception e){
                    e.printStackTrace();
                }
            }
        }.start();

    }



    public boolean tryRegistration(String examName){
        writer.println("REGISTER:"+examName+":"+stdID);
        writer.flush();
        return false;
    }


    public void start(){
        t = new Thread(this);
        t.start();
    }
}
