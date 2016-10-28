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

#ifndef __qSlicerCalculusModuleWidget_h
#define __qSlicerCalculusModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerCalculusModuleExport.h"
#include "Logic/common.h"
//---------------��ɺɺ���
//excel�й�ͷ�ļ�
#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <Qtcore/qstring.h>
//�봰���йص�ͷ�ļ�
#include <QDialog>
#include <QtCore/QVariant>
#include <qlabel.h>
#include <qpushbutton.h>
#include <QLayout>
#include <QtGui/QApplication>  
//��tableWidget�йص���
#include <QTableWidget>  
#include <QTableWidgetItem>
#include <QDialog>
#include <QAction>
#include <QApplication>
class QLabel;
class QPushButton;
//--------------------��ӽ���-----------------
class qSlicerCalculusModuleWidgetPrivate;
class vtkMRMLNode;

// CHEN
class vtkSlicerCalculusLogic;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_CALCULUS_EXPORT qSlicerCalculusModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerCalculusModuleWidget(QWidget *parent=0);
  virtual ~qSlicerCalculusModuleWidget();
  //��tableWidget�����һ�в���
  void addTableWidgetRow(QHash<QString, double> paramHash, QTableWidget* widget);
  //-----------------------------------------------------------------------------
  //��ȡ�������
  void getParamsFromUi();
signals:
  void clicked();

public slots:
//CHEN
/// Update the selection node from the combo box
void onInputVolumeMRMLNodeChanged();
void updateAcqStoneButtonState();

/// Display property button slots
///acquire the urinary calculi parameters
void onAcqStoneBtnClicked();

//��ʯ��ֱX�᷽��ɼ�����
void onX_VerticalAcqStoneBtnClicked();
//��ʯ��ֱY�᷽��ɼ�����
void onY_VerticalAcqStoneBtnClicked();
//��ʯ��ֱZ�᷽��ɼ�����
void onZ_VerticalAcqStoneBtnClicked();
/**
* @brief �����ر��¼�
* @author liuzhaobang
* @date 2016-10-14
*/
void onEndCloseEvent();
void addStoneParmsSlot(QHash<QString, double> hash);
//---------------��ɺɺ���
void InputVolumeMRMLNodeChanged();
void onMarkupsMRMLNodeChanged();
void updategenerateButtonState();
void saveClicked(); //������水ť�����������ź�
void saveClicked_2(); //������水ť�����������ź�
void generateClicked();//������ɰ�ť�����ĵ������ݵ���QTablewidget
void clearButtonClicked();//��ձ��
void clearButtonClicked_2();

//---------------��ӽ���
protected:
  QScopedPointer<qSlicerCalculusModuleWidgetPrivate> d_ptr;

  virtual void setup();
  virtual void setMRMLScene(vtkMRMLScene*);
  virtual void enter();
private:
  Q_DECLARE_PRIVATE(qSlicerCalculusModuleWidget);
  Q_DISABLE_COPY(qSlicerCalculusModuleWidget);
};

//---------------��ɺɺ���
class ExcelExportHelper
{
public:
	ExcelExportHelper(const ExcelExportHelper& other) = delete;
	ExcelExportHelper& operator=(const ExcelExportHelper& other) = delete;

	ExcelExportHelper(bool closeExcelOnExit = false);
	void SetCellValue(int lineIndex, int columnIndex, const QString& value);
	void SaveAs(const QString& fileName);

	~ExcelExportHelper();

private:
	QAxObject* m_excelApplication;
	QAxObject* m_workbooks;
	QAxObject* m_workbook;
	QAxObject* m_sheets;
	QAxObject* m_sheet;
	bool m_closeExcelOnExit;
};


//---------------��ӽ���
#endif
