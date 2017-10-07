import java.lang.reflect.Array;
import java.util.ArrayList;

public class State implements Comparable{
    static int n;
    public int[][] mat;
    int costFromInit;
    double totalCost;
    static State initialState;
    static State goalState;

    State parentState;


    public static void init(int n, State is, State gs){
        State.n = n;
        initialState = is;
        goalState = gs;
        is.costFromInit = 0;
        is.totalCost = is.h();
        is.parentState = null;

    }

    public State(){

    }


    public State(State parent, Direction dir, int position){

        mat = new int[n][n];
        costFromInit = parent.costFromInit+1;
        parentState = parent;

        for(int i=0; i<n; i++){
            for(int j=0; j<n; j++){
                mat[i][j] = parent.mat[i][j];
            }
        }

        rotate(mat, dir, position);

        totalCost = costFromInit + h();

    }

    private void rotate(int[][] mat, Direction dir, int position) {
        if(dir==Direction.left) leftRotate(mat, position);
        if(dir==Direction.right) rightRotate(mat, position);
        if(dir==Direction.up) upRotate(mat, position);
        if(dir==Direction.down) downRotate(mat, position);
    }

    private void leftRotate(int[][] mat, int position) {
        int temp = mat[position][0];
        for(int j = 0; j<n-1; j++){
            mat[position][j] = mat[position][j+1];
        }
        mat[position][n-1] = temp;
    }
    private void rightRotate(int[][] mat, int position) {
        int temp = mat[position][n-1];
        for(int j = 1; j<n; j++){
            mat[position][j] = mat[position][j-1];
        }
        mat[position][0] = temp;
    }
    private void upRotate(int[][] mat, int position) {
        int temp = mat[0][position];
        for(int i = 0; i<n-1; i++){
            mat[i][position] = mat[i+1][position];
        }
        mat[n-1][position] = temp;
    }
    private void downRotate(int[][] mat, int position) {
        int temp = mat[n-1][position];
        for(int i = 1; i<n; i++){
            mat[i][position] = mat[i-1][position];
        }
        mat[0][position] = temp;
    }

    //missmatch heuristic function-----------
    public double h(){
        int missmatch=0;
        for(int i=0; i<n; i++){
            for(int j=0; j<n; j++){
                if(mat[i][j]!=goalState.mat[i][j]){
                    missmatch++;
                }
            }
        }
        return ((double) missmatch)/n;
    }

    public double getTotalCost(){
        return totalCost;
    }


    @Override
    public int compareTo(Object o) {
        if(!( o instanceof State))
            return -1;
        State s  = (State) o;
        return (new Double(totalCost)).compareTo(new Double(s.totalCost));
    }
}
