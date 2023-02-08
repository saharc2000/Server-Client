package bgu.spl.net.srv;

import java.util.LinkedList;
import java.util.concurrent.ConcurrentHashMap;

public class ConnectionsImpl<T> implements Connections<T>{

    private ConcurrentHashMap<Integer, ConnectionHandler<T>> mapConnectionsIdToCH = new ConcurrentHashMap<>();

    private ConcurrentHashMap<String, Channel> channels = new ConcurrentHashMap<String, Channel>();

    private ConcurrentHashMap<Integer,User> connectedUsersMap = new ConcurrentHashMap<Integer,User>();

    private ConcurrentHashMap<String,User> allUsersMap = new ConcurrentHashMap<String,User>();

    private Object lockForExistUsers = new Object();
    private Object lockForNewUsers = new Object();
    private Object lock = new Object();


    public boolean send(int connectionId, T msg){
        if(msg!=null){
            if(mapConnectionsIdToCH.containsKey(connectionId)){
                ConnectionHandler<T> ch = mapConnectionsIdToCH.get(connectionId);
                ch.send(msg);
                return true;
            }
        }
        return false;
    }

    public void send(String channel, T msg){
        Channel topic = null;
        topic = channels.get(channel);
        LinkedList<User> registeresToTopic = topic.getSubscriptedUsers();
        for(User user: registeresToTopic){
            send(user.getConnectionId(), msg);
        }
    }

    public void disconnect(int connectionId){
        User u = connectedUsersMap.get(connectionId);
        if(u!=null){
            u.setConnected(false);
            synchronized(lock){
                for (Channel channel: channels.values()) {
                    if((channel).getSubscriptedUsers().contains(u)){
                        (channel).remove(u);
                    }
                }
            }
            connectedUsersMap.remove(connectionId);
            mapConnectionsIdToCH.remove(connectionId);
        }
    }

    public void add(int connectionId, ConnectionHandler<T> ch){
        mapConnectionsIdToCH.put(connectionId, ch);
    }

    public boolean isSubscribed(int connectionId, String channel){
        if(isChannelExist(channel)){
            Channel topic = channels.get(channel);
            LinkedList<User> registeresToTopic = topic.getSubscriptedUsers();
            User user = connectedUsersMap.get(connectionId);
            return registeresToTopic.contains(user);
        }
        else{
            return false;
        }
    }

    public boolean subscribe(int connectionId, String channel){
        if(isChannelExist(channel)){
            Channel topic = channels.get(channel);
            User user = connectedUsersMap.get(connectionId);
            return topic.getSubscriptedUsers().add(user);
        }
        else{
            Channel topic = new Channel (channel);
            synchronized(lock){
                channels.put(channel, topic);
                User user = connectedUsersMap.get(connectionId);
                return topic.getSubscriptedUsers().add(user);
            }
        }
    }

    private boolean isChannelExist(String channel){
        return channels.containsKey(channel);
    }

    public boolean unSubscribe(int connectionId,String channel){
        Channel chToUnsubscribe = channels.get(channel);
        User user = connectedUsersMap.get(connectionId);
        synchronized(lock){
            return chToUnsubscribe.remove(user); 
        }
    }

    public String connect(int connectionId, String userName,String password){
            //checks if exsiting client
            User sucscribedUser = null;
            if(allUsersMap.containsKey(userName)){
                sucscribedUser = allUsersMap.get(userName);
                //checks if the client trying to connect with same user or not
                if(sucscribedUser.isConnected()){
                    return "User already logged in";
                }
                else{
                    if(sucscribedUser.getPassword().equals(password)){
                        synchronized(lockForExistUsers){
                            if(!sucscribedUser.isConnected()){
                                connectedUsersMap.put(connectionId, sucscribedUser);
                                sucscribedUser.setConnected(true);
                                sucscribedUser.setConnectionId(connectionId);
                                return "Login successful";
                            }
                            else return "User already logged in";
                        }
                    }
                    else{
                        return "Wrong password";
                    }
                }
            }
            //new user
            else{
                synchronized(lockForNewUsers){
                    if(!allUsersMap.containsKey(userName)){
                        User user = new User(userName, password,connectionId);
                        connectedUsersMap.put(connectionId,user);
                        allUsersMap.put(userName, user);
                        return "Login successful";
                    }
                    else return "User already logged in";
                }
            }
    }
}