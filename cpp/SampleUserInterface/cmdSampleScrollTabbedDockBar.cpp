#include "StdAfx.h"
#include "SampleScrollTabbedDockBarDialog.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleScrollTabbedDockBar command
//

#pragma region SampleScrollTabbedDockBar command

class CCommandSampleScrollTabbedDockBar : public CRhinoCommand
{
public:
  CCommandSampleScrollTabbedDockBar() = default;
  ~CCommandSampleScrollTabbedDockBar() = default;
  UUID CommandUUID() override
  {
    // {CC486971-C2F5-4365-9FA6-70A89DDB0A27}
    static const GUID SampleScrollTabbedDockBarCommand_UUID =
    { 0xCC486971, 0xC2F5, 0x4365, { 0x9F, 0xA6, 0x70, 0xA8, 0x9D, 0xDB, 0xA, 0x27 } };
    return SampleScrollTabbedDockBarCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SampleScrollTabbedDockBar"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override ;
};

static class CCommandSampleScrollTabbedDockBar theSampleScrollTabbedDockBarCommand;

CRhinoCommand::result CCommandSampleScrollTabbedDockBar::RunCommand(const CRhinoCommandContext& context)
{
  CRhinoDoc* doc = context.Document();
  if (nullptr == doc)
    return CRhinoCommand::failure;

  ON_UUID tabId = CSampleScrollTabbedDockBarDialog::ID();

  if (context.IsInteractive())
  {
    CRhinoTabbedDockBarDialog::OpenDockbarTab(*doc, tabId);
  }
  else
  {
    bool bVisible = CRhinoTabbedDockBarDialog::IsTabVisible(*doc, tabId);

    ON_wString str;
    str.Format(L"%ls is %ls. New value", LocalCommandName(), bVisible ? L"visible" : L"hidden");

    CRhinoGetOption go;
    go.SetCommandPrompt(str);
    int h_option = go.AddCommandOption(RHCMDOPTNAME(L"Hide"));
    int s_option = go.AddCommandOption(RHCMDOPTNAME(L"Show"));
    int t_option = go.AddCommandOption(RHCMDOPTNAME(L"Toggle"));
    go.GetOption();
    if (go.CommandResult() != CRhinoCommand::success)
      return go.CommandResult();

    const CRhinoCommandOption* option = go.Option();
    if (0 == option)
      return CRhinoCommand::failure;

    int option_index = option->m_option_index;

    if (h_option == option_index && bVisible)
      CRhinoTabbedDockBarDialog::ShowDockbarTab(*doc, tabId, false, false, nullptr);
    else if (s_option == option_index && !bVisible)
      CRhinoTabbedDockBarDialog::ShowDockbarTab(*doc, tabId, true, true, nullptr);
    else if (t_option == option_index)
      CRhinoTabbedDockBarDialog::ShowDockbarTab(*doc, tabId, !bVisible, !bVisible, nullptr);
  }

  return CRhinoCommand::success;
}

#pragma endregion

//
// END SampleScrollTabbedDockBar command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
