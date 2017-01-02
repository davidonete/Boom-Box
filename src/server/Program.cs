using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Runtime.InteropServices;
using System.Data.SQLite;

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

public class UdpStateObject
{
    public IPEndPoint endPoint = null;

    public UdpClient client = null;
}

[StructLayout(LayoutKind.Sequential)]
public struct ClientGamePacket
{
    public uint id;
    public float x, y;
};

[StructLayout(LayoutKind.Sequential, Pack = 8)]
public struct ServerConfirmationPacket
{
    public uint id;
    [MarshalAs(UnmanagedType.I1)]
    public bool accepted;
    [MarshalAs(UnmanagedType.I1)]
    public bool authority;
};

[StructLayout(LayoutKind.Sequential)]
public struct ClientLogInPacket
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string username;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string password;
}

public enum ServerState
{
    WaitRoom,
    Battle
}

public class Server
{
    public static ServerState serverState = ServerState.WaitRoom;
    public static uint numPlayers = 0;

    // Thread signal.
    public static ManualResetEvent allDone = new ManualResetEvent(false);
    public static bool messageReceived = false;

    public Server() {}

    static byte[] GetBytesFromPacket(ClientGamePacket packet)
    {
        int size = Marshal.SizeOf(packet);
        byte[] bytes = new byte[size];

        IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.StructureToPtr(packet, ptr, true);
        Marshal.Copy(ptr, bytes, 0, size);
        Marshal.FreeHGlobal(ptr);
        return bytes;
    }

    static byte[] GetBytesFromPacket(ServerConfirmationPacket packet)
    {
        int size = System.Runtime.InteropServices.Marshal.SizeOf(packet);
        byte[] bytes = new byte[size];

        IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.StructureToPtr(packet, ptr, true);
        Marshal.Copy(ptr, bytes, 0, size);
        Marshal.FreeHGlobal(ptr);
        return bytes;
    }

    static ClientGamePacket GetGamePacketFromBytes(byte[] buffer)
    {
        ClientGamePacket packet = new ClientGamePacket();

        int size = Marshal.SizeOf(packet);
        IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.Copy(buffer, 0, ptr, size);
        packet = (ClientGamePacket)Marshal.PtrToStructure(ptr, packet.GetType());
        Marshal.FreeHGlobal(ptr);

        return packet;
    }

    static ClientLogInPacket GetLogInPacketFromBytes(byte[] buffer)
    {
        ClientLogInPacket packet = new ClientLogInPacket();

        int size = Marshal.SizeOf(packet);
        IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.Copy(buffer, 0, ptr, size);
        packet = (ClientLogInPacket)Marshal.PtrToStructure(ptr, packet.GetType());
        Marshal.FreeHGlobal(ptr);

        return packet;
    }

