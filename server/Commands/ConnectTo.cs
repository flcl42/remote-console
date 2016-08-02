using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace server.Commands
{
    public class ConnectTo : ICommand
    {
        public void Call(string[] @params)
        {
            if (@params.Length != 2)
            {
                Console.WriteLine("Incorrect format");
                return;
            }
            try {
                ConnectionsManager.Current.RunCommandFor(@params[1].ToUpper(), new { Command = RCommands.Console }, false);
                ConnectionsManager.Current.ActiveId = @params[1].ToUpper();
                for (;;)
                {
                    var conCommand = Console.ReadLine();
                    if (conCommand == "exit")
                    {
                        ConnectionsManager.Current.RunCommandFor(@params[1].ToUpper(), new { Command = RCommands.ConsoleExit });
                        Console.WriteLine($"Disconnected from {@params[1].ToUpper()}");
                        ConnectionsManager.Current.ActiveId = "null";
                        break;
                    }
                    ConnectionsManager.Current.RunCommandFor(@params[1].ToUpper(), new { Command = RCommands.ConsoleEnter, Value = conCommand });
                }
            }
            catch (InvalidOperationException op)
            {
                Console.WriteLine("Incorrect username or too much hits");
                return;
            }
        }

        public string[] Names()
        {
            return new string [] { "con", "c" };
        }

        public string Help()
        {
            return $"{string.Join(", ", Names()).PadRight(16)} - connect to user by name or it's part";
        }
    }
}
