package serverpkg;

import javafx.application.Application;
import javafx.stage.Stage;

public class Server extends Application {

    @Override
    public void start(Stage primaryStage){

      try{  (new ServerMain()).start(new Stage());
        }catch (Exception e){
          e.printStackTrace();
      }
    }

    @Override
    public void stop(){
        System.exit(125);
    }

    public static void main(String[] args) {
        launch(args);
    }

}
