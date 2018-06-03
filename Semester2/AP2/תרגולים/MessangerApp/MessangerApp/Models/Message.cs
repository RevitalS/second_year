using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Web;

namespace MessangerApp.Models
{
    public class Message
    {
        public int Id { get; set; }
        [Required]
        public string Sender { get; set; }
        [Required]
        public string Recipient { get; set; }
        [Required]
        public string Text { get; set; }
        public DateTime SentTime { get; set; }
    }
}