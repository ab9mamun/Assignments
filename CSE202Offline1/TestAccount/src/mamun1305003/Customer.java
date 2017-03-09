package mamun1305003;

/**
 * Created by Abdullah on 9/17/2015.
 */
public class Customer {
    private String name;
    private String email;
    private char gender;

    //constructors
    public Customer(String nam, String mail, char sex) {
        name = nam;
        email = mail;
        gender = sex;
    }
    public Customer(String nam) {
        name = nam;
    }
    //get methods
    public String getName() {
        return name;
    }
    public String getEmail() {
        return email;
    }
    public char getGender() {
        return gender;
    }
    //set methods
    public void setGender(char sex) {
        gender = sex;
    }
    public void setEmail(String mail) {
        email = mail;
    }
    //print method
    public void print() {
        System.out.println(name + " (" + gender + ") at " + email );
    }

}
