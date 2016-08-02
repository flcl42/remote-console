using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace server
{
    public static class RCommands
    {
        public const string Register = nameof(Register);
        public const string Reset = nameof(Reset);
        public const string HelloWorld = nameof(HelloWorld);
        public const string Console = nameof(Console);
        public const string ConsoleCtrlC = nameof(ConsoleCtrlC);
        public const string ConsoleExit = nameof(ConsoleExit);
        public const string ConsoleEnter = nameof(ConsoleEnter);
        public const string Update = nameof(Update);
        public const string Uninstall = nameof(Uninstall);
        public const string Migrate = nameof(Migrate);
        public const string GetFileFromClient = nameof(GetFileFromClient);
        public const string PutFileToClient = nameof(PutFileToClient);
    }
}
