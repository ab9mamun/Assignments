package clientpkg;

import javafx.fxml.FXML;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;

import java.util.StringTokenizer;

public class ClientExamController {
    private ClientMain main;


    @FXML
    private TextField examName;

    @FXML
    private TextField examStartTime;

    @FXML
    private TextField examDuration;

    @FXML
    private TextField serverTime;

    @FXML
    private TextField examBackupInterval;

    @FXML
    private TextField allowableApps;

    @FXML
    private TextArea examRules;

    @FXML
    private TextArea examCorrections;







    public void initialize(){
        examName.setDisable(true);
        examStartTime.setDisable(true);
        examBackupInterval.setDisable(true);
        serverTime.setDisable(true);
        examDuration.setDisable(true);
        allowableApps.setDisable(true);

        examRules.setDisable(true);
        examCorrections.setDisable(true);


    }



    public void setDetails(String details){
        StringTokenizer tok = new StringTokenizer(details, "|");
        examName.setText(tok.nextToken());
        examStartTime.setText(tok.nextToken());
        serverTime.setText(tok.nextToken());
        examDuration.setText(tok.nextToken());
        examBackupInterval.setText(tok.nextToken());
        examRules.setText(tok.nextToken());
        allowableApps.setText(tok.nextToken());

    }
    public void setMain(ClientMain main) {
        this.main = main;
    }
}
