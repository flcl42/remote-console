using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;

namespace server.Commands
{
    public class Help : ICommand
    {
        public void Call(string[] @params)
        {
            Console.WriteLine(
$@"Remote Control Server Application
Current version: {Assembly.GetExecutingAssembly().GetName().Version}
");
            foreach (var command in CommandsManager.Current.OrderBy(x=>x.Names().Any()).ThenBy(x=>x.Names().Any() ? x.Names().FirstOrDefault() : ""))
            {
                Console.WriteLine($"   {command.Help()}");
            }
        }

        public string[] Names()
        {
            return new string [] { "help", "h", "?" };
        }

        string ICommand.Help()
        {
            return $"{string.Join(", ", Names()).PadRight(16)} - help for this application";
        }
    }
}
