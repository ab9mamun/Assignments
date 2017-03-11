package test;

import javax.swing.*;

public class Test {
    public static void main(String[] args) {
        final String dir = System.getProperty("user.dir");
        System.out.println("current dir = " + dir);

        JOptionPane jon = new JOptionPane();
        int x = jon.showConfirmDialog(null, "Registration request from existing IP.\nDo you accept?");
        System.out.println(x);
    }
}
