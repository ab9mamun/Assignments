package serverpkg;

import java.net.Socket;

public class Examinee {
    private int StdID;
    private String ip;
    private Socket socket;

    public Examinee(int stdID, Socket socket) {
        StdID = stdID;
        this.socket = socket;
        ip = socket.getInetAddress().getHostAddress();
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
}