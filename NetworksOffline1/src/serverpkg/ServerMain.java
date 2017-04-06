package serverpkg;

import clientpkg.ClientStartController;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.io.BufferedReader;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;

public class ServerMain {

    private ServerCommunicator communicator;
    private ServerStartController startController;
    private ServerConfigController configController;
    private ServerManageController manageController;
    private Scene startScene;
    private Scene configScene;
    private Scene managePage;
    private ServerValues values;
    private Stage stage;
   private HashMap<Socket, ServerMessenger> messengers;
    private HashMap<String, Exam> exams;
    private HashMap<Socket, Examinee> examineeMap;

    public static final int port=6000;

    public void start(Stage primaryStage) throws Exception{

        primaryStage.setTitle("Server");
        stage = primaryStage;

        configureSartPage();
        configureConfigPage();
        configureManagePage();
        primaryStage.setScene(configScene);
        primaryStage.show();




        //primaryStage.setScene(startScene);

        messengers = new HashMap<>();
        exams = new HashMap<>();
        examineeMap = new HashMap<>();

        communicator = new ServerCommunicator(this);
        communicator.start();

    }




    private void configureSartPage(){
       try {
           FXMLLoader loader = new FXMLLoader();
           loader.setLocation(getClass().getResource("server_start.fxml"));
           Parent root = loader.load();
           startController = loader.getController();
           startController.setMain(this);
           startScene = new Scene(root, 600, 400);
       }catch (Exception e){
           e.printStackTrace();
       }
    }

    private void configureConfigPage(){
        try {
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(getClass().getResource("server_config.fxml"));
            Parent root = loader.load();
            configController = loader.getController();
            configController.setMain(this);
            configScene = new Scene(root, 600, 400);
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    private void configureManagePage(){
        try {
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(getClass().getResource("server_manage.fxml"));
            Parent root = loader.load();
            manageController = loader.getController();
            manageController.setMain(this);
            managePage = new Scene(root, 600, 400);
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    public int getPort(){return port;}

    public Stage getStage() {
        return stage;
    }

    public void createMessenger(Socket socket){
        messengers.put( socket, new ServerMessenger(this, socket));
    }

    public void sendMessage(Socket socket, String message){
        if(socket==null) return;
        messengers.get(socket).sendMessage(message);
    }

    public void createExam(String name, int startTime, int duration, int warningTime, int backupInterval, String validIDs, String alloableApps,
                           String answerpath, String questionPath, String rules) {

        Exam exam = new Exam(this, name, startTime, duration, warningTime, backupInterval, validIDs,
                alloableApps, answerpath, questionPath, rules);
        exams.put(name, exam);

        showManagePage();
        manageController.setExam(exam);

    }

    public void showManagePage(){
        stage.setScene(managePage);
    }


    public String registerForExam(String examName, String studentId, Socket socket) {

        Exam exam = exams.get(examName);
        if(exam==null) return studentId+":"+examName+":REJECT:Exam not found";

        return exam.registerRequest(Integer.parseInt(studentId), socket);
    }

    public String getAllExamNames(){
        String names = "";
        for(String key:exams.keySet()){
            names+=":"+exams.get(key).getName();
        }
        return names;
    }

    public void sendQuestionPaper(Socket socket,String message,  String filePath) {
        if(socket==null) return;
        messengers.get(socket).sendFile(message, filePath);
    }



    public void log(String msg){
        manageController.updateLog(msg);
    }


    public String getAnswerPath(String examName, String stdId, String ip){
        return  exams.get(examName).getAnswerPathForStudent(stdId, ip);
    }

    public void updateServerTime(int currentTime) {
        manageController.updateServerTime(currentTime);
    }


    public void updateBackup(String examName, String sender, String ip) {
        exams.get(examName).updateBackup(sender, ip);
    }

    public void mapSocketWithExaminee(Socket socket, Examinee newExaminee) {
        newExaminee.setMain(this);
        examineeMap.put(socket, newExaminee);
    }

    public void markCrash(Socket socket) {
        Examinee examinee = examineeMap.get(socket);
        //if(examinee!=null) {
          //  examinee.setSocket(null);
           // examineeMap.remove(socket);
            //Platform.runLater(()-> log(examinee.getStdID()+"_"+examinee.getIp()+" has crashed"));
       // }
        examinee.setSocket(null);
        examineeMap.remove(socket);

        messengers.remove(socket);
    }

    public String tryReconnect(String examName, Socket socket, String sender, String ip) {
       Exam exam = exams.get(examName);
        if(exam==null) return sender+":"+examName+":REJECT:Exam not found";
        return exam.reconnectRequest(socket, sender, ip);
    }
}
