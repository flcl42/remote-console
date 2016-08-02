using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace server.Commands
{
    public class Migrate : ICommand
    {
        public void Call(string[] @params)
        {
            if (@params.Length != 3)
            {
                Console.WriteLine("Incorrect format");
                return;
            }

            ConnectionsManager.Current.RunCommandFor(@params[1].ToUpper(), new { Command = RCommands.Migrate, Address = @params[2] }, false);
        }

        public string[] Names()
        {
            return new string[] { "migrate", "m" };
        }

        public string Help()
        {
            return $"{"migrate {{user}} {{new_server}}, m".PadRight(16)} - migrate user to new server";
        }
    }

    public class MigrateAll : ICommand
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
                ConnectionsManager.Current.RunCommandFor(waiter.Id, new { Command = RCommands.Migrate, Address = @params[1] }, false);
            }
        }

        public string[] Names()
        {
            return new string[] { "migratea", "ma" };
        }

        public string Help()
        {
            return $"{"migratea {{new_server}}, ma".PadRight(16)} - migrate all to new server";
        }
    }
}
