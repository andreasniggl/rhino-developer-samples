using Rhino;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SampleCsWpf
{
  internal class SampleCsWpfEventHandler
  {
    private bool _isEnabled = false;
    private int _selectionCount = 0;

    private static SampleCsWpfEventHandler _instance = null;

    private SampleCsWpfEventHandler()
    { }

    public static SampleCsWpfEventHandler Instance => _instance ?? (_instance = new SampleCsWpfEventHandler());

    public void Enable(bool enable)
    {
      if(enable != _isEnabled)
      {
        if(enable)
        {
          RhinoDoc.SelectObjects += OnSelectObjects;
          RhinoDoc.DeselectObjects += OnDeselectObjects;
          RhinoDoc.DeselectAllObjects += OnDeselectAllObjects;
        }
        else
        {
          RhinoDoc.SelectObjects -= OnSelectObjects;
          RhinoDoc.DeselectObjects -= OnDeselectObjects;
          RhinoDoc.DeselectAllObjects -= OnDeselectAllObjects;
        }
        _isEnabled = enable;
      }
    }


    public void OnSelectObjects(object sender, Rhino.DocObjects.RhinoObjectSelectionEventArgs e)
    {
      _selectionCount += e.RhinoObjects.Length;
      RhinoApp.WriteLine($"Selected {e.RhinoObjects.Length} objects. Current selection count: {_selectionCount}");
    }

    public void OnDeselectObjects(object sender, Rhino.DocObjects.RhinoObjectSelectionEventArgs e)
    {
      _selectionCount -= e.RhinoObjects.Length;
      RhinoApp.WriteLine($"De-Selected {e.RhinoObjects.Length} objects. Current selection count: {_selectionCount}");
    }

    public void OnDeselectAllObjects(object sender, Rhino.DocObjects.RhinoDeselectAllObjectsEventArgs e)
    {
      _selectionCount = 0;
      RhinoApp.WriteLine($"De-Selected all objects. Current selection count: {_selectionCount}");
    }

    private static void DebugWriteMethod()
    {
      try
      {
        var method_name = new StackTrace().GetFrame(1).GetMethod().Name;
        RhinoApp.WriteLine("> {0}", method_name);
      }
      catch
      {
        // ignored
      }
    }
  }
}
