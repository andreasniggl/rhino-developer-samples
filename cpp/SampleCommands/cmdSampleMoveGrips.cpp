#include "stdafx.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleMoveGrips command
//

#pragma region SampleMoveGrips command

class CCommandSampleMoveGrips : public CRhinoCommand
{
public:
  CCommandSampleMoveGrips() = default;
  ~CCommandSampleMoveGrips() = default;
  UUID CommandUUID() override
  {
    // {74F4A34-54C4-4B3F-B704-DCF4B148869D}
    static const GUID SampleMoveGripsCommand_UUID =
    { 0x74F4A34, 0x54C4, 0x4B3F, { 0xB7, 0x04, 0xDC, 0xF4, 0xB1, 0x48, 0x86, 0x9D } };
    return SampleMoveGripsCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SampleMoveGrips"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override ;
};

// The one and only CCommandSampleMoveGrips object
static class CCommandSampleMoveGrips theSampleMoveGripsCommand;

CRhinoCommand::result CCommandSampleMoveGrips::RunCommand(const CRhinoCommandContext& context)
{
  // Select grips to move
  CRhinoGetObject go;
  go.SetCommandPrompt(L"Select grips to move");
  go.SetGeometryFilter(CRhinoGetObject::grip_object);
  go.GetObjects(1, 0);
  if (go.CommandResult() != success)
    return go.CommandResult();

  // Fancy class for managing selected objects and grips
  CRhinoXformObjectList list;
  if (list.AddObjects(go, true) < 1)
    return CRhinoCommand::failure;

  // Point to move from
  CRhinoGetPoint gp;
  gp.SetCommandPrompt(L"Point to move from");
  gp.GetPoint();
  if (gp.CommandResult() != success)
    return gp.CommandResult();

  ON_3dPoint from = gp.Point();

  // Point to move to
  gp.SetCommandPrompt(L"Point to move to");
  gp.SetBasePoint(from);
  gp.DrawLineFromPoint(from, TRUE);
  gp.GetPoint();
  if (gp.CommandResult() != success)
    return gp.CommandResult();

  ON_3dPoint to = gp.Point();

  // Create a translation transformation
  ON_Xform xform = ON_Xform::TranslationTransformation(to - from);
  if (xform.IsValid())
  {
    // Transform the grip objects
    for (int i = 0; i < list.m_grips.Count(); i++)
    {
      CRhinoGripObject* grip = list.m_grips[i];
      if (grip)
        grip->MoveGrip(xform);
    }

    // Replace the old owner with a new one
    for (int i = 0; i < list.m_grip_owners.Count(); i++)
      RhinoUpdateGripOwner(list.m_grip_owners[i], false, nullptr);
  }

  context.m_doc.Redraw();

  return CRhinoCommand::success;
}

#pragma endregion

//
// END SampleMoveGrips command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
