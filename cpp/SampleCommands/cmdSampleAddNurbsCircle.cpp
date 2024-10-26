#include "stdafx.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleAddNurbsCircle command
//

#pragma region SampleAddNurbsCircle command

class CCommandSampleAddNurbsCircle : public CRhinoCommand
{
public:
  CCommandSampleAddNurbsCircle() = default;
  ~CCommandSampleAddNurbsCircle() = default;
  UUID CommandUUID() override
  {
    // {1E4E8E65-B040-4908-A495-E33143984611}
    static const GUID SampleAddNurbsCircleCommand_UUID =
    { 0x1E4E8E65, 0xB040, 0x4908, { 0xA4, 0x95, 0xE3, 0x31, 0x43, 0x98, 0x46, 0x11 } };
    return SampleAddNurbsCircleCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SampleAddNurbsCircle"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override ;
};

// The one and only CCommandSampleAddNurbsCircle object
static class CCommandSampleAddNurbsCircle theSampleAddNurbsCircleCommand;

CRhinoCommand::result CCommandSampleAddNurbsCircle::RunCommand(const CRhinoCommandContext& context)
{
  CRhinoDoc* doc = context.Document();
  if (nullptr == doc)
    return CRhinoCommand::failure;

  // Specify dimension, degree and number of control points.
  // The degree must be >= 1 and the number of control points
  // must be >= (degree+1). The number of knots is always
  // (number of control points + degree - 1).
  int dimension = 3;
  bool bIsRational = true;
  int degree = 2;
  int order = degree + 1;
  int cv_count = 9;
  int knot_count = cv_count + degree - 1;

  // Make a rational, degree 2 NURBS curve with 9 control points
  ON_NurbsCurve nc(dimension, bIsRational, order, cv_count);
  // Set the control points
  nc.SetCV(0, ON_4dPoint(1.0, 0.0, 0.0, 1.0));
  nc.SetCV(1, ON_4dPoint(0.707107, 0.707107, 0.0, 0.707107));
  nc.SetCV(2, ON_4dPoint(0.0, 1.0, 0.0, 1.0));
  nc.SetCV(3, ON_4dPoint(-0.707107, 0.707107, 0.0, 0.707107));
  nc.SetCV(4, ON_4dPoint(-1.0, 0.0, 0.0, 1.0));
  nc.SetCV(5, ON_4dPoint(-0.707107, -0.707107, 0.0, 0.707107));
  nc.SetCV(6, ON_4dPoint(0.0, -1.0, 0.0, 1.0));
  nc.SetCV(7, ON_4dPoint(0.707107, -0.707107, 0.0, 0.707107));
  nc.SetCV(8, ON_4dPoint(1.0, 0.0, 0.0, 1.0));
  // Set the 10 knots
  nc.SetKnot(0, 0.0);
  nc.SetKnot(1, 0.0);
  nc.SetKnot(2, 0.5*ON_PI);
  nc.SetKnot(3, 0.5*ON_PI);
  nc.SetKnot(4, ON_PI);
  nc.SetKnot(5, ON_PI);
  nc.SetKnot(6, 1.5*ON_PI);
  nc.SetKnot(7, 1.5*ON_PI);
  nc.SetKnot(8, 2.0*ON_PI);
  nc.SetKnot(9, 2.0*ON_PI);

  if (nc.IsValid())
  {
    doc->AddCurveObject(nc);
    doc->Redraw();
  }

  return CRhinoCommand::success;
}

#pragma endregion

//
// END SampleAddNurbsCircle command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
