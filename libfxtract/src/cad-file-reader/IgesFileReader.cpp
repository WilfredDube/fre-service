#include "../../include/cad-file-reader/CadFileReader.h"
#include "../../include/sheet-metal-component/SheetMetal.h"
#include "../../include/cad-file-reader/IgesFileReader.h"
#include "../../include/cad-file-reader/ReaderCreationFailedException.h"

#include <IGESControl_Reader.hxx>

using namespace Fxt::CadFileReader;
using namespace Fxt::SheetMetalComponent;

void IgesFileReader::makeReader(const std::string& igesFile)
{
    mMyIgesReader = std::make_shared<IGESControl_Reader>();

    if (mMyIgesReader->ReadFile(igesFile.c_str()) != IFSelect_RetDone) {
        throw std::invalid_argument("Cannot open IGES file");
    };

    std::cout << "File .......... : " << igesFile << '\n';
    IFSelect_PrintCount mode = IFSelect_ListByItem;
    mMyIgesReader->PrintCheckLoad(false, mode);

    //selects all IGES faces in the file and puts them into a list  called //MyList,
    Handle(TColStd_HSequenceOfTransient) myList =  mMyIgesReader->GiveList("iges-faces");
    mMyIgesReader->TransferRoots();

    // //translates MyList,
    myList->Length();
    mMyIgesReader->TransferList(myList);
}

void IgesFileReader::extractFaces(SheetMetalPtr& model, const std::string& filename)
{
  makeReader(filename);

  if(mMyIgesReader == nullptr)
    throw ReaderCreationFailedException("Failed to create IGES file reader.");

  if (mMyIgesReader->NbShapes() <= 0)
    throw std::invalid_argument("Unable to read model.");

  FaceID faceID = 0;
  size_t numShapes = mMyIgesReader->NbShapes();

  /* Go through the shape object and extract the faces. */
  for (size_t i = 1; i <= numShapes; i++) {
    std::shared_ptr<TopoDS_Shape> aShape { std::make_shared<TopoDS_Shape>(mMyIgesReader->Shape(i)) };

    ++faceID;

    model->assignFaceAttributes(faceID, aShape);
  }
}

