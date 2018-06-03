using System;
using System.Collections.Generic;
using System.Linq;
using System.Web.Http;

namespace MessangerApp
{
    public static class WebApiConfig
    {
        public static void Register(HttpConfiguration config)
        {
            // Web API configuration and services

            // Web API routes
            config.MapHttpAttributeRoutes();

            config.Routes.MapHttpRoute(
                name: "GetMessages",
                routeTemplate: "api/{controller}/{action}/{phoneNum}",
                defaults: new { controller = "Messages" }
            );

            config.Routes.MapHttpRoute(
                name: "DefaultApi",
                routeTemplate: "api/{controller}/{id}",
                defaults: new { id = RouteParameter.Optional }
            );

            
        }
    }
}
