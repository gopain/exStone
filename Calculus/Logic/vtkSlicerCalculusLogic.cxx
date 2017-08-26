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

// Calculus Logic includes
#include "vtkSlicerCalculusLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkMatrix4x4.h>

// STD includes
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

//-------------
#include <vtkImageData.h>
#include <vtkIOStream.h>

#include <QDebug>
#include <QFile>
#include <QString>
#include <QHash>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerCalculusLogic);

//----------------------------------------------------------------------------
vtkSlicerCalculusLogic::vtkSlicerCalculusLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerCalculusLogic::~vtkSlicerCalculusLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerCalculusLogic::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}


double vtkSlicerCalculusLogic::s_sliceThick;//����Ƕ���Ƭ���
double vtkSlicerCalculusLogic::s_uWater;//ˮ��˥��ϵ��
double vtkSlicerCalculusLogic::s_materialThick;//���Ϻ��
//---------------------------------------------------------------------------
void vtkSlicerCalculusLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
	vtkNew<vtkIntArray> events;
	events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
	events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
	events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
	this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerCalculusLogic::RegisterNodes()
{
	assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerCalculusLogic::UpdateFromMRMLScene()
{
	assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerCalculusLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerCalculusLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//���õ�ǰ�����ݵ�logic
void vtkSlicerCalculusLogic::setVolumesLogic(vtkSlicerVolumesLogic* logic)
{
	this->volumesLogic = logic;
}
vtkSlicerVolumesLogic* vtkSlicerCalculusLogic::getVolumesLogic()
{
	return this->volumesLogic;
}
void vtkSlicerCalculusLogic::setCropVolumeLogic(vtkSlicerCropVolumeLogic* logic)
{
	this->cropVolumeLogic = logic;
}

vtkSlicerCropVolumeLogic* vtkSlicerCalculusLogic::getCropVolumeLogic()
{
	return this->cropVolumeLogic;
}



QHash<QString, double> vtkSlicerCalculusLogic::acqSliceData(vtkImageReslice* reslice, vtkMRMLSliceNode* sliceNode, vtkMRMLVolumeNode* volumeNode)
{



	vtkSmartPointer<vtkMatrix4x4> sliceToRAS = sliceNode->GetSliceToRAS();//sliceԭ��ͷ���������
	vtkSmartPointer<vtkMatrix4x4> RASToIJKMatrix = vtkSmartPointer<vtkMatrix4x4>::New();//4*4����
	vtkSmartPointer<vtkMatrix4x4> IJKToRASMatrix = vtkSmartPointer<vtkMatrix4x4>::New();//ת������
	volumeNode->GetRASToIJKMatrix(RASToIJKMatrix);
	volumeNode->GetIJKToRASMatrix(IJKToRASMatrix);

	double normal[4];//������
	normal[0] = sliceToRAS->GetElement(0, 2);
	normal[1] = sliceToRAS->GetElement(1, 2);
	normal[2] = sliceToRAS->GetElement(2, 2);
	normal[3] = 1;

	//qDebug() << "normal:" << normal[0] << " " << normal[1] << " " << normal[2] << " " << normal[3];


	double origin[4];//������
	origin[0] = sliceToRAS->GetElement(0, 3);
	origin[1] = sliceToRAS->GetElement(1, 3);
	origin[2] = sliceToRAS->GetElement(2, 3);
	origin[3] = 1;

	//qDebug() << "origin:" << origin[0] << " " << origin[1] << " " << origin[2] << " " << origin[3];

	//����ƽ�淽��

	double A = normal[0];
	double B = normal[1];
	double C = normal[2];
	double d = -(A * origin[0] + B * origin[1] + C * origin[2]);//ƽ�淽��D


	//�õ�����>0��IJK����
	//vtkImageData* orgimage = reslice->GetImageDataInput(0);
	vtkImageData* orgimage = volumeNode->GetImageData();

	//�ߴ糤���
	int* dims = orgimage->GetDimensions();
	QList<double*> pointList;//��ʯ����Ч��


	int row = dims[1];
	int column = dims[0];
	int li = dims[2];

	qint16* pixel = new qint16[row*column *li]();
	qint16* q = pixel;
	int t = 0;
	QFile m_file("before.txt");

	if (!m_file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "Failed,acqSliceData::create file before. ";
	}
	for (int k = 0; k < li; k++)
	{
		
		for (int j = 0; j < row; j++)
		{
			for (int i = 0; i < column; i++)
			{

				qint16* p = (qint16*)(orgimage->GetScalarPointer(i, j, k));
				q[i + j*column + k*row*column] = *p;
				if (*p >0)
				{

					//std::cout << " p[" << i << "][" << j << "]" << "[" << k << "]" << *p << "  t=" << t << std::endl;
					t++;
					double*point = new double[5];
					point[0] = j;
					point[1] = i;
					point[2] = k;
					point[3] = 1;
					point[4] = *p;

					pointList.append(point);


					QTextStream out(&m_file);
					QString value = "";
					out << " p[" << i << "][" << j << "]" << "[" << k << "]" << *p << "  t=" << t <<"\n";
				}



			}

		}

	}



	//�����е�ת����RAS����

	QList<double*> rasPointList;
	//ɸѡһ�������㵽��ľ���<2
	QList<double*> resultPointList;
	
	for (int m1 = 0; m1 < pointList.count(); m1++)
	{
		double*point = pointList.at(m1);
		double* rasPoint = IJKToRASMatrix->MultiplyDoublePoint(point);
		double distance = qAbs(A*rasPoint[0] + B*rasPoint[1] + C*rasPoint[2] + d) / sqrt(A*A + B*B + C*C);
		double dis = vtkSlicerCalculusLogic::s_sliceThick/2;//һ����
		if (distance < dis)//distance�б仯����������ֵ����������仯
		{
			//qDebug() << "m1:" << m1 << " ijkPoint :" << point[0] << " " << point[1] << " " << point[2] << " " << point[3] <<  ""<< point[4];
			//qDebug() << "m1:" << m1 << " rasPoint :" << rasPoint[0] << " " << rasPoint[1] << " " << rasPoint[2] << " " << rasPoint[3];//IJKת����RAS�������б仯
			double* resultPoint = RASToIJKMatrix->MultiplyDoublePoint(rasPoint);
			//qDebug() << "distance" << distance << " resultPointijk:" << resultPoint[0] << " " << resultPoint[1] << " " << resultPoint[2] << " " << resultPoint[3] <<" " << resultPoint[4];
			double* result = new double[5]();
			result[0] = resultPoint[0];
			result[1] = resultPoint[1];
			result[2] = resultPoint[2];
			result[3] = resultPoint[3];
			result[4] = point[4];
			resultPointList.append(result);
			/*qDebug() << "normal:" << normal[0] << " " << normal[1] << " " << normal[2] << " " << normal[3];
			qDebug() << "origin:" << origin[0] << " " << origin[1] << " " << origin[2] << " " << origin[3];
			qDebug() << "distance:" << distance << "RAS:" << rasPoint[0] << " " << rasPoint[1] << " " << rasPoint[2] << "A:" << A << "B:" << B << "C:" << C << "d:" << d << "dis:" << dis;
			qDebug() << " resultPointijk:" << result[0] << " " << result[1] << " " << result[2] << " " << result[3] << " " << result[4];*/

		}
		rasPointList.append(rasPoint);
	}
	//qDebug() << "rasPointList size" << rasPointList.size() << " resultPointList size:" << resultPointList.size()<<endl;


	double* sliceDataDouble;
	QHash<QString, double> circleParamsHash;
	if (resultPointList.size() > 0)
	{
		sliceDataDouble = new double[resultPointList.size()]();
		for (int i = 0; i < resultPointList.size(); i++)
		{
			double* point = resultPointList.at(i);
			sliceDataDouble[i] = point[4];//�õ�CTֵ
		}
		circleParamsHash.insert("max", max(sliceDataDouble, resultPointList.size()));
		circleParamsHash.insert("min", min(sliceDataDouble, resultPointList.size()));
		circleParamsHash.insert("average", aver(sliceDataDouble, resultPointList.size()));
		circleParamsHash.insert("AOD", AOD(sliceDataDouble, resultPointList.size(), vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
		circleParamsHash.insert("IOD", IOD(sliceDataDouble, resultPointList.size(), vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
	
		if (sliceDataDouble!=0)
			delete[] sliceDataDouble;
	}

	
	
	if (pixel!=0)
		delete[] pixel;
	//�ͷ����е�
	for (int index = 0; index < pointList.size(); index++)
	{
		double* p = pointList.at(index);
		delete[] p;
	}
	qDebug() << "resultPointList.size():" << resultPointList.size();
	//�ͷ����е�
	for (int index = 0; index < resultPointList.size(); index++)
	{
		double* result = resultPointList.at(index);
		//qDebug() << "index : " <<index <<" "<< result[0] << " " << result[1] << " " << result[2] << " " << result[3] << " " << result[4];
		double* p = resultPointList.at(index);
		delete[] p;
	}

	qDebug() << "m_stoneParamsHash:" << " average=" << circleParamsHash.value("average") << " AOD=" << circleParamsHash.value("AOD") << " IOD=" << circleParamsHash.value("IOD");
	//std::cout << "sliceToRAS:" << std::endl;
	//for (int i = 0; i < 4; i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//	{
	//		std::cout << " [" << i << "][" << j << "] " << sliceToRAS->GetElement(i, j);
	//	}
	//	std::cout << std::endl;
	//}

	////reslice->SetResliceAxes(resliceAxes);
	//std::cout << "RAS to IJK:" << std::endl;
	//for (int i = 0; i < 4; i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//	{
	//		std::cout << " [" << i << "][" << j << "] " << RASToIJKMatrix->GetElement(i, j);
	//	}
	//	std::cout << std::endl;
	//}

	//std::cout << "IJK to RAS:" << std::endl;
	//for (int i = 0; i < 4; i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//	{
	//		std::cout << " [" << i << "][" << j << "] " << IJKToRASMatrix->GetElement(i, j);
	//	}
	//	std::cout << std::endl;
	//}




	return circleParamsHash;


}


//--------------��ɺɺ��ӳ���----------------------------------
double vtkSlicerCalculusLogic::max(double a[], int n)
{
	double  max;
	max = a[0];
	int i;
	for (i = 1; i<n; i++)
	{
		if (a[i]>max) max = a[i];
	}
	//cout << max << endl;
	return max;
}
double vtkSlicerCalculusLogic::min(double a[], int n)
{
	double min;
	min = a[0];
	int i;
	for (i = 1; i < n; i++)
	{
		if (a[i] < min) min = a[i];
	}
	//cout << min << endl;
	return  min;
}

double vtkSlicerCalculusLogic::aver(double a[], int n)
{
	double r = 0;
	int i;
	for (i = 0; i < n; i++)
		r += a[i];
	r /= (double)n;
	//cout <<"aver:"<< setprecision(10) << r << endl;
	return r;
}
//--------------------------------------------------------------
//��ȡԲ���������� 
QHash<QString, double> vtkSlicerCalculusLogic::aqcCircleData(vtkMRMLVolumeNode* input)//vtkMRMLScalarVolumeNode,����ֵ��ָ��ĺ���
{

	vtkSmartPointer<vtkMatrix4x4> RASToIJKMatrix = vtkSmartPointer<vtkMatrix4x4>::New();//4*4����
	input->GetRASToIJKMatrix(RASToIJKMatrix);//����ϵת��


	vtkImageData* outputImageData = input->GetImageData();//�õ����������ݵ�Imagedata
	int* dims = outputImageData->GetDimensions();//ԭʼͼ��ά��
	double origin[3];
	outputImageData->GetOrigin(origin);
	//qDebug() << "o" << origin[0] << " " << origin[1] << " " << origin[2];

	double spaceing[3];
	outputImageData->GetSpacing(spaceing);

	int row = dims[1];
	int column = dims[0];
	int li = dims[2];
	QList<double*> pointList;//��ʯ����Ч��

	for (int k = 0; k < li; k++)
	{
	
		for (int j = 0; j < row; j++)
		{
			for (int i = 0; i < column; i++)
			{

				qint16* p = (qint16*)(outputImageData->GetScalarPointer(i, j, k));
				if (*p >0)
				{


					double*point = new double[5];
					point[0] = j;
					point[1] = i;
					point[2] = k;
					point[3] = 1;
					point[4] = *p;

					pointList.append(point);
				}



			}

		}

	}

	double* sliceDataDouble;
	QHash<QString, double> circleParamsHash;
	if (pointList.size() > 0)
	{
		sliceDataDouble = new double[pointList.size()]();
		for (int i = 0; i < pointList.size(); i++)
		{
			double* point = pointList.at(i);
			sliceDataDouble[i] = point[4];//�õ�CTֵ
		}
		circleParamsHash.insert("max", max(sliceDataDouble, pointList.size()));
		circleParamsHash.insert("min", min(sliceDataDouble, pointList.size()));
		circleParamsHash.insert("average", aver(sliceDataDouble, pointList.size()));
		circleParamsHash.insert("AOD", AOD(sliceDataDouble, pointList.size(), vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
		circleParamsHash.insert("IOD", IOD(sliceDataDouble, pointList.size(), vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));

		if (sliceDataDouble != 0)
			delete[] sliceDataDouble;
	}

	return circleParamsHash;

}

//---------------��ӽ���----------------------------------
double vtkSlicerCalculusLogic::AOD(double a[], int n, double m, double d)//n ���ظ��� ,m��ˮ��˥��ϵ��,d���Ϻ��(ÿ�����ص�ĺ��)��ÿ��OD
{
	double r = 0;
	for (int i = 0; i < n; i++)
	{
		r = ((a[i] / 1000)*m + m)*d;
		a[i] = log10(exp(r));//ÿһ��ָ��
	}
	//cout << "AOD:" << setprecision(10) << aver(a, n) << endl;
	return aver(a, n);
}
/*
OD = lg e (CT/1000*uˮ + uˮ)*x
*/
double vtkSlicerCalculusLogic::IOD(double a[], int n, double m, double d)
{
	double r = 0;
	double sum = 0;
	for (int i = 0; i < n; i++)
	{
		r = ((a[i] / 1000)*m + m)*d;
		a[i] = log10(exp(r));//ÿһ��ָ��
		sum += a[i];//���
	}
	//cout <<"IOD:"<<setprecision(10) << sum << endl;
	return sum;
}

//-----------------------------------------------------------------------
//��ȡ��ֱ�����slice����
QHash<QString, double> vtkSlicerCalculusLogic::acqSliceVerticalData(vtkMRMLVolumeNode* input,double offset,QString direction)
{
	//�õ�����>0��IJK����
	vtkImageData* orgimage = input->GetImageData();


	
	//�ߴ糤���
	int* dims = orgimage->GetDimensions();
	QList<double*> pointList;//��ʯ����Ч��


	int row = dims[1];
	int column = dims[0];
	int li = dims[2];




	vtkSmartPointer<vtkMatrix4x4> RASToIJKMatrix = vtkSmartPointer<vtkMatrix4x4>::New();//4*4����
	input->GetRASToIJKMatrix(RASToIJKMatrix);

	double* rasPoint = new double[4]();
	memset(rasPoint,0,4);
	if (direction=="X")
		rasPoint[0] = offset;
	if (direction == "Y")
		rasPoint[1] = offset;
	if (direction == "Z")
		rasPoint[2] = offset;
	rasPoint[3] = 1;

	double* ijkPoint = RASToIJKMatrix->MultiplyDoublePoint(rasPoint);
	int intOffset;
	if (direction == "X")
		intOffset= ijkPoint[0];
	if (direction == "Y")
		intOffset = ijkPoint[1];
	if (direction == "Z")
		intOffset = ijkPoint[2];
	qint16* pixel = new qint16[row*column *li]();
	qint16* q = pixel;
	int index = 0;//��Ч����
	int t = 0;
	for (int k = 0; k < li; k++)
	{
		
		for (int j = 0; j < row; j++)
		{
			for (int i = 0; i < column; i++)
			{

				qint16* p = (qint16*)(orgimage->GetScalarPointer(i, j, k));
				q[i + j*column + k*row*column] = *p;
				if (*p >0)
				{

					
					if (direction == "X" || direction == "x")
					{
						index = i;
					}
					else if (direction == "Y" || direction == "y")
					{
						index = j;
					}
					else if (direction == "Z" || direction == "z")
					{
						index = k;
					}

					if (index == intOffset)
					{
						double*point = new double[5];
						point[0] = j;
						point[1] = i;
						point[2] = k;
						point[3] = 1;
						point[4] = *p;

						pointList.append(point);
						t++;
						std::cout << "t=" << t<< " p[" << i << "][" << j << "]" << "[" << k << "]" << *p << std::endl;
					}
				}



			}

		}

	}
	double* sliceDataDouble;
	QHash<QString, double> circleParamsHash;
	if (pointList.size() > 0)
	{
		sliceDataDouble = new double[pointList.size()]();
		for (int i = 0; i < pointList.size(); i++)
		{
			double* point = pointList.at(i);
			sliceDataDouble[i] = point[4];//�õ�CTֵ
		}

		circleParamsHash.insert("max", max(sliceDataDouble, pointList.size()));
		circleParamsHash.insert("min", min(sliceDataDouble, pointList.size()));
		circleParamsHash.insert("average", aver(sliceDataDouble, pointList.size()));
		circleParamsHash.insert("AOD", AOD(sliceDataDouble, pointList.size(), vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
		circleParamsHash.insert("IOD", IOD(sliceDataDouble, pointList.size(), vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
		if (sliceDataDouble!=0)
			delete[] sliceDataDouble;

	}

	if (pixel != 0)
		delete[] pixel;
	//�ͷ����е�
	for (int index = 0; index < pointList.size(); index++)
	{
		double* p = pointList.at(index);
		delete[] p;
	}
	

	return circleParamsHash;
}
