using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

// State object for reading client data asynchronously
public class StateObject
{
    // Client  socket.
    public Socket workSocket = null;
    // Size of receive buffer.
    public const int BufferSize = 1024;
    // Receive buffer.
    public byte[] buffer = new byte[BufferSize];
    // Received data string.
    public StringBuilder sb = new StringBuilder();
}

public class AsynchronousSocketListener
{
    // Thread signal.
    public static ManualResetEvent allDone = new ManualResetEvent(false);

    public static bool tcpServer = false;

    private static Socket udpSock;
    private static byte[] buffer;

    public AsynchronousSocketListener()
    {
    }

    public static void StartListeningTCP()
    {
        // Data buffer for incoming data.
        byte[] bytes = new Byte[1024];

        // Establish the local endpoint for the socket.
        // The DNS name of the computer
        // running the listener is "host.contoso.com".
        //IPHostEntry ipHostInfo = Dns.Resolve(Dns.GetHostName());
        IPAddress ipAddress = System.Net.IPAddress.Parse("127.0.0.1");
        int port = 8080;

        //IPEndPoint localEndPoint = new IPEndPoint(ipAddress, 11000);
        IPEndPoint localEndPoint = new IPEndPoint(ipAddress, port);

        Console.WriteLine("Server IP: " + ipAddress + ":"+port);

        // Create a TCP/IP socket.
        Socket listener = new Socket(AddressFamily.InterNetwork,
            SocketType.Stream, ProtocolType.Tcp);

        // Bind the socket to the local endpoint and listen for incoming connections.
        try
        {
            listener.Bind(localEndPoint);
            listener.Listen(100);

            while (true)
            {
                // Set the event to nonsignaled state.
                allDone.Reset();

                // Start an asynchronous socket to listen for connections.
                Console.WriteLine("Waiting for a connection...");
                listener.BeginAccept(
                    new AsyncCallback(AcceptCallback),
                    listener);

                // Wait until a connection is made before continuing.
                allDone.WaitOne();
            }

        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }

        Console.WriteLine("\nPress ENTER to continue...");
        Console.Read();

    }

    public static void AcceptCallback(IAsyncResult ar)
    {
        Console.WriteLine("Received client connection");
        // Signal the main thread to continue.
        allDone.Set();

        // Get the socket that handles the client request.
        Socket listener = (Socket)ar.AsyncState;
        Socket handler = listener.EndAccept(ar);

        // Create the state object.
        StateObject state = new StateObject();
        state.workSocket = handler;
        handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
            new AsyncCallback(ReadCallback), state);
    }

    public static void ReadCallback(IAsyncResult ar)
    {
        String content = String.Empty;

        // Retrieve the state object and the handler socket
        // from the asynchronous state object.
        StateObject state = (StateObject)ar.AsyncState;
        Socket handler = state.workSocket;

        // Read data from the client socket. 
        int bytesRead = handler.EndReceive(ar);

        if (bytesRead > 0)
        {
            // There  might be more data, so store the data received so far.
            state.sb.Append(Encoding.ASCII.GetString(
                state.buffer, 0, bytesRead));

            // Check for end-of-file tag. If it is not there, read 
            // more data.
            content = state.sb.ToString();
            if (content.IndexOf("\0") > -1)
            {
                // All the data has been read from the 
                // client. Display it on the console.
                Console.WriteLine("Read {0} bytes from socket. \nData : {1}",
                    content.Length, content);
                // Echo the data back to the client.
                Send(handler, content);
            }
            else
            {
                // Not all data received. Get more. ??????
                handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                new AsyncCallback(ReadCallback), state);
            }
        }
        else
        {
            Console.WriteLine("Nothing received");
        }
    }

    private static void Send(Socket handler, String data)
    {
        // Convert the string data to byte data using ASCII encoding.
        byte[] byteData = Encoding.ASCII.GetBytes(data);

        // Begin sending the data to the remote device.
        handler.BeginSend(byteData, 0, byteData.Length, 0,
            new AsyncCallback(SendCallback), handler);
    }

    private static void SendCallback(IAsyncResult ar)
    {
        try
        {
            // Retrieve the socket from the state object.
            Socket handler = (Socket)ar.AsyncState;

            // Complete sending the data to the remote device.
            int bytesSent = handler.EndSend(ar);
            Console.WriteLine("Sent {0} bytes to client.", bytesSent);

            handler.Shutdown(SocketShutdown.Both);
            handler.Close();

        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    public static void StartListeningUDP()
    {
        IPAddress ipAddress = System.Net.IPAddress.Parse("127.0.0.1");
        int port = 8080;

        //Setup the socket and message buffer
        udpSock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        udpSock.Bind(new IPEndPoint(ipAddress, port));
        buffer = new byte[1024];

        //Start listening for a new message.
        EndPoint newClientEP = new IPEndPoint(IPAddress.Any, 0);
        while (true)
        {                
            // Set the event to nonsignaled state.
            allDone.Reset();

            // Start an asynchronous socket to listen for connections.
            Console.WriteLine("Waiting for a connection...");

            udpSock.BeginReceiveFrom(buffer, 0, buffer.Length, SocketFlags.None, ref newClientEP, DoReceiveFrom, udpSock);
            // Wait until a connection is made before continuing.
            allDone.WaitOne();
        }
    }

    private static void DoReceiveFrom(IAsyncResult iar)
    {
        try
        {
            allDone.Set();
            //Get the received message.
            Socket handler = (Socket)iar.AsyncState;

            EndPoint clientEP = new IPEndPoint(IPAddress.Any, 0);
            int msgLen = handler.EndReceiveFrom(iar, ref clientEP);
            byte[] localMsg = new byte[msgLen];
            Array.Copy(buffer, localMsg, msgLen);

            string content = Encoding.ASCII.GetString(localMsg, 0, msgLen);

            if (content.IndexOf("\0") > -1)
            {
                //Handle the received message
                Console.WriteLine("Recieved {0} bytes from {1}:{2} \nData: {3}",
                                  msgLen,
                                  ((IPEndPoint)clientEP).Address,
                                  ((IPEndPoint)clientEP).Port,
                                  content);

                byte[] byteData = Encoding.ASCII.GetBytes(content);

                // Begin sending the data to the remote device. (not working)
                //Send(handler, content);
            }
            else
            {
                // Not all data received. Get more. ???????
                EndPoint newClientEP = new IPEndPoint(IPAddress.Any, 0);
                udpSock.BeginReceiveFrom(buffer, 0, buffer.Length, SocketFlags.None, ref newClientEP, DoReceiveFrom, udpSock);
            }

        }
        catch (ObjectDisposedException)
        {
            //expected termination exception on a closed socket.
        }
    }

    public static int Main(String[] args)
    {
        if (tcpServer)
            StartListeningTCP();
        else
            StartListeningUDP();

        return 0;
    }
}
