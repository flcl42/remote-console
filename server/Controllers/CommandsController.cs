using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNet.Mvc;

namespace server.Controllers
{
    [Route("api/[controller]")]
    public class CommandsController : Controller
    {
        ConnectionsManager connectionsManager;
        public CommandsController(ConnectionsManager connectionsManager)
        {
            this.connectionsManager = connectionsManager;
        }

        // GET api/commands/xxxxyyyyzzzz
        // login with id or register and long pool for commands
        [HttpGet("{id?}")]
        public IActionResult Get(string id = null)
        {
            if (id == null)
            {
                return new JsonResult(new
                {
                    Command = RCommands.Register,
                    Id = connectionsManager.CreateUser().Id
                });
            }

            object result = null;
           var res= Task.WaitAny(
                Task.Run(() => result = connectionsManager.GetCommandFor(id, Request.Headers["OS"], HttpContext.Request.Headers["X-Forwarded-For"].FirstOrDefault() ?? "<no ip>").Result),
                Task.Delay(15000)
                );

            if(result==null)
            {
                connectionsManager.RemoveWaiter(id);
            }
            return new JsonResult(result ?? new { Command = RCommands.Reset });
        }
        public class InputData
        {
            public string Value { get; set; }
        }
        

        // for use output
        [HttpPost("{id}")]
        public void Post(string id, [FromBody]InputData data)
        {
            if (id.StartsWith(connectionsManager.ActiveId.ToUpper()) && data != null)
            {
                Console.Write(data.Value);
            }
        }        
    }
}
