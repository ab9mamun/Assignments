package serverpkg;

import java.net.Socket;

public class Examinee {
    private int StdID;
    private String ip;
    private Socket socket;
    private int lastBackup;

    private ServerMain main;

    public Examinee(int stdID, Socket socket, int lastBackup) {
        StdID = stdID;
        this.socket = socket;
        ip = socket.getInetAddress().getHostAddress();
        this.lastBackup = lastBackup;
    }

    public int getStdID() {
        return StdID;
    }

    public void setStdID(int stdID) {
        StdID = stdID;
    }

    public Socket getSocket() {
        return socket;
    }

    public void setSocket(Socket socket) {
        this.socket = socket;
        ip = socket.getInetAddress().getHostAddress();
    }

    public String getIp() {
        return ip;
    }

    public void setLastBackup(int lastBackup) {
        this.lastBackup = lastBackup;
    }

    public int getLastBackup() {
        return lastBackup;
    }


    public void setMain(ServerMain main) {
        this.main = main;
    }
}