    public static void StartListeningTCP()
    {
        // Data buffer for incoming data.
        byte[] bytes = new Byte[1024];

        // Establish the local endpoint for the socket.
        // The DNS name of the computer
        // running the listener is "host.contoso.com".
        IPAddress ipAddress = System.Net.IPAddress.Parse("127.0.0.1");
        int port = 8080;

        //IPEndPoint localEndPoint = new IPEndPoint(ipAddress, 11000);
        IPEndPoint localEndPoint = new IPEndPoint(ipAddress, port);

        Console.WriteLine("Server IP: " + ipAddress + ":" + port);

        // Create a TCP/IP socket.
        Socket listener = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

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
                Console.WriteLine("Waiting for a TCP connection...");
                listener.BeginAccept(new AsyncCallback(AcceptCallback), listener);

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
        handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0, new AsyncCallback(ReceiveCallbackTCP), state);
    }

    public static void ReceiveCallbackTCP(IAsyncResult ar)
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
            if (serverState == ServerState.WaitRoom)
            {
                int LoginPacketSize = System.Runtime.InteropServices.Marshal.SizeOf(typeof(ClientLogInPacket));
                if (bytesRead == LoginPacketSize)
                {
                    ClientLogInPacket dataReceived = GetLogInPacketFromBytes(state.buffer);
                    Console.WriteLine("Received {0} bytes from {1}:{2}\nUser:{3}, Pass:{4}", bytesRead, ((IPEndPoint)handler.RemoteEndPoint).Address, ((IPEndPoint)handler.RemoteEndPoint).Port, dataReceived.username, dataReceived.password);

                    //Confirm log in credentials through database
                    SQLiteConnection db_connection = new SQLiteConnection("Data Source="+ System.Environment.CurrentDirectory + "\\database.db; FailIfMissing=True");
                    db_connection.Open();

                    string query = "select * from Users";
                    SQLiteCommand command = new SQLiteCommand(query, db_connection);

                    SQLiteDataReader reader = command.ExecuteReader();

                    ServerConfirmationPacket packet;
                    packet.id = 0;
                    packet.accepted = false;
                    packet.authority = false;

                    if (reader.HasRows)
                    {
                        while (reader.Read())
                        {
                            if ((string)reader["USERNAME"] == dataReceived.username)
                            {
                                if ((string)reader["PASSWORD"] == dataReceived.password)
                                {
                                    if (numPlayers < 1)
                                        packet.authority = true;

                                    numPlayers++;
                                    packet.id = numPlayers;
                                    packet.accepted = true;
                                }
                                break;
                            }
                        }
                    }
                    db_connection.Close();
                    Send(handler, GetBytesFromPacket(packet));
                }
            }
            /*
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
                Console.WriteLine("Received {0} bytes from {1}:{2}: {3}", content.Length, ((IPEndPoint)handler.RemoteEndPoint).Address, ((IPEndPoint)handler.RemoteEndPoint).Port, content);
                //Console.WriteLine("Read {0} bytes from socket. \nData : {1}", content.Length, content);
                // Echo the data back to the client.
                Send(handler, content);
            }
            else
            {
                // Not all data received. Get more. ??????
                handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                new AsyncCallback(ReceiveCallbackTCP), state);
            }
            */
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
            new AsyncCallback(SendCallbackTCP), handler);
    }

    private static void Send(Socket handler, byte[] byteData)
    {
        // Begin sending the data to the remote device.
        handler.BeginSend(byteData, 0, byteData.Length, 0,
            new AsyncCallback(SendCallbackTCP), handler);
    }

    private static void SendCallbackTCP(IAsyncResult ar)
    {
        try
        {
            // Retrieve the socket from the state object.
            Socket handler = (Socket)ar.AsyncState;

            // Complete sending the data to the remote device.
            int bytesSent = handler.EndSend(ar);
            Console.WriteLine("Sent {0} bytes to client.", bytesSent);

            // Create the state object.
            StateObject state = new StateObject();
            state.workSocket = handler;
            handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                new AsyncCallback(ReceiveCallbackTCP), state);

            //handler.Shutdown(SocketShutdown.Both);
            //handler.Close();

        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    public static void StartListeningUDP()
    {
        // Receive a message and write it to the console. 
        IPEndPoint EndPoint = new IPEndPoint(IPAddress.Any, 8080);
        UdpClient Client = new UdpClient(EndPoint);
        UdpStateObject Net = new UdpStateObject();
        Net.endPoint = EndPoint;
        Net.client = Client;
        // Do some work while we wait for a message. 
        while (true)
        {
            messageReceived = false;
            Console.WriteLine("Waiting for UDP conection...");
            Client.BeginReceive(new AsyncCallback(ReceiveCallbackUDP), Net);
            while (!messageReceived)
            {
                // Do something 
                //Console.WriteLine("asdasdas");
            }
        }
    }

    private static void ReceiveCallbackUDP(IAsyncResult ar)
    {
        UdpClient Client = (UdpClient)((UdpStateObject)(ar.AsyncState)).client;
        IPEndPoint EndPoint = (IPEndPoint)((UdpStateObject)(ar.AsyncState)).endPoint;
        Byte[] bytesReceived = Client.EndReceive(ar, ref EndPoint);

        String content = Encoding.ASCII.GetString(bytesReceived);

        Console.WriteLine("Received {0} bytes from {1}:{2} {3}", bytesReceived.Length, EndPoint.Address, EndPoint.Port, content);

        messageReceived = true;
    }

    private static void SendCallbackUDP(IAsyncResult ar)    
    {
        UdpClient client = (UdpClient)ar.AsyncState;

        Console.WriteLine("bytes sent: {0}", client.EndSend(ar));
    }

    public static int Main(String[] args)
    {
        Thread TCPThread = new Thread(StartListeningUDP);
        TCPThread.Start();

        StartListeningTCP();

        return 0;
    }
}
