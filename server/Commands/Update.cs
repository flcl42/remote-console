using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace server.Commands
{
    public class Update : ICommand
    {
        public void Call(string[] @params)
        {
            if (@params.Length != 3)
            {
                Console.WriteLine("Incorrect format");
                return;
            }

            ConnectionsManager.Current.RunCommandFor(@params[1].ToUpper(), new { Command = RCommands.Update, Address = @params[2] }, false);
        }

        public string[] Names()
        {
            return new string[] { "update", "m" };
        }

        public string Help()
        {
            return $"{"update {{user}} {{ftp_address}}, up".PadRight(16)} - update user's client from ftp";
        }
    }

    public class UpdateAll : ICommand
    {
        public void Call(string[] @params)
        {
            if (@params.Length != 2)
            {
                Console.WriteLine("Incorrect format");
                return;
            }

            foreach (var waiter in ConnectionsManager.Current.GetWaiters())
            {
                ConnectionsManager.Current.RunCommandFor(waiter.Id, new { Command = RCommands.Update, Address = @params[1] }, false);
            }
        }

        public string[] Names()
        {
            return new string[] { "updatea", "upa" };
        }

        public string Help()
        {
            return $"{"updatea {{ftp_address}}, upa".PadRight(16)} - update all user's clients from ftp";
        }
    }
}
