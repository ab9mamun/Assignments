package test;

import serverpkg.ServerValues;

import javax.swing.*;
import java.io.*;

public class Test {
    public static void main(String[] args) {
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
