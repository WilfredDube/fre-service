#include "../../include/cad-file-reader/CadFileReader.h"
#include "../../include/sheet-metal-component/SheetMetal.h"
#include "../../include/cad-file-reader/StepFileReader.h"
#include "../../include/cad-file-reader/ReaderCreationFailedException.h"

#include <TopExp_Explorer.hxx>
#include <STEPControl_Reader.hxx>

using namespace Fxt::CadFileReader;
using namespace Fxt::SheetMetalComponent;

void StepFileReader::makeReader(const std::string& stepFile)
{
    mMyStepReader = std::make_shared<STEPControl_Reader>();

    if (mMyStepReader->ReadFile(stepFile.c_str()) != IFSelect_RetDone) {
        throw std::invalid_argument("Cannot open STEP file");
    }

    std::cout << "File .......... : " << stepFile << '\n';
    IFSelect_PrintCount mode = IFSelect_ListByItem;
    mMyStepReader->PrintCheckLoad(false, mode);

    //Transfer whole file
    Standard_Integer NbRoots = mMyStepReader->NbRootsForTransfer();
    mMyStepReader->ClearShapes();

    for (Standard_Integer n = 1; n<= NbRoots; n++) {
      mMyStepReader->TransferRoot(n);
    }
}

void StepFileReader::extractFaces(SheetMetalPtr& model, const std::string& filename)
{
  makeReader(filename);

  if(mMyStepReader == nullptr)
    throw ReaderCreationFailedException("Failed to create STEP file reader.");

  if (mMyStepReader->NbShapes() <= 0)
    throw std::invalid_argument("Unable to read model.");

  FaceID faceID = 0;
  TopoDS_Shape topoDSFaces;
  size_t numShapes = mMyStepReader->NbShapes();

  /* Go through the shape object and extract the faces. */
  for (size_t i = 1; i <= numShapes; i++) {
    topoDSFaces = mMyStepReader->Shape(i);
    TopExp_Explorer myFaceExplorer(topoDSFaces, TopAbs_FACE);

    while (myFaceExplorer.More()){
      std::shared_ptr<TopoDS_Shape> aShape { std::make_shared<TopoDS_Shape>(myFaceExplorer.Current()) };

      ++faceID;

      model->assignFaceAttributes(faceID, aShape);

      myFaceExplorer.Next();
    }
  }
}
