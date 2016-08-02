using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNet.Builder;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.PlatformAbstractions;
using Microsoft.AspNet.Server.Features;
using System.Threading;
using Microsoft.AspNet.Hosting;
using System.Reflection;
using server.Commands;

[assembly: AssemblyVersion("1.0.0.*")]
namespace server
{
    public class Startup
    {
        private static Action applicationStarted;

        public Startup(IHostingEnvironment env)
        {
            // Set up configuration sources.
            var builder = new ConfigurationBuilder()
                .AddJsonFile("appsettings.json")
                .AddEnvironmentVariables();
            Configuration = builder.Build();
        }

        public IConfigurationRoot Configuration { get; set; }
        public static IServiceProvider Provider { get; private set; }

        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            // Add framework services.
            //services.AddSingleton<ILoggerFactory, DevNull>();
            services.AddMvc();
            services.AddSingleton((x)=>ConnectionsManager.Current);
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IHostingEnvironment env, ILoggerFactory f, IApplicationLifetime applicationLifetime, IServiceProvider provider)
        {
            //f.MinimumLevel = LogLevel.None;
            //f.AddConsole(LogLevel.None);
            //loggerFactory.AddConsole(Configuration.GetSection("Logging"));
            //loggerFactory.AddDebug();
            //loggerFactory.ised = LogLevel.Critical;
            Provider = provider;
            app.UseIISPlatformHandler();

            app.UseMvc();

            applicationLifetime.ApplicationStarted.Register(()=> applicationStarted());
        }

     

        // Entry point for the application.
        public static void Main(string[] args)
        {
           

            applicationStarted = new Action(() =>
            {
                Task.Run(() =>
                {
                    Console.WriteLine("Hello, write help for available commands.");
                    for (;;)
                    {
                        try
                        {
                            Console.Write("#>");
                            var result = ReadLine();
                            if (!result.Any())
                                continue;
                            var command = (result[0] ?? "").ToLower();

                            var commandInstance = CommandsManager.Current.FirstOrDefault(x => x.Names().Contains(command));

                            if (commandInstance == null)
                            {
                                Console.WriteLine($"Unknown command \"{command}\"");
                                continue;
                            }

                            commandInstance.Call(result);
                        }
                        catch (Exception e)
                        {
                            Console.WriteLine($"e: {e.Message}");
                            break;
                        }
                    }


                });            
            });

            Microsoft.AspNet.Hosting.WebApplication.Run<Startup>(args);
        }

        static string[] ReadLine()
        {
            var input = Console.ReadLine() + '\0';
            var result = new List<string>();
            //var state = new { 
            var isWordStarted = false;
            var isWordContainered = false;
            var slashBefore = false;
            var word = "";
            //};

            foreach (var @char in input)
                switch (@char)
                {
                    case '\"':
                        if (!isWordStarted) { isWordStarted = true; isWordContainered = true; break; }
                        if (isWordStarted && !slashBefore) { isWordStarted = false; isWordContainered = false; result.Add(word); word = ""; slashBefore = false; break; }
                        if (isWordStarted && slashBefore) { word += "\""; slashBefore = false; break; }
                        break;
                    case '\\':
                        if (slashBefore) { word += "\\"; slashBefore = false; break; }
                        slashBefore = true;
                        break;
                    case ' ':
                        slashBefore = false;
                        if (isWordStarted && isWordContainered) { word += " "; break; }
                        if (isWordStarted && !isWordContainered) { isWordStarted = false; result.Add(word); word = ""; slashBefore = false; }
                        break;
                    case '\0':
                        if (isWordStarted) { result.Add(word); }
                        break;
                    default:
                        word += @char;
                        isWordStarted = true;
                        slashBefore = false;
                        break;
                }

            return result.ToArray();
        }
    }

    //internal class DevNull : ILoggerFactory
    //{
    //    public LogLevel MinimumLevel
    //    {
    //        get; set;
    //    }

    //    public void AddProvider(ILoggerProvider provider)
    //    {

    //    }

    //    public ILogger CreateLogger(string categoryName)
    //    {
    //        return new DevNullLogger();
    //    }

    //    public void Dispose()
    //    {

    //    }
    //}

    //internal class DevNullLogger : ILogger
    //{
    //    public IDisposable BeginScopeImpl(object state)
    //    {
    //        return null;
    //    }

    //    public bool IsEnabled(LogLevel logLevel)
    //    {
    //        return true;
    //    }

    //    public void Log(LogLevel logLevel, int eventId, object state, Exception exception, Func<object, Exception, string> formatter)
    //    {

    //    }
    //}
}
