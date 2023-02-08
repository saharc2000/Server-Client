package bgu.spl.net.impl.stomp;

import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl<T> implements StompMessagingProtocol<T>{

    private int connectionId;

    private int messageId=0;

    private Connections<T> connections;

    private String[] lines;

    private ConcurrentHashMap<Integer,String> subsIdToChannelMap = new ConcurrentHashMap<>();

    private ConcurrentHashMap<String,Integer> channelToSubIdMap = new ConcurrentHashMap<>();

    private volatile boolean shouldTerminate = false;

    public void start(int connectionId, Connections<T> connections){
        this.connectionId = connectionId;
        this.connections = connections;
    }

    public T process(T message){
        if(!(message instanceof String)){
            return (T)buildErrorFrame("message illegal", "");
        }
        lines= ((String)message).split("\n");
        String header = lines[0];
        String response=null;
        if(header.equals("SEND")){
            response = processSEND();
        }
        else if(header.equals("SUBSCRIBE")){
            response = processSUBSCRIBE();
        }
        else if(header.equals("UNSUBSCRIBE")){
            response = processUNSUBSCRIBE();
        }

        else if(header.equals("CONNECT")){
            response = processCONNECT();
        }

        else if(header.equals("DISCONNECT")){
            response = processDISCONNECT();
        }
        else if(!(header.equals("")||header==null)){
            return (T)buildErrorFrame("illegal command", "");
        }
        return (T)response;

    }

    public boolean shouldTerminate(){
        return this.shouldTerminate;
    }

    private void setShouldTerminate(boolean shouldTerminate){
        this.shouldTerminate = shouldTerminate;
    }

    private String processSEND(){
        String channel = "";
        int receiptId=-1;
        for(int i=1; i<3; i++){
            if(!lines[i].equals("")&&!lines[i].equals("\n")){
                String line[] = lines[i].split(":");
                if(line[0].equals("destination")) channel = line[1].substring(1);
                else if(line[0].equals("receipt")) receiptId = Integer.parseInt(line[1]);
            }
        }
        if(channel.equals("")) return buildErrorFrame("destination not found", "");    
        String body="",message="";
        for(int i=3;i<lines.length;i++)
            body += lines[i]+"\n";
        if(connections.isSubscribed(connectionId,channel)){
            message = buildMessageFrame(channel)+body;
            connections.send(channel, (T)message);
        }
        else{
            String error = buildErrorFrame("message wasn't sent", "destined:"+channel);
            return error;
        }
        if(receiptId!=-1){
            return buildReceiptFrame(Integer.toString(receiptId));
        }
        return null;
    }

    private String processSUBSCRIBE(){
        int subscriptionId=-1;
        String channel=null;
        String receiptIdFromFrame="";
        for(int i=1; i<4; i++){
            String line[] = lines[i].split(":");
            if(line[0].equals("id"))
                subscriptionId= Integer.parseInt(line[1]);
            else if(line[0].equals("destination")){
                channel = line[1].substring(1);
                if(subsIdToChannelMap.contains(channel))
                    return buildErrorFrame("You were already joined to this channel", "destined:"+channel);;
            }
            else if(line[0].equals("receipt")){
                receiptIdFromFrame = line[1];
            }
            else{
                String error = buildErrorFrame("wrong subscription frame", "destined:"+channel);
                return error;
            }
        }
        if(connections.subscribe(connectionId, channel)){
            subsIdToChannelMap.put(subscriptionId,channel);
            channelToSubIdMap.put(channel,subscriptionId);
        }
        else{
            return buildErrorFrame("can't join the channel", "");
        }
        
        return buildReceiptFrame(receiptIdFromFrame);
    }

    private String processUNSUBSCRIBE(){
        int i = 1;
        String ans = null;
        while(i<lines.length-1){
            String line[] = lines[i].split(":");
            if(line[0].equals("id")){
                int subscriptionId= Integer.parseInt(line[1]);
                if(subsIdToChannelMap.containsKey(subscriptionId)){
                    String channel = subsIdToChannelMap.remove(subscriptionId);
                    channelToSubIdMap.remove(channel);
                    connections.unSubscribe(connectionId, channel);
                }
                else{
                    String error = buildErrorFrame("Subscription id was not found", "");
                    return error;
                }
            }
            else if(line[0].equals("receipt")){
                ans = buildReceiptFrame(line[1]);
            }
            i++;
        }
        return ans;
    }

    private String processCONNECT(){
        String userName=null;
        String password=null;
        String version="";
        String receipt="";
        for(int i=1; i<6; i++){
            if(!lines[i].equals("")&!lines[i].equals("\n")){
                String line[] = lines[i].split(":");
                if(line[0].equals("accept-version")){
                    version = line[1];
                }
                else if(line[0].equals("login")){
                    userName = line[1]; 
                }
                else if(line[0].equals("passcode")){
                    password = line[1];
                }
                else if(line[0].equals("receipt")){
                    receipt = "RECEIPT\nreceipt-id"+line[1];
                    connections.send(connectionId, (T)receipt);
                }
                else if(!line[0].equals("host")){
                    return buildErrorFrame("wrong connect frame", "version:"+version);
                }
            }
        }
        if(userName==null||password==null) return buildErrorFrame("wrong connect frame", "version:"+version);
        String response = connections.connect(connectionId,userName,password);
        if(response.equals("Login successful"))
            return "CONNECTED\nversion"+version+"\n";
        else if (response.equals("User already logged in")){
            return buildErrorFrame("User already logged in", "");
        }
        else if(response.equals("Wrong password")){
            return buildErrorFrame("Wrong password", "");
        }
        else if(response.equals("The client is already logged in, log out before trying again")){
            return buildErrorFrame("The client is already logged in, log out before trying again", "");
        }
        return response;
    }

    private String processDISCONNECT(){
        String line1[] = lines[1].split(":");
        if(!line1[0].equals("receipt")) return buildErrorFrame("send reciept", "");
        int receiptId = Integer.parseInt(line1[1]);
        connections.disconnect(connectionId);
        subsIdToChannelMap.clear();
        channelToSubIdMap.clear();
        
        // this.setShouldTerminate(true);
        return buildReceiptFrame(Integer.toString(receiptId));
    }

    private String buildErrorFrame(String message, String messageBody){
        connections.send(connectionId,(T)( "ERROR\nmessage: " + message + "\n\n" + "The message:\n-----\n" + messageBody+"\n"));
        connections.disconnect(connectionId);
        subsIdToChannelMap.clear();
        channelToSubIdMap.clear();
        this.setShouldTerminate(true);
        return null;
    }

    private String buildReceiptFrame(String receiptId){
        return "RECEIPT\nreceipt-id:" + receiptId + "\n";
    }
    
    private String buildMessageFrame(String channel){
        return "MESSAGE\nsubscription:"+ channelToSubIdMap.get(channel)+"\nmessage-id:"+messageId+"\ndestination:/"+channel+"\n\n";
    }
}