package clientpkg;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class ClientMain extends Application {
    private static ClientMain main;
    private ClientCommunicator communicator;


    @Override
    public void start(Stage primaryStage) throws Exception{
        main = this;

        Parent root = FXMLLoader.load(getClass().getResource("client_start.fxml"));
        primaryStage.setTitle("Hello World");
        primaryStage.setScene(new Scene(root, 600,400));
        primaryStage.show();
    }




    public void connectToServer(String serverIP, String port, String stdID){
        communicator = new ClientCommunicator(serverIP, Integer.parseInt(port), stdID);
        communicator.start();
    }

    public static void main(String[] args) {
        launch(args);
    }



    public static ClientMain main(){
        return main;
    }

}
