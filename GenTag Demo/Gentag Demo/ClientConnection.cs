using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
//using Protocol;

//namespace GenTag_Demo
//{
//    class ClientConnection
//    {
//        private TcpClient client;

//        public ClientConnection(string HostName, int PortNumber)
//        {
//            client = new TcpClient(HostName, PortNumber);
//        }

//        public void SendPacket(Packet pkt)
//        {
//            pkt.Stream = client.GetStream();
//            pkt.SendPacket();
//        }

//        public void GetPacket(Packet pkt)
//        {
//            pkt.Stream = client.GetStream();
//            pkt.GetPacket();
//        }

//        public void Close()
//        {
//            Packet close = new Packet(PacketTypes.CloseConnectionRequest);
//            SendPacket(close);
//            client.Close();
//        }
//    }
//}