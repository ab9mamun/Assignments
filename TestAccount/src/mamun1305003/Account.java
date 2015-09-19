package mamun1305003;

/**
 * Created by Abdullah on 9/17/2015.
 */
public class Account {
    private Customer customer;
    private int accountNo;
    private String branch;
    private double balance;

    public Account(Customer cust, int acctNo, double initBal) {
        customer = cust;
        accountNo = acctNo;
        balance = initBal;
    }
    public Account(Customer cust, int acctNo, String shakha) {
        customer = cust;
        accountNo = acctNo;
        branch = shakha;
    }
    //set methods
    public void setBranch(String shakha) {
        branch = shakha;
    }
    public void setBalance(double initBal) {
        balance = initBal;
    }
    public void credit(double add) {
        balance += add;
    }
    public void debit(double minus) {
        if(balance < minus) System.out.println("Amount withdrawn exceeds the current balance!");
        else balance -= minus;
    }
    //get methods
    public String getCustomerName() {
        return customer.getName();
    }
    public String getBranch() {
        return branch;
    }
    public int getAccountNo() {
        return accountNo;
    }
    public double getBalnce() {
        return balance;
    }

    public int getAccountNumber() {
        return accountNo;
    }
    public double getBalance() {
        return balance;
    }
    public Customer getCustomer() {
        return customer;
    }
    public void print() {
        System.out.println("Customer: " + customer.getName() + " Branch: " + branch + " A/C no: " + accountNo + " Balance: " + balance );
    }

}
