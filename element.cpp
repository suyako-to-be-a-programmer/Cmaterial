void CElement::SetEleMass()
{
	double density;
	if(m_iType==0 || m_iType==2 || m_iType==3 || m_iType==7 || m_iType==8 || m_iType==11 || m_iType==12)   //成员变量可以直接使用  
	{
		density= GetModel()->m_pMaterial->GetMortarDensity();
	}
	else if(m_iType==1)
	{
		density=GetModel()->m_pMaterial->GetAggDensity();
	}
	else
	{
		density = 0.0; 
	}
	m_dEleMass =m_dEleVolume* density;    //!!! 注意此处是体积乘密度，与二维程序不同
}

void CElement::DeltaCalcu(const C3DVec ResF,const C3DVec ResM)    //注意真实的弹簧数量不等于弹簧点数
{
	int NumFault = 0;                                             //已破坏的弹簧数
	double Vx=0.0,Vy=0.0,Vz=0.0,Wx=0.0,Wy=0.0,Wz=0.0;                                  //三个方向的速度
	double DK=1.0;                                               //根据破坏弹簧的个数，对阻尼的调整系数	
	for(int i=0;i<m_pSprList->GetSize();i++)//确定失效的弹簧个数  m_nNumSpr=>m_pSprList->GetSize()
	{
		if(!(m_pSprList->GetAt(i)->GetState()))
		{
			NumFault++;
		}
	}

	//	DK=DK*(1.-(NumFault/double(m_pSprList->GetSize())));  
	ASSERT(DK>=0.);                                             //!!!!!
	ASSERT((2*m_dEleMass+m_dIncre*m_dDampM)!=0.);
	ASSERT((2*m_dIx+m_dIncre*m_dDampIx)!=0.);
	ASSERT((2*m_dIy+m_dIncre*m_dDampIy)!=0.);
	ASSERT((2*m_dIz+m_dIncre*m_dDampIz)!=0.);

	ASSERT(2*m_dEleMass+m_dIncre*m_dDampM*DK>=1E-10);           //防止除零

	ASSERT(2*m_dIx+m_dIncre*m_dDampIx*30000*DK>=1E-10);
	ASSERT(2*m_dIy+m_dIncre*m_dDampIy*30000*DK>=1E-10);
	ASSERT(2*m_dIz+m_dIncre*m_dDampIz*30000*DK>=1E-10);

//中心差分法的应用，注意与“王泳嘉”小书上的对比，下面的 m_dDampM = 质量*质量阻尼比例系数
	if (NumFault == m_pSprList->GetSize())   //弹簧都坏，单元坏后速度归零，使其不乱飞
	{
		Vx=0.0;
		Vy=0.0;
		Vz=0.0;
		Wx=0.0;
		Wy=0.0;
		Wz=0.0;
	} 
	else 
	{
		if(m_iType==0 || m_iType==2 || m_iType==3 || m_iType==7 || m_iType==8 || m_iType==11 || m_iType==12)   //砂浆单元
		{
			Vx=((ResF.m_dX)*(2*m_dIncre)+(m_VPre.m_dX)*(2*m_dEleMass-m_dIncre*m_dDampM*DK))/(2*m_dEleMass+m_dIncre*m_dDampM*DK); //t+△t/2时刻的速度，由中心差分法求得
			Vy=((ResF.m_dY)*(2*m_dIncre)+(m_VPre.m_dY)*(2*m_dEleMass-m_dIncre*m_dDampM*DK))/(2*m_dEleMass+m_dIncre*m_dDampM*DK);
			Vz=((ResF.m_dZ)*(2*m_dIncre)+(m_VPre.m_dZ)*(2*m_dEleMass-m_dIncre*m_dDampM*DK))/(2*m_dEleMass+m_dIncre*m_dDampM*DK);

            Wx=((ResM.m_dX)*(2*m_dIncre)+(m_WPre.m_dX)*(2*m_dIx-m_dIncre*m_dDampIx*30000*DK))/(2*m_dIx+m_dIncre*m_dDampIx*30000*DK);
			Wy=((ResM.m_dY)*(2*m_dIncre)+(m_WPre.m_dY)*(2*m_dIy-m_dIncre*m_dDampIy*30000*DK))/(2*m_dIy+m_dIncre*m_dDampIy*30000*DK);
			Wz=((ResM.m_dZ)*(2*m_dIncre)+(m_WPre.m_dZ)*(2*m_dIz-m_dIncre*m_dDampIz*30000*DK))/(2*m_dIz+m_dIncre*m_dDampIz*30000*DK); 
			                                                              //30000与下面的400都是阻尼调整的参数 //注意参数取值是否合适
		}
		else if(m_iType==1)                          //骨料单元，由于其质量是砂浆单元多倍，因此考虑不同的质量阻尼系数
		{
			Vx=((ResF.m_dX)*(2*m_dIncre)+(m_VPre.m_dX)*(2*m_dEleMass-m_dIncre*m_dDampM*400*DK))/(2*m_dEleMass+m_dIncre*m_dDampM*400*DK);
			Vy=((ResF.m_dY)*(2*m_dIncre)+(m_VPre.m_dY)*(2*m_dEleMass-m_dIncre*m_dDampM*400*DK))/(2*m_dEleMass+m_dIncre*m_dDampM*400*DK);
			Vz=((ResF.m_dZ)*(2*m_dIncre)+(m_VPre.m_dZ)*(2*m_dEleMass-m_dIncre*m_dDampM*400*DK))/(2*m_dEleMass+m_dIncre*m_dDampM*400*DK);
            
			Wx=((ResM.m_dX)*(2*m_dIncre)+(m_WPre.m_dX)*(2*m_dIx-m_dIncre*m_dDampIx*400*DK))/(2*m_dIx+m_dIncre*m_dDampIx*400*DK);
			Wy=((ResM.m_dY)*(2*m_dIncre)+(m_WPre.m_dY)*(2*m_dIy-m_dIncre*m_dDampIy*400*DK))/(2*m_dIy+m_dIncre*m_dDampIy*400*DK);
			Wz=((ResM.m_dZ)*(2*m_dIncre)+(m_WPre.m_dZ)*(2*m_dIz-m_dIncre*m_dDampIz*400*DK))/(2*m_dIz+m_dIncre*m_dDampIz*400*DK);
		}
		else      //补充m_iType = 4/5时
		{			
			int a = 2;
		}
	}

	C3DVec VNex(Vx,Vy,Vz);
	C3DVec WNex(Wx,Wy,Wz);

	m_DDelta = VNex*m_dIncre;
	m_TDelta = WNex*m_dIncre;

	ASSERT(m_dIncre!=0.);   //!!!!!!!!!!!! 
	C3DVec DAcc = (VNex-m_VPre)/m_dIncre;
	C3DVec TAcc = (WNex-m_WPre)/m_dIncre;

	//本时步不平衡力和力矩 //Doc中进行循环和不平衡力的迭代中，利用到了GetErrFX()
	if (m_iType==0 || m_iType==2 || m_iType==3 || m_iType==7 || m_iType==8 || m_iType==11 || m_iType==12)
	{
		m_dErrFX=fabs(m_dEleMass*(DAcc.m_dX))-fabs((ResF.m_dX)-(m_dDampM*DK*(((VNex+m_VPre)/2).m_dX)));
		m_dErrFY=fabs(m_dEleMass*(DAcc.m_dY))-fabs((ResF.m_dY)-(m_dDampM*DK*(((VNex+m_VPre)/2).m_dY)));
		m_dErrFZ=fabs(m_dEleMass*(DAcc.m_dZ))-fabs((ResF.m_dZ)-(m_dDampM*DK*(((VNex+m_VPre)/2).m_dZ)));

		m_dErrM.m_dX=fabs(m_dEleMass*(TAcc.m_dX))-fabs((ResM.m_dX)-(m_dDampIx*DK*30000*(((WNex+m_WPre)/2).m_dX)));
		m_dErrM.m_dY=fabs(m_dEleMass*(TAcc.m_dY))-fabs((ResM.m_dY)-(m_dDampIy*DK*30000*(((WNex+m_WPre)/2).m_dY)));
		m_dErrM.m_dZ=fabs(m_dEleMass*(TAcc.m_dZ))-fabs((ResM.m_dZ)-(m_dDampIz*DK*30000*(((WNex+m_WPre)/2).m_dZ)));
	} 
	else if (m_iType==1)
	{
		m_dErrFX=fabs(m_dEleMass*(DAcc.m_dX))-fabs((ResF.m_dX)-(m_dDampM*DK*400*(((VNex+m_VPre)/2).m_dX)));
		m_dErrFY=fabs(m_dEleMass*(DAcc.m_dY))-fabs((ResF.m_dY)-(m_dDampM*DK*400*(((VNex+m_VPre)/2).m_dY)));
		m_dErrFZ=fabs(m_dEleMass*(DAcc.m_dZ))-fabs((ResF.m_dZ)-(m_dDampM*DK*400*(((VNex+m_VPre)/2).m_dZ)));

		m_dErrM.m_dX=fabs(m_dEleMass*(TAcc.m_dX))-fabs((ResM.m_dX)-(m_dDampIx*DK*400*(((WNex+m_WPre)/2).m_dX)));
		m_dErrM.m_dY=fabs(m_dEleMass*(TAcc.m_dY))-fabs((ResM.m_dY)-(m_dDampIy*DK*400*(((WNex+m_WPre)/2).m_dY)));
		m_dErrM.m_dZ=fabs(m_dEleMass*(TAcc.m_dZ))-fabs((ResM.m_dZ)-(m_dDampIz*DK*400*(((WNex+m_WPre)/2).m_dZ)));
	}

	m_dErrM = C3DVec(m_dErrM.m_dX,m_dErrM.m_dY,m_dErrM.m_dZ);
	m_VPre = VNex;
	m_WPre = WNex;
	if((m_DDelta.GetLength()>=0.005)||(m_TDelta.GetLength()>=0.0005))    //注意参数取值是否合适
	{
		GetModel()->bToRemesh=true;	
	}
}
