using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using Microsoft.AspNet.SignalR;
using System.Collections.Concurrent;

namespace MessangerApp.Controllers
{
    public class MessagesHub : Hub
    {
        private static ConcurrentDictionary<string, string> connectedUsers =
            new ConcurrentDictionary<string, string>();

        public void Connect(string phoneNum)
        {
            connectedUsers[phoneNum] = Context.ConnectionId;
        }

        public void SendMessage(string senderPhoneNum, string recipientPhoneNum, string text)
        {
            string recipientId = connectedUsers[recipientPhoneNum];
            if (recipientId == null)
                return;
            Clients.Client(recipientId).gotMessage(senderPhoneNum, text);
        }
    }
}