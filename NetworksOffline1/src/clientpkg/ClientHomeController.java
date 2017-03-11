package clientpkg;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.ComboBox;

import java.util.ArrayList;
import java.util.Observable;

public class ClientHomeController {
    private ClientMain main;
    @FXML
    private ComboBox<String> examNames;



    public void applyForRegistration(){
        String examName = examNames.getValue();
        System.out.println(examName);
        main.tryRegistration(examName);
    }





    public void updateExamList(ArrayList<String> examList){
        ObservableList<String> data = FXCollections.observableArrayList();
        data.addAll(examList);
        examNames.setItems(data);

    }





    public void setMain(ClientMain main) {
        this.main = main;
    }

    public void initialize(){

    }
}
