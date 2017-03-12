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
    private HashMap<String, Examinee> map;


    public Exam(ServerMain main, String name, int startTime, int duration, int warningTime,
                int backupInterval, String validIDs, String allowableApps,
                String answerPath, String questionPath, String rules) {

        examinees = new ArrayList<>();
        map = new HashMap<>();

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

        if(ServerValues.verify(validIDs, stdID)==false) return stdID+":"+name+":"+"REJECTED:Please sign up with a valid StudentID.\n";


        String ip = socket.getInetAddress().getHostAddress();
        for(Examinee x:examinees){
                String i = x.getIp();
                int id = x.getStdID();

            if(id==stdID && i.equals(ip)) {
                return stdID+":"+name+":"+"REJECTED:You have already registered.\n";
            }
            if(i.equals(ip)) {

                Platform.runLater(()->main.log("Registration request from "+ip+" again. 5 seconds to decide."));;
                try {
                    Thread.sleep(5000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                if(sameIPAllowed) {
                    Platform.runLater(()->main.log("Registration accepted for "+ip));;
                    break;
                }
                Platform.runLater(()->main.log("Registration rejected for "+ip));;
                return stdID+":"+name+":"+"REJECTED:Registration denied.";
            }
            if(id==stdID){

                Platform.runLater(()->main.log("Registration request from "+stdID+" again. 5 seconds to decide."));;
                try {
                    Thread.sleep(5000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                if(sameStdIdAllowed) {
                    Platform.runLater(()->main.log("Registration accepted for "+stdID));;
                    break;
                }
                Platform.runLater(()->main.log("Registration rejected for "+stdID));;
                return stdID+":"+name+":"+"REJECTED:Registration denied.";

            }


        }
        register(stdID,socket);
        return stdID+":"+name+":"+"ACCEPTED:"+name+"|"+startTime+"|"+currentTime+"|"+duration+"|"+backupInterval+"|"+rules+"|"+allowableApps;
    }
    private void register(int stdid, Socket socket){
        Examinee newExaminee = new Examinee(stdid, socket, startTime);
        examinees.add(newExaminee);
        map.put(stdid+"_"+newExaminee.getIp(), newExaminee);
        main.mapSocketWithExaminee(socket, newExaminee);
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

                    if(currentTime== startTime+duration-warningTime){
                        for(Examinee x:examinees){
                            main.sendMessage(x.getSocket(),x.getStdID()+":"+name+":UPDATE"+":WARNING:"+warningTime+" seconds left");
                        }
                    }
                     if(currentTime!=startTime && (currentTime-startTime)%backupInterval==0 && currentTime != startTime+duration){
                        for(Examinee x:examinees){
                            main.sendMessage(x.getSocket(),x.getStdID()+":"+name+":SEND FILE");
                            checkLastBackup(x);
                        }
                    }
                    currentTime++;


                }
///----------------------------------------------------------------------------------------------------------exam ends--------------
                for(Examinee x: examinees){

                    main.sendMessage(x.getSocket(), x.getStdID()+":"+name+":UPDATE"+":"+"EXAM END");
                    main.sendMessage(x.getSocket(),x.getStdID()+":"+name+":SEND FILE");
                    checkLastBackup(x);
                }
                Platform.runLater(()->main.log("Exam ended"));;
            }



        }.start();
    }


    private void checkLastBackup(Examinee x){

        new Thread(){
            @Override
            public void run(){
                if(x.getLastBackup() < currentTime-backupInterval*1.5){
                    Platform.runLater(()->main.log(x.getStdID()+"_"+x.getIp()+" is not sending backups"));;
                }
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

    public void updateBackup(String sender, String ip) {
        map.get(sender+"_"+ip).setLastBackup(currentTime);
    }


    public String reconnectRequest(Socket socket, String sender, String ip) {
        Examinee examinee = map.get(sender+"_"+ip);
        if(examinee==null) return sender+":"+name+":REJECTED:You were not registered";
        if(examinee.getSocket()!=null) return sender+":"+name+":REJECTED:You were conencted from another process";
        if(currentTime>=startTime+duration) return sender+":"+name+":REJECTED:Exam ended";

        ///--------------------------
        examinee.setSocket(socket);

        Thread backupSender = new Thread(){
            @Override
            public void run(){
                try {
                    Thread.sleep(3000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                if(examinee.getLastBackup()> startTime)
                main.sendQuestionPaper(socket,sender+":"+name+":RECEIVE FILE", getAnswerPathForStudent(sender, ip));

                else if(examinee.getLastBackup()==startTime){
                    main.sendQuestionPaper(socket,sender+":"+name+":RECEIVE FILE", questionPath);
                }
            }
        };

         backupSender.start();
        return sender+":"+name+":"+"ACCEPTED_RECONNECT:"+name+"|"+startTime+"|"+currentTime+"|"+duration+"|"+backupInterval+"|"+rules+"|"+allowableApps;
    }
}
