package test;

import serverpkg.ServerValues;

import javax.swing.*;
import java.io.*;
import java.util.ArrayList;

public class Test {
    public static void main(String[] args) {
            String valid = "201305001-201305120,200805001,200905127";
            String stdId = "200805001";

        System.out.println(verify(valid, stdId));
    }

    public static boolean verify(String valid, String stdid){

        int id = Integer.parseInt(stdid);


        String[] commas = valid.split(",");
        for(int i=0; i<commas.length; i++){
            if (commas[i].contains("-")) {
                String[] ranges = commas[i].split("-");
                int low = Integer.parseInt(ranges[0]);
                int high = Integer.parseInt(ranges[1]);
                if (low <= id && id <= high) {
                    return true;
                }
            }
            else if(id==Integer.parseInt(commas[i])) return true;

        }



        return false;

    }

    public void lame(){


        String s = "no cheating\n" +
                "no sidetalk\n" +
                "no app except the allowable apps";
        System.out.println(s.replace('\n', '#'));
        final String dir = System.getProperty("user.dir");
        System.out.println("current dir = " + dir);

        //    JOptionPane jon = new JOptionPane();
        //  int x = jon.showConfirmDialog(null, "Registration request from existing IP.\nDo you accept?");
        //  System.out.println(x);


        File file = new File(ServerValues.questionPath());
        File file2 = new File(ServerValues.questionPath()+"p");

        try {
            BufferedInputStream bis = new BufferedInputStream(new FileInputStream(file));
            BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(file2));
            int byteread = 0;
            byte[] contents = new byte[100000];
            while(true){
                byteread = bis.read(contents);
                if(byteread<=0) break;
                bos.write(contents, 0, byteread);

            }
            bos.flush();
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }
}
