using Microsoft.Extensions.DependencyInjection;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;

namespace server.Commands
{
    public class CommandsManager : IEnumerable<ICommand>
    {
        private static CommandsManager commandsManager;

        CommandsManager()
        {
            commands = Assembly.GetExecutingAssembly().GetTypes().Where(x => x.GetInterfaces().Contains(typeof(ICommand)))
                               .Select(x => ActivatorUtilities.CreateInstance(Startup.Provider, x) as ICommand).ToList().AsReadOnly();
        }

        IEnumerable<ICommand> commands;

        public static CommandsManager Current { get { return commandsManager ?? (commandsManager = new CommandsManager()); } }

        public IEnumerator<ICommand> GetEnumerator()
        {
            return commands.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return commands.GetEnumerator();
        }
    }
}
