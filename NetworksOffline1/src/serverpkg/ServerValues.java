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





    public static boolean verify(String valid, int id){



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

}
