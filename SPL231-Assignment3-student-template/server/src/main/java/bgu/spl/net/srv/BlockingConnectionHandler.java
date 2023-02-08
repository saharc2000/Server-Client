package bgu.spl.net.srv;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.impl.stomp.StompMessagingProtocolImpl;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;

public class BlockingConnectionHandler<T> implements Runnable, ConnectionHandler<T> {

    private final MessagingProtocol<T> protocol;
    private final MessageEncoderDecoder<T> encdec;
    private final Socket sock;
    private BufferedInputStream in;
    private BufferedOutputStream out;
    private volatile boolean connected = true;
    private Connections<T> connections;
    private int connectionId;

    public BlockingConnectionHandler(Socket sock, MessageEncoderDecoder<T> reader, MessagingProtocol<T> protocol,Connections<T> connections,int connectionId) {
        this.sock = sock;
        this.encdec = reader;
        this.protocol = protocol;
        this.connections = connections;
        this.connectionId = connectionId;
    }

    @Override
    public void run() {
        try (Socket sock = this.sock) { //just for automatic closing
            int read;

            connections.add(connectionId, this);

            if(protocol instanceof StompMessagingProtocolImpl){
                ((StompMessagingProtocolImpl)protocol).start(connectionId,connections);
            }
            
            out = new BufferedOutputStream(sock.getOutputStream());
            in = new BufferedInputStream(sock.getInputStream());


            while (!protocol.shouldTerminate() && connected && (read = in.read()) >= 0) {
                T nextMessage = encdec.decodeNextByte((byte) read);
                if (nextMessage != null) {
                    T response = protocol.process(nextMessage);
                    if (response != null) {
                        out.write(encdec.encode(response));
                        out.flush();
                    }
                }
            }

        } catch (IOException ex) {
            ex.printStackTrace();
        }

    }

    @Override
    public void close() throws IOException {
        connected = false;
        sock.close();
    }

    @Override
    public void send(T msg) {
        //IMPLEMENT IF NEEDED
        if (msg != null) {
            try{
                out.write(encdec.encode(msg));
                out.flush();
            }
            catch(IOException ex){
                ex.printStackTrace();
            }
        }
    }

    public boolean getConnect(){
        return this.connected;
    }

    public void setConnect(boolean isConnect){
        this.connected = isConnect;
    }
}
