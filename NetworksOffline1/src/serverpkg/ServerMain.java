package serverpkg;

import clientpkg.ClientStartController;
import javafx.application.Application;
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
    private Scene startScene;
    private Scene configScene;
    private ServerValues values;
    private Stage stage;
   private ArrayList<ServerMessenger> messengers;
    private HashMap<String, Exam> exams;

    public static final int port=6000;

    public void start(Stage primaryStage) throws Exception{

        primaryStage.setTitle("Server");
        stage = primaryStage;

        configureSartPage();
        configureConfigPage();
        primaryStage.setScene(configScene);
        primaryStage.show();


        //primaryStage.setScene(startScene);

        messengers = new ArrayList<>();
        exams = new HashMap<>();

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

    public int getPort(){return port;}

    public Stage getStage() {
        return stage;
    }

    public void createMessenger(Socket socket){
        messengers.add( new ServerMessenger(this, socket));
    }


    public void sendMessage(String ip, String message){

    }

    public void createExam(String name, int startTime, int duration, int warningTime, int backupInterval, String validIDs, String alloableApps,
                           String answerpath, String questionPath, String rules) {

        exams.put(name, new Exam(this, name, startTime, duration, warningTime, backupInterval, validIDs,
                alloableApps, answerpath, questionPath, rules));
        

    }


    public String registerForExam(String examName, String studentId, Socket socket) {

        Exam exam = exams.get(examName);
        if(exam==null) return "REJECT:Exam not found";

        return exam.registerRequest(Integer.parseInt(studentId), socket);
    }

    public String getAllExamNames(){
        String names = "";
        for(String key:exams.keySet()){
            names+=":"+exams.get(key).getName();
        }
        return names;
    }
}
