



void init_CubeSmapRendering(LF_LIGHT* litPointLight_)
{
	proc_mode=PROC_LATE;
	
	var i;
	
	var vCubeSettings1[6]={180,90,0,270,90,90};
	var vCubeSettings2[6]={0,0,0,0,-90,90};
	
	//////////////////
	update_zbuffer(litPointLight_->iSMapResolution);
	
	BMAP*	bCubeMap = bmap_createblack(litPointLight_->iSMapResolution*6,litPointLight_->iSMapResolution,14444);
	bmap_to_cubemap(bCubeMap);

	litPointLight_->bShadowMap=bCubeMap;

	ent_setskin(litPointLight_->eLight,litPointLight_->bShadowMap,3);
	
	VIEW* vwShadowViews[6];
	
	for(i=0;i<6;i++)
	{
		vwShadowViews[i]=view_create(0);
		vwShadowViews[i]->skill_x=4;

		vwShadowViews[i]->arc = 90;
		vwShadowViews[i]->pan = vCubeSettings1[i];
		vwShadowViews[i]->tilt = vCubeSettings2[i];
		vwShadowViews[i]->roll = 0;

		vwShadowViews[i]->pos_x = litPointLight_->iSMapResolution*i;
		vwShadowViews[i]->size_x = litPointLight_->iSMapResolution;
		vwShadowViews[i]->size_y = litPointLight_->iSMapResolution;
		set(vwShadowViews[i],SHOW);	
		//		set(vwShadowViews[i],SHOW|NOFLAG1);	
	}
	//	wait(-8);
	
	for(i=0;i<6;i++)
	vwShadowViews[i]->bmap = litPointLight_->bShadowMap;
	
	
	for(i=0;i<5;i++)
	{
		vwShadowViews[i]->skill_y=vwShadowViews[i+1];//pointer chain	
	}
	
	litPointLight_->vwShadowView=vwShadowViews[0];
	
	int iScreenSizeX_old=screen_size.x;
	int iScreenSizeY_old=screen_size.y;
	int iVideoScreen_last=video_screen;
	
	while(vwShadowViews[0]!=NULL)
	{
		if(vwShadowViews[0]->skill_x!=4)
		return;
		
		for(i=0;i<6;i++)
		{
			vwShadowViews[i]->clip_far=litPointLight_->vLightRange*1.5;
			
			vwShadowViews[i]->x=litPointLight_->vcLightPos->x;
			vwShadowViews[i]->y=litPointLight_->vcLightPos->y;
			vwShadowViews[i]->z=litPointLight_->vcLightPos->z;
		}
		
		if(iScreenSizeX_old!=screen_size.x || iScreenSizeY_old!=screen_size.y  || (iVideoScreen_last!=video_screen))//re-initialize
		{
			/////////////
			wait(1);
			update_zbuffer(litPointLight_->iSMapResolution);
			///////////////////////////////////////
			
			ptr_remove(bCubeMap);
			
			BMAP*	bCubeMap = bmap_createblack(litPointLight_->iSMapResolution*6,litPointLight_->iSMapResolution,14444);
			bmap_to_cubemap(bCubeMap);

			litPointLight_->bShadowMap=bCubeMap;

			ent_setskin(litPointLight_->eLight,litPointLight_->bShadowMap,3);
			
			for(i=0;i<6;i++)
			vwShadowViews[i]->bmap = litPointLight_->bShadowMap;
		}
		
		iScreenSizeX_old=screen_size.x;
		iScreenSizeY_old=screen_size.y;
		iVideoScreen_last=video_screen;
		
		wait(1);
	}
}