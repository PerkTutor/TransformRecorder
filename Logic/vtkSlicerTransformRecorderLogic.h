/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// .NAME vtkSlicerTransformRecorderLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerTransformRecorderLogic_h
#define __vtkSlicerTransformRecorderLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"
#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"
#include "vtkObjectFactory.h"

#include "vtkSmartPointer.h"

// MRML includes
#include "vtkMRMLTransformBufferNode.h"
// STD includes
#include <cstdlib>

#include "vtkSlicerTransformRecorderModuleLogicExport.h"



/// \ingroup Slicer_QtModules_TransformRecorder
class VTK_SLICER_TRANSFORMRECORDER_MODULE_LOGIC_EXPORT vtkSlicerTransformRecorderLogic :
  public vtkSlicerModuleLogic
{
public:
  vtkTypeMacro(vtkSlicerTransformRecorderLogic,vtkSlicerModuleLogic);

  static vtkSlicerTransformRecorderLogic *New();

  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkSlicerTransformRecorderLogic();
  virtual ~vtkSlicerTransformRecorderLogic();

  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
  
public:
  /// Initialize listening to MRML events
  void InitializeEventListeners();

  // Convenience methods for intersection between widget and MRML
  void ClearTransforms( vtkMRMLTransformBufferNode* bufferNode );

  void AddMessage( vtkMRMLTransformBufferNode* bufferNode, std::string messageString, double time );
  void RemoveMessage( vtkMRMLTransformBufferNode* bufferNode, int index );
  void ClearMessages( vtkMRMLTransformBufferNode* bufferNode );

  void ObserveAllRecordedTransforms( vtkMRMLTransformBufferNode* bufferNode );
  void AddAllRecordedTransformsToScene( vtkMRMLTransformBufferNode* bufferNode );

  // Editing operations on transform buffers
  void MergeTransformBuffers( vtkMRMLTransformBufferNode* inBufferNode1, vtkMRMLTransformBufferNode* inBufferNode2, vtkMRMLTransformBufferNode* outBufferNode, double offset = 0 );
  void SplitTransformBuffer( vtkMRMLTransformBufferNode* inBufferNode, vtkMRMLTransformBufferNode* outBufferNode1, vtkMRMLTransformBufferNode* outBufferNode2, double absTime );

  void ImportFromXMLFile( vtkMRMLTransformBufferNode* bufferNode, std::string fileName );
  void ExportToFile( vtkMRMLTransformBufferNode* bufferNode, std::string fileName );

  void ImportFromMHAFile( vtkMRMLTransformBufferNode* bufferNode, std::string fileName );



  // Grab module logic
  static vtkMRMLAbstractLogic* GetSlicerModuleLogic( std::string moduleName );
  
private:

  vtkSlicerTransformRecorderLogic(const vtkSlicerTransformRecorderLogic&); // Not implemented
  void operator=(const vtkSlicerTransformRecorderLogic&);               // Not implemented
  // Reference to the module MRML node.

};

#endif

