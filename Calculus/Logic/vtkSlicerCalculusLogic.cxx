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
//Qt includes
#include<qdebug.h>
// Calculus Logic includes
#include "vtkSlicerCalculusLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>
//----------------��ɺɺ���---------------
#include "vtkSlicerCalculusLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkMatrix4x4.h>

#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLMarkupsNode.h"
#include "vtkSlicerMarkupsLogic.h"
// STD includes
#include <cassert>
//-------------
#include <vtkImageData.h>
#include <cassert>
#include <vector>
#include <vtkIOStream.h>
#include<sstream> 
#include<qstring.h>
#include <QDebug>
#include <fstream>

#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <QtGui>

#include <Qtcore/qstring.h>
#include <Qtcore/QFile>
#include <stdexcept>

#include <QHash>
#include "vtkSlicerCalculusLogic.h"
using namespace std;
class QAxObject;
//----------------------------��ӽ���----------------------------

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

QHash<QString, double> vtkSlicerCalculusLogic::acqSliceData(vtkImageReslice* reslice)
{
	//Set background box
	//Set pixels outside a box which is larger than the tightbox to be background
	MyBasic::Range3D bkgBox;
	MyBasic::Range3D imgBox;
	vtkImageData* orgimage = reslice->GetImageDataInput(0);
	//�ߴ糤���
	int* dims = orgimage->GetDimensions();

	m_gData.wholeRange.col = dims[0];
	m_gData.wholeRange.row = dims[1];
	m_gData.wholeRange.sli = dims[2];
	qDebug() << "image dims:" << dims[0] << " " << dims[1] << " " << dims[2] << endl;

	//ͼ��Χ
	int extent[6];
	orgimage->GetExtent(extent);
	qDebug() << "image extent:" << extent[0] << " " << extent[1] << " " << extent[2] << " " << extent[3] << " " << extent[4] << " " << extent[5];
	//ÿ�����ص�����RGB
	int numberOfScalarComponents = orgimage->GetNumberOfScalarComponents();
	qDebug() << "image numberOfScalarComponents:" << numberOfScalarComponents;
	//ͼ���׼��
	double origin[3];
	orgimage->GetOrigin(origin);

	qDebug() << "image origin:" << origin[0] << " " << origin[1] << "" << origin[2];
	//���ؼ��
	double spaceing[3];
	orgimage->GetSpacing(spaceing);
	qDebug() << "pixel space:" << spaceing[0] << " " << spaceing[1] << "" << spaceing[2];

	//
	////loadImage into gData
	//gData.loadImage(orgimage, imgBox);

	//Data3D<bool> mat_mask(gData.image.getSize(), true);

	//gData.seeds.set(gData.shifttightBox, UNKNOWN);


	int size = 0;



	int row = dims[1];
	int column = dims[0];
	int li = dims[2];

	uint16* pixel = new uint16[row*column *li]();
	uint16* q = pixel;
	int t = 0;
	QList<double> data;
	for (int k = 0; k < li; k++)
	{
		/*QString fileName = QString("before-%1").arg(k);
		QByteArray array = fileName.toLocal8Bit();
		FILE* file = fopen(array.data(), "wb");
		if (!file)
		{
			qDebug() << fileName << "�����ļ���ʧ�ܣ�\n";
		}*/
		for (int j = 0; j < row; j++)
		{
			for (int i = 0; i < column; i++)
			{

				uint16* p = (uint16*)(orgimage->GetScalarPointer(i, j, k));
				q[i + j*column +k*row*column] = *p;
				//q[i + j*m_gData.wholeRange.col + k*m_gData.wholeRange.sli] = *(p+1);
				//q[i + j*m_gData.wholeRange.col + k*m_gData.wholeRange.sli] = *(p+2);
				if (*p >0)
				{

					std::cout << " p[" << i << "][" << j << "]" << "[" << k << "]" << *p << "  t=" << t << std::endl;
					t++;
					data.append(*p);
				}



			}

		}
		/*fwrite(pixel, sizeof(uint16), column *row, file);
		fclose(file);*/
	}


	double* sliceDataDouble = new double[data.size()]();
	for (int i = 0; i < data.size(); i++)
	{
		sliceDataDouble[i] = data.at(i);
	}

	QHash<QString, double> circleParamsHash;
	circleParamsHash.insert("max", max(sliceDataDouble, data.size()));
	circleParamsHash.insert("min", min(sliceDataDouble, data.size()));
	circleParamsHash.insert("average", aver(sliceDataDouble, data.size()));


	delete[] sliceDataDouble;
	delete[] pixel;

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
	cout << max << endl;
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
	cout << min << endl;
	return  min;
}

