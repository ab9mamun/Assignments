package serverpkg;

import javafx.application.Platform;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonType;
import org.jetbrains.annotations.NotNull;

import javax.swing.*;
import java.io.File;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Optional;
import java.util.Scanner;

public class Exam {
    ServerMain main;

    private boolean flag;

    private  String name;
    private  int startTime;
    private  int duration;
    private  int warningTime;
    private  int backupInterval;
    private  String validIDs;
    private  String allowableApps;
    private String answerPath;
    private String questionPath;
    private  String rules;
    private boolean sameIPAllowed;
    private boolean sameStdIdAllowed;

    private int currentTime;

    private ArrayList<Examinee> examinees;


    public Exam(ServerMain main, String name, int startTime, int duration, int warningTime,
                int backupInterval, String validIDs, String allowableApps,
                String answerPath, String questionPath, String rules) {

        examinees = new ArrayList<>();

        this.main = main;

        currentTime = 0;

        this.name = name;
        this.startTime = startTime;
        this.duration = duration;
        this.warningTime = warningTime;
        this.backupInterval = backupInterval;
        this.validIDs = validIDs;
        this.allowableApps = allowableApps;
        this.answerPath = answerPath+"\\"+name; (new File(this.answerPath)).mkdir();
        this.questionPath = questionPath;
        this.rules = rules;

        startExamMonitor();
    }



    public String registerRequest(int stdID, Socket socket){
        String ip = socket.getInetAddress().getHostAddress();
        for(Examinee x:examinees){
                String i = x.getIp();
                int id = x.getStdID();

            if(id==stdID && i.equals(ip)) {
                return stdID+":"+name+":"+"REJECTED:You have already registered.\n";
            }
            if(i.equals(ip)) {

                if(sameIPAllowed) break;
                return stdID+":"+name+":"+"REJECTED:Registration denied.";
            }
            if(id==stdID){

                if(sameStdIdAllowed) break;
                return stdID+":"+name+":"+"REJECTED:Registration denied.";

            }


        }
        register(stdID,socket);
        return stdID+":"+name+":"+"ACCEPTED:"+name+"|"+startTime+"|"+currentTime+"|"+duration+"|"+backupInterval+"|"+rules+"|"+allowableApps;
    }
    private void register(int stdid, Socket socket){
        Examinee newExaminee = new Examinee(stdid, socket);
        examinees.add(newExaminee);
        (new File(answerPath+"\\"+newExaminee.getStdID()+"_"+newExaminee.getIp())).mkdir();


    }





    public void startExamMonitor(){
        new Thread(){
            @Override
            public void run(){
                while(currentTime<startTime) {
                    main.updateServerTime(currentTime);

                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    currentTime++;
                }
/////-----------------------------------------------------------------------------------------------------------exam started-----------------
                for(Examinee x: examinees){
                    main.sendMessage(x.getSocket(), x.getStdID()+":"+name+ ":UPDATE"+":"+"EXAM START");

                    new Thread(){
                        @Override
                        public void run(){
                            main.sendQuestionPaper(x.getSocket(), x.getStdID()+":"+name+":RECEIVE FILE",questionPath);
                        }
                    }.start();
                }
                Platform.runLater(()->main.log("Exam started"));;

                while(currentTime<(startTime+duration)){
                    main.updateServerTime(currentTime);

                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    currentTime++;
                    if(currentTime== startTime+duration-warningTime){
                        for(Examinee x:examinees){
                            main.sendMessage(x.getSocket(),x.getStdID()+":"+name+":UPDATE"+":WARNING:"+warningTime+" seconds left");
                        }
                    }
                     if(currentTime%backupInterval==0){
                        for(Examinee x:examinees){
                            main.sendMessage(x.getSocket(),x.getStdID()+":"+name+":SEND FILE");
                        }
                    }
                }
///----------------------------------------------------------------------------------------------------------exam ends--------------
                for(Examinee x: examinees){
                    main.sendMessage(x.getSocket(), x.getStdID()+":"+name+":UPDATE"+":"+"EXAM END");

                }
                Platform.runLater(()->main.log("Exam ended"));;
            }

        }.start();
    }




    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getStartTime() {
        return startTime;
    }

    public void setStartTime(int startTime) {
        this.startTime = startTime;
    }

    public int getDuration() {
        return duration;
    }

    public void setDuration(int duration) {
        this.duration = duration;
    }

    public int getWarningTime() {
        return warningTime;
    }

    public void setWarningTime(int warningTime) {
        this.warningTime = warningTime;
    }

    public int getBackupInterval() {
        return backupInterval;
    }

    public void setBackupInterval(int backupInterval) {
        this.backupInterval = backupInterval;
    }

    public String getValidIDs() {
        return validIDs;
    }

    public void setValidIDs(String validIDs) {
        this.validIDs = validIDs;
    }

    public String getAllowableApps() {
        return allowableApps;
    }

    public void setAllowableApps(String allowableApps) {
        this.allowableApps = allowableApps;
    }

    public String getAnswerPath() {
        return answerPath;
    }

    public void setAnswerPath(String answerPath) {
        this.answerPath = answerPath;
    }

    public String getQuestionPath() {
        return questionPath;
    }

    public void setQuestionPath(String questionPath) {
        this.questionPath = questionPath;
    }

    public String getRules() {
        return rules;
    }

    public void setRules(String rules) {
        this.rules = rules;
    }



    public boolean isSameIPAllowed() {
        return sameIPAllowed;
    }

    public void setSameIPAllowed(boolean sameIPAllowed) {
        this.sameIPAllowed = sameIPAllowed;
    }

    public boolean isSameStdIdAllowed() {
        return sameStdIdAllowed;
    }

    public void setSameStdIdAllowed(boolean sameStdIdAllowed) {
        this.sameStdIdAllowed = sameStdIdAllowed;
    }

    public void sendCorrection(String text) {
        for(Examinee x: examinees){
            main.sendMessage(x.getSocket(), x.getStdID()+":"+name+":UPDATE:CORRECTION:"+text);
        }
    }

    public String getAnswerPathForStudent(String stdId, String ip) {
        return answerPath+"\\"+stdId+"_"+ip+"\\backupAnswer.docx";
    }
}
