

import java.util.Arrays;

public class State implements Comparable{
    static int n;
    public int[][] mat;
    int costFromInit;
    double totalCost;
    static State initialState;
    static State goalState;
    static State[] optionalGoalStates;
    boolean isthisgoal;
    static boolean anystate;

    State parentState;
    String movement;


    public static void init(int n, State is, State gs, boolean a_state){
        State.n = n;
        initialState = is;
        goalState = gs;
        is.costFromInit = 0;

        is.parentState = null;
        is.isthisgoal = false;
        gs.isthisgoal = true;
        is.movement = "Initially";
        gs.movement = "Goal State";
        anystate = a_state;

        if(a_state){
            optionalGoalStates = new State[4];
            optionalGoalStates[0] = goalState;
            for(int i=1; i<4; i++){
                optionalGoalStates[i] = new State();
                optionalGoalStates[i].mat = optionalGoalStates[i-1].mat.clone();
                rotateMatrix(n, optionalGoalStates[i].mat);
            }

        }
        is.totalCost = is.h();

    }

    public State(){

    }


    public State(State parent, Direction dir, int position){

        mat = new int[n][n];
        costFromInit = parent.costFromInit+1;
        parentState = parent;

        if(dir==Direction.left || dir==Direction.right){
            movement = "Row    "+(position+1)+": "+dir;
        }
        else {
            movement = "Column "+(position+1)+": "+dir;
        }

        for(int i=0; i<n; i++){
            for(int j=0; j<n; j++){
                mat[i][j] = parent.mat[i][j];
            }
        }

        rotate(mat, dir, position);

        totalCost = costFromInit + h();

    }

    private  void rotate(int[][] mat, Direction dir, int position) {

        if(dir==Direction.left) leftRotate(mat, position);
        if(dir== Direction.right) rightRotate(mat, position);
        if(dir==Direction.up) upRotate(mat, position);
        if(dir==Direction.down) downRotate(mat, position);

    }

    private  void leftRotate(int[][] mat, int position) {
        int temp = mat[position][0];
        for(int j = 0; j<n-1; j++){
            mat[position][j] = mat[position][j+1];
        }
        mat[position][n-1] = temp;
    }
    private  void rightRotate(int[][] mat, int position) {
        int temp = mat[position][n-1];
        for(int j = n-1; j>0; j--){
            mat[position][j] = mat[position][j-1];
        }
        mat[position][0] = temp;
    }
    private  void upRotate(int[][] mat, int position) {
        int temp = mat[0][position];
        for(int i = 0; i<n-1; i++){
            mat[i][position] = mat[i+1][position];
        }
        mat[n-1][position] = temp;
    }
    private  void downRotate(int[][] mat, int position) {
        int temp = mat[n-1][position];
        for(int i = n-1; i>0; i--){
            mat[i][position] = mat[i-1][position];
        }
        mat[0][position] = temp;
    }

    //missmatch heuristic function-----------
    public int h(State goal){
        int missmatch=0;
        for(int i=0; i<n; i++){
            for(int j=0; j<n; j++){
                if(mat[i][j] != goal.mat[i][j]){
                    missmatch++;
                }
            }
        }

        if(missmatch==0) isthisgoal = true;

        return  missmatch;
    }

    public double h(){
        int missmatch = h(goalState);
        if(!anystate) return missmatch*1.0/n;

        for(int i=1; i<4; i++)
            missmatch = Math.min(missmatch, h(optionalGoalStates[i]));

        if(missmatch==0) isthisgoal = true;
        return missmatch*1.0/n;

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


    public void print(){
        printMovement();
        Test.printMat(mat);
    }

    public static void printGoalState(){
        goalState.print();
    }
    public static void printInitState(){
        initialState.print();
    }

    public void printMovement() {
        System.out.println(movement);
    }


    @Override
    public int hashCode() {
//        int[] b = new int[n*n];
//        for(int i=0; i<n; i++){
//            for(int j=0; j<n; j++){
//                b[i*n+j] = mat[i][j];
//            }
//        }
        //return Arrays.hashCode(b);
        return Arrays.hashCode(mat);
    }

    @Override
    public boolean equals(Object obj) {
        if(obj instanceof State){
            State s = (State) obj;
            if(mat.length!=s.mat.length) return false;
            for(int i=0; i<n; i++){
                if(mat[i].length!=s.mat.length) return false;
                for(int j=0; j<n; j++){
                    if(mat[i][j]!=s.mat[i][j]) return false;
                }
            }

            return true;

        }
        return false;
    }


    static void rotateMatrix(int N, int mat[][])
    {
        // Consider all squares one by one
        for (int x = 0; x < N / 2; x++)
        {
            // Consider elements in group of 4 in
            // current square
            for (int y = x; y < N-x-1; y++)
            {
                // store current cell in temp variable
                int temp = mat[x][y];

                // move values from right to top
                mat[x][y] = mat[y][N-1-x];

                // move values from bottom to right
                mat[y][N-1-x] = mat[N-1-x][N-1-y];

                // move values from left to bottom
                mat[N-1-x][N-1-y] = mat[N-1-y][x];

                // assign temp to left
                mat[N-1-y][x] = temp;
            }
        }
    }
}
