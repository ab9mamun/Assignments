package clientpkg;

import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.Pagination;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;

import java.util.ArrayList;
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
    @FXML
    private TextArea log;







    public void initialize(){
        examName.setEditable(false);
        examStartTime.setEditable(false);
        examBackupInterval.setEditable(false);
        serverTime.setEditable(false);
        examDuration.setEditable(false);
        allowableApps.setEditable(false);

        examRules.setEditable(false);
        examCorrections.setEditable(false);
        log.setEditable(false);


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

        stopWatch();


    }
    public void setMain(ClientMain main) {
        this.main = main;
    }


    public void updateLog(String msg){
        log.setText(log.getText()+msg+"\n");
    }

    public void updateCorrection(String s) {
        examCorrections.setText(s);
    }




    public void stopWatch(){
        new Thread(){
            @Override
            public void run(){
                int currentTime = Integer.parseInt(serverTime.getText());
                int endTime = Integer.parseInt(examStartTime.getText())+ Integer.parseInt(examDuration.getText());
                while(currentTime<=endTime){
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    currentTime++;
                    final int x = currentTime;
                    Platform.runLater(
                            ()->serverTime.setText(x+"")
                    );
                }

            }
        }.start();
    }
}
