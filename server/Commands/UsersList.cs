using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace server.Commands
{
    public class UsersList : ICommand
    {
        public void Call(string[] @params)
        {
            foreach (var waiter in ConnectionsManager.Current.GetWaiters())
            {
                Console.WriteLine($"{waiter.Id}\t{waiter.Ip.PadLeft(16, ' ')}\t{waiter.OS}");
            }
        }

        public string[] Names()
        {
            return new string [] { "list", "l" };
        }

        public string Help()
        {
            return $"{string.Join(", ", Names()).PadRight(16)} - list active users(connections)";
        }
    }
}
