package serverpkg;

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
            System.out.println(e);
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
                            writer.println(studentId+":"+examName+":"+message);
                            writer.flush();
                        }

                        else if(opcode.startsWith("RECEIVE FILE")) {
                            String examName = tok.nextToken();
                            String sender = tok.nextToken();
                            //downloadFile(sender);
                            System.out.println("file downloaded");
                        }
                    }


                }catch (Exception e){
                    System.out.println(e);
                }
            }
        }.start();

    }
    private void downloadFile(String sender){
                try {
                    byte[] contents = new byte[10000];
                    BufferedOutputStream bos = new BufferedOutputStream(
                            new FileOutputStream(ServerValues.answerPath()+"\\"+sender+"_"+"\\answer.docx")
                    );
                    int bytesRead = 0;

                    while (true)    //loop is continued until received byte=totalfilesize
                    {
                        bytesRead = fileDownloader.read(contents);
                        if(bytesRead<=0) break;
                        bos.write(contents, 0, bytesRead);
                    }
                    bos.flush();
                }
                catch (Exception e){
                    System.out.println(e);
                }

    }
}
