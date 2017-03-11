package clientpkg;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.util.ArrayList;

public class ClientMain extends Application {
    private ClientCommunicator communicator;
    private ClientStartController startController;
    private String stdID;
    private ArrayList<String> examList;


    @Override
    public void start(Stage primaryStage) throws Exception{

        FXMLLoader loader = new FXMLLoader();
        loader.setLocation(getClass().getResource("client_start.fxml"));
        Parent root = loader.load();
        startController = loader.getController();
        startController.setMain(this);

        primaryStage.setTitle("Hello World");
        primaryStage.setScene(new Scene(root, 600,400));
        primaryStage.show();
    }







    public void setStdID(String stdID){
        this.stdID = stdID;
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

    public static void main(String[] args) {
        launch(args);
    }

}
