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
// Qt includes
#include <QDebug>
#include <QMouseEvent>
#include <QToolTip>
// SlicerQt includes
#include "qSlicerCalculusModuleWidget.h"
#include "ui_qSlicerCalculusModuleWidget.h"
#include "qSlicerAbstractCoreModule.h"

#include <qSlicerCoreApplication.h>
#include <qSlicerModuleManager.h>


// MRML includes
#include "vtkMRMLScene.h"

// Markups includes
#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLMarkupsNode.h"
#include "vtkSlicerMarkupsLogic.h"

#include "vtkMRMLCropVolumeParametersNode.h"

#include "qSlicerCoreApplication.h"
#include "qSlicerModuleManager.h"


// vtkSlicerCalculusLogic includes
#include "vtkSlicerCalculusLogic.h"
#include "qSlicerCalculusReformatWidget.h"
#include "vtkSlicerReformatLogic.h"

//--------------��ɺɺ���-------------------------------
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLSelectionNode.h"
#include "vtkMRMLSliceLogic.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLLabelMapVolumeNode.h"
//------------------------------
#include "vtkMRMLCropVolumeParametersNode.h"

#include "qSlicerCoreApplication.h"
#include "qSlicerModuleManager.h"

// vtkSlicerCalculusLogic includes
#include "vtkSlicerCalculusLogic.h"
//-----------------------
#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <QtGui>

#include <Qtcore/qstring.h>
#include <Qtcore/QFile>
#include <stdexcept>

#include <QTableWidget>  
#include <QTableWidgetItem>
using namespace std;
class QAxObject;
//-------------------��ӽ���--------------------------------------
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerCalculusModuleWidgetPrivate: public Ui_qSlicerCalculusModuleWidget
{
	Q_DECLARE_PUBLIC(qSlicerCalculusModuleWidget);
protected:
	qSlicerCalculusModuleWidget* const q_ptr;
public:
	qSlicerCalculusModuleWidgetPrivate(qSlicerCalculusModuleWidget& object);
	~qSlicerCalculusModuleWidgetPrivate();

  vtkSlicerCalculusLogic* logic() const;
};

//-----------------------------------------------------------------------------
// qSlicerCalculusModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerCalculusModuleWidgetPrivate::qSlicerCalculusModuleWidgetPrivate(qSlicerCalculusModuleWidget& object):q_ptr(&object)
{
}
qSlicerCalculusModuleWidgetPrivate::~qSlicerCalculusModuleWidgetPrivate()
{
}
vtkSlicerCalculusLogic* qSlicerCalculusModuleWidgetPrivate::logic() const
{
	Q_Q(const qSlicerCalculusModuleWidget);
	return vtkSlicerCalculusLogic::SafeDownCast(q->logic());
}
//-----------------------------------------------------------------------------
// qSlicerCalculusModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerCalculusModuleWidget::qSlicerCalculusModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr(new qSlicerCalculusModuleWidgetPrivate(*this))
{


}

//-----------------------------------------------------------------------------
qSlicerCalculusModuleWidget::~qSlicerCalculusModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerCalculusModuleWidget::setup()
{
  Q_D(qSlicerCalculusModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // set up buttons connection
  connect(d->acqStoneBtn, SIGNAL(clicked()),
	  this, SLOT(onAcqStoneBtnClicked()));
  connect(d->x_verticalAcqStoneBtn, SIGNAL(clicked()),
	  this, SLOT(onX_VerticalAcqStoneBtnClicked()));
  connect(d->y_verticalAcqStoneBtn, SIGNAL(clicked()),
	  this, SLOT(onY_VerticalAcqStoneBtnClicked()));
  connect(d->z_verticalAcqStoneBtn, SIGNAL(clicked()),
	  this, SLOT(onZ_VerticalAcqStoneBtnClicked()));
  
  // set up input&output&markups connection
  connect(d->inputVolumeMRMLNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
	  this, SLOT(onInputVolumeMRMLNodeChanged()));
  //connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->reformatWidget, SIGNAL(d->reformatWidget->mrmlSceneChanged(vtkMRMLScene*)));

  //--------------��ɺɺ��ӵĳ���----------------------------
  connect(d->inputVolumeMRMLNodeComboBox_2, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(InputVolumeMRMLNodeChanged()));

  connect(d->markupsMRMLNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(onMarkupsMRMLNodeChanged()));

  connect(d->generateButton, SIGNAL(clicked()), this, SLOT(generateClicked()));// ��������ɰ�ťʱ�����������ļ��ĺ������ļ����봰�ڱ����)Clicked()

  connect(d->saveButton, SIGNAL(clicked()), this, SLOT(saveClicked()));//�� �±��水ť����������������excel��
  connect(d->saveButton_2, SIGNAL(clicked()), this, SLOT(saveClicked_2()));//�� �±��水ť����������������excel��

  connect(d->clearButton, SIGNAL(clicked()), this, SLOT(clearButtonClicked()));
  connect(d->clearButton_2, SIGNAL(clicked()), this, SLOT(clearButtonClicked_2()));

  //--------------------��ӽ���-------------------------

  qSlicerAbstractCoreModule* reformatModule =
	  qSlicerCoreApplication::application()->moduleManager()->module("Reformat");
  if (reformatModule)
  {
	  vtkSlicerReformatLogic* reformatLogic =
		  vtkSlicerReformatLogic::SafeDownCast(reformatModule->logic());
	  //��ȡReformat module��logic
	  d->reformatWidget->setReformatLogic(reformatLogic);
  }
  //���ݱ�module��logic
  d->reformatWidget->setCalculusLogic(d->logic());

  connect(d->reformatWidget, SIGNAL(newStoneParms(QHash<QString,double>)), this, SLOT(addStoneParmsSlot(QHash<QString,double>)));
 
}

//-----------------------------------------------------------------------------
void qSlicerCalculusModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
	this->Superclass::setMRMLScene(scene);
	if (scene == NULL)
	{
		return;
	}

	// observe close event so can re-add a parameters node if necessary
	qvtkReconnect(this->mrmlScene(), vtkMRMLScene::EndCloseEvent,
		this, SLOT(onEndCloseEvent()));

}

