package bgu.spl.net.srv;


public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    void add(int connectionId, ConnectionHandler<T> ch);

    boolean isSubscribed(int id, String channel);

    boolean subscribe(int connectionId, String channel);

    boolean unSubscribe(int subscribeId,String channel);

    String connect(int connectionId,String userName,String password);
}
