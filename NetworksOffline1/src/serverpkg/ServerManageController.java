package serverpkg;

import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.RadioButton;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;

public class ServerManageController {
    private ServerMain main;
    private Exam exam;
    @FXML
    private TextArea corrections;
    @FXML
    private TextArea log;
    @FXML
    private CheckBox dupIP;
    @FXML
    private CheckBox dupID;
    @FXML
    private TextField serverTime;




    public void onDupIP(){
        exam.setSameIPAllowed(dupIP.isSelected());
        System.out.println(exam.isSameIPAllowed());
    }
    public void onDupID(){
        exam.setSameStdIdAllowed(dupID.isSelected());
        System.out.println(exam.isSameStdIdAllowed());
    }

    public void sendCorrections(){
        exam.sendCorrection(corrections.getText());
    }

    public void setMain(ServerMain main) {
        this.main = main;
    }
    public void setExam(Exam exam){
        this.exam = exam;
    }

    public void initialize(){

        log.setEditable(false);
    }

    public void updateLog(String msg) {
        log.setText(log.getText()+msg+"\n");
    }

    public void updateServerTime(int currentTime) {
        serverTime.setText(currentTime+"");
    }
}
