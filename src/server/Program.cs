//#define MonoCS

using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Runtime.InteropServices;
#if MonoCS
    using SQLiteCommand = Mono.Data.Sqlite.SqliteCommand;
    using SQLiteConnection = Mono.Data.Sqlite.SqliteConnection;
    using SQLiteDataReader = Mono.Data.Sqlite.SqliteDataReader;
    using SQLiteException = Mono.Data.Sqlite.SqliteException;
#else
    using SQLiteCommand = System.Data.SQLite.SQLiteCommand;
    using SQLiteConnection = System.Data.SQLite.SQLiteConnection;
    using SQLiteDataReader = System.Data.SQLite.SQLiteDataReader;
    using SQLiteException = System.Data.SQLite.SQLiteException;
#endif
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
    public float x, y, rotation;
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

public struct ServerChangeBombPacket
{
    public uint fromID;
    public uint toID;
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
public struct ClientBombChangePacket
{
    public uint ID;
    public uint otherID;
    public uint msg;
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
    public bool ready;
    public uint bomb;
    public Socket handler;
    public IPEndPoint ep;
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
    Server_StartBattleScene,
    Server_StartBattle,
    Server_BombPossession,
}

public enum ClientRequest
{
    Request_GetPlayersInfo,
    Request_DisconnectPlayer,
    Request_StartBattleScene,
    Request_PlayerReady
}

public enum ConnectionType
{
    TCP,
    UDP,
}

public enum Platform
{
    Windows,
    Linux,
    Mac
}

public class Server
{
    public static ServerState serverState = ServerState.WaitRoom;
    public static List<PlayerInfo> players;

    // Thread signal.
    public static ManualResetEvent allDone = new ManualResetEvent(false);
    public static bool messageReceived = false;
    public static bool messageSent = false;

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

    static byte[] GetBytesFromPacket(ServerChangeBombPacket packet)
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

    static ClientBombChangePacket GetBombRequestPacketFromBytes(byte[] buffer)
    {
        ClientBombChangePacket packet = new ClientBombChangePacket();

        int size = Marshal.SizeOf(packet);
        IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.Copy(buffer, 0, ptr, size);
        packet = (ClientBombChangePacket)Marshal.PtrToStructure(ptr, packet.GetType());
        Marshal.FreeHGlobal(ptr);

        return packet;
    }

    public static void StartListeningTCP()
    {
        // Data buffer for incoming data.
        byte[] bytes = new Byte[1024];

        // Establish the local endpoint for the socket.
        int port = 8080;

        IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Any, port);

