using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace server.Commands
{
    public class GetFileFromClient : ICommand
    {
        public void Call(string[] @params)
        {
            if (@params.Length != 4)
            {
                Console.WriteLine("Incorrect format");
                return;
            }

            ConnectionsManager.Current.RunCommandFor(@params[1].ToUpper(), new { Command = RCommands.GetFileFromClient, Address = @params[2], ClientPath = @params[3] }, false);
        }

        public string[] Names()
        {
            return new string[] { "get", "g" };
        }

        public string Help()
        {
            return $"get {{user}} {{ftp_address}} {{client_path}}, g - upload file to ftp server from client";
        }
    }

    public class PutFileToClient : ICommand
    {
        public void Call(string[] @params)
        {
            if (@params.Length != 4)
            {
                Console.WriteLine("Incorrect format");
                return;
            }

            ConnectionsManager.Current.RunCommandFor(@params[1].ToUpper(), new { Command = RCommands.PutFileToClient, Address = @params[2], ClientPath = @params[3] }, false);
        }

        public string[] Names()
        {
            return new string[] { "put", "p" };
        }

        public string Help()
        {
            return $"put {{user}} {{ftp_address}} {{client_path}}, p - put file from ftp server to client";
        }
    }
}
