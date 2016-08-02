using Microsoft.AspNet.Hosting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace server.Commands
{
    public class Exit : ICommand
    {
        private IApplicationLifetime applicationLifetime;

        public Exit(IApplicationLifetime applicationLifetime)
        {
            this.applicationLifetime = applicationLifetime;
        }
        public void Call(string[] @params)
        {
            applicationLifetime.StopApplication();
            throw new Exception("Goodbye!");
        }

       

        public string[] Names()
        {
            return new string [] { "exit" };
        }

        public string Help()
        {
            return $"{string.Join(", ", Names()).PadRight(16)} - exit application";
        }
    }
}
