/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerRecorderControlsWidget.h"

#include <QtGui>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_CreateModels
class qSlicerRecorderControlsWidgetPrivate
  : public Ui_qSlicerRecorderControlsWidget
{
  Q_DECLARE_PUBLIC(qSlicerRecorderControlsWidget);
protected:
  qSlicerRecorderControlsWidget* const q_ptr;

public:
  qSlicerRecorderControlsWidgetPrivate( qSlicerRecorderControlsWidget& object);
  ~qSlicerRecorderControlsWidgetPrivate();
  virtual void setupUi(qSlicerRecorderControlsWidget*);
};

// --------------------------------------------------------------------------
qSlicerRecorderControlsWidgetPrivate
::qSlicerRecorderControlsWidgetPrivate( qSlicerRecorderControlsWidget& object) : q_ptr(&object)
{
}

qSlicerRecorderControlsWidgetPrivate
::~qSlicerRecorderControlsWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerRecorderControlsWidgetPrivate
::setupUi(qSlicerRecorderControlsWidget* widget)
{
  this->Ui_qSlicerRecorderControlsWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerRecorderControlsWidget methods

//-----------------------------------------------------------------------------
qSlicerRecorderControlsWidget
::qSlicerRecorderControlsWidget(QWidget* parentWidget) : qSlicerWidget( parentWidget ) , d_ptr( new qSlicerRecorderControlsWidgetPrivate(*this) )
{
  this->TransformBufferNode = NULL;
  this->TransformRecorderLogic = vtkSlicerTransformRecorderLogic::SafeDownCast( vtkSlicerTransformRecorderLogic::GetSlicerModuleLogic( "TransformRecorder" ) );
  this->setup();
}


qSlicerRecorderControlsWidget
::~qSlicerRecorderControlsWidget()
{
}


void qSlicerRecorderControlsWidget
::setup()
{
  Q_D(qSlicerRecorderControlsWidget);

  d->setupUi(this);

  connect( d->TransformCheckableComboBox, SIGNAL( checkedNodesChanged() ), this, SLOT( onCheckedTransformsChanged() ) );

  connect( d->StartStopButton, SIGNAL( clicked( bool ) ), this, SLOT( onStartStopButtonClicked( bool ) ) );
  connect( d->ClearButton, SIGNAL( clicked() ), this, SLOT( onClearButtonClicked() ) );

  this->updateWidget();
}


void qSlicerRecorderControlsWidget
::setTransformBufferNode( vtkMRMLNode* newTransformBufferNode )
{
  Q_D(qSlicerRecorderControlsWidget);

  this->qvtkDisconnectAll();

  this->TransformBufferNode = vtkMRMLTransformBufferNode::SafeDownCast( newTransformBufferNode );

  this->qvtkConnect( this->TransformBufferNode, vtkMRMLTransformBufferNode::ActiveTransformAddedEvent, this, SLOT( onTransformBufferActiveTransformsChanged() ) );
  this->qvtkConnect( this->TransformBufferNode, vtkMRMLTransformBufferNode::ActiveTransformRemovedEvent, this, SLOT( onTransformBufferActiveTransformsChanged() ) );
  this->qvtkConnect( this->TransformBufferNode, vtkMRMLTransformBufferNode::RecordingStateChangedEvent, this, SLOT( updateWidget() ) );

  this->updateWidget();
}


void qSlicerRecorderControlsWidget
::onTransformBufferActiveTransformsChanged()
{
  Q_D(qSlicerRecorderControlsWidget);

  // Disable to the onCheckedChanged listener when initializing the selections
  // We don't want to simultaneously update the observed nodes from selections and selections from observed nodes
  disconnect( d->TransformCheckableComboBox, SIGNAL( checkedNodesChanged() ), this, SLOT( onCheckedTransformsChanged() ) );

  // Assume the default is not checked, and check all those that are observed
  for ( int i = 0; i < d->TransformCheckableComboBox->nodeCount(); i++ )
  {
    if ( this->TransformBufferNode != NULL && this->TransformBufferNode->IsActiveTransformID( d->TransformCheckableComboBox->nodeFromIndex(i)->GetID() ) )
    {
	    d->TransformCheckableComboBox->setCheckState( d->TransformCheckableComboBox->nodeFromIndex(i), Qt::Checked );
    }
    else
    {
      d->TransformCheckableComboBox->setCheckState( d->TransformCheckableComboBox->nodeFromIndex(i), Qt::Unchecked );
    }
  }

  connect( d->TransformCheckableComboBox, SIGNAL( checkedNodesChanged() ), this, SLOT( onCheckedTransformsChanged() ) );
}


void qSlicerRecorderControlsWidget
::onCheckedTransformsChanged()
{
  Q_D(qSlicerRecorderControlsWidget);

  if ( this->TransformBufferNode == NULL )
  {
    return;
  }
    
  // Go through transform types (ie ProbeToReference, StylusTipToReference, etc)
  std::vector< std::string > activeTransformIDs;
  for ( int i = 0; i < d->TransformCheckableComboBox->nodeCount(); i++ )
  {
    if( d->TransformCheckableComboBox->checkState( d->TransformCheckableComboBox->nodeFromIndex(i) ) == Qt::Checked  )
    {
      activeTransformIDs.push_back( d->TransformCheckableComboBox->nodeFromIndex(i)->GetID() );
    }
  }

  this->TransformBufferNode->SetActiveTransformIDs( activeTransformIDs );
}



void qSlicerRecorderControlsWidget
::onStartStopButtonClicked( bool state )
{
  Q_D(qSlicerRecorderControlsWidget);

  if ( this->TransformBufferNode == NULL )
  {
    return;
  }
  
  // If recording is started
  if ( state == true )
  {
    this->TransformBufferNode->StartRecording();
    d->StatusResultLabel->setText( "Recording" );
    d->StartStopButton->setText( "Stop" );
  }
  else
  {
    this->TransformBufferNode->StopRecording();
    d->StatusResultLabel->setText( "Stopped" );
    d->StartStopButton->setText( "Start" );
  }
  
  this->updateWidget();
}



void qSlicerRecorderControlsWidget
::onClearButtonClicked()
{
  Q_D(qSlicerRecorderControlsWidget);

  if ( this->TransformBufferNode == NULL )
  {
    return;
  }

  this->TransformRecorderLogic->ClearTransforms( this->TransformBufferNode );
  
  this->updateWidget();
}


void qSlicerRecorderControlsWidget
::updateWidget()
{
  Q_D(qSlicerRecorderControlsWidget);

  if ( this->TransformBufferNode == NULL )
  {
    return;
  }

  // Set the text indicating recording
  if ( this->TransformBufferNode->GetRecording() )
  {
    d->StatusResultLabel->setText( "Recording" );
    d->StartStopButton->setChecked( true );
    d->StartStopButton->setText( "Stop" );
  }
  else
  {
    d->StatusResultLabel->setText( "Stopped" );
    d->StartStopButton->setChecked( false );
    d->StartStopButton->setText( "Start" );
  }

  this->onTransformBufferActiveTransformsChanged();
}
