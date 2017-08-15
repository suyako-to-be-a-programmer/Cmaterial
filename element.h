#pragma once


#include <afxtempl.h>
#include "3DVec.h"
#include "..\inc\Math\2DVec.h"
#include "Matrix.h"
#include "Face.h"

class CBox;
class CBoxList;
class CSprList;
class CVertList;
class CSideList;
class CSprNoList;
class CConList;

// CElement command target

class CElement : public CObject
{
	DECLARE_SERIAL(CElement)
private:
	int m_iNum;         //单元编号
	int m_iType;        //单元类型,0表示砂浆单元,1为骨料单元,2为上加载板周边砂浆单元，3为下加载板周边砂浆单元，4为上加载板，5为下加载板 ,7为左加载板周边砂浆单元，8为右加载板周边砂浆单元，9为左侧加载板，10为右侧加载板！！
	                    //如果考虑骨料可以发生破坏，骨料与砂浆连接界面上的单元为 6,实际操作现在改为0为砂浆单元，1为骨料单元； 11为前加载板单元周边砂浆单元，12为后加载板单元周边砂浆单元；13为前加载板，14为后加载板。

	int m_nNumVert;     //单元顶点数
	int m_nNumFace;     //面数
	int m_nNumBox;      //单元包含的盒子//与本单元相交的检索盒子数!
	int m_nNumSpr;      //弹簧数
	int	m_nNumSprNo;    //弹簧点数等于弹簧数  //不包括边界弹簧
	int m_nNumNeighbor; //可能接触的个数(即位于同一格子内的别的单元的个数)
	int m_nNumCon;      //真实接触个数

	//////////////////////三维中标定单元所在的标距轴号比二维复杂，是与三个坐标面平行的坐标（2014.6.11 Juny,可能还需要修改）
	int m_iYAxes;        //单元所在的求应变的标距轴号 (与Y坐标面（上下两个面）平行的标距，与y坐标轴垂直的)
	int m_iXAxes;       // (与x坐标面（左右两个面）平行的标距，与x坐标轴垂直的)
	int m_iZAxes;        // (与z坐标面（前后两个面）平行的标距，与z坐标轴垂直的)

	C3DVec m_OriTriVer;    //单元对应的初始vertex(母点)
	C3DVec m_OriCentr;     //单元初始质心
	C3DVec m_Centr;        //单元质心位置	

	CVertList * m_pVertList;         //本单元顶点链表
	CVertList * m_pLocalVertList;    //本单元顶点链表(局部坐标系下的，在doc中赋了初值后不再变化)
	CFaceList * m_pFaceList;         //本单元面链表

	CSprNoList * m_pSprNoList;       //本单元弹簧点链表
	CSprNoList * m_pLocalSprNoList;  //弹簧点局部坐标
	CSprList * m_pSprList;           //本单元弹簧链表
	CConList * m_pConList;           // 本单元接触链表（此处为可能接触）
	CBoxList * m_pBoxList;           //与本单元相交的检索盒子链表

	CSprNoList * m_pOriSprNoList;    //初始弹簧点链表，输入荷载时用
	CVertList  * m_pOriVertList;     //初始顶点链表，输入荷载时用

	C3DVecList * m_pSprStateList;    // 弹簧编号以及弹簧状态的等效数组，后处理时用到(Juny->)此处弹簧编号、弹簧状态只有两个元素，而C3DVec的构造函数有三个元素，因此，最后一个元素用0.0代替，没有具体意义
	C3DVecList *m_pSprStressStateList;   //弹簧编号以及弹簧应力状态的等效数组，后处理时用到
	C3DVecList * m_pSprFailModeList; // 弹簧破坏模式的显示

	double m_dEleVolume;               //单元体积
	double m_dEleMass;               //单元质量

	double m_dEleStrength;           //单元强度，初步考虑通过其各个连接弹簧的强度的平均值获得，如果不理想则分开考虑每跟弹簧显示不均匀性
	int m_iEleStrengthLevel;         //单元强度等级，用于显示强度灰度图像

	C3DVec m_vecEleStress;           //单元应力
	int m_iEleStressLevel;
	int m_iEleForceLevel;

	double m_dErrFX;    //本时步不平衡力和力矩
	double m_dErrFY;
	double m_dErrFZ;
	C3DVec m_dErrM;	

	double m_dIx;       //转动惯量
	double m_dIy;
	double m_dIz;
	
	double m_dDampM;    //粘性阻尼系数（块体质心运动方程中的α，与质量成比例的粘性阻尼系数）
	double m_dDampIx;
	double m_dDampIy;
	double m_dDampIz;

	double m_dIncre;    //时步

	C3DVec m_VPre;   //前半时刻线速度，v(t-Δt/2)
	C3DVec m_WPre;   //前半时刻角速度，ω(t-Δt/2)

	C3DVec m_DDelta; //线位移增量    
	C3DVec m_TDelta; //角位移增量     合力矩顺时针为正，逆时针为负 

	C3DVec m_ExterF;//外力（即输入地震波，不包括重力！）
	C3DVec m_ResF;//合力
	C3DVec m_ResM;//合力矩


	BOOL m_bHaveUpdate; //标识本单元是否已经更新，初值为true!
	BOOL m_bBearW;      //标识本单元是否受重力作用，初值为false!

