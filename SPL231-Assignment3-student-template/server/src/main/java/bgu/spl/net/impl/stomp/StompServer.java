package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        int port = Integer.parseInt(args[0]);
        String serverType = args[1];
        if(serverType.equals("tpc")){
            Server.threadPerClient(
                port, 
                StompMessagingProtocolImpl::new, //protocol factory
                stompEncoderDecoder::new //message encoder decoder factory
            ).serve();      
        }

        else if(serverType.equals("reactor")){
            Server.reactor(
                3, 
                port,
                StompMessagingProtocolImpl::new, //protocol factory
                () -> new stompEncoderDecoder() //message encoder decoder factory
            ).serve();
        }
    }
}
