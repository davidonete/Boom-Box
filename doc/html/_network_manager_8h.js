var _network_manager_8h =
[
    [ "LogInPacket", "struct_log_in_packet.html", "struct_log_in_packet" ],
    [ "GamePacket", "struct_game_packet.html", "struct_game_packet" ],
    [ "PlayerInfoPacket", "struct_player_info_packet.html", "struct_player_info_packet" ],
    [ "ChatPacket", "struct_chat_packet.html", "struct_chat_packet" ],
    [ "ServerMessagePacket", "struct_server_message_packet.html", "struct_server_message_packet" ],
    [ "ServerChangeBombPacket", "struct_server_change_bomb_packet.html", "struct_server_change_bomb_packet" ],
    [ "ClientRequestPacket", "struct_client_request_packet.html", "struct_client_request_packet" ],
    [ "PlayerBombChangePacket", "struct_player_bomb_change_packet.html", "struct_player_bomb_change_packet" ],
    [ "PlayerInformation", "struct_player_information.html", "struct_player_information" ],
    [ "NetworkManager", "class_network_manager.html", "class_network_manager" ],
    [ "ConnectionType", "_network_manager_8h.html#aa1f0e2efd52935fd01bfece0fbead81f", [
      [ "TCP", "_network_manager_8h.html#aa1f0e2efd52935fd01bfece0fbead81faa040cd7feeb588104634cdadf35abf1c", null ],
      [ "UDP", "_network_manager_8h.html#aa1f0e2efd52935fd01bfece0fbead81fadb542475cf9d0636e4225e216cee9ae6", null ]
    ] ],
    [ "PacketType", "_network_manager_8h.html#a0a80a7bc045affcf10846075b88cbca0", [
      [ "Type_Null", "_network_manager_8h.html#a0a80a7bc045affcf10846075b88cbca0ad9a80cd24e03e5818272507a85380fa8", null ],
      [ "Type_GamePacket", "_network_manager_8h.html#a0a80a7bc045affcf10846075b88cbca0a0b238f6d27c6e50e83c07b2ce89f738f", null ],
      [ "Type_ChatPacket", "_network_manager_8h.html#a0a80a7bc045affcf10846075b88cbca0a8888b8989e057340f5b567819074d82c", null ],
      [ "Type_ServerMessagePacket", "_network_manager_8h.html#a0a80a7bc045affcf10846075b88cbca0aaa4778f6bc2d50008ad7e48a15c7ad75", null ],
      [ "Type_PlayerInfoPacket", "_network_manager_8h.html#a0a80a7bc045affcf10846075b88cbca0a3b67d18b9f17a74e0eee288297528f4c", null ],
      [ "Type_ClientRequestPacket", "_network_manager_8h.html#a0a80a7bc045affcf10846075b88cbca0a5a6b562fb59ff5d65dfc461fce8ca6f1", null ],
      [ "Type_ServerChangeBombPacket", "_network_manager_8h.html#a0a80a7bc045affcf10846075b88cbca0aeec519508e56be0c0ae9df4e66f8c8de", null ]
    ] ],
    [ "RequestMessage", "_network_manager_8h.html#ad5f0a01fd855bd0e709e146e2db995ff", [
      [ "Request_GetPlayersInfo", "_network_manager_8h.html#ad5f0a01fd855bd0e709e146e2db995ffacb0f6517c265f8f4094eef74b540a35a", null ],
      [ "Request_DisconnectPlayer", "_network_manager_8h.html#ad5f0a01fd855bd0e709e146e2db995ffaf08b03e7201d690cf86747a15aa12cbb", null ],
      [ "Request_StartBattleScene", "_network_manager_8h.html#ad5f0a01fd855bd0e709e146e2db995ffa319d6c61220c1efa3959c760304113f1", null ],
      [ "Request_PlayerReady", "_network_manager_8h.html#ad5f0a01fd855bd0e709e146e2db995ffad79118eddefbc310daad984f12353fe3", null ],
      [ "Request_TimeOver", "_network_manager_8h.html#ad5f0a01fd855bd0e709e146e2db995ffa81abd4a56cdac5addcd54ef11c51db16", null ],
      [ "Request_PlayerChangeBomb", "_network_manager_8h.html#ad5f0a01fd855bd0e709e146e2db995ffaa10b8c31843092501f1cb2e466167387", null ]
    ] ],
    [ "ServerMessage", "_network_manager_8h.html#a511330522f00fe1c91e0afff9993dbe7", [
      [ "Server_Denied", "_network_manager_8h.html#a511330522f00fe1c91e0afff9993dbe7a6351b18dd707a8b21cb3e1f6ea81be68", null ],
      [ "Server_Denied_NotAuthority", "_network_manager_8h.html#a511330522f00fe1c91e0afff9993dbe7a3d9224de84650e3804987bfbc575ba0d", null ],
      [ "Server_Denied_AlreadyLogged", "_network_manager_8h.html#a511330522f00fe1c91e0afff9993dbe7a10bff70a7a20f77dcc4d4f3655da6c3e", null ],
      [ "Server_Denied_NotEnoughPlayers", "_network_manager_8h.html#a511330522f00fe1c91e0afff9993dbe7a27526aac4023dbdfc397877e1c6c9494", null ],
      [ "Server_Accepted", "_network_manager_8h.html#a511330522f00fe1c91e0afff9993dbe7a2787dc00b75e89ce1b52db1c6c0bd751", null ],
      [ "Server_AcceptedAuthority", "_network_manager_8h.html#a511330522f00fe1c91e0afff9993dbe7a214ad910217b217702b1b51f4ab9be36", null ],
      [ "Server_PlayerConnected", "_network_manager_8h.html#a511330522f00fe1c91e0afff9993dbe7aea700d62754c0aebd2442f0ab459c71c", null ],
      [ "Server_PlayerDisconnected", "_network_manager_8h.html#a511330522f00fe1c91e0afff9993dbe7a8a5c0e3a846f9b3c3ee25983e9ce7fca", null ],
      [ "Server_StartBattleScene", "_network_manager_8h.html#a511330522f00fe1c91e0afff9993dbe7a49f38e525b3a133120c81b70bd301e81", null ],
      [ "Server_StartBattle", "_network_manager_8h.html#a511330522f00fe1c91e0afff9993dbe7a025df98d078bd411f0ecbc04970a741c", null ],
      [ "Server_BombPossession", "_network_manager_8h.html#a511330522f00fe1c91e0afff9993dbe7aba24555d893b5a2556a1fe73696f5e87", null ],
      [ "Server_PlayerDead", "_network_manager_8h.html#a511330522f00fe1c91e0afff9993dbe7a265ed9ab8edba8d23b18c355c39b01cf", null ]
    ] ]
];