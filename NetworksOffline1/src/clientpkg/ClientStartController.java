package clientpkg;

import javafx.fxml.FXML;
import javafx.scene.control.TextField;
import javafx.scene.text.Text;

import java.io.File;

public class ClientStartController {
    private ClientMain main;

    @FXML
    private TextField serverIP;
    @FXML
    private TextField serverPort;
    @FXML
    private TextField stdID;

    public void connectToServer(){

        System.out.println(stdID.getText());
        ClientMain.main().connectToServer(serverIP.getText(), serverPort.getText(), stdID.getText());
    }
}
