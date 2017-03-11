package serverpkg;

public class ServerValues {
    public static final int serverPort = 6000;

    public static String projectPath() {return System.getProperty("user.dir");}
   ///------------------------------------------------------------server_config--------------------------------------
    private static String answerPath = projectPath();
    private static String questionPath = projectPath()+"\\Resource\\question.docx";

    public static String questionPath(){
        return questionPath;
    }
    public static String answerPath() {
        return answerPath;
    }





}
