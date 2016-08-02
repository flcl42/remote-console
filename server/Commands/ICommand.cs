namespace server.Commands
{
    public interface ICommand
    {
        void Call(string[] @params);
        string[] Names();
        string Help();
    }

 

}