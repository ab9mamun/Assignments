package clientpkg;

public class ClientValues {


   private final static boolean init = true;
    public static String projectPath() {return System.getProperty("user.dir");}
    ///----------------------------------------client_start----------------------------------
    public final static String serverIP = "127.0.0.1";
    private static String filePath;



    public static String filePath(){
        if(filePath==null){
            filePath = projectPath()+"\\Client\\answer.docx";
        }
        return filePath;
    }

    public static void setFilePath(String filePath) {
        ClientValues.filePath = filePath;
    }
}
