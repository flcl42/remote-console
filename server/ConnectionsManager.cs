using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace server
{
    public class ConnectionsManager
    {
        private IDatabase<DatabaseModel> database;

        HashSet<Waiter> waiters = new HashSet<Waiter>();
        public class Waiter
        {
            public string Ip { get; set; }
            public string OS { get; set; }
            public string Id { get; set; }
            public TaskCompletionSource<object> Source { get; internal set; }
            public Task<object> Task { get; set; }
        }

        HashSet<WaiterTask> waiterTasks = new HashSet<WaiterTask>();
        private static ConnectionsManager self = new ConnectionsManager();

        public static ConnectionsManager Current { get { return self; } }

        public string ActiveId { get; internal set; }

        class WaiterTask
        {
            public DateTime Created { get; set; }
            public string Id { get; set; }
            public object Task { get; set; }
        }

        public Task<object> GetCommandFor(string id, string ip, string os)
        {
            try
            {
                WaiterTask waiterTask = null;
                lock (waiterTasks)
                {
                    waiterTask = waiterTasks.FirstOrDefault(x => x.Id == id);
                    if (waiterTask != null)
                    {
                        waiterTasks.Remove(waiterTask);
                        return Task.FromResult(waiterTask.Task);
                    }
                }

                var result = new TaskCompletionSource<object>();
                lock (waiters)
                {
                    var oldWaiter = waiters.FirstOrDefault(x => x.Id == id);

                    if (oldWaiter != null)
                        waiters.Remove(oldWaiter);

                    waiters.Add(new Waiter { Id = id, Task = result.Task, Source = result, Ip = ip, OS = os });
                    return result.Task;
                }
            }
            catch (Exception)
            {
                throw;
            }
        }

        internal void RemoveWaiter(string id)
        {

            lock (waiters)
            {
                waiters.RemoveWhere(x => x.Id == id);
            }
        }

        public void RunCommandFor(string id, object command, bool useQueue=true)
        {

            lock (waiters)
            {
                try {
                    var activeWaiter = waiters.Single(x => x.Id.StartsWith(id));

                    if (!useQueue && activeWaiter == null)
                    {
                        return;
                    }

                    if (activeWaiter != null)
                    {
                        waiters.Remove(activeWaiter);
                        activeWaiter.Source.SetResult(command);
                    }
                    else
                    {
                        lock (waiterTasks)
                        {
                            waiterTasks.Add(new WaiterTask
                            {
                                Id = id,
                                Task = command,
                                Created = DateTime.Now
                            });
                        }
                    }
                }
                catch
                {
                    Console.WriteLine("User not found");
                }
            }
        }

        private ConnectionsManager()
        {
            database = new JsonFileDatabase<DatabaseModel>("data.json");
        }

        internal IEnumerable<Waiter> GetWaiters()
        {
            return waiters;
        }

        public User CreateUser()
        {
            var id = generateId(database.Data.Counter ++);          
            var result = new User { Id = id };
            database.Data.Users.Add(result);
            database.Save();
            return result;
        }

        private string generateId(int counter)
        {
            const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            var random = new Random();
            return $"{counter.ToString().PadLeft(3, '0')}{(new string(Enumerable.Repeat(chars, 5).Select(s => s[random.Next(s.Length)]).ToArray()))}";
        }
    }
    internal interface IDatabase<T>
    {
        T Data { get; }
        void Save();
    }

    internal class DatabaseModel
    {
        public List<User> Users { get; set; } = new List<User>();
        public int Counter { get; set; } = 1;
    }
    
    public class User
    {
        public string Id { get; internal set; }
    }
}