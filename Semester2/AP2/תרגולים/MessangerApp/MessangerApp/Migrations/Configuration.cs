namespace MessangerApp.Migrations
{
    using Models;
    using System;
    using System.Data.Entity;
    using System.Data.Entity.Migrations;
    using System.Linq;

    internal sealed class Configuration : DbMigrationsConfiguration<MessangerApp.Models.MessangerAppContext>
    {
        public Configuration()
        {
            AutomaticMigrationsEnabled = false;
        }

        protected override void Seed(MessangerApp.Models.MessangerAppContext context)
        {
            //  This method will be called after migrating to the latest version.

            //  You can use the DbSet<T>.AddOrUpdate() helper extension method 
            //  to avoid creating duplicate seed data. E.g.
            //
            //    context.People.AddOrUpdate(
            //      p => p.FullName,
            //      new Person { FullName = "Andrew Peters" },
            //      new Person { FullName = "Brice Lambson" },
            //      new Person { FullName = "Rowan Miller" }
            //    );
            //
            context.Messages.AddOrUpdate(
                    new Message { Sender = "12345", Recipient = "55555", Text ="hello there", SentTime = DateTime.Now },
                    new Message { Sender = "55555", Recipient = "12345", Text = "what's up?", SentTime = DateTime.Now.AddSeconds(1) }
                );
        }
    }
}
