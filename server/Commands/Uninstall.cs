using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace server.Commands
{
    public class Uninstall : ICommand
    {
        public void Call(string[] @params)
        {
            if (@params.Length != 2)
            {
                Console.WriteLine("Incorrect format");
                return;
            }

            ConnectionsManager.Current.RunCommandFor(@params[1].ToUpper(), new { Command = RCommands.Uninstall }, false);
        }

        public string[] Names()
        {
            return new string[] { "unin", "un" };
        }

        public string Help()
        {
            return $"{"unin {user}, un".PadRight(16)} - unistall and clean user's client";
        }
    }
}