        var host = Dns.GetHostEntry(Dns.GetHostName());
        foreach (var ip in host.AddressList)
        {
            if (ip.AddressFamily == AddressFamily.InterNetwork)
            {
                Console.WriteLine("Server IP: {0}:{1}", ip.ToString(), port);
                //break;
            }
        }
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
                Thread.Sleep(100);
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
                    RequestHandler(bytesRead, state, handler);
            }
            else if (serverState == ServerState.Battle)
                RequestHandler(bytesRead, state, handler);
        }
    }

    public static void RequestHandler(int bytesRead, StateObject state, Socket handler)
    {
        int RequestSize = System.Runtime.InteropServices.Marshal.SizeOf(typeof(ClientRequestPacket));
        int BombRequestSize = System.Runtime.InteropServices.Marshal.SizeOf(typeof(ClientBombChangePacket));
        if (bytesRead == RequestSize)
        {
            ClientRequestPacket dataReceived = GetRequestPacketFromBytes(state.buffer);

            if (!CheckPlayerLogged(dataReceived.ID))
                return;

            ClientRequest request = (ClientRequest)dataReceived.request;
            if (request == ClientRequest.Request_GetPlayersInfo)
                GetPlayersInfoHandler(handler, dataReceived.ID);
            else if (request == ClientRequest.Request_DisconnectPlayer)
                LogOutHandler(handler, dataReceived.ID);
            else if (request == ClientRequest.Request_StartBattleScene)
                StartBattleSceneHandler(handler, dataReceived.ID);
            else if (request == ClientRequest.Request_PlayerReady)
                StartBattleHandler(handler, dataReceived.ID);
        }
        else if (bytesRead == BombRequestSize)
        {
            ClientBombChangePacket dataReceived = GetBombRequestPacketFromBytes(state.buffer);
            ChangePlayerBombHandler(handler, dataReceived);
        }
        else
        {
            ServerMessagePacket packet;
            packet.msg = (uint)ServerMessage.Server_Denied;
            SendTCP(handler, GetBytesFromPacket(packet));
        }
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

        //Confirm log in credentials through database
        string databasePath = "";
        Platform platform = RunningPlatform();
        if (platform == Platform.Windows)
            databasePath = "Data Source=" + System.Environment.CurrentDirectory + "\\database.db; FailIfMissing=True";
        else if (platform == Platform.Mac)
            databasePath = "Data Source=" + System.Environment.CurrentDirectory + "/data/database.db; FailIfMissing=True";

        SQLiteConnection db_connection = new SQLiteConnection(databasePath);
        db_connection.Open();

        string query = "select * from Users where USERNAME = @username and PASSWORD = @password";
        SQLiteCommand command = new SQLiteCommand(query, db_connection);
        command.Parameters.AddWithValue("@username", dataReceived.username);
        command.Parameters.AddWithValue("@password", dataReceived.password);

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
                    if(CheckPlayerLogged(id))
                    { 
                        packet.msg = (uint)ServerMessage.Server_Denied_AlreadyLogged;
                        break;
                    }

                    if (packet.msg != (uint)ServerMessage.Server_Denied_AlreadyLogged)
                    {
                        PlayerInfo newPlayer;
                        newPlayer.ID = id;
                        newPlayer.handler = handler;
                        newPlayer.authority = false;
                        newPlayer.username = dataReceived.username;
                        newPlayer.ready = false;
                        newPlayer.bomb = 0;
                        newPlayer.winCount = Convert.ToUInt32(reader["WINCOUNT"]);
                        newPlayer.ep = null;

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
        SendTCP(handler, GetBytesFromPacket(packet));
    }

    public static void LogOutHandler(Socket handler, uint ID)
    {
        Console.WriteLine("Received 'Log Out' Message. Players: {0}", players.Count - 1);

        for (int i = 0; i < players.Count; i++)
        {
            if (players[i].ID == ID)
            {
                if (players[i].authority && players.Count > 1)
                {
                    //Change the authority to another player
                    players[i + 1] = SetPlayerAuthority(players[i + 1], true);
                }

                try
                {
                    handler.Shutdown(SocketShutdown.Both);
                    handler.Close();
                }
                catch (Exception e) { }
                players.Remove(players[i]);

                //Broadcast the message to the rest of the players
                ServerMessagePacket packet;
                packet.msg = (uint)ServerMessage.Server_PlayerDisconnected;
                Broadcast(ConnectionType.TCP, GetBytesFromPacket(packet));
            }
            else
                players[i] = SetPlayerReady(players[i], false);
        }

        if (serverState == ServerState.Battle)
            serverState = ServerState.WaitRoom;
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
                if (serverState == ServerState.Battle)
                    lastPacket.winCount = players[i].bomb;
                else
                    lastPacket.winCount = players[i].winCount;
                lastPacket.authority = Convert.ToUInt32(players[i].authority);
                lastPacket.username = players[i].username;
            }
            else
            {
                PlayerInfoPacket packet;
                packet.ID = players[i].ID;
                if (serverState == ServerState.Battle)
                    packet.winCount = players[i].bomb;
                else
                    packet.winCount = players[i].winCount;
                packet.authority = Convert.ToUInt32(players[i].authority);
                packet.username = players[i].username;
                SendTCP(handler, GetBytesFromPacket(packet));
            }
        }

        SendTCP(handler, GetBytesFromPacket(lastPacket));
    }
    
    public static void StartBattleSceneHandler(Socket handler, uint ID)
    {
        Console.WriteLine("Received 'Start Battle' Request from {0}:{1}", ((IPEndPoint)handler.RemoteEndPoint).Address, ((IPEndPoint)handler.RemoteEndPoint).Port);
        ServerMessagePacket packet;
        if (CheckPlayerAuthority(ID))
        {
            if (players.Count > 1)
            {
                ResetPlayersStatus();

                int chosen = new Random().Next(0, players.Count);
                players[chosen] = SetPlayerBomb(players[chosen], 1);

                packet.msg = (uint)ServerMessage.Server_StartBattleScene;
                Broadcast(ConnectionType.TCP, GetBytesFromPacket(packet));
                serverState = ServerState.Battle;
            }
            else
            {
                packet.msg = (uint)ServerMessage.Server_Denied_NotEnoughPlayers;
                SendTCP(handler, GetBytesFromPacket(packet));
            }
        }
        else
        {
            packet.msg = (uint)ServerMessage.Server_Denied_NotAuthority;
            SendTCP(handler, GetBytesFromPacket(packet));
        }
    }

    public static void StartBattleHandler(Socket handler, uint ID)
    {
        Console.WriteLine("Received 'Player Ready' Request from {0}:{1}", ((IPEndPoint)handler.RemoteEndPoint).Address, ((IPEndPoint)handler.RemoteEndPoint).Port);

        uint playersReady = 0;
        for (int i = 0; i < players.Count; i++)
        {
            if (players[i].ID == ID)
            {
                players[i] = SetPlayerReady(players[i], true);
                playersReady++;
            }
            else if (players[i].ready)
                playersReady++;
        }

        if(playersReady == players.Count)
        {
            ServerMessagePacket packet;
            packet.msg = (uint)ServerMessage.Server_StartBattle;
            Broadcast(ConnectionType.TCP, GetBytesFromPacket(packet));
        }
    }

    public static void ChangePlayerBombHandler(Socket handler, ClientBombChangePacket data)
    {
        bool confirmed = true;
        if (CheckPlayerLogged(data.ID) && CheckPlayerLogged(data.otherID))
        {
            if (CheckPlayerBomb(data.ID) && !CheckPlayerBomb(data.otherID))
            {
                for (int i = 0; i < players.Count; i++)
                {
                    if (players[i].ID == data.ID)
                        players[i] = SetPlayerBomb(players[i], 0);
                    else if (players[i].ID == data.otherID)
                        players[i] = SetPlayerBomb(players[i], 1);
                }

                ServerChangeBombPacket packet;
                packet.fromID = data.ID;
                packet.toID = data.otherID;
                packet.msg = (uint)ServerMessage.Server_BombPossession;
                Broadcast(ConnectionType.TCP, GetBytesFromPacket(packet));
            }
            else
                confirmed = false;
        }
        else
            confirmed = false;

        if (!confirmed)
        {
            ServerMessagePacket packet;
            packet.msg = (uint)ServerMessage.Server_Denied;
            SendTCP(handler, GetBytesFromPacket(packet));
        }
    }

    public static bool CheckPlayerLogged(uint ID)
    {
        if (players.Count > 0)
        {
            for (int i = 0; i < players.Count; i++)
            {
                if (players[i].ID == ID)
                    return true;
            }
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

    public static bool CheckPlayerBomb(uint ID)
    {
        for (int i = 0; i < players.Count; i++)
        {
            if (players[i].ID == ID && players[i].bomb == 1)
                return true;
        }
        return false;
    }

    public static void ResetPlayersStatus()
    {
        for (int i = 0; i < players.Count; i++)
        {
            players[i] = SetPlayerReady(players[i], false);
            players[i] = SetPlayerBomb(players[i], 0);
        }
    }

    public static PlayerInfo SetPlayerAuthority(PlayerInfo player, bool authority)
    {
        PlayerInfo modifiedPlayer;
        modifiedPlayer.ID = player.ID;
        modifiedPlayer.username = player.username;
        modifiedPlayer.handler = player.handler;
        modifiedPlayer.winCount = player.winCount;
        modifiedPlayer.authority = authority;
        modifiedPlayer.ready = player.ready;
        modifiedPlayer.bomb = player.bomb;
        modifiedPlayer.ep = player.ep;

        return modifiedPlayer;
    }

    public static PlayerInfo SetPlayerReady(PlayerInfo player, bool ready)
    {
        PlayerInfo modifiedPlayer;
        modifiedPlayer.ID = player.ID;
        modifiedPlayer.username = player.username;
        modifiedPlayer.handler = player.handler;
        modifiedPlayer.winCount = player.winCount;
        modifiedPlayer.authority = player.authority;
        modifiedPlayer.ready = ready;
        modifiedPlayer.bomb = player.bomb;
        modifiedPlayer.ep = player.ep;

        return modifiedPlayer;
    }

    public static PlayerInfo SetPlayerBomb(PlayerInfo player, uint bomb)
    {
        PlayerInfo modifiedPlayer;
        modifiedPlayer.ID = player.ID;
        modifiedPlayer.username = player.username;
        modifiedPlayer.handler = player.handler;
        modifiedPlayer.winCount = player.winCount;
        modifiedPlayer.authority = player.authority;
        modifiedPlayer.ready = player.ready;
        modifiedPlayer.bomb = bomb;
        modifiedPlayer.ep = player.ep;

        return modifiedPlayer;
    }

    public static PlayerInfo SetPlayerEP(PlayerInfo player, IPEndPoint ep)
    {
        PlayerInfo modifiedPlayer;
        modifiedPlayer.ID = player.ID;
        modifiedPlayer.username = player.username;
        modifiedPlayer.handler = player.handler;
        modifiedPlayer.winCount = player.winCount;
        modifiedPlayer.authority = player.authority;
        modifiedPlayer.ready = player.ready;
        modifiedPlayer.bomb = player.bomb;
        modifiedPlayer.ep = ep;

        return modifiedPlayer;
    }

    public static void Broadcast(ConnectionType type, byte[] data)  
    {
        if(type == ConnectionType.TCP)
        {
            for (int i = 0; i < players.Count; i++)
                SendTCP(players[i].handler, data);
        }
        else
        {
            for (int i = 0; i < players.Count; i++)
                if(players[i].ep != null)
                    SendUDP(players[i].ep, data);
        }
    }

    private static void SendTCP(Socket handler, String data)
    {
        // Convert the string data to byte data using ASCII encoding.
        byte[] byteData = Encoding.ASCII.GetBytes(data);

        // Begin sending the data to the remote device.
        handler.BeginSend(byteData, 0, byteData.Length, 0,
            new AsyncCallback(SendCallbackTCP), handler);
    }

    private static void SendTCP(Socket handler, byte[] byteData)
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
                //Thread.Sleep(16);
            }
        }
    }

    private static void ReceiveCallbackUDP(IAsyncResult ar)
    {
        UdpClient Client = (UdpClient)((UdpStateObject)(ar.AsyncState)).client;
        IPEndPoint EndPoint = (IPEndPoint)((UdpStateObject)(ar.AsyncState)).endPoint;

        Byte[] bytesReceived = Client.EndReceive(ar, ref EndPoint); 
        int bytesRead = bytesReceived.Length;

        if (bytesRead > 0)
        {
            if (serverState == ServerState.Battle)
            {
                int GamePacketSize = System.Runtime.InteropServices.Marshal.SizeOf(typeof(ClientGamePacket));
                if (bytesRead == GamePacketSize)
                    GameUpdateHandler(EndPoint, bytesReceived);

                //Console.WriteLine("Received {0} bytes from {1}:{2} {3}", bytesReceived.Length, EndPoint.Address, EndPoint.Port, content);
                messageReceived = true;
            }
        }
    }

    public static void GameUpdateHandler(IPEndPoint ep, byte[] bytesReceived)
    {
        Console.WriteLine("Received 'Game Update' Packet from {0}:{1}", bytesReceived.Length, ep.Address, ep.Port);
        ClientGamePacket packet = GetGamePacketFromBytes(bytesReceived);

        for(int i = 0; i < players.Count; i++)
        {
            if (packet.ID == players[i].ID)
            {
                if (players[i].ep == null)
                    players[i] = SetPlayerEP(players[i], ep);
                break;
            }
        }

        if(CheckPlayerLogged(packet.ID))
            Broadcast(ConnectionType.UDP, GetBytesFromPacket(packet));
    }

    private static void SendUDP(IPEndPoint ep, byte[] data)
    {
        messageSent = false;
        UdpClient client = new UdpClient();
        client.Connect(ep);

        client.BeginSend(data, data.Length, new AsyncCallback(SendCallbackUDP), client);

        while(!messageSent)
        {
            //Thread.Sleep(100);
        }    
    }

    private static void SendCallbackUDP(IAsyncResult ar)    
    {
        UdpClient client = (UdpClient)ar.AsyncState;
        Console.WriteLine("Sent {0} bytes to client.", client.EndSend(ar));
        messageSent = true;
    }
    public static Platform RunningPlatform()
    {
        switch (Environment.OSVersion.Platform)
        {
            case PlatformID.MacOSX:
				Console.WriteLine ("Detected Mac OS");
                return Platform.Mac;

			case PlatformID.Unix:
				Console.WriteLine ("Detected Mac OS");
				return Platform.Mac;

            default:
			Console.WriteLine ("Detected Windows OS");
                return Platform.Windows;
        }
    }

    private static void Init()
    {
        players = new List<PlayerInfo>();
    }

    public static int Main(String[] args)
    {
        Init();

        Thread TCPThread = new Thread(StartListeningTCP);
        TCPThread.Start();
        StartListeningUDP();

        return 0;
    }
}
