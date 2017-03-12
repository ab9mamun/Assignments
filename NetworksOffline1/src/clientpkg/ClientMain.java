package clientpkg;

import com.sun.org.apache.xpath.internal.SourceTree;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.util.ArrayList;

public class ClientMain extends Application {
    private ClientCommunicator communicator;
    private ClientStartController startController;
    private ClientHomeController homeController;
    private ClientExamController examController;
    private String myExamName;

    private String stdID;
    private ArrayList<String> examList;

    private Scene homePage;
    private Scene startPage;
    private Scene examPage;
    private Stage stage;


    @Override
    public void start(Stage primaryStage) throws Exception{

        stage = primaryStage;
        examList = new ArrayList<>();

        configureStartPage();

        primaryStage.setTitle("Examinee's World");

        primaryStage.setScene(startPage);
        configureHomePage();
        configureExamPage();


        primaryStage.show();
    }

    public void configureStartPage(){
        FXMLLoader loader = new FXMLLoader();
        loader.setLocation(getClass().getResource("client_start.fxml"));
        try {
        Parent root;

           root = loader.load();

        startController = loader.getController();
        startController.setMain(this);
        startPage = new Scene(root, 600, 400);
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }

    public void configureHomePage(){
        FXMLLoader loader = new FXMLLoader();
        loader.setLocation(getClass().getResource("client_home.fxml"));
        try {
            Parent root;

            root = loader.load();

            homeController = loader.getController();
            homeController.setMain(this);
            homePage = new Scene(root, 600, 400);
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }

    public void configureExamPage(){
        FXMLLoader loader = new FXMLLoader();
        loader.setLocation(getClass().getResource("client_exam.fxml"));
        try {
            Parent root;

            root = loader.load();

            examController = loader.getController();
            examController.setMain(this);
            examPage = new Scene(root, 600, 400);
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }

    public void showHomePage(){
        stage.setScene(homePage);
        homeController.updateExamList(examList);
        homeController.setStdID(stdID);
    }



    public void setStdID(String stdID){
        this.stdID = stdID;
        communicator.setStdID(stdID);
    }

    public void connectToServer(String serverIP, String port, String stdID){
        this.stdID = stdID;
        communicator = new ClientCommunicator(this, serverIP, Integer.parseInt(port), stdID);
        communicator.start();
    }


    public void setExamList(ArrayList<String> examList) {
        this.examList = examList;
        System.out.println(examList);
    }

    public void showExamPage(String examName, String examDetails){
        examController.setDetails(examDetails);
        stage.setScene(examPage);

    }

    public static void main(String[] args) {
        launch(args);
    }

    public void tryRegistration(String examName) {
        myExamName = examName;
        communicator.tryRegistration(examName);
    }



    public String getMyExamName() {
        return myExamName;
    }

    public Stage getStage() {
        return stage;
    }

    @Override
    public void stop(){
        System.exit(112);
    }

    public void log(String s) {
        examController.updateLog(s);
        homeController.updateLog(s);
    }

    public void updateCorrection(String s) {
        examController.updateCorrection(s);
    }


    public String getStdID() {
        return stdID;
    }

    public void tryReconnecting(String examName) {
        myExamName = examName;
        communicator.tryReconnection(examName);
    }
}
