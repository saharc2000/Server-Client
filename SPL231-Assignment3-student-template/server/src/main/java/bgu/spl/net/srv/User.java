package bgu.spl.net.srv;

public class User {
    private String userName;
    private String password;
    private volatile boolean connect;
    private int connectionId;

    public User(String userName,String password, int connectionId){
        this.password = password;
        this.userName = userName;
        this.connect = true;
        this.connectionId = connectionId;
    }
    public String getUserName(){
        return this.userName;
    }

    public String getPassword(){
        return this.password;
    }

    public boolean isConnected(){
        return this.connect;
    }

    public void setConnected(boolean isConnect){
        this.connect = isConnect;
    }

    public int getConnectionId(){
        return this.connectionId;
    }

    public void setConnectionId(int connectionId){
        this.connectionId=connectionId;
    }
}