//-----------------------------------------------------------------------------
void qSlicerCalculusModuleWidget::enter()
{
	// if there are already some
	// volumes or ROIs in the scene, they can be set up for use

	this->onInputVolumeMRMLNodeChanged();
	Q_D(qSlicerCalculusModuleWidget);
	//d->outputVolumeMRMLNodeComboBox->setEnabled(false);

	this->Superclass::enter();
}
//-----------------------------------------------------------------------------
//��ȡ�������
void qSlicerCalculusModuleWidget::getParamsFromUi()
{
	Q_D(qSlicerCalculusModuleWidget);
	//��ȡ�����������
	vtkSlicerCalculusLogic::s_sliceThick = d->sliceThickSpinBox->value();//��� ����ǶȲɼ����� ���������
	vtkSlicerCalculusLogic::s_uWater = d->uWaterSpinBox->value();//uˮ ����ƽ���ܶȡ������ܶȲ�����
	vtkSlicerCalculusLogic::s_materialThick = d->materialThickSpinBox->value();//���Ϻ�� ����ƽ���ܶȡ������ܶȲ�����


	vtkMRMLNode *mrmlNode = this->mrmlScene()->GetNodeByID("vtkMRMLSliceNodeRed");

	//����VolumeMRMLNode,��ʱû��
	d->reformatWidget->setVtkMRMLVolumeNode(vtkMRMLVolumeNode::SafeDownCast(d->inputVolumeMRMLNodeComboBox->currentNode()));
	//����VolumeMRMLNode,��ʱû��
	d->reformatWidget->setVtkMRMLSliceNodeRed(vtkMRMLSliceNode::SafeDownCast(mrmlNode));
	//����VolumeMRMLScene
	d->reformatWidget->setVtkMRMLScene(this->mrmlScene());
}
//��ʯ����ǶȲɼ�����
void qSlicerCalculusModuleWidget::onAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);

	getParamsFromUi();//��ȡ�������

	d->reformatWidget->closeAllReformat();
	d->reformatWidget->enableReformat(true, "vtkMRMLSliceNodeRed");//����Red����
	d->reformatWidget->randRotate();//��ʼ���ɲɼ�


}
//��ʯ��ֱX�᷽��ɼ�����
void qSlicerCalculusModuleWidget::onX_VerticalAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);

	getParamsFromUi();//��ȡ�������

	//======ҵ�����=====//
	d->reformatWidget->closeAllReformat();
	d->reformatWidget->enableReformat(true, "vtkMRMLSliceNodeYellow");//����Red����
	//��ʼ��ֱ�ɼ�
	d->reformatWidget->verticalAcq();

}
//��ʯ��ֱY�᷽��ɼ�����
void qSlicerCalculusModuleWidget::onY_VerticalAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);

	getParamsFromUi();//��ȡ�������

	//======ҵ�����=====//
	d->reformatWidget->closeAllReformat();
	d->reformatWidget->enableReformat(true, "vtkMRMLSliceNodeGreen");//����Red����
	//��ʼ��ֱ�ɼ�
	d->reformatWidget->verticalAcq();

}
//��ʯ��ֱZ�᷽��ɼ�����
void qSlicerCalculusModuleWidget::onZ_VerticalAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);

	getParamsFromUi();//��ȡ�������

	//======ҵ�����=====//
	d->reformatWidget->closeAllReformat();
	d->reformatWidget->enableReformat(true, "vtkMRMLSliceNodeRed");//����Red����
	//��ʼ��ֱ�ɼ�
	d->reformatWidget->verticalAcq();

}
void qSlicerCalculusModuleWidget::onInputVolumeMRMLNodeChanged()
{
	Q_D(qSlicerCalculusModuleWidget);
	Q_ASSERT(d->inputVolumeMRMLNodeComboBox);
	updateAcqStoneButtonState();
	getParamsFromUi();//��ȡ�������������һЩ����

	if (d->inputVolumeMRMLNodeComboBox->currentNodeID() != "")
	{
		d->acqStoneBtn->setEnabled(true);
		d->x_verticalAcqStoneBtn->setEnabled(true);
		d->y_verticalAcqStoneBtn->setEnabled(true);
		d->z_verticalAcqStoneBtn->setEnabled(true);
	}
	else
	{
		d->acqStoneBtn->setEnabled(false);
		d->x_verticalAcqStoneBtn->setEnabled(false);
		d->y_verticalAcqStoneBtn->setEnabled(false);
		d->z_verticalAcqStoneBtn->setEnabled(false);
	}

	//qDebug() << "onInputVolumeMRMLNodeChanged" << endl;
}
void qSlicerCalculusModuleWidget::updateAcqStoneButtonState()
{

	Q_D(qSlicerCalculusModuleWidget);
	if (d->inputVolumeMRMLNodeComboBox->currentNode())
	{
		d->acqStoneBtn->setToolTip("Input volume is required to do the segmentation.");
		QToolTip::showText(mapToGlobal(d->acqStoneBtn->pos()), QString("d->inputVolumeMRMLNodeComboBox"), (QWidget*)d->acqStoneBtn, QRect());
		d->acqStoneBtn->setEnabled(true);
	}
	


}
void qSlicerCalculusModuleWidget::onEndCloseEvent()
{
	Q_D(qSlicerCalculusModuleWidget);
	vtkSmartPointer<vtkSlicerCalculusLogic> logic = d->logic();
	//----------------------��ɺɺ���---------------------
	d->generateButton->setEnabled(true);
	d->markupsMRMLNodeComboBox->setEnabled(true);
//----------------------��ӽ���---------------------
	//logic->reset(vtkMRMLMarkupsFiducialNode::SafeDownCast(d->markupsMRMLNodeComboBox->currentNode()), 0);
	//d->acqStoneBtn->setEnabled(true);
	cout << "close scene!" << endl;

}
//----------------------��ɺɺ���---------------------
void qSlicerCalculusModuleWidget::InputVolumeMRMLNodeChanged()
{
	Q_D(qSlicerCalculusModuleWidget);
	Q_ASSERT(d->inputVolumeMRMLNodeComboBox);
	updategenerateButtonState();
	qDebug() << "InputVolumeMRMLNodeChanged" << endl;
}
void qSlicerCalculusModuleWidget::onMarkupsMRMLNodeChanged()
{
	Q_D(qSlicerCalculusModuleWidget);
	Q_ASSERT(d->markupsMRMLNodeComboBox);
	qDebug() << "onMarkupsMRMLNodeChanged" << endl;
}
void qSlicerCalculusModuleWidget::updategenerateButtonState()
{

	Q_D(qSlicerCalculusModuleWidget);
	if (d->inputVolumeMRMLNodeComboBox->currentNode())
	{
		d->generateButton->setToolTip("Input volume is required to do the segmentation.");
		d->generateButton->setEnabled(true);
	}
}
void qSlicerCalculusModuleWidget::generateClicked()
{
	Q_D(qSlicerCalculusModuleWidget);
	vtkSlicerCalculusLogic *logic = d->logic();
	QHash<QString, double> paramHash;
	paramHash = logic->aqcCircleData(vtkMRMLVolumeNode::SafeDownCast(d->inputVolumeMRMLNodeComboBox_2->currentNode()), vtkMRMLMarkupsFiducialNode::SafeDownCast(d->markupsMRMLNodeComboBox->currentNode()));

	addTableWidgetRow(paramHash,d->tableblock);
	d->saveButton->setEnabled(true);
	
}
void qSlicerCalculusModuleWidget::addStoneParmsSlot(QHash<QString,double> hash)
{
	Q_D(qSlicerCalculusModuleWidget);
	addTableWidgetRow(hash,d->tableblock_2);
	d->saveButton_2->setEnabled(true);
}
//��tableWidget�����һ�в���
void qSlicerCalculusModuleWidget::addTableWidgetRow(QHash<QString,double> paramHash,QTableWidget* widget)
{
	int counts = widget->rowCount();
	widget->insertRow(counts);
	QString a, b, c,e,f;
	a = QString::number(paramHash.value("max"));
	b = QString::number(paramHash.value("min"));
	c = QString::number(paramHash.value("average"));//��double����תΪqstring��ʽ
	e = QString::number(paramHash.value("AOD"));
	f = QString::number(paramHash.value("IOD"));
	qDebug() << b << endl;
	widget->setItem(counts, 0, new QTableWidgetItem(a));
	widget->setItem(counts, 1, new QTableWidgetItem(b));
	widget->setItem(counts, 2, new QTableWidgetItem(c));
	widget->setItem(counts, 3, new QTableWidgetItem(e));
	widget->setItem(counts, 4, new QTableWidgetItem(f));
	/*qDebug() << paramHash.value("max") << endl;
	*/
}
////������Դ���ĵ���ת�Ƶ�excel��,����ʵ�ֽ�����������ΪQString�ͣ�Ȼ���������������excel
void qSlicerCalculusModuleWidget::saveClicked()
{
	Q_D(qSlicerCalculusModuleWidget);
	ExcelExportHelper excel;
	for (int i = 0; i < (d->tableblock->rowCount()); i++)
	{
		for (int j = 0; j < (d->tableblock->columnCount() ); j++)
		{
			QString str = d->tableblock->item((i ), (j ))->text();
			qDebug() << str << endl;
			excel.SetCellValue(i+1, j+1, str);
		}
	}
	//const QString fileName = "E:\\kaka14.xlsx";
	//excel.SaveAs(fileName);
}
////������Դ���ĵ���ת�Ƶ�excel��,����ʵ�ֽ�����������ΪQString�ͣ�Ȼ���������������excel
void qSlicerCalculusModuleWidget::saveClicked_2()
{
	Q_D(qSlicerCalculusModuleWidget);
	ExcelExportHelper excel;
	for (int i = 0; i < (d->tableblock_2->rowCount()); i++)
	{
		for (int j = 0; j < (d->tableblock_2->columnCount()); j++)
		{
			QString str = d->tableblock_2->item((i), (j))->text();
			qDebug() << str << endl;
			excel.SetCellValue(i+1, j+1+8, str);
		}
	}
	//const QString fileName = "E:\\kaka14.xlsx";
	//excel.SaveAs(fileName);
}
void qSlicerCalculusModuleWidget::clearButtonClicked()
{
	Q_D(qSlicerCalculusModuleWidget);
	int counts = d->tableblock->rowCount();
	for (int i = 0; i < counts; i++)
	{
		d->tableblock->removeRow(d->tableblock->rowCount()-1);
	}
}
void qSlicerCalculusModuleWidget::clearButtonClicked_2()
{
	Q_D(qSlicerCalculusModuleWidget);
	int counts = d->tableblock_2->rowCount();
	for (int i = 0; i < counts; i++)
	{
		d->tableblock_2->removeRow(d->tableblock_2->rowCount()-1);
	}
}
ExcelExportHelper::ExcelExportHelper(bool closeExcelOnExit)
{
	m_closeExcelOnExit = closeExcelOnExit;
	m_excelApplication = nullptr;
	m_sheet = nullptr;
	m_sheets = nullptr;
	m_workbook = nullptr;
	m_workbooks = nullptr;
	m_excelApplication = nullptr;

	m_excelApplication = new QAxObject("Excel.Application", 0);

	if (m_excelApplication == nullptr)
		throw invalid_argument("Failed to initialize interop with Excel (probably Excel is not installed)");

	m_excelApplication->dynamicCall("SetVisible(bool)", false); // hide excel
	m_excelApplication->setProperty("DisplayAlerts", 1); // disable alerts

	m_workbooks = m_excelApplication->querySubObject("Workbooks");
	m_workbook = m_workbooks->querySubObject("Add");
	m_sheets = m_workbook->querySubObject("Worksheets");
	m_sheet = m_sheets->querySubObject("Add");
}

void ExcelExportHelper::SetCellValue(int lineIndex, int columnIndex, const QString& value)//�˴������ǽ�����д��excel����е�
{
	QAxObject *cell = m_sheet->querySubObject("Cells(int,int)", lineIndex, columnIndex);
	cell->setProperty("Value", value);//�������е���һ�����ͳ�����
	delete cell;
}
ExcelExportHelper::~ExcelExportHelper()
{
	if (m_excelApplication != nullptr)
	{
		if (!m_closeExcelOnExit)
		{
			m_excelApplication->setProperty("DisplayAlerts", 1);
			m_excelApplication->dynamicCall("SetVisible(bool)", true);
		}

		if (m_workbook != nullptr && m_closeExcelOnExit)
		{
			m_workbook->dynamicCall("Close (Boolean)", true);
			m_excelApplication->dynamicCall("Quit (void)");
		}
	}

	delete m_sheet;
	delete m_sheets;
	delete m_workbook;
	delete m_workbooks;
	delete m_excelApplication;
}
//----------------------��ӽ���---------------------