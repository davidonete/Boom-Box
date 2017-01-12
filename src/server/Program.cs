using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Runtime.InteropServices;
using System.Data.SQLite;
using System.Collections.Generic;

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
    public uint ID;
    public float x, y;
};

[StructLayout(LayoutKind.Sequential)]
public struct ServerConfirmationPacket
{
    public uint ID;
    public uint msg;
};

[StructLayout(LayoutKind.Sequential)]
public struct ServerMessagePacket
{
    public uint msg;
}

[StructLayout(LayoutKind.Sequential)]
public struct ClientLogInPacket
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string username;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string password;
}

[StructLayout(LayoutKind.Sequential)]
public struct ClientChatPacket
{
    public uint ID;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 52)]
    public string message;
}

[StructLayout(LayoutKind.Sequential)]
public struct ClientRequestPacket
{
    public uint ID;
    public uint request;
}

[StructLayout(LayoutKind.Sequential)]
public struct PlayerInfoPacket
{
    public uint ID;
    public uint winCount;
    public uint authority;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string username;
}

public struct PlayerInfo
{
    public uint ID;
    public uint winCount;
    public bool authority;
    public string username;
    public Socket handler;
}

public enum ServerState
{
    WaitRoom,
    Battle
}

public enum ServerMessage
{
    Server_Denied,
    Server_Denied_NotAuthority,
    Server_Denied_AlreadyLogged,
    Server_Denied_NotEnoughPlayers,
    Server_Accepted,
    Server_AcceptedAuthority,
    Server_PlayerConnected,
    Server_PlayerDisconnected,
    Server_StartBattle,
}

public enum ClientRequest
{
    Request_GetPlayersInfo,
    Request_DisconnectPlayer,
    Request_StartBattle,
}

public enum ConnectionType
{
    TCP,
    UDP,
}

public class Server
{
    public static ServerState serverState = ServerState.WaitRoom;
    public static List<PlayerInfo> players;

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

    static byte[] GetBytesFromPacket(ClientChatPacket packet)
    {
        int size = System.Runtime.InteropServices.Marshal.SizeOf(packet);
        byte[] bytes = new byte[size];

        IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.StructureToPtr(packet, ptr, true);
        Marshal.Copy(ptr, bytes, 0, size);
        Marshal.FreeHGlobal(ptr);
        return bytes;
    }

    static byte[] GetBytesFromPacket(PlayerInfoPacket packet)
    {
        int size = System.Runtime.InteropServices.Marshal.SizeOf(packet);
        byte[] bytes = new byte[size];

        IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.StructureToPtr(packet, ptr, true);
        Marshal.Copy(ptr, bytes, 0, size);
        Marshal.FreeHGlobal(ptr);
        return bytes;
    }

    static byte[] GetBytesFromPacket(ServerMessagePacket packet)
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

    static ClientChatPacket GetChatPacketFromBytes(byte[] buffer)
    {
        ClientChatPacket packet = new ClientChatPacket();

        int size = Marshal.SizeOf(packet);
        IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.Copy(buffer, 0, ptr, size);
        packet = (ClientChatPacket)Marshal.PtrToStructure(ptr, packet.GetType());
        Marshal.FreeHGlobal(ptr);

        return packet;
    }