	CMatrix m_TransM; //转换矩阵

public:
	void Serialize(CArchive &ar);
	CElement();
	CElement(int iNum, C3DVec OriTriVer);         //单元构造函数，单元编号+初始母点
	virtual ~CElement();

	void SetEleNum(int iNum);
	int GetEleNum();             //获得单元编号
	C3DVec GetOriTriVer();       //单元包含的初始母点

	void SetType(int Type);      //单元类型
	int GetType();
	int GetSprNum();

	void SetVertList(CVertList *pVertList);       //本单元顶点列表
	void SetOriVertList(CVertList *pVertList);

	CVertList * GetVertList();        //返回指向链表的指针（不能直接返回链表，因为没有拷贝构造函数，无法用已有同类对象初始化一个新对象！）
	CVertList * GetOriVertList();     //获得单元初始顶点坐标
	int GetNumVert();                 //  获得单元顶点数

	void SetLocalVertList(CVertList *pVertList);  //本单元顶点列表(局部坐标系下)
	CVertList * GetLocalVertList();               //返回指向链表

	void SetFaceList(CFaceList *pFaceList);       //本单元面列表
	CFaceList * GetFaceList();
	int GetNumFace(); 
	
	void SetSprNoList(CSprNoList *pSprNoList);       //本单元弹簧点列表
	CSprNoList * GetSprNoList();

	void SetLocalSprNoList(CSprNoList *pSprNoList);  //本单元弹簧点列表
	CSprNoList * GetLocalSprNoList();

	void SetSprList(CSprList *pSprList);             //本单元弹簧列表
	CSprList * GetSprList();//面数 

	void SetCenAndVolume(CFaceList *pFaceList, C3DVec OriTriVer);        //求单元初始质心和体积
	C3DVec GetCentr(); 
	C3DVec GetOriCentr();                            //单元初始形心
	double GetEleVolume();

	void SetYAxes(int YAxes);                          //单元所在轴 水平方向
	int GetYAxes(); 

	void SetXAxes(int XAxes);                         //单元所在轴
	int GetXAxes(); 

	void SetZAxes(int ZAxes);
	int GetZAxes();

	void AddBox(CBox *pBox);                         //本单元盒子列表
	void SetBoxList(CBoxList *pBoxList);
	CBoxList * GetBoxList();
	void BoxClear();
	void DeleteBox(int iNum);

	void ContactGen(CElement *pEle, double Kn, double Kt);  //本单元接触列表
	CConList * GetConList();
	void ContactClear();	

	void SpringGen(CElement *pEle, int Type);               //本单元弹簧生成
	void SpringClear();

	double GetErrFX();          //获得不平衡力
	double GetErrFY();
	double GetErrFZ();
	C3DVec GetErrM();

	int GetConNum();            //真实接触个数

	C3DVec GetWPre();           //前半时刻角速度
	C3DVec GetVPre();           //前半时刻线速度

	C3DVec GetTDelta();       //单元角位移增量
	C3DVec GetDDelta();       //单元线位移增量

	void SetBearWeight(BOOL bBearWeitht);  //标识本单元是否受重力作用,

	void SetTimeInterV(double TimeInterV);  //设置时步
	double GetTimeInterV();  

	void SetDamp(double Damp);	            //设置阻尼
	double GetDampM(); 
	double GetDampIz();
	double GetDampIy();
	double GetDampIx();

	void SetEleMass();
	double GetEleMass();                       //单元质量

	void SetEleStrength(CSprList *pSprList);   //单元强度
	double GetEleStrength();
	void SetEleStrengthLevel(int Leveln);      //单元强度等级
	int GetEleStrengthLevel();

	void SetEleForceLevel(int Leveln);

	void SetEleStress(CSprList *pSprList);     //单元应力
	C3DVec GetEleStress();
	void SetEleStressLevel(int Leveln);        //单元应力等级
	int GetEleStressLevel();

	void ForceInput(C3DVec *pVec);             //外荷载力输入
	void DisInput(C3DVec *pVec);               //位移加载方式输入
	void SetCentr(C3DVec *pCentr);             //根据计算更新坐标

	///////////////////////////////////////////以下注释段内函数仅仅声明，并未定义，待其他类定义完成后再定义2014.8.27
	void Calculate();					                   //单元总计算程序
	void EleResCalcu(C3DVec &ResF,C3DVec &ResM);           //计算单元合力、合力矩
	void DeltaCalcu(const C3DVec ResF,const C3DVec ResM);  //计算位移增量（默认为中心差分法）
	void EleLoose();                                       //根据计算的合力（合力矩），放松单元即坐标更新
	C3DVec GetResF();                                      //合力
	C3DVec GetResM();                                      //合力矩
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CMatrix GetTransMatrix();
	void SetTransMatrix(CMatrix &matrix);


	//后处理用到，弹簧状态以及弹簧破坏模式
	void SetSprStateList(C3DVecList *pSprStateList);
	C3DVecList * GetSprStateList();
	void SetSprFailModeList(C3DVecList *pSprFailModeList);
	C3DVecList * GetSprFailModeList();
	void SetSprStressStateList(C3DVecList *pSprStressStateList);
	C3DVecList * GetSprStressStateList();
};

typedef CTypedPtrArray<CObArray, CElement *> CEleListBase;   //单元列表

class CEleList: public CEleListBase
{
public:
	//	int DeepAppend(const CEleList& src);
	//	void DeepCopy(const CEleList& src);
	void DeepRemoveAll();
	//	void OutputToText(std::fstream &cout);
};
