using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Linq;
using System.Diagnostics.Eventing.Reader;
using System.Diagnostics;
using System.Collections.Generic;

using Rhino.Geometry;
using Rhino.Input.Custom;

using CommandLine;
using Grasshopper.Kernel;


namespace RunGrasshopper
{
   class Program
   {
      class Options
      {
         [Option('t', "trace", Default = false, Required = false, HelpText = "Activate Debug Tracing")]
         public bool Tracing { get; set; }

         [Value(0, MetaName = "gh_file", Required = true, HelpText = "Grasshopper File to Execute (*.gh)")]
         public string GrasshopperFile { get; set; }
      }


      static Program()
      {
         RhinoInside.Resolver.Initialize();
      }

      static void Main(string[] args)
      {
         string grasshopper_file = string.Empty;

         // parse arguments (see: https://github.com/commandlineparser/commandline)
         var opts = Parser.Default.ParseArguments<Options>(args)
            .WithParsed<Options>(o =>
            {
               grasshopper_file = System.IO.Path.ChangeExtension(o.GrasshopperFile,"gh");
               
               if(o.Tracing)
               {
                  Trace.Listeners.Add(new TextWriterTraceListener(Console.Out));
               }
            })
            .WithNotParsed<Options>(o =>
            {
               Console.WriteLine($"Unable to parse options {o.ToString()}");
            });

         // 
         using (var core = new Rhino.Runtime.InProcess.RhinoCore())
         {
            RunHelper(grasshopper_file);
         }
      }

      // Currently need a separate RunHelper function so the .NET runtime won't attempt to load the
      // Grasshopper assembly until after RhinoCore has been created. This should be "fixable" in a
      // future version of the RhinoInside nuget package
      static void RunHelper(string grasshopper_file)
      {
         // Start grasshopper in "headless" mode
         var pluginObject = Rhino.RhinoApp.GetPlugInObject("Grasshopper") as Grasshopper.Plugin.GH_RhinoScriptInterface;
         pluginObject.RunHeadless();

         // open grasshopper file
         var io = new Grasshopper.Kernel.GH_DocumentIO();
         if (!io.Open(grasshopper_file))
         {
            Console.WriteLine($"Failed to load Grasshopper Definition: {grasshopper_file}");
            return;
         }
         

         var doc = io.Document;

         string working_path = System.IO.Path.GetDirectoryName(grasshopper_file);

         // connect events
         doc.SolutionEnd += OnDocumentSolutionEnd;

         // Solve definition
         Console.WriteLine($"Executing Grasshopper Definition: {grasshopper_file}");
         
         doc.Enabled = true;
         doc.NewSolution(false, Grasshopper.Kernel.GH_SolutionMode.CommandLine);

         foreach (var obj in doc.Objects)
         {
            // process contextual inputs
            if( obj is IGH_ContextualParameter ctx_param)
            {
               Trace.WriteLine($"Found Contextual Input with Name: {obj.Name}, Nickname: {obj.NickName}","Results");
            }
            else if(obj is IGH_Param igh_param)
            {
               Trace.WriteLine($"Found Parameter {igh_param.GetType().Name} with Name: {igh_param.Name}, Nickname: {igh_param.NickName}", "Results");
            }
            else if(obj is GH_Component component)
            {
               var class_name = obj.GetType().Name;
               if(class_name == "ContextBakeComponent")
               {
                  var bake_param = component.Params.Input.First();
                  Trace.WriteLine($"Found Component {class_name} with Input Name: {bake_param.Name}, Nickname:  {bake_param.NickName}", "Results");

                  ProcessContextualBake( bake_param, working_path );
               }
               else if(class_name == "ContextPrintComponent")
               {
                  var prnt_param = component.Params.Input.First();
                  Trace.WriteLine($"Found Component {class_name} with Input Name: {prnt_param.Name}, Nickname: {prnt_param.NickName}", "Results");

                  ProcessContextualPrint(prnt_param, working_path );
               }
               else
               {
                  Trace.WriteLine($"Found Component {class_name} with Name: {component.Name}, Nickname: {component.NickName}", "Results");
               }
            }
            else
            {
               Trace.WriteLine($"Found Other Type {obj.GetType().Name} with Name: {obj.Name}, Nickname: {obj.NickName}", "Results");
            }
         }

         Console.WriteLine("Done... press any key to exit");
         Console.ReadKey();
      }

      private static void ProcessContextualBake(IGH_Param param, string working_path)
      {
         // collect all items in list
         var all_items = new List<(string, Grasshopper.Kernel.Types.IGH_Goo)>();
         foreach( var item in param.VolatileData.AllData(true))
         {
            all_items.Add((item.GetType().FullName, item));    
         }

         // serialize to json and write to file
         string json = Newtonsoft.Json.JsonConvert.SerializeObject(all_items, Newtonsoft.Json.Formatting.Indented);
         
         string output_file = System.IO.Path.Combine(working_path, param.NickName);
         System.IO.File.WriteAllText(output_file, json);
      }

      private static void ProcessContextualPrint(IGH_Param param, string working_path)
      {
         string output_file = System.IO.Path.Combine(working_path, param.NickName); 
         using( var file_stream = new System.IO.StreamWriter(output_file))
         {
            foreach( var item in param.VolatileData.AllData(true))
            {
               file_stream.WriteLine(item.ToString());
            }
         }
      }

      private static void OnDocumentSolutionEnd(object sender, GH_SolutionEventArgs e)
      {
         Trace.WriteLine($"Document Solution Executed: TimeSpan: {e.Duration.TotalMilliseconds} ms", "Events");
      }
   }
}
