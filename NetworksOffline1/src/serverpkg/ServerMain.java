package serverpkg;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.net.Socket;

public class ServerMain extends Application {
    private static ServerMain main;

    private ServerCommunicator communicator;


    @Override
    public void start(Stage primaryStage) throws Exception{

        main = this;

        Parent root = FXMLLoader.load(getClass().getResource("server_start.fxml"));
        primaryStage.setTitle("Hello World");
        primaryStage.setScene(new Scene(root, 600,400));
        primaryStage.show();

        communicator = new ServerCommunicator();
        communicator.start();


    }


    public static void main(String[] args) {
        launch(args);
    }


    public static ServerMain main(){
        return main;
    }



    public void verifyStudent(Socket clientSocket, String stdID){
        System.out.println(stdID);
    }
}
