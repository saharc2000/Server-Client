package bgu.spl.net.srv;

import java.util.LinkedList;

public class Channel {
    private String name;
    private LinkedList <User> subscriptedUsers;
    
    public Channel(String name){
        this.name = name;
        this.subscriptedUsers = new LinkedList<User>();
    }

    public void add(User user){
        subscriptedUsers.add(user);
    }

    public boolean remove(User user){
        return subscriptedUsers.remove(user);
    }

    public String getName(){
        return this.name;
    }

    public LinkedList <User> getSubscriptedUsers(){
        return this.subscriptedUsers;
    }
}
