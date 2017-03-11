package serverpkg;

import javafx.fxml.FXML;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.text.Text;
import javafx.stage.DirectoryChooser;
import javafx.stage.FileChooser;

import java.io.File;

public class ServerConfigController {
    private ServerMain main;

    @FXML
    private TextField examName;
    @FXML
    private TextField examStartTime;
    @FXML
    private TextField examDuration;
    @FXML
    private TextField examWarningTime;
    @FXML
    private TextField examBackupInterval;
    @FXML
    private TextField examQuestionPath;
    @FXML
    private TextField examAnswerPath;
    @FXML
    private TextArea examRules;
    @FXML
    private TextField validIDs;
    @FXML
    private TextField allowableApps;

//fxml-on action
    public void chooseExamQuestionPath(){
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Select Question Paper");
        File file = fileChooser.showOpenDialog(main.getStage());
        if(file!=null) {
            examQuestionPath.setText(file.getAbsolutePath());
        }

    }
    //fxml-on action
    public void configureDone(){
        main.createExam( examName.getText(),  Integer.parseInt(examStartTime.getText()),  Integer.parseInt(examDuration.getText()),
                Integer.parseInt(examWarningTime.getText()),
                Integer.parseInt(examBackupInterval.getText()),
         validIDs.getText(),  allowableApps.getText(),  examAnswerPath.getText(),  examQuestionPath.getText(),  examRules.getText());
    }

    //fxml-on action
    public void chooseExamAnswerPath(){
        DirectoryChooser directoryChooser = new DirectoryChooser();
        directoryChooser.setTitle("Select Answer Location");
        File file = directoryChooser.showDialog(main.getStage());
        if(file!=null) {
            examAnswerPath.setText(file.getAbsolutePath());
        }
    }


    public void setMain(ServerMain main) {
        this.main = main;
    }

    public void initialize(){

        examQuestionPath.setText(ServerValues.questionPath());
        examAnswerPath.setText(ServerValues.answerPath());


    }
}
