using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Entity;
using System.Data.Entity.Infrastructure;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Web.Http;
using System.Web.Http.Description;
using MessangerApp.Models;

namespace MessangerApp.Controllers
{
    public class MessagesController : ApiController
    {
        private MessangerAppContext db = new MessangerAppContext();
                
        // GET: api/Messages/GetMessages/12345 
        [ActionName("GetMessages")]  
        public IEnumerable<Message> GetUserMessages(string phoneNum)
        {
            return db.Messages.Where(m => m.Recipient == phoneNum);                       
        }

        // GET: api/Messages/GetLast/12345
        [ActionName("GetLast")]
        public DateTime GetLastMessageSentTime(string phoneNum)
        {
            return db.Messages.Max(m => m.SentTime);
        }

        // POST: api/Messages
        [ResponseType(typeof(Message))]
        public IHttpActionResult PostMessage(Message message)
        {
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            message.SentTime = DateTime.Now;
            db.Messages.Add(message);
            db.SaveChanges();

            return CreatedAtRoute("DefaultApi", new { id = message.Id }, message);
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                db.Dispose();
            }
            base.Dispose(disposing);
        }
    }
}