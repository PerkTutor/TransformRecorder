

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
#include <iostream>
// Qt includes

// SlicerQt includes
#include "qSlicerTransformRecorderModuleWidget.h"
#include "ui_qSlicerTransformRecorderModule.h"
#include "qSlicerIO.h"
#include "qSlicerIOManager.h"
#include "qSlicerApplication.h"
#include <QtGui>

// MRMLWidgets includes
#include <qMRMLUtils.h>

#include "vtkSlicerTransformRecorderLogic.h"
#include "vtkMRMLLinearTransformNode.h"

#include "qMRMLNodeComboBox.h"
#include "vtkMRMLViewNode.h"
#include "vtkSlicerTransformRecorderLogic.h"
#include "vtkMRMLTransformBufferNode.h"
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_TransformRecorder
class qSlicerTransformRecorderModuleWidgetPrivate: public Ui_qSlicerTransformRecorderModule
{
  Q_DECLARE_PUBLIC( qSlicerTransformRecorderModuleWidget ); 

protected:
  qSlicerTransformRecorderModuleWidget* const q_ptr;
public:
  qSlicerTransformRecorderModuleWidgetPrivate( qSlicerTransformRecorderModuleWidget& object );
  ~qSlicerTransformRecorderModuleWidgetPrivate();

  vtkSlicerTransformRecorderLogic* logic() const;

  // Add embedded widgets here
  qSlicerTransformBufferWidget* TransformBufferWidget;
  qSlicerRecorderControlsWidget* RecorderControlsWidget;
  qSlicerMessagesWidget* MessagesWidget;
};

//-----------------------------------------------------------------------------
// qSlicerTransformRecorderModuleWidgetPrivate methods



qSlicerTransformRecorderModuleWidgetPrivate::qSlicerTransformRecorderModuleWidgetPrivate( qSlicerTransformRecorderModuleWidget& object ) : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------

qSlicerTransformRecorderModuleWidgetPrivate::~qSlicerTransformRecorderModuleWidgetPrivate()
{
}


vtkSlicerTransformRecorderLogic* qSlicerTransformRecorderModuleWidgetPrivate::logic() const
{
  Q_Q( const qSlicerTransformRecorderModuleWidget );
  return vtkSlicerTransformRecorderLogic::SafeDownCast( q->logic() );
}


//-----------------------------------------------------------------------------
// qSlicerTransformRecorderModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerTransformRecorderModuleWidget::qSlicerTransformRecorderModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerTransformRecorderModuleWidgetPrivate( *this ) )
{
}



qSlicerTransformRecorderModuleWidget::~qSlicerTransformRecorderModuleWidget()
{
}


void qSlicerTransformRecorderModuleWidget
::setupEmbeddedWidgets()
{
  Q_D(qSlicerTransformRecorderModuleWidget);

  // Adding embedded widgets
  d->TransformBufferWidget = new qSlicerTransformBufferWidget();
  d->BufferGroupBox->layout()->addWidget( d->TransformBufferWidget );
  d->TransformBufferWidget->setMRMLScene( NULL );
  d->TransformBufferWidget->setMRMLScene( d->logic()->GetMRMLScene() );
  d->TransformBufferWidget->setTransformBufferNode( NULL ); // Do not automatically select a node on entering the widget

  d->RecorderControlsWidget = new qSlicerRecorderControlsWidget();
  d->ControlsGroupBox->layout()->addWidget( d->RecorderControlsWidget );
  d->RecorderControlsWidget->setMRMLScene( NULL );
  d->RecorderControlsWidget->setMRMLScene( d->logic()->GetMRMLScene() );

  d->MessagesWidget = new qSlicerMessagesWidget();
  d->MessagesGroupBox->layout()->addWidget( d->MessagesWidget );
  d->MessagesWidget->setMRMLScene( NULL );
  d->MessagesWidget->setMRMLScene( d->logic()->GetMRMLScene() );

  // Setting up connections for embedded widgets
  // Connect the child widget to the transform buffer node change event (they already observe the modified event)
  connect( d->TransformBufferWidget, SIGNAL( transformBufferNodeChanged( vtkMRMLNode* ) ), d->RecorderControlsWidget, SLOT( setTransformBufferNode( vtkMRMLNode* ) ) );
  connect( d->TransformBufferWidget, SIGNAL( transformBufferNodeChanged( vtkMRMLNode* ) ), d->MessagesWidget, SLOT( setTransformBufferNode( vtkMRMLNode* ) ) );
}


void qSlicerTransformRecorderModuleWidget
::setup()
{
  Q_D(qSlicerTransformRecorderModuleWidget);

  d->setupUi(this);

  // Embed widgets here
  this->setupEmbeddedWidgets();

  // If the transform buffer node is changed, update everything
  connect( d->TransformBufferWidget, SIGNAL( transformBufferNodeChanged( vtkMRMLNode* ) ), this, SLOT( updateWidget() ) );
  connect( d->TransformBufferWidget, SIGNAL( transformBufferNodeModified() ), this, SLOT( updateWidget() ) );

  this->updateWidget();
}


void qSlicerTransformRecorderModuleWidget
::updateWidget()
{
  Q_D( qSlicerTransformRecorderModuleWidget );

  // The statistics should be reset to zeros if no buffer is selected
  if ( d->TransformBufferWidget->getTransformBufferNode() == NULL )
  {
    d->TotalTimeResultLabel->setText( "0.00" );
    d->NumTransformsResultLabel->setText( "0" );
    d->NumMessagesResultLabel->setText( "0" );
    return;
  }
  
  std::stringstream ss;

  ss.str( "" );
  ss.precision( 2 );
  ss << std::fixed << d->TransformBufferWidget->getTransformBufferNode()->GetTotalTime();
  d->TotalTimeResultLabel->setText( ss.str().c_str() );
  
  ss.str( "" );
  ss.precision( 0 );
  ss << std::fixed << d->TransformBufferWidget->getTransformBufferNode()->GetNumTransforms();
  d->NumTransformsResultLabel->setText( ss.str().c_str() );
  
  ss.str( "" );
  ss.precision( 0 );
  ss << std::fixed << d->TransformBufferWidget->getTransformBufferNode()->GetNumMessages();
  d->NumMessagesResultLabel->setText( ss.str().c_str() );
   
}
