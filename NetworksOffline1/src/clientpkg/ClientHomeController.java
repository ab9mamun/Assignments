package clientpkg;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.ComboBox;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.stage.DirectoryChooser;

import java.io.File;
import java.util.ArrayList;
import java.util.Observable;

public class ClientHomeController {
    private ClientMain main;
    @FXML
    private ComboBox<String> examNames;
    @FXML
    private TextField answerPath;
    @FXML
    private TextArea log;
    @FXML
    private TextField stdID;

    public void chooseAnswerPath(){

        DirectoryChooser directoryChooser = new DirectoryChooser();
        directoryChooser.setTitle("Select Answer Location");
        File file = directoryChooser.showDialog(main.getStage());
        if(file!=null) {
            answerPath.setText(file.getAbsolutePath());
           ClientValues.setFilePath(file.getAbsolutePath()+"\\answer.docx");
        }
    }



    public void applyForRegistration(){
        String examName = examNames.getValue();
        main.setStdID(stdID.getText());
        if(examName==null || examName.equals("")) return;
        System.out.println(examName);
        //ClientValues.setFilePath(answerPath.getText()+"\\answer.docx");
        main.tryRegistration(examName);
    }

    public void applyForReconnect(){
        String examName = examNames.getValue();
        main.setStdID(stdID.getText());
        if(examName==null || examName.equals("")) return;
        System.out.println(examName);
        //ClientValues.setFilePath(answerPath.getText()+"\\answer.docx");
        main.tryReconnecting(examName);
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
        log.setEditable(false);

    }

    public void updateLog(String msg) {
        log.setText(log.getText()+msg+"\n");
    }


    public void setStdID(String stdID) {
        this.stdID.setText(stdID);
    }
}
