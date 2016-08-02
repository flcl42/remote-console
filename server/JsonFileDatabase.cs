using Newtonsoft.Json;
using System;
using System.IO;

namespace server
{
    internal class JsonFileDatabase<T> : IDatabase<T>
    {
        private T data;
        private string fileName;

        public JsonFileDatabase(string fileName)
        {
            this.fileName = fileName;
        }

        public T Data
        {
            get
            {
                if(data ==null)
                {
                    data = loadData();
                }

                return data;
            }
        }

        private T loadData()
        {
            lock (this)
            {
                if (File.Exists(fileName))
                {
                    data = (T)JsonConvert.DeserializeObject<T>(File.ReadAllText(fileName));
                }
                else
                {
                    data = (T)Activator.CreateInstance(typeof(T));
                }
                return data;
            }
        }

        public void Save()
        {
            lock (this)
            {
                File.WriteAllText(fileName, JsonConvert.SerializeObject(data));
            }
        }
    }
}