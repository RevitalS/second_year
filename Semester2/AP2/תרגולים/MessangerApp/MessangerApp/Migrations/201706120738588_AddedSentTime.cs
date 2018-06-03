namespace MessangerApp.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddedSentTime : DbMigration
    {
        public override void Up()
        {
            AddColumn("dbo.Messages", "SentTime", c => c.DateTime(nullable: true));
        }
        
        public override void Down()
        {
            DropColumn("dbo.Messages", "SentTime");
        }
    }
}