    static ClientRequestPacket GetRequestPacketFromBytes(byte[] buffer)
    {
        ClientRequestPacket packet = new ClientRequestPacket();

        int size = Marshal.SizeOf(packet);
        IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.Copy(buffer, 0, ptr, size);
        packet = (ClientRequestPacket)Marshal.PtrToStructure(ptr, packet.GetType());
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
        int bytesRead = 0;

        // Read data from the client socket. 
        try { bytesRead = handler.EndReceive(ar); }
        //This will be called when a client closes the application
        catch (Exception e) { handler.Close(); }

        if (bytesRead > 0)
        {
            //Console.WriteLine("Received {0} bytes from {1}:{2}", bytesRead, ((IPEndPoint)handler.RemoteEndPoint).Address, ((IPEndPoint)handler.RemoteEndPoint).Port);
            if (serverState == ServerState.WaitRoom)
            {
                //If the packet is a LogInPacket
                int LoginPacketSize = System.Runtime.InteropServices.Marshal.SizeOf(typeof(ClientLogInPacket));
                int ChatPacketSize = System.Runtime.InteropServices.Marshal.SizeOf(typeof(ClientChatPacket));
                if (bytesRead == LoginPacketSize)
                    LogInHandler(state, handler);
                else if (bytesRead == ChatPacketSize)
                    ChatHandler(state, handler);
                else
                    RequestHandler(state, handler);
            }
            else if (serverState == ServerState.Battle)
            {
                //Refuse all incoming TCP messages and requests from clients
                ServerMessagePacket packet;
                packet.msg = (uint)ServerMessage.Server_Denied;
                Send(handler, GetBytesFromPacket(packet));
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
    }

    public static void RequestHandler(StateObject state, Socket handler)
    {
        ClientRequestPacket dataReceived = GetRequestPacketFromBytes(state.buffer);

        if (!CheckPlayerLogged(dataReceived.ID))
            return;

        ClientRequest request = (ClientRequest)dataReceived.request;
        if (request == ClientRequest.Request_GetPlayersInfo)
            GetPlayersInfoHandler(handler, dataReceived.ID);
        else if (request == ClientRequest.Request_DisconnectPlayer)
            LogOutHandler(handler, dataReceived.ID);
        else if (request == ClientRequest.Request_StartBattle)
            StartBattleHandler(handler, dataReceived.ID);
    }

    public static void ChatHandler(StateObject state, Socket handler)
    {
        Console.WriteLine("Received 'Chat' Message from {0}:{1}", ((IPEndPoint)handler.RemoteEndPoint).Address, ((IPEndPoint)handler.RemoteEndPoint).Port);
        ClientChatPacket dataReceived = GetChatPacketFromBytes(state.buffer);

        //Broadcast the message to the rest of the players
        Broadcast(ConnectionType.TCP, GetBytesFromPacket(dataReceived));
    }

    public static void LogInHandler(StateObject state, Socket handler)
    {
        Console.WriteLine("Received 'Log In' Request from {0}:{1}", ((IPEndPoint)handler.RemoteEndPoint).Address, ((IPEndPoint)handler.RemoteEndPoint).Port);

        ClientLogInPacket dataReceived = GetLogInPacketFromBytes(state.buffer);
        //Console.WriteLine("Received {0} bytes from {1}:{2}\nUser:{3}, Pass:{4}", bytesRead, ((IPEndPoint)handler.RemoteEndPoint).Address, ((IPEndPoint)handler.RemoteEndPoint).Port, dataReceived.username, dataReceived.password);

        //Confirm log in credentials through database
        SQLiteConnection db_connection = new SQLiteConnection("Data Source=" + System.Environment.CurrentDirectory + "\\database.db; FailIfMissing=True");
        db_connection.Open();

        string query = "select * from Users";
        SQLiteCommand command = new SQLiteCommand(query, db_connection);

        SQLiteDataReader reader = command.ExecuteReader();

        ServerConfirmationPacket packet;
        packet.ID = 0;
        packet.msg = 0;

        if (reader.HasRows)
        {
            while (reader.Read())
            {
                //Check if the username & password are correct
                if ((string)reader["USERNAME"] == dataReceived.username && (string)reader["PASSWORD"] == dataReceived.password)
                {
                    uint id = Convert.ToUInt32(reader["ID"]);
                    //Check if that user has already logged in
                    if (players.Count > 0)
                    {
                        for (int i = 0; i < players.Count; i++)
                        {
                            if (players[i].ID == id)
                            {
                                packet.msg = (uint)ServerMessage.Server_Denied_AlreadyLogged;
                                break;
                            }
                        }
                    }

                    if (packet.msg != (uint)ServerMessage.Server_Denied_AlreadyLogged)
                    {
                        PlayerInfo newPlayer;
                        newPlayer.ID = id;
                        newPlayer.handler = handler;
                        newPlayer.authority = false;
                        newPlayer.username = dataReceived.username;
                        newPlayer.winCount = Convert.ToUInt32(reader["WINCOUNT"]);

                        packet.ID = id;
                        packet.msg = (uint)ServerMessage.Server_Accepted;

                        if (players.Count < 1)
                        {
                            packet.msg = (uint)ServerMessage.Server_AcceptedAuthority;
                            newPlayer.authority = true;
                        }

                        //Broadcast the message to the rest of the players
                        ServerMessagePacket broadcastPacket;
                        broadcastPacket.msg = (uint)ServerMessage.Server_PlayerConnected;
                        Broadcast(ConnectionType.TCP, GetBytesFromPacket(broadcastPacket));

                        players.Add(newPlayer);
                    }
                    break;
                }
            }
        }
        db_connection.Close();
        Send(handler, GetBytesFromPacket(packet));
    }

    public static void LogOutHandler(Socket handler, uint ID)
    {
        Console.WriteLine("Received 'Log Out' Message. Players: {0}", players.Count - 1);

        for(int i = 0; i < players.Count; i++)
        {
            if(players[i].ID == ID)
            {
                if (players[i].authority && players.Count > 1)
                {
                    //Change the authority to an other player
                    PlayerInfo modifiedPlayer;
                    modifiedPlayer.ID = players[i + 1].ID;
                    modifiedPlayer.username = players[i + 1].username;
                    modifiedPlayer.handler = players[i + 1].handler;
                    modifiedPlayer.winCount = players[i + 1].winCount;
                    modifiedPlayer.authority = true;

                    players[i + 1] = modifiedPlayer; 
                }

                try
                {
                    handler.Shutdown(SocketShutdown.Both);
                    handler.Close();
                }
                catch (Exception e) {}
                players.Remove(players[i]);

                //Broadcast the message to the rest of the players
                ServerMessagePacket packet;
                packet.msg = (uint)ServerMessage.Server_PlayerDisconnected;
                Broadcast(ConnectionType.TCP, GetBytesFromPacket(packet));

                break;
            }
        }
    }

    public static void GetPlayersInfoHandler(Socket handler, uint ID)
    {
        Console.WriteLine("Received 'Get Player Info' Request from {0}:{1}", ((IPEndPoint)handler.RemoteEndPoint).Address, ((IPEndPoint)handler.RemoteEndPoint).Port);
        PlayerInfoPacket lastPacket;
        lastPacket.ID = 0;
        lastPacket.winCount = 0;
        lastPacket.authority = 0;
        lastPacket.username = "";

        for (int i = 0; i < players.Count; i++)
        {
            if (players[i].ID == ID)
            {
                lastPacket.ID = players[i].ID;
                lastPacket.winCount = players[i].winCount;
                lastPacket.authority = Convert.ToUInt32(players[i].authority);
                lastPacket.username = players[i].username;
            }
            else
            {
                PlayerInfoPacket packet;
                packet.ID = players[i].ID;
                packet.winCount = players[i].winCount;
                packet.authority = Convert.ToUInt32(players[i].authority);
                packet.username = players[i].username;
                Send(handler, GetBytesFromPacket(packet));
            }
        }

        Send(handler, GetBytesFromPacket(lastPacket));
    }
    
    public static void StartBattleHandler(Socket handler, uint ID)
    {
        Console.WriteLine("Received 'Start Battle' Request from {0}:{1}", ((IPEndPoint)handler.RemoteEndPoint).Address, ((IPEndPoint)handler.RemoteEndPoint).Port);
        ServerMessagePacket packet;
        if (CheckPlayerAuthority(ID))
        {
            if (players.Count > 1)
            {
                packet.msg = (uint)ServerMessage.Server_StartBattle;
                Broadcast(ConnectionType.TCP, GetBytesFromPacket(packet));
                serverState = ServerState.Battle;
            }
            else
            {
                packet.msg = (uint)ServerMessage.Server_Denied_NotEnoughPlayers;
                Send(handler, GetBytesFromPacket(packet));
            }
        }
        else
        {
            packet.msg = (uint)ServerMessage.Server_Denied_NotAuthority;
            Send(handler, GetBytesFromPacket(packet));
        }
    }

    public static bool CheckPlayerLogged(uint ID)
    {
        for(int i = 0; i < players.Count; i++)
        {
            if (players[i].ID == ID)
                return true;
        }
        return false;
    }

    public static bool CheckPlayerAuthority(uint ID)
    {
        for (int i = 0; i < players.Count; i++)
        {
            if (players[i].ID == ID && players[i].authority)
                return true;
        }
        return false;
    }

    public static void Broadcast(ConnectionType type, byte[] data)
    {
        if(type == ConnectionType.TCP)
        {
            for (int i = 0; i < players.Count; i++)
                Send(players[i].handler, data);
        }
        else
        {

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

    private static void Init()
    {
        players = new List<PlayerInfo>();
    }

    public static int Main(String[] args)
    {
        Init();

        Thread UDPThread = new Thread(StartListeningUDP);
        UDPThread.Start();
        StartListeningTCP();

        return 0;
    }
}