double vtkSlicerCalculusLogic::aver(double a[], int n)
{
	double r = 0;
	int i;
	for (i = 0; i < n; i++)
		r += a[i];
	r /= (double)n;
	cout << r << endl;
	return r;
}
//--------------------
QHash<QString, double> vtkSlicerCalculusLogic::aqc(vtkMRMLVolumeNode* input, vtkMRMLMarkupsFiducialNode* markups)//vtkMRMLScalarVolumeNode,����ֵ��ָ��ĺ���
{
	/*vtkNew<vtkImageData> outputImageData;
	outputImageData->DeepCopy(input->GetImageData());*/
	/*int originalImageExtents[6];
	outputImageData->GetExtent(originalImageExtents);*/
	//��׼���������
	//vtkSmartPointer<vtkMatrix4x4> RASToIJKMatrix = vtkSmartPointer<vtkMatrix4x4>::New();//4*4����
	//input->GetRASToIJKMatrix(RASToIJKMatrix);//����ϵת��,�õ���������
	//int markupNum = markups->GetNumberOfFiducials();
	//qDebug() << markupNum << endl;
	//vector<float> position;  //temp variance to store points' IJK 3 coordinates�洢��ά����

	//vector<float> worldposition;  //temp variance to store points' World 3 coordinates

	//vector<vector <float> > points;  //vector to store sorted markup points in IJK coordinate

	//vector<vector <float> > worldpoints;
	//if (markupNum != 4)//
	//	qDebug() << "error No markupNum =4";
	//else
	//{
	//	for (int i = 0; i < markupNum; i++)
	//	{
	//		double pos[4];//�ĸ���
	//		markups->GetNthFiducialWorldCoordinates(i, pos);//�õ�ÿ���������
	//		cout << "WorldPOS:" << pos[0] << ";" << pos[1] << ";" << pos[2] << ";" << pos[3] << endl;//�ĸ��������
	//		float temp[4];
	//		std::copy(pos, pos + 4, temp);
	//		float* ijkpos = RASToIJKMatrix->MultiplyPoint(temp);
	//		cout << "IJKPOS:" << ijkpos[0] << ";" << ijkpos[1] << ";" << ijkpos[2] << ";" << ijkpos[3] << endl;//IJK����ϵ�µ�λ������
	//		for (int j = 0; j < 3; j++)
	//		{
	//			position.push_back(ijkpos[j]);
	//			worldposition.push_back(temp[j]);
	//		}
	//		if (i == 0)
	//		{
	//			points.push_back(position);
	//			worldpoints.push_back(worldposition);
	//		}
	//		else
	//		{
	//			int j;
	//			for (j = 0; j<points.size(); j++)
	//			{
	//				if (points.at(j).at(2)>position.at(2))
	//					break;
	//			}
	//			if (j == points.size())
	//			{
	//				points.push_back(position);
	//				worldpoints.push_back(worldposition);
	//			}
	//			else
	//			{
	//				points.insert(points.begin() + j, position);
	//				worldpoints.insert(worldpoints.begin() + j, worldposition);
	//			}
	//		}
	//		position.clear();
	//		worldposition.clear();
	//	}
	//}

	//vector<float> star_first = points.front();
	//vector<float> star_last = points.back();
	//vector<float> box1 = points.at(1);
	//vector<float> box2 = points.at(2);

	//cout << "POINTS[0]:" << star_first[0] << ";" << star_first[1] << ";" << star_first[2] << endl;
	//cout << "POINTS[1]:" << box1[0] << ";" << box1[1] << ";" << box1[2] << endl;
	//cout << "POINTS[2]:" << box2[0] << ";" << box2[1] << ";" << box2[2] << endl;
	//cout << "POINTS[3]:" << star_last[0] << ";" << star_last[1] << ";" << star_last[2] << endl;//points���ĸ���ǵ����꣬��ʸ��

	vtkImageData* outputImageData = input->GetImageData();//�õ����������ݵ�Imagedata
	int* dims = outputImageData->GetDimensions();//ԭʼͼ��ά��
	double origin[3];
	outputImageData->GetOrigin(origin);
	qDebug() << "o" << origin[0] << " " << origin[1] << " " << origin[2];

	//int dims[3]s;
	//outputImageData->GetDimensions(dims);
	//ExcelExportHelper excel;
	//	QString b;
	//pixel = (double*)malloc(sizeof(double));//
	int n = dims[0] * dims[1];
	double  *pixel;
	pixel = new double[n]();

	//double pixel[990] = { 0 };
	unsigned short *temp=0;
	double temp_value;



	FILE* outFile = fopen("after.txt", "wb");
	if (!outFile)
	{
		qDebug("outFile�����ļ���ʧ�ܣ�\n");
	}
	int size = 0;


	int z = dims[2] - 1;
	//for (int z = 1; z < dims[2]; z++)
	//{
	for (int y = 0; y < dims[1]; y++)
	{
		for (int x = 0; x < dims[0]; x++)
		{
			/*if (x < 33 && y < 30 && 2 < z < 4)*/
			/*	if (x < 33 && y < 30)
			{*/
			temp = (unsigned short *)(outputImageData->GetScalarPointer(x, y, z));

			temp_value = temp[0];
			if (temp_value < 60000)
			{

				pixel[x + dims[0] * y] = temp[0];//�и���������ת����unsigned char��double
			}
			else
				pixel[x + dims[0] * y] = 0;
			std::cout << pixel[x + dims[0] * y] << " ";

			//}
		}
		std::cout << std::endl;

	}
	/*	std::cout << std::endl;
	}*/

	//δ�ָ�ԭʼ����ƽ�������С
	double a = max(pixel, n);
	double b = min(pixel, n);
	//aver(pixel, n);
	//--------------------------ԭʼ���ݹ�һ��,ѹ����255��
	/*int n = dims[0] * dims[1];*/
	double  *graypixel;
	graypixel = new double[n]();
	for (int i = 0; i < dims[1]; i++)//��30
	{
		for (int j = 0; j < dims[0]; j++)//��33
		{
			graypixel[j + dims[0] * i] = ((pixel[j + dims[0] * i] - b) / (a - b)) * 255;
		}
	}
	//---------------------------------------

	//----------��ֵ�ָ����ָ��ǶԵ�----------------------------

	int nMaxIter = 100;
	int iDiffRec = 0;
	int F[256] = { 0 }; //ֱ��ͼ����  

	int iTotalGray = 0;//�Ҷ�ֵ��  
	int iTotalPixel = 0;//��������  

	int bt;//ĳ�������ֵ  

	double iThrehold, iNewThrehold;//��ֵ���·�ֵ  
	for (int j = 0; j < n; j++)
	{
		bt = (int)graypixel[j];
		F[bt]++;
	}//ͳ��ÿ���Ҷȼ��ĸ���

	double iMaxGrayValue;
	iMaxGrayValue = max(graypixel, n);
	double iMinGrayValue;
	iMinGrayValue = min(graypixel, n);

	//�����С�Ҷ�ֵ���Ѿ������max min
	//20 uchar iMaxGrayValue = 0, iMinGrayValue = 255;//ԭͼ���е����Ҷ�ֵ����С�Ҷ�ֵ  

	double iMeanGrayValue1, iMeanGrayValue2;//uchar��double����

	iThrehold = 0;//���ո�����ֵ
	iNewThrehold = (iMinGrayValue + iMaxGrayValue) / 2;//��ʼ��ֵ  
	iDiffRec = iMaxGrayValue - iMinGrayValue;

	for (int a = 0; (abs(iThrehold - iNewThrehold) > 0.5) && a < nMaxIter; a++)//������ֹ����  
	{
		iThrehold = iNewThrehold;
		//С�ڵ�ǰ��ֵ���ֵ�ƽ���Ҷ�ֵ  
		for (int i = iMinGrayValue; i < iThrehold; i++)
		{
			iTotalGray += F[i] * i;//F[]�洢ͼ����Ϣ  
			iTotalPixel += F[i];
		}
		iMeanGrayValue1 = (double)(iTotalGray / iTotalPixel);
		//���ڵ�ǰ��ֵ���ֵ�ƽ���Ҷ�ֵ   
		iTotalPixel = 0;
		iTotalGray = 0;
		for (int j = iThrehold + 1; j < iMaxGrayValue; j++)
		{
			iTotalGray += F[j] * j;//F[]�洢ͼ����Ϣ  
			iTotalPixel += F[j];
		}
		iMeanGrayValue2 = (double)(iTotalGray / iTotalPixel);

		iNewThrehold = (iMeanGrayValue2 + iMeanGrayValue1) / 2; //�·�ֵ  
		iDiffRec = abs(iMeanGrayValue2 - iMeanGrayValue1);
	}
	iThrehold = (iThrehold / 255)*(a - b) + b;
	cout << "The Threshold of this Image in imgIteration is:" << iThrehold << endl;
	//-------------------------

	for (int i = 0; i < dims[1]; i++)//��30
	{
		for (int j = 0; j < dims[0]; j++)//��33
		{
			if (pixel[j + dims[0] * i] < iThrehold)//���С����ֵ�����Ϊ0
			{
				double value1 = 0;
				fwrite(&value1, sizeof(double), 1, outFile);
				pixel[j + dims[0] * i] = 0;
			}
			else
			{
				double value = 0;
				value = ((pixel[j + dims[0] * i] - b) / (a - b)) * 255;
				fwrite(&value, sizeof(double), 1, outFile);
				//pixel[j + dims[0] * i] = value;

			}
		}

	}
	fclose(outFile);
	QHash<QString, double> circleParamsHash;
	circleParamsHash.insert("max", max(pixel, n));
	circleParamsHash.insert("min", min(pixel, n));
	circleParamsHash.insert("average", aver(pixel, n));

	qDebug() << circleParamsHash.value("max") << endl;



	delete[] pixel;
	delete[] graypixel;
	return circleParamsHash;
}
//---------------��ӽ���----------------------------------
