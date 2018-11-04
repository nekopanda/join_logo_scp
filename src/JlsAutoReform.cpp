//
// join_logo_scp : Auto�nCM�\����������
//  �o�́F
//    pdata(chap,arstat)
//  ��Ɨ̈�F
//    pdata(score)
//  �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//
#include "stdafx.h"
#include "CommonJls.hpp"
#include "JlsAutoReform.hpp"
#include "JlsCmdSet.hpp"
#include "JlsDataset.hpp"


//=====================================================================
// �����\���ɕK�v�Ȏ��O�ݒ�
//=====================================================================

//---------------------------------------------------------------------
// ������
//---------------------------------------------------------------------
JlsAutoReform::JlsAutoReform(JlsDataset *pdata){
	//--- �֐��|�C���^�쐬 ---
	this->pdata = pdata;
}

//=====================================================================
// �����\�����쐬
//=====================================================================

//---------------------------------------------------------------------
// �\�����������s�iauto�n�R�}���h�̈�ԍŏ��ɕK�����s�j
// ���́F
//    cmdarg�F�����f�[�^�iAutoCM�������ꍇ�̂݃��S�Ȃ������Ŏg�p�j
//---------------------------------------------------------------------
void JlsAutoReform::mkReformAll(JlsCmdArg &cmdarg){
	//--- ���S�D��x����\���𐄑� ---
	if (pdata->isUnuseLevelLogo()){			// ���S�Ȃ��\����������ꍇ
		mkReformAllNologo(cmdarg);
	}
	else{
		mkReformAllLogo();
	}
}

//---------------------------------------------------------------------
// CM�\�����o�R�}���h�J�n�i���S�Ȃ����̂ݎ��s�����j
// ���́F
//    cmdarg�F�����f�[�^�i���S�Ȃ������Ŏg�p�j
//---------------------------------------------------------------------
bool JlsAutoReform::startAutoCM(JlsCmdArg &cmdarg){
	bool exeflag = false;
	if (pdata->isUnuseLevelLogo()){			// ���S�Ȃ��\����������ꍇ
		FormAutoCMArg param_autocm;
		bool reform_all = false;
		setReformParam(param_autocm, cmdarg, reform_all);
		exeflag = detectCM(param_autocm);
	}
	return exeflag;
}

//---------------------------------------------------------------------
// �Ώۈʒu�i�P�_�܂��͂Q�_�͈́j�̍\�����m��
// ���́F
//    nsc_from�F�͈͊J�n�ʒu
//    nsc_to�F  �͈͏I���ʒu
//    logomode�F���S�ݒ�i0:���S�Ȃ��A1:���S����j
//    resutuct�F�\���č\�z�i0:�͈͓��͒P��\���A1:�͈͓��̍\�����č\�z�j
// �o�́F
//   pdata(chap,arstat)
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
void JlsAutoReform::mkReformTarget(Nsc nsc_from, Nsc nsc_to, bool logoon, int restruct){
	//--- from��O���Ato���㑤 ---
	if (nsc_from > nsc_to){
		Nsc tmp = nsc_from;
		nsc_from = nsc_to;
		nsc_to   = tmp;
	}
	//--- �����}�C�i�X�̎��͐ݒ�Ȃ� ---
	if (nsc_to < 0){
		return;
	}
	//--- �����J�n ---
	Msec msec_to = -1;
	Msec msec_from = -1;
	Sec  sec_dif = 0;
	//--- �O���ʒu�̊m�菈�� ---
	if (nsc_from >= 0){
		msec_from = pdata->getMsecScp(nsc_from);
		//--- ��O�ʒu�ɐݒ肷��\�����擾 ---
		Nsc nsc_chapfrom = pdata->getNscNextScpDecide(nsc_from-1, SCP_END_EDGEIN);
		ScpArType arstat_from = pdata->getScpArstat(nsc_chapfrom);
		//--- ��O�ʒu�̐ݒ� ---
		pdata->setScpChap(nsc_from, SCP_CHAP_DFORCE);
		pdata->setScpArstat(nsc_from, arstat_from);
		//--- �ΏۊԂ̊m��ʒu������ ---
		while (nsc_chapfrom >= 0 && nsc_chapfrom < nsc_to){
			if (nsc_chapfrom > nsc_from){
				pdata->setScpChap(nsc_chapfrom, SCP_CHAP_NONE);
			}
			nsc_chapfrom = pdata->getNscNextScpDecide(nsc_chapfrom, SCP_END_EDGEIN);
		}
	}
	//--- �㑤�ʒu�̊m�菈�� ---
	if (nsc_to >= 0){
		msec_to = pdata->getMsecScp(nsc_to);
		//--- �ΏۊԂ̋����擾 ---
		ScpArType arstat_to = SCP_AR_L_OTHER;
		if (nsc_from >= 0){
			sec_dif = pdata->cnv.getSecFromMsec(msec_to - msec_from);
		}
		//--- �㑤�ʒu�ɐݒ肷��\�����擾 ---
		if (logoon == 0){
			if (sec_dif > 0 && sec_dif <= 120 && (sec_dif % 15 == 0)){
				arstat_to = SCP_AR_N_UNIT;
			}
			else{
				arstat_to = SCP_AR_N_OTHER;
			}
		}
		//--- �㑤�ʒu�̐ݒ� ---
		pdata->setScpChap(nsc_to, SCP_CHAP_DFORCE);
		pdata->setScpArstat(nsc_to, arstat_to);
	}
	//--- �Q�_�Ԃ̍\���č\�z ---
	if (nsc_from >= 0 && nsc_to >= 0 && restruct > 0){
		//--- ���S�Ȃ��č\�z ---
		if (logoon == 0){
			RangeFixMsec scope;
			scope.st = msec_from;
			scope.ed = msec_to;
			scope.fixSt = true;
			scope.fixEd = true;
			if (nsc_from == 0){
				scope.fixSt = false;
				scope.st     = -1;
			}
			if (nsc_to == pdata->sizeDataScp()-1){
				scope.fixEd = false;
				scope.ed     = -1;
			}
			int  interfill = 2;			// CM�\�������͋����I��CM��
			RangeMsec bounds;
			mkRangeCM(bounds, scope, logoon, interfill);
		}
		//--- ���S����č\�z ---
		else{
			RangeNsc rnsc_target = {nsc_from, nsc_to};
			setInterpolar(rnsc_target, logoon);
		}
	}
}




//=====================================================================
// �S�̂̍\���������s
//=====================================================================

//---------------------------------------------------------------------
// ���S�����g���č\�����������s
//---------------------------------------------------------------------
void JlsAutoReform::mkReformAllLogo(){
	bool lastlogo  = true;			// �ŏI�ʒu�̃��S�L�t���O
	//--- ���S�̂Ȃ����Ԃ�CM�����鏈�� ---
	{
		NrfCurrent logopt = {};			// ���݂̃��S�ʒu�i�[
		RangeFixMsec scope = {};		// �����͈�
		scope.st = -1;
		Msec msec_lg_remain_st = -1;	// ���̎c���ӏ��̐擪�ʒu
		bool logoon_st = false;			// �擪�����̃��S���
		bool flagend   = false;			// �I���t���O
		while(flagend == false){
			//--- ���S�ʒu�擾 ---
			if ( pdata->getNrfptNext(logopt, LOGO_SELECT_VALID) ){
				scope.ed = logopt.msecRise;
			}
			else{
				scope.ed = -1;
				flagend = true;			// �Ō�̒[���������ďI��
			}
			//--- CM�\�����쐬 ---
			int  interfill = 1;			// ������CM�\���ł͊Ԃ�CM
			RangeMsec bounds;
			bool det = mkRangeCM(bounds, scope, logoon_st, interfill);
			//--- �ŏI�ʒu�̃��S�L�����f ---
			if (flagend == true && det){
				if (logopt.msecFall < bounds.ed + pdata->msecValLap2){
					lastlogo = false;
				}
			}
			//--- �c���ӏ��iCM��O�j�̍\���������\������ ---
			if (flagend == true || det){				// �Ō�̈ʒu�܂���CM���o�����ꍇ
				if (logoon_st){							// ��O�Ƀ��S���������ꍇ
					int msec_lg_remain_ed;				// �c���ӏ��̏I���ʒu
					if (det){
						msec_lg_remain_ed = bounds.st;
					}
					else{
						msec_lg_remain_ed = -1;
					}
					//--- �c���ӏ��̓������� ---
					RangeMsec rmsec_new = {msec_lg_remain_st, msec_lg_remain_ed};
					mkReformAllLogoUpdate(rmsec_new);
				}
				msec_lg_remain_st = bounds.ed;		// ���̎c���ӏ��̐擪�ʒu
			}
			//--- CM��������Ȃ����̓��S���Ԏ��̂��P�\���ɂȂ�Ȃ����m�F ---
			else if (det == false){
				det = mkReformAllLogoUnit(msec_lg_remain_st, logopt);
			}
			//--- CM�F���܂��̓��S���Ԃ������ꍇ�͎��̊J�n�����S�I���ʒu�ɐݒ� ---
			Msec msec_recmin = pdata->getConfig(CONFIG_VAR_msecWLogoLgMin);
			if (flagend == false && (det || (logopt.msecFall - logopt.msecRise) >= msec_recmin)){
				scope.st = logopt.msecFall;
			}
			//--- ���̈ʒu�ݒ� ---
			logoon_st = true;
		}
	}

	//--- �擪�ʒu�̒ǉ����� ---
	{
		FormFirstInfo tmpinfo;
		setFirstArea(tmpinfo);
	}
	//--- �Ō�ʒu�̒ǉ����� ---
	{
		Nsc nsc_final = pdata->sizeDataScp() - 1;
		if (lastlogo == false){
			pdata->setScpArstat(nsc_final, SCP_AR_N_OTHER);
		}
		else{
			pdata->setScpArstat(nsc_final, SCP_AR_L_OTHER);
		}
		setFinalArea();
	}
	//--- �\���v�f�̃��S���������iMIX/Border�ǉ��j ---
	addLogoComponent( pdata->getLevelUseLogo() );
}


//---------------------------------------------------------------------
// ���S�������P�\���ƂȂ�ꍇ�͍\���ǉ�
// ���́F
//   logopt: ���o���郍�S���
// ���o��:
//   msec_lg_remain_st: ���ݒ胍�S�J�n�ʒu
// �o��:
//   �Ԃ�l: �X�V�L���ifalse=�Ȃ�  true=����j
//---------------------------------------------------------------------
bool JlsAutoReform::mkReformAllLogoUnit(Msec &msec_lg_remain_st, NrfCurrent &logopt){
	//--- �Ή�����V�[���`�F���W�ԍ����擾 ---
	RangeNsc  rnsc_lg_cur = {-1, -1};
	{
		RangeMsec rmsec_decide = {logopt.msecRise, logopt.msecFall};
		Nsc nsc_decide_st = pdata->getNscFromMsecChap(rmsec_decide.st, pdata->msecValLap2, SCP_CHAP_DECIDE);
		Nsc nsc_decide_ed = pdata->getNscFromMsecChap(rmsec_decide.ed, pdata->msecValLap2, SCP_CHAP_DECIDE);
		Nsc nsc_lg_any_st = pdata->getNscFromMsecChap(rmsec_decide.st, pdata->msecValLap2, SCP_CHAP_NONE);
		Nsc nsc_lg_any_ed = pdata->getNscFromMsecChap(rmsec_decide.ed, pdata->msecValLap2, SCP_CHAP_NONE);
		if (nsc_decide_st >= 0){
			rnsc_lg_cur.st = nsc_decide_st;
		}
		else if (nsc_lg_any_st >= 0){
			rnsc_lg_cur.st = nsc_lg_any_st;
		}
		if (nsc_decide_ed >= 0){
			rnsc_lg_cur.ed = nsc_decide_ed;
		}
		else if (nsc_lg_any_ed >= 0){
			rnsc_lg_cur.ed = nsc_lg_any_ed;
		}
	}
	//--- �Ή����閳���V�[���`�F���W������΂P�\�����Ԃ̔��� ---
	bool det = false;
	if (rnsc_lg_cur.st >= 0 && rnsc_lg_cur.ed >= 0 && rnsc_lg_cur.st < rnsc_lg_cur.ed){
		RangeMsec rmsec_lg_cur = getRangeMsec(rnsc_lg_cur);
		//--- ���݂̃��S�����F�����Ԃł��邩�m�F ---
		FormLogoLevelExt extype = {};
		extype.en_long = true;						// �����ԍ\����F��
		bool match_len = isLengthLogoLevel(rmsec_lg_cur.ed - rmsec_lg_cur.st, extype);
		if (match_len){
			det = true;
			if (rnsc_lg_cur.st > 0){
				if (pdata->getScpChap(rnsc_lg_cur.st) < SCP_CHAP_DFIX){
					pdata->setScpChap(rnsc_lg_cur.st, SCP_CHAP_DFIX);
				}
			}
			if (rnsc_lg_cur.ed > 0){
				if (pdata->getScpChap(rnsc_lg_cur.ed) < SCP_CHAP_DFIX){
					pdata->setScpChap(rnsc_lg_cur.ed, SCP_CHAP_DFIX);
				}
			}
			if (msec_lg_remain_st < rmsec_lg_cur.st){		// ���O�܂ł̃��S�ݒ�
				RangeMsec rmsec_new = {msec_lg_remain_st, rmsec_lg_cur.st};
				mkReformAllLogoUpdate(rmsec_new);
			}
			{
				RangeMsec rmsec_new = {rmsec_lg_cur.st, rmsec_lg_cur.ed};
				mkReformAllLogoUpdate(rmsec_new);
			}
			msec_lg_remain_st = rmsec_lg_cur.ed;		// ���S�擪�ʒu���X�V
		}
	}
	return det;
}


//---------------------------------------------------------------------
// ���S���Ԃ̍X�V
//---------------------------------------------------------------------
void JlsAutoReform::mkReformAllLogoUpdate(RangeMsec rmsec_new){
	RangeNsc rnsc_new;
	pdata->getRangeNscFromRangeMsec(rnsc_new, rmsec_new);
	if (rnsc_new.ed > 0){
		pdata->setScpArstat(rnsc_new.ed, SCP_AR_L_UNIT);	// �O�̂��ߐݒ�
	}
	bool cm_inter = true;
	mkRangeInterLogo(rnsc_new, cm_inter);
}


//---------------------------------------------------------------------
// ���S���g�킸�\�����������s
//---------------------------------------------------------------------
void JlsAutoReform::mkReformAllNologo(JlsCmdArg &cmdarg){
	//--- CM�\�����쐬 ---
	RangeMsec bounds;
	RangeFixMsec scope_org;
	scope_org.st = -1;				// �����J�n�ʒu���w�肵�Ȃ�
	scope_org.ed = -1;				// �����I���ʒu���w�肵�Ȃ�
	scope_org.fixSt = false;		// �Œ�ݒ�ł͂Ȃ�
	scope_org.fixEd = false;		// �Œ�ݒ�ł͂Ȃ�			
	bool logo1st = true;			// �擪���烍�S����Ƃ��Č���
	int  interfill = 0;				// CM��CM�̊Ԃ�CM�Ŗ��߂Ȃ�
	mkRangeCM(bounds, scope_org, logo1st, interfill);
	//--- ���C���������s ---
	FormAutoCMArg param_autocm;
	bool reform_all = true;
	setReformParam(param_autocm, cmdarg, reform_all);
	detectCM(param_autocm);
}

//---------------------------------------------------------------------
// �����\���쐬�p�̃p�����[�^�ݒ�
//---------------------------------------------------------------------
void JlsAutoReform::setReformParam(FormAutoCMArg &param_autocm, JlsCmdArg &cmdarg, bool reform_all){
	//--- �p�����[�^�擾 ---
	int  level_cmdet = 6;				// �S�̐ݒ莞�̃��S���o���x���f�t�H���g�l
	Msec msec_tailarea_keep = 0;
	Msec msec_tailarea_unit = 0;
	Sec  sec_maxcm = 60;
	RangeMsec rmsec_headtail = {-1, -1};
	if (!reform_all) rmsec_headtail = pdata->recHold.rmsecHeadTail;
	if (rmsec_headtail.st < 0) rmsec_headtail.st = 0;
	if (rmsec_headtail.ed < 0) rmsec_headtail.ed = pdata->getMsecTotalMax();
	//--- �R�}���h�L���� ---
	if (cmdarg.cmdsel == JLCMD_SEL_AutoCM){
		//--- �S�̐ݒ莞�͗L���ݒ�̂ݍX�V ---
		int tmp_level_cmdet = cmdarg.getOpt(JLOPT_DATA_AutopCode) % 100;
		if (reform_all == false || tmp_level_cmdet > 0){
			level_cmdet = tmp_level_cmdet;
		}
		//--- Auto�R�}���h�L������AutoMode�ݒ� ---
		if (level_cmdet > 0){
			pdata->setFlagAutoMode(true);
		}
		//--- �Ō�̈ʒu ---
		Sec sectmp_keep = cmdarg.getOpt(JLOPT_DATA_AutopScope);
		msec_tailarea_keep = 0;
		if (sectmp_keep > 0){
			msec_tailarea_keep = rmsec_headtail.ed - sectmp_keep * 1000;
		}
		//--- �Ō��60/90/120�b���f�ʒu ---
		Sec sectmp_unit = cmdarg.getOpt(JLOPT_DATA_AutopScopeX);
		msec_tailarea_unit = 0;
		if (sectmp_unit > 0){
			msec_tailarea_unit = rmsec_headtail.ed - sectmp_unit * 1000;
		}
		//--- ���Ԑݒ� ---
		sec_maxcm   = cmdarg.getOpt(JLOPT_DATA_AutopMaxPrd);
		if (sec_maxcm <= 0){
			sec_maxcm = 60;			// ���ݒ莞�̏����l
		}
	}

	//--- ���ʔ��f ---
	param_autocm.levelCmDet = level_cmdet;
	param_autocm.msecTailareaKeep = msec_tailarea_keep;
	param_autocm.msecTailareaUnit = msec_tailarea_unit;
	param_autocm.secMaxCm = sec_maxcm;
	param_autocm.rmsecHeadTail = rmsec_headtail;
}



//=====================================================================
// ���S�����g���čŌ�ɍ\�����ǉ��i���S���g����������j
//=====================================================================

//---------------------------------------------------------------------
// �\���v�f�̃��S���������iMIX/Border�ǉ��j
// �o�́F
//   pdata(chap,arstat)
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
void JlsAutoReform::addLogoComponent(int lvlogo){
		addLogoEdge();
		addLogoBorder();
		addLogoMix();
		return;
}

//---------------------------------------------------------------------
// ���S���������Ń��S�Ȃ��������������̕␳
// �o�́F
//   pdata(chap,arstat)
//---------------------------------------------------------------------
void JlsAutoReform::addLogoEdge(){
	//--- �F���ŏ����� ---
	Msec msec_recmin = pdata->getConfig(CONFIG_VAR_msecWLogoCmMin);
	//--- ���S�\����ԓ��̒[�\����CM�����鏈���ݒ� ---
	if (pdata->getConfigAction(CONFIG_ACT_LogoDelWide) == 0){
		return;								// �J�b�g���Ȃ��ݒ�ł͏I��
	}
	int rev_del_edge = pdata->getConfigAction(CONFIG_ACT_LogoDelEdge);
	//--- ���S�ʒu�����ԂɌ��� ---
	NrfCurrent logopt = {};
	bool remain_logo = pdata->getNrfptNext(logopt, LOGO_SELECT_VALID);
	//--- �ŏ��ƍŌ�͑ΏۊO�Ƃ��Č��� ---
	while(remain_logo){
		remain_logo = pdata->getNrfptNext(logopt, LOGO_SELECT_VALID);
		//--- �Ō�̃��S�͕ʓr�����i���S�����I���ӏ������̃��S�J�n�����ɂ���j ---
		bool valid_last = false;
		if (remain_logo == false && logopt.msecLastFall > 0){
			bool over = false;
			ElgCurrent elg = {};
			while( pdata->getElgptNext(elg) && over == false){
				//--- ���S�����\���I�������S����������ȍ~�Ȃ���Ƃ��� ---
				if (elg.msecFall > logopt.msecLastFall){
					over = true;
					valid_last = true;
					logopt.msecRise = elg.msecFall;
				}
			}
		}
		if (remain_logo || valid_last){
			if (logopt.msecRise - logopt.msecLastFall >= msec_recmin){
				//--- ���S�ʒu�擾�i�O��fall���猻�݂�rise�܂ł̋�ԁj ---
				RangeWideMsec area_nologo;
				pdata->getWideMsecLogoNrf(area_nologo.st, logopt.nrfLastFall);
				pdata->getWideMsecLogoNrf(area_nologo.ed, logopt.nrfRise);
				if (rev_del_edge == 0){			// �s�m�蕔���͎g��Ȃ��ݒ莞
					area_nologo.st.early = area_nologo.st.late  - pdata->msecValSpc;
					area_nologo.ed.late  = area_nologo.ed.early + pdata->msecValSpc;
					if (area_nologo.st.just < area_nologo.st.early){
						area_nologo.st.just = area_nologo.st.early;
					}
					if (area_nologo.ed.just > area_nologo.ed.late){
						area_nologo.ed.just = area_nologo.ed.late;
					}
				}
				else if (rev_del_edge <= 2){	// ���S�t�߂܂łɐ���
					area_nologo.st.early = area_nologo.st.just - pdata->msecValSpc;
					area_nologo.ed.late  = area_nologo.ed.just + pdata->msecValSpc;
				}
				else{
					area_nologo.st.early -= pdata->msecValSpc;
					area_nologo.ed.late  += pdata->msecValSpc;
				}
				if (valid_last){		// �ŏI���S�̏I���n�_���Œ�
					area_nologo.ed.just  = logopt.msecRise;
					area_nologo.ed.early = logopt.msecRise;
					area_nologo.ed.late  = logopt.msecRise;
				}
				//--- �ݒ� ---
				addLogoEdgeUpdate(area_nologo, msec_recmin);
			}
		}
	}
}

//---------------------------------------------------------------------
// ���S���������Ń��S�Ȃ��������������̕␳-�X�V����
// ���́F
//   area_nologo: ���S�Ȃ�����
//   msec_recmin: �X�V�����s����Œ����
// �o�́F
//   pdata(chap,arstat)
//---------------------------------------------------------------------
void JlsAutoReform::addLogoEdgeUpdate(RangeWideMsec area_nologo, int msec_recmin){
	//--- �����͈͐ݒ� ---
	RangeNsc rnsc_target;
	RangeNsc rnsc_cmp;
	{
		RangeWideMsec area_search = area_nologo;
		area_search.st.late  = area_nologo.ed.early;
		area_search.ed.early = area_nologo.st.late;
		rnsc_target.st = pdata->getNscFromWideMsecFull(area_search.st, SCP_CHAP_DECIDE, SCP_END_EDGEIN);
		rnsc_target.ed = pdata->getNscFromWideMsecFull(area_search.ed, SCP_CHAP_DECIDE, SCP_END_EDGEIN);
		rnsc_cmp.st    = pdata->getNscFromWideMsecFull(area_search.st, SCP_CHAP_NONE, SCP_END_EDGEIN);
		rnsc_cmp.ed    = pdata->getNscFromWideMsecFull(area_search.ed, SCP_CHAP_NONE, SCP_END_EDGEIN);
	}
	//--- �����Ώۂ��Ȃ���ΏI�� ---
	if (rnsc_cmp.st <= 0 || rnsc_cmp.ed <= 0){
		return;
	}
	//--- �Ώۈʒu�̑I�� ---
	RangeNsc rnsc_select = rnsc_target;			// �g�p���郍�S�Ȃ����
	//--- �m��ȊO�̉ӏ��ɂ��邩�`�F�b�N ---
	if (rnsc_target.st != rnsc_cmp.st){
		if (rnsc_target.st < 0){
			rnsc_select.st = rnsc_cmp.st;
		}
		else{
			//--- ���S�F�����Ԃ̈ʒu���� ---
			bool cont = true;
			Nsc nsc_cur = rnsc_cmp.st;
			while(cont && nsc_cur > 0 && nsc_cur < rnsc_target.st){
				Msec msec_dif = pdata->getMsecScp(rnsc_target.st) - pdata->getMsecScp(nsc_cur);
				FormLogoLevelExt extype = {};
				extype.en_long = true;						// �����ԍ\����F��
				extype.longmod = true;						// �����Ԃ�5/15�b�P�ʂ�F��
				if (isLengthLogoLevel(msec_dif, extype)){	// ���Ԃ����S�F������
					rnsc_select.st = nsc_cur;
					cont = false;
				}
				nsc_cur = pdata->getNscNextScpChap(nsc_cur, SCP_CHAP_NONE);
			}
			//--- �ŏI�n�_�܂ŗ���Ă�����F�����ԂɊ֌W�Ȃ��ݒ� ---
			if (rnsc_select.st != rnsc_cmp.st){
				Msec msec_dif = pdata->getMsecScp(rnsc_select.st) - pdata->getMsecScp(rnsc_cmp.st);
				if (msec_dif >= msec_recmin){
					rnsc_select.st = rnsc_cmp.st;
				}
			}
		}
	}
	if (rnsc_target.ed != rnsc_cmp.ed){
		if (rnsc_target.ed < 0){
			rnsc_select.ed = rnsc_cmp.ed;
		}
		else{
			//--- ���S�F�����Ԃ̈ʒu���� ---
			bool cont = true;
			Nsc nsc_cur = rnsc_cmp.ed;
			while(cont && nsc_cur > 0 && nsc_cur > rnsc_target.ed){
				Msec msec_dif = pdata->getMsecScp(nsc_cur) - pdata->getMsecScp(rnsc_target.ed);
				FormLogoLevelExt extype = {};
				extype.en_long = true;						// �����ԍ\����F��
				extype.longmod = true;						// �����Ԃ�5/15�b�P�ʂ�F��
				if (isLengthLogoLevel(msec_dif, extype)){	// ���Ԃ����S�F������
					rnsc_select.ed = nsc_cur;
					cont = false;
				}
				nsc_cur = pdata->getNscPrevScpChap(nsc_cur, SCP_CHAP_NONE);
			}
			//--- �ŏI�n�_�܂ŗ���Ă�����F�����ԂɊ֌W�Ȃ��ݒ� ---
			if (rnsc_select.ed != rnsc_cmp.ed){
				Msec msec_dif = pdata->getMsecScp(rnsc_cmp.ed) - pdata->getMsecScp(rnsc_select.ed);
				if (msec_dif >= msec_recmin){
					rnsc_select.ed = rnsc_cmp.ed;
				}
			}
		}
	}
	//--- �ʒu�擾 ---
	RangeMsec rmsec_select;			// �g�p���郍�S�Ȃ���ԁi�~���b�j
	rmsec_select.st = pdata->getMsecScp(rnsc_select.st);
	rmsec_select.ed = pdata->getMsecScp(rnsc_select.ed);
	//--- ���ԃ`�F�b�N ---
	if (rmsec_select.ed - rmsec_select.st >= msec_recmin){
		ElgCurrent elg = {};
		while( pdata->getElgptNext(elg) ){
			//--- ���S�Ȃ���ԓ��Ń��S�����͈̔͂��擾 ---
			RangeNsc rnsc_del = rnsc_select;	// �폜���郍�S�Ȃ����
			if (rnsc_del.st < elg.nscRise){
				rnsc_del.st = elg.nscRise;
			}
			if (rnsc_del.ed > elg.nscFall){
				rnsc_del.ed = elg.nscFall;
			}
			if (rnsc_del.st < rnsc_del.ed){
				RangeMsec rmsec_del;
				rmsec_del.st = pdata->getMsecScp(rnsc_del.st);
				rmsec_del.ed = pdata->getMsecScp(rnsc_del.ed);
				//--- ���S�Ȃ��̃��S�������Ԃ��ݒ�l�ȏ�A�܂��͍\���S�̂��폜�͈͓��Ȃ�X�V ---
				if ((rmsec_del.ed - rmsec_del.st >= msec_recmin) ||
					(rnsc_del.st <= elg.nscRise && rnsc_del.ed >= elg.nscFall)){
					ScpChapType chap_st = pdata->getScpChap(rnsc_del.st);
					ScpChapType chap_ed = pdata->getScpChap(rnsc_del.ed);
					if (chap_st < SCP_CHAP_DFIX){
						pdata->setScpChap(rnsc_del.st, SCP_CHAP_DFIX);	// �X�V
						pdata->setScpArstat(rnsc_del.st, SCP_AR_L_OTHER);
					}
					if (chap_ed < SCP_CHAP_DFIX){
						pdata->setScpChap(rnsc_del.ed, SCP_CHAP_DFIX);	// �X�V
						pdata->setScpArstat(rnsc_del.ed, SCP_AR_N_OTHER);
					}
					Nsc nsc_tmp = rnsc_del.st;
					while(nsc_tmp > 0 && nsc_tmp < rnsc_del.ed){
						nsc_tmp = pdata->getNscNextScpChap(nsc_tmp, SCP_CHAP_DECIDE);
						if (nsc_tmp > 0 && nsc_tmp <= rnsc_del.ed){
							//--- ���S�Ȃ������ɕύX ---
							ScpArType arstat_tmp = pdata->getScpArstat(nsc_tmp);
							ScpArType arstat_new = arstat_tmp;
							if (arstat_tmp == SCP_AR_L_UNIT){
								arstat_new = SCP_AR_N_UNIT;
							}
							else if (arstat_tmp == SCP_AR_L_OTHER){
								arstat_new = SCP_AR_N_OTHER;
							}
							if (arstat_tmp != arstat_new){
								pdata->setScpArstat(nsc_tmp, arstat_new);	// �X�V
							}
						}
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------
// �\���v�f��Mix��ǉ��iBorder�����ł��Ȃ������\���j
// �o�́F
//   pdata(arstat)
//---------------------------------------------------------------------
void JlsAutoReform::addLogoMix(){
	NrfCurrent logopt = {};
	bool remain_logo = pdata->getNrfptNext(logopt, LOGO_SELECT_VALID);
	if (remain_logo == false){
		return;
	}
	Nsc nsc_last     = pdata->getNscNextScpChap(0, SCP_CHAP_DECIDE);
	while(nsc_last > 0){
		Nsc  nsc_cur   = pdata->getNscNextScpChap(nsc_last, SCP_CHAP_DECIDE);		// ���̍\��
		Msec msec_cur  = pdata->getMsecScp(nsc_cur);
		Msec msec_last = pdata->getMsecScp(nsc_last);
		int sumlogo   = 0;
		bool cmpnext = true;
		while(nsc_cur > 0 && remain_logo && cmpnext && logopt.msecRise < msec_cur - pdata->msecValLap2){
			if (remain_logo){
				if (logopt.msecFall > msec_last + pdata->msecValLap2){
					sumlogo ++;			// ���S�����J�E���g
				}
				//--- ���̃��S�ʒu�ڍs���� ---
				if (logopt.msecFall > msec_cur - pdata->msecValLap2){
					cmpnext = false;
				}
				else{
					remain_logo = pdata->getNrfptNext(logopt, LOGO_SELECT_VALID);
				}
			}
		}
		//--- ���S�������ɂQ�ȏ゠���Mix ---
		if (sumlogo > 1){
			ScpArType arstat_cur = pdata->getScpArstat(nsc_cur);
			if (arstat_cur == SCP_AR_L_OTHER){
				pdata->setScpArstat(nsc_cur, SCP_AR_L_MIXED);
			}
		}
		nsc_last = nsc_cur;
	}
}

//---------------------------------------------------------------------
// �\���v�f�̃��S���������iBorder�ǉ��j
// �o�́F
//   pdata(chap,arstat)
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
void JlsAutoReform::addLogoBorder(){
	NrfCurrent logopt = {};
	bool remain_logo = pdata->getNrfptNext(logopt, LOGO_SELECT_VALID);
	if (remain_logo == false){
		return;
	}
	Nsc nsc_last = pdata->getNscNextScpChap(0, SCP_CHAP_DECIDE);
	//--- �e�\���ʒu�̃��S�����ԂɊm�F ---
	while(nsc_last > 0){
		Nsc  nsc_cur   = pdata->getNscNextScpChap(nsc_last, SCP_CHAP_DECIDE);		// ���̍\��
		Msec msec_cur  = pdata->getMsecScp(nsc_cur);
		Msec msec_last = pdata->getMsecScp(nsc_last);
		Msec msec_res  = msec_last;
		bool enable_st = true;		// �擪�ʒu�̊m����
		bool cmpnext = true;		// ���̃��S�ʒu����r���邩
		//--- ���\���ʒu�܂ł̃��S�����ԂɊm�F ---
		while(nsc_cur > 0 && remain_logo && cmpnext && logopt.msecRise < msec_cur - pdata->msecValLap2){
			if (remain_logo){
				//--- ���S�����オ��̍\������ ---
				RangeMsec component = {msec_last, msec_cur};
				RangeNrf  rnrf_logo = {logopt.nrfRise, logopt.nrfFall};
				msec_res = addLogoBorderSub(component, rnrf_logo, enable_st);
				enable_st = (msec_res >= 0)? true : false;
				if (msec_res > msec_last){
					msec_last = msec_res;
				}
				//--- ���̃��S�ʒu�ڍs���� ---
				if (logopt.msecFall > msec_cur - pdata->msecValLap2){
					cmpnext = false;			// ���݂̍\�����̓��S�I��
				}
				else{
					remain_logo = pdata->getNrfptNext(logopt, LOGO_SELECT_VALID);
				}
			}
			//--- ���S����������̍\������ ---
			if (logopt.nrfLastFall > 0 && cmpnext){
				RangeMsec component = {msec_last, msec_cur};
				RangeNrf  rnrf_logo = {logopt.nrfLastFall, logopt.nrfRise};
				msec_res = addLogoBorderSub(component, rnrf_logo, enable_st);
				enable_st = (msec_res >= 0) ? true : false;
				if (msec_res > msec_last){
					msec_last = msec_res;
				}
			}
		}
		//--- ��؂�}���Ŕԍ����ς�邱�Ƃ��邽�߈ʒu���Ď擾 ---
		if (nsc_cur > 0){
			nsc_last = pdata->getNscFromMsecAll(msec_cur);
			if (nsc_last > 0 && nsc_last < nsc_cur){	// �O�̂��ߖ߂邱�ƂȂ��悤�ɕی�
				nsc_last = nsc_cur;
			}
		}
		else{
			nsc_last = nsc_cur;
		}
	}
	//--- �\������Border�t�� ---
	addLogoBorderSet();
}

//---------------------------------------------------------------------
// �\����؂��񂩂�Border�ǉ�
// �o�́F
//   pdata(arstat)
//---------------------------------------------------------------------
void JlsAutoReform::addLogoBorderSet(){
	Nsc nsc_last = pdata->getNscNextScpChap(0, SCP_CHAP_DECIDE);
	Nsc nsc_cur  = pdata->getNscNextScpChap(nsc_last, SCP_CHAP_DECIDE);		// ���̍\��
	while(nsc_cur > 0){
		Nsc nsc_next  = pdata->getNscNextScpChap(nsc_cur, SCP_CHAP_DECIDE);	// ���̍\��
		//--- border��؂茟�o ---
		if (pdata->getScpChap(nsc_cur) == SCP_CHAP_DBORDER){
			//--- �O���̔��� ---
			ScpArType arstat_cur = pdata->getScpArstat(nsc_cur);
			if (arstat_cur == SCP_AR_N_UNIT){
				pdata->setScpArstat(nsc_cur, SCP_AR_B_UNIT);
			}
			else if (arstat_cur == SCP_AR_N_OTHER){
				pdata->setScpArstat(nsc_cur, SCP_AR_B_OTHER);
			}
			else if (jlsd::isScpArTypeLogo(arstat_cur)){
				bool enable_short = false;
				addCommonComponentOne(nsc_cur, nsc_last, enable_short);
			}
			//--- �㑤�̔��� ---
			if (nsc_next > 0){
				ScpArType arstat_next = pdata->getScpArstat(nsc_next);
				if (arstat_next == SCP_AR_N_UNIT){
					pdata->setScpArstat(nsc_next, SCP_AR_B_UNIT);
				}
				else if (arstat_next == SCP_AR_N_OTHER){
					pdata->setScpArstat(nsc_next, SCP_AR_B_OTHER);
				}
				else if (jlsd::isScpArTypeLogo(arstat_next)){
					bool enable_short = false;
					addCommonComponentOne(nsc_cur, nsc_next, enable_short);
				}
			}
		}
		//--- ���̈ʒu�ݒ� ---
		nsc_last = nsc_cur;
		nsc_cur  = nsc_next;
	}
}

//---------------------------------------------------------------------
// �P�ʍ\���ƃ��S���Q�_����P�ʍ\�����𕪊�
// ���́F
//   component: �Ώۂ̌��ݍ\��
//   rnrf_logo: ���S�̊J�n�ʒu�ƏI���ʒu�̔ԍ�
//   enable_st: �擪�ʒu�̊m���ԁifalse=���m��Atrue=�m��j
// �o�́F
//   �Ԃ�l:�\���m�肵���ʒu�i�~���b�j
//   pdata(chap,arstat)
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
Msec JlsAutoReform::addLogoBorderSub(RangeMsec component, RangeNrf rnrf_logo, bool enable_st){
	Msec msec_ret = -1;

	if (rnrf_logo.st < 0) return -1;
	//--- ���S�ʒu���擾���Ώ۔͈͂��m�F ---
	RangeMsec logo_targets = component;
	//--- �O�̃��S�ʒu���Ώ۔͈͂��m�F ---
	{
		WideMsec wmsec_logost;
		pdata->getWideMsecLogoNrf(wmsec_logost, rnrf_logo.st);
		if (wmsec_logost.early < component.st + pdata->msecValLap2) {	// ���S�J�n�ƍ\���J�n������
			if (enable_st) {
				return component.st;
			}
			else {
				return -1;
			}
		}
		else if (wmsec_logost.late > component.ed - pdata->msecValLap2) {	// ���S�J�n���\���I���t��
			return component.ed;
		}
		logo_targets.st = wmsec_logost.just;
		//--- �Ή��ӏ��̖����V�[���`�F���W���邩�m�F ---
		Nsc nsc_inspos = pdata->getNscFromMsecAll(wmsec_logost.just);
		if (nsc_inspos >= 0) {
			logo_targets.st = pdata->getMsecScp(nsc_inspos);	// ���S�ɑΉ����閳���V�[���`�F���W
		}
	}
	//--- ��̃��S�ʒu���Ώ۔͈͂��m�F ---
	if (rnrf_logo.ed >= 0) {
		WideMsec wmsec_logoed;
		pdata->getWideMsecLogoNrf(wmsec_logoed, rnrf_logo.ed);
		//--- ���S���Ԃ��قƂ�ǂȂ���ΑΏۊO ---
		if (wmsec_logoed.early < logo_targets.st + pdata->msecValLap2) {
			return component.st;
		}
		//--- ���̃��S���\���r���ʒu�ɂ���ꍇ ---
		else if (wmsec_logoed.late <= component.ed - pdata->msecValLap2) {
			logo_targets.ed = wmsec_logoed.just;
		}
	}
	//--- ���S�ʒu�ɑΉ������\�����������s ---
	bool rise_logost = jlsd::isLogoEdgeRiseFromNrf(rnrf_logo.st);
	msec_ret = addLogoBorderSubUpdate(component, logo_targets, rise_logost, enable_st);
	return msec_ret;
}

//---------------------------------------------------------------------
// ���S�؂�ւ��ʒu�̍\�����ɂ��ĕ������s
// ���́F
//   component:     �Ώۂ̌��ݍ\��
//   logo_targets:  Border���f���郍�S�ʒu�Q�ӏ�
//   rise_logost:   �擪�̃��S�ʒu�͗����オ�肩
//   enable_st:     �擪�ʒu�̊m���ԁifalse=���m��Atrue=�m��j
// �o�́F
//   pdata(chap,arstat)
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
Msec JlsAutoReform::addLogoBorderSubUpdate(RangeMsec component, RangeMsec logo_targets, bool rise_logost, bool enable_st){
	Msec msec_ret = -1;

	//--- �\���O��̈ʒu����Ώ۔͈͂��m�F ---
	if (logo_targets.st <= 0) return -1;

	//--- �\���O��̋����擾 ---
	bool unit15s_logost = false;		// ���S�O�������܂ł̍\����15�b�P�ʂ�
	bool unit15s_logoed = false;		// ���S�㔼�����܂ł̍\����15�b�P�ʂ�
	int  update_logost = 0;				// ���S�O�������ŏ��������邩
	int  update_logoed = 0;				// ���S�㔼�����ŏ��������邩
	{
		CalcDifInfo calc1;
		CalcDifInfo calc2;
		int type1 = calcDifSelect(calc1, component.st, logo_targets.st);
		int type2 = calcDifSelect(calc2, logo_targets.st, logo_targets.ed);
		//--- �\���X�V���f ---
		int chknum1 = 0;
		int chknum2 = 0;
		for(int i=0; i<2; i++){
			int types;
			CalcDifInfo calcs;
			if (i==0){
				types = type1;
				calcs = calc1;
			}
			else{
				types = type2;
				calcs = calc2;
			}
			int lvlogo = pdata->getLevelUseLogo();
			if ((types > 0 && calcs.sec % 15 == 0 && calcs.sec >= 15) ||
				(types > 0 && calcs.sec % 5 == 0 && lvlogo >= CONFIG_LOGO_LEVEL_USE_MIDH) ||
				(types > 0 && calcs.sec >= 3 && lvlogo >= CONFIG_LOGO_LEVEL_USE_HIGH)) {
				if (i==0){
					chknum1 = 5;
				}
				else{
					chknum2 = 5;
				}
			}
		}
		if (chknum1 < 5){
			if (calc1.sec > 30){
				chknum1 = 4;
			}
			else if (type1 > 0 && calc1.sec > calc2.sec + 5){
				chknum1 = 3;
			}
		}
		if (chknum2 < 5){
			if (calc2.sec > 30){
				chknum2 = 4;
			}
			else if (type2 > 0 && calc2.sec > calc1.sec + 5){
				chknum2 = 3;
			}
		}
		//--- �\�����̎擾 ---
		Nsc nsc_ed = pdata->getNscFromMsecAll(component.ed);
		ScpArType arstat_ed = pdata->getScpArstat(nsc_ed);
		bool flag_arstat_logo = jlsd::isScpArTypeLogo(arstat_ed);	// �Ώۍ\�������S�L��
		//--- �\���X�V�ʒu���擾 ---
		if (chknum1 + chknum2 >= 7){
			if ((flag_arstat_logo == true) && (rise_logost == true)){
				if (type1 > 0 && calc1.sec <= 60 && enable_st){
					update_logost = 1;				// �O�������S�Ȃ��ɐݒ�
				}
			}
			else if ((flag_arstat_logo == false) && (rise_logost == false)){
				if (type1 > 0 && calc1.sec <= 60 && enable_st){
					update_logost = 2;				// �O�������S����ɐݒ�
				}
			}
			else if ((flag_arstat_logo == true) && (rise_logost == false)){
				if (type2 > 0 && calc2.sec <= 60){
					update_logoed = 1;				// ���S�ʒu������S�Ȃ��ɐݒ�
				}
			}
			else if ((flag_arstat_logo == false) && (rise_logost == true)){
				if (type2 > 0 && calc2.sec <= 60){
					update_logoed = 2;				// ���S�ʒu������S����ɐݒ�
				}
			}
		}
		//--- 15�b�\�����f ---
		if (calc1.sec % 15 == 0) unit15s_logost = true;
		if (calc2.sec % 15 == 0) unit15s_logoed = true;
	}
	//--- �\�����X�V ---
	if (update_logost > 0 || update_logoed > 0){
		bool border_logost = false;
		bool border_logoed = false;
		Nsc  nsc_logost = pdata->getNscFromMsecAll(logo_targets.st);
		//--- �����V�[���`�F���W���Ȃ���΋����I�ɍ쐬����Border�ɂ��� ---
		if (nsc_logost < 0){
			nsc_logost = pdata->getNscForceMsec(logo_targets.st, LOGO_EDGE_RISE);
			border_logost = true;
		}
		//--- �ŏ��̃��S�ʒu�̍\���ύX ---
		if (update_logost > 0 || update_logoed > 0){
			msec_ret = logo_targets.st;
			//--- chap�ݒ� ---
			if (border_logost){
				pdata->setScpChap(nsc_logost, SCP_CHAP_DBORDER);
			}
			else if (pdata->getScpChap(nsc_logost) < SCP_CHAP_DINT){
				pdata->setScpChap(nsc_logost, SCP_CHAP_DINT);
			}
			//--- arstat�ݒ� ---
			if (update_logost == 1){
				ScpArType arstat_st = (unit15s_logost)? SCP_AR_N_UNIT : SCP_AR_N_OTHER;
				pdata->setScpArstat(nsc_logost, arstat_st);
			}
			else if (update_logost == 2){
				ScpArType arstat_st = (unit15s_logost)? SCP_AR_L_UNIT : SCP_AR_L_OTHER;
				pdata->setScpArstat(nsc_logost, arstat_st);
			}
		}
		//--- ���̈ʒu�̍\���ύX ---
		if (update_logoed > 0){
			msec_ret = logo_targets.ed;
			int nsc_logoed = pdata->getNscFromMsecAll(logo_targets.ed);
			//--- �����V�[���`�F���W���Ȃ���΋����I�ɍ쐬����Border�ɂ��� ---
			if (nsc_logoed < 0){
				nsc_logoed = pdata->getNscForceMsec(logo_targets.ed, LOGO_EDGE_RISE);
				border_logoed = true;
			}
			//--- ��؂�����X�V ---
			if (border_logoed){
				pdata->setScpChap(nsc_logoed, SCP_CHAP_DBORDER);
			}
			else if (pdata->getScpChap(nsc_logoed) < SCP_CHAP_DINT){
				pdata->setScpChap(nsc_logoed, SCP_CHAP_DINT);
			}
			//--- �Ώۈʒu�̍\�������X�V ---
			if (update_logoed == 1){
				ScpArType arstat_st = (unit15s_logost)? SCP_AR_L_UNIT : SCP_AR_L_OTHER;
				ScpArType arstat_ed = (unit15s_logoed)? SCP_AR_N_UNIT : SCP_AR_N_OTHER;
				pdata->setScpArstat(nsc_logost, arstat_st);
				pdata->setScpArstat(nsc_logoed, arstat_ed);
			}
			else if (update_logoed == 2){
				ScpArType arstat_st = (unit15s_logost)? SCP_AR_N_UNIT : SCP_AR_N_OTHER;
				ScpArType arstat_ed = (unit15s_logoed)? SCP_AR_L_UNIT : SCP_AR_L_OTHER;
				pdata->setScpArstat(nsc_logost, arstat_st);
				pdata->setScpArstat(nsc_logoed, arstat_ed);
			}
		}
	}
	return msec_ret;
}



//=====================================================================
// ���S���g�킸CM�\���𐄑�
//=====================================================================

//---------------------------------------------------------------------
// ���S�Ȃ���CM�ʒu�𐄑�
//---------------------------------------------------------------------
bool JlsAutoReform::detectCM(FormAutoCMArg param_autocm){
	AutoCMCount cntset[AUTOCM_NUM_SMAX];

	//--- �p�����[�^�擾 ---
	int  level_cmdet         = param_autocm.levelCmDet;
	Msec msec_tailarea_keep  = param_autocm.msecTailareaKeep;
	Msec msec_tailarea_unit  = param_autocm.msecTailareaUnit;
	Sec  sec_maxcm           = param_autocm.secMaxCm;
	RangeMsec rmsec_headtail = param_autocm.rmsecHeadTail;
	Msec msec_wlogo_trmax    = pdata->getConfig(CONFIG_VAR_msecWLogoTRMax);
	//--- ���S�\����ԓ���15�b�\����CM���̔��f�ǉ��ݒ� ---
	int sub_autocm = pdata->getConfig(CONFIG_VAR_AutoCmSub);
	bool is_sub_cmhead = ((sub_autocm % 10) == 1)? true : false;
	bool is_sub_off45s = ((sub_autocm / 10 % 10) == 1)? true : false;

	//--- �����m�F ---
	if (level_cmdet <= 0){
		if (level_cmdet < 0) pdata->setFlagAutoMode(false);		// �F���ς݂Ŗ����̏�Ԃɖ߂�
		return false;
	}

	//--- ������ ---
	detectCMCount(cntset, AUTOCM_T_INIT, AUTOCM_NUM_SMAX, AUTOCM_NUM_SMAX);

	//--- �擪�ʒu���� ---
	FormFirstInfo info_first;
	if (rmsec_headtail.st <= 0){
		setFirstArea(info_first);
	}
	else{							// �J�n���S�̂̐擪�ł͂Ȃ��ꍇ
		info_first.limitHead = AUTOFIRST_STRICT;	// �擪CM�͌�����
		info_first.nscTreat = pdata->getNscFromMsecChap(rmsec_headtail.st, -1, SCP_CHAP_DECIDE);
	}
	Nsc nsc_last = info_first.nscTreat;
	Nsc nsc_detend = -1;
	if (nsc_last < 0){
		nsc_last = 0;
	}
	//--- CM�����������E�ǉ� ---
	bool flag_final = false;
	AutoCMStateType  state = AUTOCM_ST_S0N_FIRST;
	cntset[AUTOCM_ST_S0N_FIRST].rnsc.st = nsc_last;
	Nsc i = nsc_last;
	while(i >= 0 && flag_final == false){
		//--- ���̈ʒu�ݒ�i�\����؂�ACM�P�ʗD��j ---
		i = pdata->getNscNextScpCheckCmUnit(i, SCP_END_EDGEIN);
		//--- �b���擾�ƍŏI�ʒu���f ---
		if (i >= 0){
			Msec msec_i  = pdata->getMsecScp(i);
			if (msec_i > rmsec_headtail.ed){		// �I���t���[���𒴂�����ŏI�ɃZ�b�g
				flag_final = true;
				nsc_detend = nsc_last;
			}
		}
		else if (flag_final == false){		// �����Ȃ��Ă������O�Ȃ�ŏI�ɃZ�b�g
			int num_scpos = pdata->sizeDataScp();
			flag_final = true;
			i = num_scpos - 1;
		}
		if (i >= 0){
			Msec msec_last = pdata->getMsecScp(nsc_last);
			Msec msec_i    = pdata->getMsecScp(i);
			Sec  secdif_cur = pdata->cnv.getSecFromMsec(msec_i - msec_last);
			bool change = false;
			int cutlevel = level_cmdet;
			bool is_state_logo = false;
			if (state == AUTOCM_ST_S1L_LOGO || state == AUTOCM_ST_S3L_DEFER){
				is_state_logo = true;
			}
			bool flag_cm = false;
			if (((secdif_cur % 15)==0 && secdif_cur <= sec_maxcm) ||
				((secdif_cur % 30)==0 && secdif_cur <= 120 && cutlevel >= 10)){
				if (is_sub_off45s == false || (secdif_cur % 30)==0 || secdif_cur <= 30){
					flag_cm = true;
				}
			}
			int type_mute = 0;
			{	// �������������E�ɃV�[���`�F���W�Ȃ�����CM����O��
				RangeNsc rnsc = {nsc_last, i};
				type_mute = checkMuteMany(rnsc);
				if (type_mute >= 2){
					flag_cm = false;
				}
			}
			bool flag_tailcm = false;
			if (msec_tailarea_keep > 0 && msec_tailarea_keep < msec_i &&
				(msec_i - msec_last <= msec_wlogo_trmax) &&
				(is_state_logo == false || cntset[state].dov15 == 0)){
				flag_tailcm = true;
			}
			if (msec_tailarea_unit > 0 && msec_tailarea_unit < msec_i &&
				(secdif_cur >= 60 && secdif_cur <= 120 && (secdif_cur % 30 == 0))){
				flag_tailcm = true;
			}
			if (is_state_logo == false){			// CM��⌟��
				if (flag_cm || flag_tailcm){
					cntset[state].rnsc.ed = i;
				}
				else{
					change = true;
				}
			}
			else if (is_state_logo == true){		// ���S���Ԍ���
				if (flag_cm || flag_tailcm){
					change = true;
				}
				else{
					cntset[state].rnsc.ed = i;
				}
			}
			if (flag_final){	// �Ō�͋����ݒ�
				change = true;
			}
			//--- �ω������������ɔ��f ---
			if (change){
				//--- CM�������������� ---
				if (is_state_logo == false){
					//--- �擪�ʒu�J�b�g�����̕␳ ---
					if (state == AUTOCM_ST_S0N_FIRST){
						switch (info_first.limitHead){
							case AUTOFIRST_STRICT:		// �擪�J�b�g������
								if (cutlevel > 3){
									cutlevel -= 3;
								}
								else if (cutlevel > 1){
									cutlevel = 1;
								}
								break;
							case AUTOFIRST_NOCUT:		// �擪�J�b�g�Ȃ�
								cutlevel = 0;
								break;
							case AUTOFIRST_LAX:			// �擪�J�b�g�����ɂ�
								if (cutlevel > 0) cutlevel += 3;
								break;
						}
						if (is_sub_cmhead){		// �擪��15�b�P�ʂ�ϋɓI�ɃJ�b�g
							if (cutlevel < 9){
								cutlevel = 9;
							}
						}
					}
					//--- �ŏI�n�_�̕␳ ---
					if (flag_final){
						cutlevel = 10;
					}
					//--- �s�m��摗��n�_�̔��f ---
					if (state == AUTOCM_ST_S4N_JUDGE){
						if ((cntset[AUTOCM_ST_S1L_LOGO].det +
							 cntset[AUTOCM_ST_S3L_DEFER].det <= 4) &&
							(cntset[AUTOCM_ST_S1L_LOGO].sec +
							 cntset[AUTOCM_ST_S3L_DEFER].sec >= 120)){
							//--- �Ώە�����CM�����ɕύX ---
							//--- ��O���S������CM�����łȂ������f��V�t�g ---
							if (cntset[AUTOCM_ST_S1L_LOGO].dov15 > 0){
								detectCMSetLogo(cntset[AUTOCM_ST_S1L_LOGO].rnsc);
							}
							detectCMCount(cntset, AUTOCM_T_SFT, AUTOCM_ST_S0N_FIRST, AUTOCM_ST_S2N_CM);
						}
						else{
							//--- ���S���������Ƃ��Čp�� ---
							detectCMCount(cntset, AUTOCM_T_MRG, AUTOCM_ST_S1L_LOGO, AUTOCM_ST_S3L_DEFER);
						}
						state = AUTOCM_ST_S2N_CM;
					}
					//--- �Ώےn�_�����S�Ȃ������ɂ��邩���f ---
					// cutlevel 0 : �J�b�g�Ȃ�
					//          1 : 4�\���ȏ�CM
					//          2 : 4�\���ȏ�CM�A3�\���̈ꕔ�i���͍\�����j
					//          3 : 4�\���ȏ�CM�A3�\���̈ꕔ
					//          4 : 3�\���ȏ�CM
					//          5 : 3�\���ȏ�CM�A2�\���̈ꕔ�i���͍\�����j
					//          6 : 3�\���ȏ�CM�A2�\���̈ꕔ
					//          7 : 2�\���ȏ�CM
					//          8 : 2�\���ȏ�CM�A1�\���̈ꕔ�i���͍\�����j
					//          9 : 1�\���ȏ�CM
					//          10 : 1�\���ȏ�CM�A120�b�\���܂ŋ���
					int dettype = 0;
					if ((cntset[state].det > 3 && cutlevel > 0) ||
						(cntset[state].det > 2 && cutlevel > 3) ||
						(cntset[state].det > 1 && cutlevel > 6) ||
						(cntset[state].det > 0 && cutlevel > 8)){
						dettype = 1;		// �m��
					}
					else if ((cntset[state].det == 2 &&
							  (cutlevel == 5 || cutlevel == 6) &&
							  (cntset[state].d15 >= 2 || cntset[state].d30 >= 2)) ||
							 (cntset[state].det == 3 &&
							  (cutlevel == 2 || cutlevel == 3) &&
							  (cntset[state].d15 + cntset[state].d30 >= 3)) ||
							 (cntset[state].det == 1 &&
							  (cutlevel == 8) &&
							  (cntset[state].d15 + cntset[state].d30 >= 1))){
						if (state == AUTOCM_ST_S0N_FIRST || cutlevel == 3 || cutlevel == 6){
							dettype = 1;		// �擪�ʒu�A���͊֌W�Ȃ���������m��
						}
						else{
							dettype = 2;		// �����摗��
						}
					}
					//--- ���S�Ȃ������ɂ���ύX���� ---
					if ((dettype == 1) || (dettype == 2 && state == AUTOCM_ST_S0N_FIRST)){
						//--- �Ώە�����CM�����ɕύX ---
						//--- ��O���S������CM�����łȂ������f��V�t�g ---
						if (state != AUTOCM_ST_S0N_FIRST){
							if (cntset[AUTOCM_ST_S1L_LOGO].dov15 > 0){
								detectCMSetLogo(cntset[AUTOCM_ST_S1L_LOGO].rnsc);
							}
							else{
								detectCMCancelShort(cntset[AUTOCM_ST_S1L_LOGO].rnsc);
							}
							detectCMCount(cntset, AUTOCM_T_SFT, AUTOCM_ST_S0N_FIRST, AUTOCM_ST_S2N_CM);
						}
						state = AUTOCM_ST_S1L_LOGO;
						cntset[state].rnsc.st = nsc_last;
						cntset[state].rnsc.ed = i;
					}
					else if (dettype == 2){
						//--- ���f�͐摗�� ---
						state = AUTOCM_ST_S3L_DEFER;
						cntset[state].rnsc.st = nsc_last;
						cntset[state].rnsc.ed = i;
					}
					else if (state == AUTOCM_ST_S0N_FIRST){
						//--- ���S���������Ƃ��Čp�� ---
						state = AUTOCM_ST_S1L_LOGO;
						cntset[state].rnsc.st = cntset[AUTOCM_ST_S0N_FIRST].rnsc.st;
						cntset[state].rnsc.ed = i;
					}
					else{
						//--- ���S���������Ƃ��Čp�� ---
						if (cntset[AUTOCM_ST_S1L_LOGO].dov15 > 0){
							//--- 15�b�ȏ�̃��S���蕔������Ίm�肵�Ēu������ ---
							detectCMSetLogo(cntset[AUTOCM_ST_S1L_LOGO].rnsc);
							detectCMCount(cntset, AUTOCM_T_SFT, AUTOCM_ST_S1L_LOGO, AUTOCM_ST_S2N_CM);
						}
						else{
							//--- 15�b�ȏ�̃��S���蕔���Ȃ���Ό��݂̃��S���蕔���ɒǉ� ---
							detectCMCount(cntset, AUTOCM_T_MRG, AUTOCM_ST_S1L_LOGO, AUTOCM_ST_S2N_CM);
						}
						state = AUTOCM_ST_S1L_LOGO;
//						cntset[state].rnsc.st = nsc_last;
						cntset[state].rnsc.ed = i;
						cntset[state].dov15 = 0;		// 15�b�ȏ�J�E���g�͏�����
					}
				}
				//--- ���S�������������� ---
				else{
					if (flag_final){		// �Ō�ʒu
						if (cntset[state].dov15 > 0){
							detectCMSetLogo(cntset[state].rnsc);
						}
						else{
							detectCMCount(cntset, AUTOCM_T_SFT, AUTOCM_ST_S0N_FIRST, state);
							flag_cm = true;
						}
					}
					// state ++
					if (state == AUTOCM_ST_S3L_DEFER){
						state = AUTOCM_ST_S4N_JUDGE;
					}
					else{
						state = AUTOCM_ST_S2N_CM;
					}
					cntset[state].rnsc.st = nsc_last;
					cntset[state].rnsc.ed = i;
				}
				if (flag_final){				// �Ō�ʒu
					if (flag_cm || flag_tailcm){
						//--- �ŏI�\�����ݒ�Ȃ烍�S�Ȃ��ɃZ�b�g ---
						if (pdata->getScpArstat(i) == SCP_AR_UNKNOWN){
							pdata->setScpArstat(i, SCP_AR_N_OTHER);
						}
					}
				}
			}
			//--- �J�E���g ---
			cntset[state].det ++;
			cntset[state].sec += secdif_cur;
			if (secdif_cur == 15){
				cntset[state].d15 ++;
			}
			else if (secdif_cur == 30){
				cntset[state].d30 ++;
			}
			else if (secdif_cur >= 15){
				cntset[state].dov15 ++;
			}
			nsc_last = i;
		}
	}
	//--- ���S��񂩂�̕␳ ---
	detectCMAssistLogo(info_first.nscTreat, nsc_detend);
	//--- �ŏI�ʒu���� ---
	if (rmsec_headtail.ed >= pdata->getMsecTotalMax()){
		setFinalArea();
	}

	return true;
}

//---------------------------------------------------------------------
// �J�E���g��Ԃ�ύX
// �o�́F
//  cntset  : CM�\����⌟���p�̃J�E���g���
//---------------------------------------------------------------------
void JlsAutoReform::detectCMCount(AutoCMCount *cntset, AutoCMCommandType type, AutoCMStateType st, AutoCMStateType ed){

	if (type == AUTOCM_T_INIT){
		AutoCMStateType stnew = (st == AUTOCM_NUM_SMAX)? AUTOCM_ST_S0N_FIRST : st;
		AutoCMStateType ednew = (ed == AUTOCM_NUM_SMAX)? AUTOCM_ST_S4N_JUDGE : ed;
		//--- ���������� ---
		for(int i=stnew; i<=ednew; i++){
			cntset[i] = {};
		}
	}
	else if (type == AUTOCM_T_SFT){
		//--- �V�t�g���� ---
		for(int i=st; i<AUTOCM_NUM_SMAX; i++){
			if (i < AUTOCM_NUM_SMAX+st-ed){
				cntset[i] = cntset[i+ed-st];
			}
			else{
				cntset[i] = {};
			}
		}
	}
	else if (type == AUTOCM_T_MRG){
		//--- �������� ---
		for(int i=st+1; i<=ed; i++){
			cntset[st].det += cntset[i].det;
			cntset[st].sec += cntset[i].sec;
			cntset[st].d15 += cntset[i].d15;
			cntset[st].d30 += cntset[i].d30;
			cntset[st].dov15 += cntset[i].dov15;
		}
		cntset[st].rnsc.ed = cntset[ed].rnsc.ed;
		//--- �c����V�t�g ---
		for(int i=st+1; i<AUTOCM_NUM_SMAX; i++){
			if (i < AUTOCM_NUM_SMAX+st-ed){
				cntset[i] = cntset[i+ed-st];
			}
			else{
				cntset[i] = {};
			}
		}
	}
}

//---------------------------------------------------------------------
// �Q�_�Ԃ��c���̈�ɐݒ�
//---------------------------------------------------------------------
bool JlsAutoReform::detectCMSetLogo(RangeNsc rnsc){
	if (pdata->getScpChap(rnsc.st) < SCP_CHAP_DFIX){
		pdata->setScpChap(rnsc.st, SCP_CHAP_DFIX);
	}
	if (pdata->getScpChap(rnsc.ed) < SCP_CHAP_DFIX){
		pdata->setScpChap(rnsc.ed, SCP_CHAP_DFIX);
	}
	pdata->setScpArstat(rnsc.ed, SCP_AR_L_OTHER);
	bool cm_inter = false;				// ���S��ԓ���CM�`�F�b�N���Ȃ�
	bool ret = mkRangeInterLogo(rnsc, cm_inter);
	return ret;
}

//---------------------------------------------------------------------
// �Q�_�Ԃ��Z�����̓��S���葮�����폜
//---------------------------------------------------------------------
bool JlsAutoReform::detectCMCancelShort(RangeNsc rnsc){
	if (pdata->getScpChap(rnsc.st) < SCP_CHAP_DFIX ||
		pdata->getScpChap(rnsc.ed) < SCP_CHAP_DFIX){
		return false;
	}
	bool ret = false;
	Msec msec_st = pdata->getMsecScp(rnsc.st);
	Msec msec_ed = pdata->getMsecScp(rnsc.ed);
	Sec sec_dif = pdata->cnv.getSecFromMsec(msec_ed - msec_st);
	//--- �S�̂�15�b�ȉ��̎��̓��S���O�� ---
	if (sec_dif >= 0 && sec_dif < 15){
		int i = rnsc.st;
		while(i > 0 && i < rnsc.ed){
			i = pdata->getNscNextScpChap(i, SCP_CHAP_DFIX);
			if (i > 0 && i <= rnsc.ed){
				ScpArType arstat_i = pdata->getScpArstat(i);
				if (arstat_i == SCP_AR_L_OTHER){
					pdata->setScpArstat(i, SCP_AR_N_OTHER);
					ret = true;
				}
			}
		}
	}
	return ret;
}

//---------------------------------------------------------------------
// ���S�Ȃ�������̃��S�����g���������ǉ�
//---------------------------------------------------------------------
void JlsAutoReform::detectCMAssistLogo(int nsc_det1st, int nsc_detend) {
	int lvlogo = pdata->getLevelUseLogo();
	if (lvlogo <= CONFIG_LOGO_LEVEL_UNUSE_ALL) {		// ���S���g�p���Ȃ��ꍇ
		return;
	}
	Nsc nsc_cur = nsc_det1st;
	while (nsc_cur >= 0 && (nsc_detend < 0 || nsc_cur < nsc_detend)) {
		Nsc nsc_last = nsc_cur;
		nsc_cur = pdata->getNscNextScpChap(nsc_cur, SCP_CHAP_DECIDE);
		if (nsc_cur > 0 && (nsc_detend < 0 || nsc_cur < nsc_detend)) {
			//--- ���ԏ��擾 ---
			Msec msec_st = pdata->getMsecScp(nsc_last);
			Msec msec_ed = pdata->getMsecScp(nsc_cur);
			Sec  sec_full = pdata->cnv.getSecFromMsec(msec_ed - msec_st);
			Sec  sec_logo = pdata->getSecLogoComponentFromLogo(msec_st, msec_ed);
			ScpArType arstat_cur = pdata->getScpArstat(nsc_cur);
			bool flag_logoon_cur = jlsd::isScpArTypeLogo(arstat_cur);
			//--- ���S���̒ǉ� ---
			if (lvlogo >= CONFIG_LOGO_LEVEL_UNUSE_EX1 && flag_logoon_cur == false) {
				if ((sec_full - 1 <= sec_logo) ||
					(sec_full >= 10 && sec_full <= sec_logo * 2)) {
					if ((sec_full + 1) % 15 <= 2) {
						pdata->setScpArstat(nsc_cur, SCP_AR_L_UNIT);
					}
					else {
						pdata->setScpArstat(nsc_cur, SCP_AR_L_OTHER);
					}
				}
			}
			//--- ���S���̍폜 ---
			if (lvlogo >= CONFIG_LOGO_LEVEL_UNUSE_EX2 && flag_logoon_cur == true) {
				if ((sec_logo <= 1) ||
					(sec_full >= 15 && sec_full >= sec_logo * 4)) {
					if ((sec_full + 1) % 15 <= 2) {
						pdata->setScpArstat(nsc_cur, SCP_AR_N_UNIT);
					}
					else {
						pdata->setScpArstat(nsc_cur, SCP_AR_N_OTHER);
					}
				}
			}
		}
	}
}


//=====================================================================
// �Ώ۔͈͂̍\���m�菈��
//=====================================================================

//---------------------------------------------------------------------
// ���S��Ԃ�����Ő����\�����擾���Ȃ���ݒ�
// ���́F
//    nscbounds: ���S���
//    cm_inter:  ������15�b�P��CM���o�ifalse=���Ȃ�  true=CM���o����j
//---------------------------------------------------------------------
bool JlsAutoReform::mkRangeInterLogo(RangeNsc nscbounds, bool cm_inter){
	//--- �ʒu�擾 ---
	RangeMsec bounds = getRangeMsec(nscbounds);

	//--- �͈͓��̃X�R�A�Ƌ�؂���擾 ---
	RangeFixMsec scope_logo;
	scope_logo.st = bounds.st;
	scope_logo.ed = bounds.ed;
	scope_logo.fixSt = true;
	scope_logo.fixEd = true;
	if (bounds.st < 0){
		scope_logo.fixSt = false;
	}
	if (bounds.ed < 0){
		scope_logo.fixEd = false;
	}
	//--- ���[�Œ莞�̕ۑ� ---
	ScpChapType chap_bakst = SCP_CHAP_NONE;
	ScpChapType chap_baked = SCP_CHAP_NONE;
	RangeNsc rnsc_fix = {-1, -1};
	if (scope_logo.fixSt){
		rnsc_fix.st = pdata->getNscFromMsecAll(scope_logo.st);
		if (rnsc_fix.st > 0){
			chap_bakst = pdata->getScpChap(rnsc_fix.st);
		}
	}
	if (scope_logo.fixEd){
		rnsc_fix.ed = pdata->getNscFromMsecAll(scope_logo.ed);
		if (rnsc_fix.ed > 0){
			chap_baked = pdata->getScpChap(rnsc_fix.ed);
		}
	}
	//--- ���s ---
	setScore(scope_logo);
	setChap(scope_logo);
	//--- ���[�Œ莞�̃f�[�^�� ---
	if (scope_logo.fixSt && jlsd::isScpChapTypeDecide(chap_bakst)){
		pdata->setScpChap(rnsc_fix.st, chap_bakst);
	}
	if (scope_logo.fixEd && jlsd::isScpChapTypeDecide(chap_baked)){
		pdata->setScpChap(rnsc_fix.ed, chap_baked);
	}

	//---���S�ݒ� ---
	Nsc nsc_to_ar = nscbounds.ed;
	if (nsc_to_ar < 0){
		nsc_to_ar = pdata->sizeDataScp()-1;
	}
	pdata->setScpArstat(nsc_to_ar, SCP_AR_L_OTHER);

	//--- ���S��Ԑݒ� ---
	bool ret = setInterLogo(nscbounds, cm_inter);
	return ret;
}


//---------------------------------------------------------------------
// �͈͂�CM�\����؂���쐬
// ���́F
//   fixscope  : CM�����̌����͈�
//   logo1st   : �擪�����̃��S��ԁifalse=���S�Ȃ� true=���S����j
//   interfill:  1=CM��CM�̊Ԃ�CM�Ŗ��߂� 2=CM��CM�̊Ԃ�CM�Ŗ��߂�i�����ݒ�j
// �o�́F
//   �Ԃ�l : �\���ǉ��ifalse=�Ȃ�  true=����j
//   bounds : CM�Ɣ��f�����͈�
//---------------------------------------------------------------------
bool JlsAutoReform::mkRangeCM(RangeMsec &bounds, RangeFixMsec fixscope, bool logo1st, int interfill){
	//--- �s���m�������܂߂����S�ʒu(cmscope)�擾 ---
	RangeWideMsec cmscope;
	mkRangeCMGetLogoEdge(cmscope.st, fixscope.st, fixscope.fixSt, LOGO_EDGE_FALL);	// ���S���������肪CM�J�n�ʒu
	mkRangeCMGetLogoEdge(cmscope.ed, fixscope.ed, fixscope.fixEd, LOGO_EDGE_RISE);	// ���S�����オ�肪CM�I���ʒu
	cmscope.fixSt = fixscope.fixSt;
	cmscope.fixEd = fixscope.fixEd;
	cmscope.logomode = false;
	//--- �Œ莞�̃f�[�^�ۑ� ---
	ScpChapType chap_bakst = SCP_CHAP_NONE;
	ScpChapType chap_baked = SCP_CHAP_NONE;
	{
		Nsc nsc_st = pdata->getNscFromMsecAll(fixscope.st);
		Nsc nsc_ed = pdata->getNscFromMsecAll(fixscope.ed);
		if (nsc_st > 0) chap_bakst = pdata->getScpChap(nsc_st);
		if (nsc_ed > 0) chap_baked = pdata->getScpChap(nsc_ed);
	}

	//--- �͈͓��̃X�R�A�Ƌ�؂���擾 ---
	RangeFixMsec scope_chap;
	scope_chap.st = (cmscope.fixSt == false)? cmscope.st.early : cmscope.st.just;
	scope_chap.ed = (cmscope.fixEd == false)? cmscope.ed.late  : cmscope.ed.just;
	scope_chap.fixSt = cmscope.fixSt;
	scope_chap.fixEd = cmscope.fixEd;
	setScore(scope_chap);
	setChap(scope_chap);

	//--- CM�\����؂�쐬 ---
	bool logointer = true;								// CM�Ԃ̒��Ԓn�_�̓��S������
	bool det = setCMForm(bounds, cmscope, logo1st, logointer);

	//--- CM���̕⊮���� ---
	if (det){
		RangeNsc nscbounds;
		pdata->getRangeNscFromRangeMsec(nscbounds, bounds);
		setInterMultiCM(nscbounds, interfill);
	}

	//--- �Œ莞�̃f�[�^�� ---
	if (fixscope.fixSt && isScpChapTypeDecide(chap_bakst) ){
		Nsc nsc_st = pdata->getNscFromMsecAll(fixscope.st);
		pdata->setScpChap(nsc_st, chap_bakst);
	}
	if (fixscope.fixEd && isScpChapTypeDecide(chap_baked) ){
		Nsc nsc_ed = pdata->getNscFromMsecAll(fixscope.ed);
		pdata->setScpChap(nsc_ed, chap_baked);
	}

	return det;
}

//---------------------------------------------------------------------
// �Ώۈʒu�̃��S�ω��n�_�Ƃ��ĉ\���͈͂��擾
// ���́F
//	 msec_target : �������郍�S�ʒu�i�~���b�j
//   flag_fix    : �Ώےn�_���m��ʒu
//	 edge        : �����オ��^����������
// �o�́F
//   �Ԃ�l: ���S���݁ifalse=�Ȃ�  true=����j
//   wmsec : ���S�ʒu
//---------------------------------------------------------------------
bool JlsAutoReform::mkRangeCMGetLogoEdge(WideMsec &wmsec, int msec_target, bool flag_fix, LogoEdgeType edge){
	bool detect = false;
	Msec msec_sftmrg = pdata->getConfig(CONFIG_VAR_msecWLogoSftMrg);

	if (msec_target < 0){
		wmsec.just  = -1;
		wmsec.early = -1;
		wmsec.late  = -1;
	}
	else{
		if (flag_fix){
			wmsec.just  = msec_target;
			wmsec.early = msec_target - pdata->msecValSpc;
			wmsec.late  = msec_target + pdata->msecValSpc;
		}
		else{
			//--- �Ώۈʒu�̃��S�ԍ������� ---
			Nrf nrf = pdata->getNrfLogoFromMsec(msec_target, edge);
			if (nrf >= 0){
				detect = true;
				pdata->getWideMsecLogoNrf(wmsec, nrf);
				wmsec.early -= msec_sftmrg;
				wmsec.late  += msec_sftmrg;
			}
			else{				// �Ώۈʒu�Ƀ��S���Ȃ���Β��S�ʒu���Z�b�g
				wmsec.just  = msec_target;
				wmsec.early = msec_target;
				wmsec.late  = msec_target;
			}
		}
	}
	return detect;
}



//=====================================================================
// �S�̂ɂ�����[�����̐����\��
//=====================================================================

//---------------------------------------------------------------------
// �擪�ʒu�̐����\�����쐬
// �o�́F
//   �Ԃ�l: �X�V�L���ifalse=�Ȃ�  true=����j
//---------------------------------------------------------------------
bool JlsAutoReform::setFirstArea(FormFirstInfo &info_first){
	bool ret = false;
	//--- ���ʏ����l�i�[ ---
	info_first.limitHead = AUTOFIRST_LAX;
	info_first.nscTreat = -1;
	if (pdata->sizeDataScp() <= 2) return false;

	//--- �ݒ�l�i�[ ---
	FormFirstLoc locinfo;
	{
		//--- �擪�D��ʒu�ݒ���擾 ---
		locinfo.msec1stSel = pdata->getConfig(CONFIG_VAR_msecPosFirst);
		locinfo.lvPos1stSel = pdata->getConfig(CONFIG_VAR_priorityPosFirst);
		if (locinfo.lvPos1stSel == 3 && pdata->recHold.msecSelect1st >= 0) {
			locinfo.msec1stSel = pdata->recHold.msecSelect1st;
		}
		locinfo.msec1stZone = pdata->getConfig(CONFIG_VAR_msecZoneFirst);
		//--- �擪�\���J�b�g����ő�擪�ʒu ---
		locinfo.msecWcomp1st = pdata->getConfig(CONFIG_VAR_msecWCompFirst);
		if (locinfo.msecWcomp1st <= 0){
			locinfo.msecWcomp1st = 30 * 1000;			// �����ݒ�
		}
		//--- �����I���ʒu ---
		locinfo.msecEnd1st = 30 * 1000 + pdata->msecValLap2;
		if (locinfo.msec1stSel > 0){
			locinfo.msecEnd1st += locinfo.msec1stSel;
		}
		//--- �擪���S���莞�̃J�b�g����ő�擪�ʒu ---
		locinfo.msecLgCut1st = pdata->getConfig(CONFIG_VAR_msecLgCutFirst);
	}
	//--- �擪���S�ƍ\���m��ʒu���擾 ---
	locinfo.nscLogo1st = -1;
	locinfo.nscDecide = -1;
	locinfo.msecLogo1st = -1;
	locinfo.msecDecide = -1;
	locinfo.rnscLogo1st = {-1, -1};
	bool unuse_lvlogo = pdata->isUnuseLevelLogo();
	if (unuse_lvlogo == false){						// ���S���莞
		//--- ���S�ʒu�擾 ---
		Nrf nrf_1st  = pdata->getNrfNextLogo(-1, LOGO_EDGE_RISE, LOGO_SELECT_VALID);
		//--- ���������肪�ŏ��̖������ԓ��������ꍇ�͎��̃G�b�W�ɂ��� ---
		Nrf nrf_fall = pdata->getNrfNextLogo(nrf_1st, LOGO_EDGE_FALL, LOGO_SELECT_VALID);
		Msec msec_tmp = pdata->getMsecLogoNrf(nrf_fall);
		if (msec_tmp < locinfo.msec1stZone && locinfo.msec1stZone > 0){
			nrf_1st  = pdata->getNrfNextLogo(nrf_fall, LOGO_EDGE_RISE, LOGO_SELECT_VALID);
		}
		//--- �����p���S�͈͎擾 ---
		if (nrf_1st >= 0){
			WideMsec wmsec_logo1st;
			pdata->getWideMsecLogoNrf(wmsec_logo1st, nrf_1st);
			if (wmsec_logo1st.early >= pdata->msecValNear1){	// �擪���烍�S�J�n������Ă����猟��
				wmsec_logo1st.early -= pdata->msecValSpc;
				wmsec_logo1st.late  += pdata->msecValSpc;
				locinfo.nscLogo1st = pdata->getNscFromWideMsecByChap(wmsec_logo1st, SCP_CHAP_NONE);
				if (locinfo.nscLogo1st > 0){
					//--- �Ώۃ��S�����オ��\������͈͂��擾 ---
					for(int i=1; i<pdata->sizeDataScp()-1; i++){
						Msec msec_tmp = pdata->getMsecScp(i);
						if ((msec_tmp >= wmsec_logo1st.early && msec_tmp <= wmsec_logo1st.late) ||
							i == locinfo.nscLogo1st){
							locinfo.rnscLogo1st.ed = i;
							if (locinfo.rnscLogo1st.st < 0){
								locinfo.rnscLogo1st.st = i;
							}
						}
					}
				}
			}
		}
		Msec msec_logo1st_org = pdata->getMsecLogoNrf(nrf_1st);
		if (locinfo.nscLogo1st >= 0){
			locinfo.msecLogo1st = pdata->getMsecScp(locinfo.nscLogo1st);
			//--- 0�n�_�̕����߂��ꍇ�̏��� ---
			if (msec_logo1st_org < locinfo.msecLogo1st - msec_logo1st_org){
				if (locinfo.msecLgCut1st >= 0 && locinfo.msecLgCut1st < locinfo.msecLogo1st){
					locinfo.nscLogo1st = -1;
					locinfo.msecLogo1st = 0;
				}
			}
		}
		else{
			locinfo.msecLogo1st = msec_logo1st_org;
		}
		//--- �擪���烍�S�������ꍇ�͂Ȃ��������Ƃ� ---
		if (locinfo.msecLogo1st < pdata->msecValNear2){	// �擪���烍�S
			if (locinfo.msecLgCut1st < 0){				// �擪����̃��S����Ɉӎ����Ȃ��ꍇ
				if (locinfo.lvPos1stSel > 0){
					locinfo.nscLogo1st = -1;
					locinfo.msecLogo1st = -1;
				}
			}
		}
		//--- �\���m��ʒu�擾�B�����ʒu�O�̎��͂Ȃ��������Ƃ� ---
		locinfo.nscDecide = pdata->getNscNextScpChap(0, SCP_CHAP_DECIDE);
		if (locinfo.nscDecide > 0){
			locinfo.msecDecide  = pdata->getMsecScp(locinfo.nscDecide);
			if (locinfo.msecDecide > locinfo.msecEnd1st){
				locinfo.nscDecide = -1;
				locinfo.msecDecide = -1;
			}
		}
	}
	//--- �擪�t�߂̍\�����m�� ---
	{
		//--- �D��ʒu0�ݒ莞�̏��� ---
		if (locinfo.msec1stSel == 0){
			NrfCurrent logopt = {};
			if ( pdata->getNrfptNext(logopt, LOGO_SELECT_VALID) ){
				WideMsec wmsec_1stlogo;
				pdata->getWideMsecLogoNrf(wmsec_1stlogo, logopt.nrfRise);
				if (wmsec_1stlogo.early <= pdata->msecValNear2){
					info_first.nscTreat = -1;
					info_first.limitHead = AUTOFIRST_NOCUT;		// �擪�J�b�g�Ȃ�
				}
			}
		}
		//--- �擪�ʒu����������ѐݒ� ---
		if (info_first.limitHead != AUTOFIRST_NOCUT){		// �擪�J�b�g�Ȃ��͏���
			bool update = setFirstAreaUpdate(info_first, locinfo);
			if (update) ret = true;
			//--- �擾�����擪�ʒu�����m�胍�S�ʒu�ł���Ίm�肵�čČ��� ---
			if (update && locinfo.msecLogo1st > pdata->msecValLap2){
				int msec_head = pdata->getMsecScp(info_first.nscTreat);
				if (msec_head + pdata->msecValLap2 < locinfo.msecDecide || locinfo.nscDecide < 0){
					setFirstAreaUpdate(info_first, locinfo);
				}
			}
		}
	}
	//--- �擪�ʒu��CM�� ---
	if (info_first.limitHead != AUTOFIRST_NOCUT){		// �擪�J�b�g�Ȃ��͏���
		Nsc nsc_head = pdata->getNscNextScpChap(0, SCP_CHAP_DECIDE);
		Msec msec_head = 0;
		if (nsc_head > 0){
			msec_head = pdata->getMsecScp(nsc_head);
			if (msec_head <= locinfo.msecWcomp1st){
				//--- ���S�ʒu�ɂ�鐧����ǉ� ---
				bool before_logo = false;
				if (unuse_lvlogo){			// ���S�Ȃ���
					before_logo = true;
				}
				else{						// ���S���莞
					if (msec_head <= locinfo.msecLogo1st && locinfo.msecLogo1st >= 0){
						before_logo = true;
					}
				}
				if (before_logo ||
					msec_head <= locinfo.msecLgCut1st || locinfo.msecLgCut1st < 0){
					ret = true;
					info_first.nscTreat = nsc_head;
					pdata->setScpArstat(nsc_head, SCP_AR_N_OTHER);
				}
			}
		}
		//--- �w��ʒu�܂�CM�� ---
		Msec msec_nologo = locinfo.msecLogo1st;
		if (msec_nologo < locinfo.msec1stZone && locinfo.msec1stZone > 0){
			msec_nologo = locinfo.msec1stZone;
		}
		Msec msec_last = 0;
		while(nsc_head > 0 && msec_head <= msec_nologo){
			ScpArType arstat_head = pdata->getScpArstat(nsc_head);
			if (arstat_head != SCP_AR_N_UNIT && arstat_head != SCP_AR_N_OTHER){
				ret = true;
				info_first.nscTreat = nsc_head;
				int sec_dif = pdata->cnv.getSecFromMsec(msec_head - msec_last);
				if (sec_dif > 0 && sec_dif % 15 == 0){
					pdata->setScpArstat(nsc_head, SCP_AR_N_UNIT);
				}
				else{
					pdata->setScpArstat(nsc_head, SCP_AR_N_OTHER);
				}
			}
			msec_last = msec_head;
			nsc_head = pdata->getNscNextScpChap(nsc_head, SCP_CHAP_DECIDE);
			msec_head = pdata->getMsecScp(nsc_head);
		}
	}
	//--- ���S����ŕύX�������A�擪�ʒu����P�\�������ǉ����� ---
	if (ret && pdata->isUnuseLevelLogo() == false){
		Nsc nsc_head = info_first.nscTreat;
		Nsc nsc_next = pdata->getNscNextScpChap(nsc_head, SCP_CHAP_DECIDE);
		if (nsc_head > 0 && nsc_next > 0){
			bool enable_short = true;
			addCommonComponentOne(nsc_head, nsc_next, enable_short);
		}
	}
	return ret;
}

//---------------------------------------------------------------------
// �擪���ʒu���擾�E�X�V
// �o�́F
//   �Ԃ�l: �擪�ʒu�̒ǉ��ifalse=�Ȃ��Atrue=����j
//---------------------------------------------------------------------
bool JlsAutoReform::setFirstAreaUpdate(FormFirstInfo &info_first, FormFirstLoc locinfo){
	int num_scpos = pdata->sizeDataScp();
	AutoFirstType limit_cand = AUTOFIRST_LAX;
	Nsc  nsc_cand = -1;
	int  npoint_cand  = 0;
	Msec msec_hold1st = -1;
	int  lvchap_max = 1;			// �Ώۓ���n�_�Őݒ肷��l
	for(int i=1; i < num_scpos-1; i++){
		Msec msec_i = pdata->getMsecScp(i);
		ScpChapType chap_i = pdata->getScpChap(i);
		//--- �����ʒu�𒴂�����I�� ---
		if (msec_i > locinfo.msecEnd1st){
			break;
		}
		if ((i > locinfo.rnscLogo1st.ed || locinfo.rnscLogo1st.ed < 0) &&	// ���S�͈͒���
			msec_i > locinfo.msecLogo1st  && locinfo.msecLogo1st  >= 0 &&	// ���S�ʒu����
			msec_i > locinfo.msecLgCut1st && locinfo.msecLgCut1st >= 0){	// �����ʒu����
			break;
		}
		//--- �m��ʒu����̋����擾 ---
		int lvchap = 0;
		if (locinfo.nscDecide > 0 || locinfo.nscLogo1st > 0){
			CalcDifInfo calc1;
			if (i == locinfo.nscDecide || (i >= locinfo.rnscLogo1st.st && i <= locinfo.rnscLogo1st.ed)){
				lvchap = lvchap_max;
			}
			else if (locinfo.nscDecide > 0){
				lvchap = calcDifSelect(calc1, msec_i, locinfo.msecDecide);
				//--- �m��\������15�b�����͐��m�ɕb���P�ʂłȂ���Ό�₩��O�� ---
				if (calc1.sec < 15 && (calc1.gap > pdata->msecValExact)){
					lvchap = -1;		// �����ΏۊO
				}
				if (calc1.sec < 3){
					if (locinfo.msec1stSel < 0){	// �擪�D��ʒu�ݒ�Ȃ�
						lvchap = -1;				// �����ΏۊO
					}
					else if (abs(msec_i - locinfo.msec1stSel) > abs(locinfo.msecDecide - locinfo.msec1stSel)){
						lvchap = -1;				// �����ΏۊO
					}
				}
			}
			else if (locinfo.nscLogo1st > 0){
				if (i <= locinfo.nscLogo1st || (locinfo.msec1stSel >= 0 && locinfo.lvPos1stSel >= 1)){
					// ���S��O�܂��̓��S�ȍ~������
					lvchap = 0;
					int lvchap_tmp;
					for(int k=locinfo.rnscLogo1st.st; k<=locinfo.rnscLogo1st.ed; k++){
						lvchap_tmp = calcDifSelect(calc1, msec_i, locinfo.msecLogo1st);
						//--- ���S�n�_����15�b�����͐��m�ɕb���P�ʂłȂ���Ό�₩��O�� ---
						if (calc1.sec < 15 && (calc1.gap > pdata->msecValExact)){
							//--- �擪�D��ʒu�ݒ肩�痣��Ă���ꍇ���ݒ�Ȃ��ꍇ�̂� ---
							if (abs(msec_i - locinfo.msec1stSel) > 3000 || locinfo.msec1stSel < 0){
								lvchap_tmp = 0;
							}
						}
						if (lvchap_tmp > 0){
							lvchap = 1;
							
						}
					}
				}
			}
			if (lvchap > lvchap_max){
				if (i < locinfo.nscLogo1st || i < locinfo.nscDecide){		// �Ώۓ���n�_���O
					lvchap_max = lvchap;						// �Ώۓ���n�_�̒l��ύX
				}
				else{
					lvchap = lvchap_max;					// �Ώۓ���n�_�̒l�ɐ���
				}
			}
		}
		if (lvchap >= 0){
			//--- �擪�ێ� ---
			if (msec_hold1st < 0){			// �ŏ��̍\���ʒu���
				msec_hold1st = msec_i;
			}
			//--- �ʒu�ɂ��D��x ---
			int npoint_i = 0;
			if (locinfo.msec1stSel >= 0){			// �擪�D��ʒu�ݒ肠��
				int msec_dif = abs(msec_i - locinfo.msec1stSel);
				if (msec_dif < 3000){				// 3�b�ȓ��Ȃ�߂��قǗD��
					npoint_i = 3000 - msec_dif;
				}
				if (npoint_i == 0 && msec_hold1st == msec_i){	// �Ȃ���ΐ擪�ʒu������
					npoint_i = 1;
				}
			}
			else if (locinfo.msecLogo1st >= 0 && (locinfo.msecLogo1st + 13000 < locinfo.msecDecide || locinfo.nscDecide < 0)){
				// ���S���m��ʒu���O�ɂ���΃��S�ɋ߂��قǗD��
				Msec msec_dif = abs(msec_i - locinfo.msecLogo1st);
				npoint_i = 3000 - (msec_dif/100);
				if (npoint_i <= 0 && msec_hold1st == msec_i){	// �Ȃ���ΐ擪�ʒu������
					npoint_i = 1;
				}
			}
			else{								// �擪�D��ʒu�ݒ�Ȃ�
				Msec msec_dif = abs(msec_i - msec_hold1st);
				if (msec_dif < 3000){				// 3�b�ȓ��Ȃ�߂��قǗD��
					npoint_i = 3000 - msec_dif;
				}
			}
			AutoFirstType limit_i = AUTOFIRST_LAX;
			if (npoint_i > 0){
				limit_i = AUTOFIRST_STRICT;			// �ʒu�Ŋm��̏ꍇ�͐擪�J�b�g������
			}
			if (locinfo.lvPos1stSel >= 2){			// �ʒu�D��x�����ꍇ
				npoint_i *= 10;
			}
			//--- ���S�ʒu�̗D��x ---
			if (i >= locinfo.rnscLogo1st.st && i <= locinfo.rnscLogo1st.ed &&
				(locinfo.nscLogo1st < locinfo.nscDecide || locinfo.nscDecide < 0) &&
				(abs(locinfo.msecLogo1st - locinfo.msecDecide) > pdata->msecValLap2)){
				if (locinfo.msec1stSel < 0){			// �擪�D��ʒu�ݒ�Ȃ�
					npoint_i = 3001;					// �ʒu�D��ɐݒ�
				}
				if (i == locinfo.nscLogo1st){			// ���S�ŋߎ��ʒu�����������D��
					npoint_i += 2;
				}
			}
			//--- ��؂�ɂ��D��x�ǉ� ---
			if (i >= locinfo.nscDecide && locinfo.nscDecide >= 0){
			}
			else if (chap_i >= SCP_CHAP_DECIDE || lvchap == 2){
				npoint_i += 15000;
			}
			else if (chap_i >= SCP_CHAP_CPOSIT || lvchap == 1){
				npoint_i += 10000;
			}
			//--- �œK�ʒu�̔��f ---
			if ((npoint_cand < npoint_i || nsc_cand < 0) && npoint_i > 0){
				npoint_cand = npoint_i;
				limit_cand = limit_i;
				nsc_cand = i;
			}
		}
	}
	//--- ���ʂ��X�V ---
	bool update = false;
	if (nsc_cand > 0 && (nsc_cand < info_first.nscTreat || info_first.nscTreat < 0)){
		//--- �ʒu�w��ƃ��S�ʒu�̗������鎞�A0�n�_���g�����ǂ�����r ---
		if (locinfo.msec1stSel >= 0 && locinfo.msecLogo1st >= 0){
			Msec msec_cand = pdata->getMsecScp(nsc_cand);
			//--- �Ώېݒ�ʒu�������0�n�_���߂��A��₪���S����ŗ���Ă���ꍇ ---
			if (abs(locinfo.msec1stSel - msec_cand) > locinfo.msec1stSel &&
				msec_cand - locinfo.msecLogo1st > locinfo.msecLogo1st){
				info_first.nscTreat = -1;
				info_first.limitHead = AUTOFIRST_NOCUT;		// �擪�J�b�g�Ȃ�
				return false;
			}
		}
		//--- �\���ǉ��̍X�V ---
		update = true;
		pdata->setScpChap(nsc_cand, SCP_CHAP_DFIX);
		pdata->setScpArstat(nsc_cand, SCP_AR_L_OTHER);	// �O�̂��ߒǉ�
		info_first.limitHead = limit_cand;
		info_first.nscTreat = nsc_cand;
	}

	return update;
}


//---------------------------------------------------------------------
// �ŏI�ʒu�̐����\�����쐬
// �o�́F
//   �Ԃ�l�F �X�V�L���ifalse=�Ȃ�  true=����j
//---------------------------------------------------------------------
bool JlsAutoReform::setFinalArea(){
	int num_scpos = pdata->sizeDataScp();
	int ret = false;
	//--- �Ō�D��ʒu�ݒ���擾 ---
	Nsc  nsc_total_last = num_scpos - 1;
	Msec msec_total_last  = pdata->getMsecScp(nsc_total_last);
	Msec msec_var_lastzone = pdata->getConfig(CONFIG_VAR_msecZoneLast);
	Msec msec_lastzone = -1;
	if (msec_var_lastzone >= 0){
		msec_lastzone = msec_total_last - msec_var_lastzone;
	}
	//--- �D��ʒu�ɂȂ����J�b�g����ő�Ō�b�� ---
	Msec msec_wcomp_lastloc = msec_total_last - pdata->getConfig(CONFIG_VAR_msecWCompLast);
	if (msec_wcomp_lastloc >= msec_total_last){
		msec_wcomp_lastloc = msec_total_last - 30 * 1000;			// �����ݒ�
	}

	//--- �ŏI�\�����ݒ�Ȃ烍�S����ɃZ�b�g ---
	if (pdata->getScpArstat(nsc_total_last) == SCP_AR_UNKNOWN){
			pdata->setScpArstat(nsc_total_last, SCP_AR_L_OTHER);
	}

	//--- �ŏI���S���擾 ---
	Nsc  nsc_logolast = -1;
	Msec msec_logolast = -1;
	int  lvlogo = pdata->getLevelUseLogo();
	if (lvlogo >= CONFIG_LOGO_LEVEL_USE_LOW){
		//--- ���S�ʒu�擾 ---
		Nlg nlg_last = pdata->sizeDataLogo()-1;
		Nrf nrf_last = jlsd::nrfFromNlg(nlg_last, LOGO_EDGE_FALL);
		nrf_last = pdata->getNrfPrevLogo(nrf_last+1, LOGO_EDGE_FALL, LOGO_SELECT_VALID);
		//--- �����オ�肪�Ō�̖������ԓ��������ꍇ�͎��̃G�b�W�ɂ��� ---
		Nrf nrf_rise = pdata->getNrfPrevLogo(nrf_last, LOGO_EDGE_RISE, LOGO_SELECT_VALID);
		Msec msec_tmp = pdata->getMsecScp(nrf_rise);
		if (msec_tmp > msec_lastzone && msec_lastzone > 0){
			nrf_last  = pdata->getNrfPrevLogo(nrf_rise, LOGO_EDGE_FALL, LOGO_SELECT_VALID);
		}
		nsc_logolast = pdata->getNscFromNrf(nrf_last, 3000, SCP_CHAP_NONE);
		if (nsc_logolast > 0){
			msec_logolast = pdata->getMsecScp(nsc_logolast);
		}
		else{
			msec_logolast = pdata->getMsecLogoNrf(nrf_last);
		}
	}

	//--- �Ō�\���̓����������� ---
	if (msec_var_lastzone >= 0){			// -1�ݒ莞�͏������Ȃ�
		ScpArType arstat_last = pdata->getScpArstat(nsc_total_last);
		bool flag_last_logo = jlsd::isScpArTypeLogo(arstat_last);
		Nsc  nsc_last_decide = pdata->getNscPrevScpChap(nsc_total_last, SCP_CHAP_DECIDE);
		Msec msec_last_decide = pdata->getMsecScp(nsc_last_decide);
		if (nsc_last_decide > 0 && msec_last_decide < msec_lastzone){
			RangeNsc rnsc_last = {nsc_last_decide, -1};
			setInterpolar(rnsc_last, flag_last_logo);
		}
	}
	//--- �Ō�t�߂�15�b�P�ʍ\���ǉ� ---
	{
		ScpArType arstat_last = pdata->getScpArstat(nsc_total_last);
		bool flag_last_logo = jlsd::isScpArTypeLogo(arstat_last);
		Nsc nsc_last_decide = pdata->getNscPrevScpChap(nsc_total_last, SCP_CHAP_DECIDE);
		Nsc nsc_last_cdet   = pdata->getNscPrevScpChap(nsc_total_last, SCP_CHAP_CDET);
		//--- �Ō�t�߂̍\�����m�F ---
		if (nsc_last_cdet > 0){
			Msec msec_last_decide = pdata->getMsecScp(nsc_last_decide);
			Msec msec_last_cdet   = pdata->getMsecScp(nsc_last_cdet);
			//--- �m��ʒu�̌��15�b�P�ʈʒu������ꍇ ---
			if ((nsc_last_decide < nsc_last_cdet) && (nsc_last_decide > 0)){
				Msec msec_dif = msec_last_cdet - msec_last_decide;
				CalcModInfo calcmod;
				int modtype = calcDifMod0515(calcmod, msec_dif);
				if (modtype >= 2){
					ret = true;
					pdata->setScpChap(nsc_last_cdet, SCP_CHAP_DFIX);
					if (flag_last_logo) {
						pdata->setScpArstat(nsc_last_cdet, SCP_AR_L_UNIT);
					}
					else{
						pdata->setScpArstat(nsc_last_cdet, SCP_AR_N_UNIT);
					}
					RangeNsc rnsc_last = {nsc_last_decide, nsc_last_cdet};
					setInterpolar(rnsc_last, flag_last_logo);
				}
				else{
					//--- 15�b�P�ʈʒu�̍Ō�Q�̍\�� ---
					Nsc nsc_last2_cdet = pdata->getNscPrevScpChap(nsc_last_cdet, SCP_CHAP_CDET);
					if (nsc_last_decide < nsc_last2_cdet){
						int msec_last2_cdet = pdata->getMsecScp(nsc_last2_cdet);
						if (msec_last2_cdet - msec_last_decide > pdata->msecValLap2){
							msec_dif = msec_last_cdet - msec_last2_cdet;
							modtype = calcDifMod0515(calcmod, msec_dif);
							if (modtype >= 2) {
								ret = true;
								pdata->setScpChap(nsc_last2_cdet, SCP_CHAP_DFIX);
								pdata->setScpChap(nsc_last_cdet, SCP_CHAP_DFIX);
								if (flag_last_logo) {
									pdata->setScpArstat(nsc_last2_cdet, SCP_AR_L_OTHER);
									pdata->setScpArstat(nsc_last_cdet, SCP_AR_L_UNIT);
								}
								else {
									pdata->setScpArstat(nsc_last2_cdet, SCP_AR_N_OTHER);
									pdata->setScpArstat(nsc_last_cdet, SCP_AR_N_UNIT);
								}
							}
						}
					}
				}
			}
		}
	}
	//--- �Ō�̍\����ǉ��m�F ---
	if (msec_var_lastzone >= 0){			// -1�ݒ莞�͏������Ȃ�
		Nsc nsc_last_decide = pdata->getNscPrevScpChap(nsc_total_last, SCP_CHAP_DECIDE);
		Nsc nsc_last_cand   = pdata->getNscPrevScpChap(nsc_total_last, SCP_CHAP_NONE);
		if (nsc_last_decide < nsc_last_cand && nsc_last_decide > 0){
			Msec msec_last_decide = pdata->getMsecScp(nsc_last_decide);
			Msec msec_last_cand   = pdata->getMsecScp(nsc_last_cand);
			CalcDifInfo calcdif;
			int diftype = calcDifSelect(calcdif, msec_last_decide, msec_last_cand);
			if (diftype > 0 && calcdif.gap <= pdata->msecValExact){
			}
			else{		// �ΏۊO�Ȃ�P�O���m�F
				Nsc nsc_tmp   = pdata->getNscPrevScpChap(nsc_last_cand, SCP_CHAP_NONE);
				if (nsc_last_decide < nsc_tmp){
					nsc_last_cand  = nsc_tmp;
					msec_last_cand = pdata->getMsecScp(nsc_last_cand);
					diftype = calcDifSelect(calcdif, msec_last_decide, msec_last_cand);
				}
			}
			if (diftype > 0 && calcdif.gap <= pdata->msecValExact){
				if (msec_last_cand >= msec_wcomp_lastloc && msec_wcomp_lastloc > 0 &&
					msec_last_decide < msec_lastzone){
					ScpArType arstat_last = pdata->getScpArstat(nsc_total_last);
					bool flag_last_logo = jlsd::isScpArTypeLogo(arstat_last);
					ret = true;
					pdata->setScpChap(nsc_last_cand, SCP_CHAP_DFIX);
					if (flag_last_logo) {
						pdata->setScpArstat(nsc_last_cand, SCP_AR_L_OTHER);
					}
					else{
						pdata->setScpArstat(nsc_last_cand, SCP_AR_N_OTHER);
					}
					RangeNsc rnsc_last = {nsc_last_decide, nsc_last_cand};
					setInterpolar(rnsc_last, flag_last_logo);
				}
			}
		}
	}

	//--- �w��ӏ��ȍ~�̓��S�Ȃ� ---
	if (msec_lastzone > 0 || msec_logolast > 0){
		Nsc  nsc_last_start = nsc_total_last;
		Msec msec_endtarget = msec_lastzone;
		if ((msec_lastzone > msec_logolast && msec_logolast > 0) || msec_lastzone <= 0){
			msec_endtarget = msec_logolast;
		}
		bool checkend = false;
		//--- �w��ӏ��ɑΉ�����ʒu�̍\�����o ---
		while(checkend == false && nsc_last_start > 0){
			Nsc  nsc_tmp  = pdata->getNscPrevScpChap(nsc_last_start, SCP_CHAP_DECIDE);
			Msec msec_tmp = pdata->getMsecScp(nsc_tmp);
			if (msec_tmp < msec_endtarget || nsc_tmp < 0){
				checkend = true;
			}
			else{
				nsc_last_start = nsc_tmp;
			}
		}
		//--- �w��ӏ��ȍ~�̓��S�Ȃ��ɂ��� ---
		if (nsc_last_start > 0 && nsc_last_start < nsc_total_last){
			//--- ���O�̃��S��� ---
			Nsc nsc_cur = nsc_last_start;
			do{
				//--- ���̍\�����o�i�I�����͍Ō�̏ꏊ�ɐݒ�j---
				nsc_cur = pdata->getNscNextScpChap(nsc_last_start, SCP_CHAP_DECIDE);
				if (nsc_cur < 0){
					nsc_cur = nsc_total_last;
				}
				ScpArType arstat_cur = pdata->getScpArstat(nsc_cur);
				bool flag_logo_cur = jlsd::isScpArTypeLogo(arstat_cur);
				if (flag_logo_cur){
					ret = true;
					if (arstat_cur == SCP_AR_L_OTHER){
						pdata->setScpArstat(nsc_cur, SCP_AR_N_OTHER);
					}
					else if (arstat_cur == SCP_AR_L_UNIT){
						pdata->setScpArstat(nsc_cur, SCP_AR_N_UNIT);
					}
				}
				nsc_last_start = nsc_cur;
			} while(nsc_cur > 0 && nsc_cur < nsc_total_last);
		}
	}

	//--- �ŏI�\���̃��S�Ȃ��ύX���f ---
	if (msec_wcomp_lastloc > 0){
		Nsc  nsc_prev = pdata->getNscPrevScpChap(nsc_total_last, SCP_CHAP_DECIDE);
		Msec msec_prev = pdata->getMsecScp(nsc_prev);
		if (msec_prev > msec_wcomp_lastloc){
			ret = true;
			pdata->setScpArstat(nsc_total_last, SCP_AR_N_OTHER);
		}
	}

	return ret;
}



//=====================================================================
// �Q�_�Ԃ̍\���ݒ�
//=====================================================================

//---------------------------------------------------------------------
// �m��Q�_�Ԃ����S�ݒ� - ������CM�\�������o
// ���́F
//    nscbounds: �m��Q�_
//    cm_inter:  ������15�b�P��CM���o�ifalse=���Ȃ�  true=CM���o����j
// �o�́F
//   �Ԃ�l: �\���ǉ��ifalse=�Ȃ�  true=����j
//---------------------------------------------------------------------
bool JlsAutoReform::setInterLogo(RangeNsc nscbounds, bool cm_inter){
	//--- ���S�\����ԓ���15�b�\����CM�����鏈�� ---
	bool rev_del_mid  = (pdata->getConfigAction(CONFIG_ACT_LogoDelMid))? true : false;
	//--- 15�b�P�ʂ̍\�����o ---
	bool det = false;
	if (rev_del_mid && cm_inter){		// ������CM�����o
		//--- �͈͐ݒ� ---
		RangeMsec bounds = getRangeMsec(nscbounds);
		//--- CM�\���ʒu���o ---
		RangeMsec rmsec_dmy;
		RangeWideMsec cmscope;
		cmscope.st = {bounds.st, bounds.st, bounds.st};
		cmscope.ed = {bounds.ed, bounds.ed, bounds.ed};
		cmscope.fixSt = true;
		cmscope.fixEd = true;
		cmscope.logomode = true;
		bool logo1st = true;
		bool logointer = true;
		det = setCMForm(rmsec_dmy, cmscope, logo1st, logointer);
	}
	//--- ���S��Ԑݒ� ---
	if (det == false){		// �Ԃ�CM���Ȃ���ΑS�̂̍\������
		bool logomode = true;
		det = setInterpolar(nscbounds, logomode);
	}
	else{					// �Ԃ�CM�������CM�������č\������
		//--- �͈͐ݒ� ---
		int num_scpos = pdata->sizeDataScp();
		RangeNsc nscope = nscbounds;
		if (nscope.st < 0) nscope.st = pdata->getNscNextScpChap(0, SCP_CHAP_DECIDE);
		if (nscope.ed < 0) nscope.ed = pdata->getNscPrevScpChap(num_scpos-1, SCP_CHAP_DECIDE);
		//--- CM�\�����o ---
		RangeNsc cmterm = {-1, -1};	// CM���J�n�I���ʒu
		int count    = 0;			// CM���\�����J�E���g
		Nsc nsc_fin = nscope.st;	// �⊮�������Ă���ʒu
		Nsc nsc_cur = nscope.st;
		while(nsc_cur >= nscope.st && nsc_cur < nscope.ed && nsc_cur >= 0){
			Nsc nsc_last = nsc_cur;
			nsc_cur = pdata->getNscNextScpChap(nsc_cur, SCP_CHAP_DECIDE);
			if (nsc_cur >= nscope.st && nsc_cur <= nscope.ed){
				bool det15s = (pdata->getScpArstat(nsc_cur) == SCP_AR_L_UNIT)? true : false;
				Msec msec_last = pdata->getMsecScp(nsc_last);
				Msec msec_cur  = pdata->getMsecScp(nsc_cur);
				CalcDifInfo calc1;
				int type1 = calcDifSelect(calc1, msec_last, msec_cur);
				//--- 30�b�܂łɐ��� ---
				if (calc1.sec > 31){
					det15s = false;
				}
				//--- �Z���ԃ��S��Ԃ́i�[�ȊO�́jCM�����Ƃ��� ---
				bool shortcm = (nsc_fin == nscope.st)? false : true;
				//--- CM�\�������o�� ---
				if (det15s && type1 == 2){
					if (count == 0) cmterm.st = nsc_last;
					cmterm.ed = nsc_cur;
					count ++;
				}
				//--- CM�ȊO�̍\�������o�� ---
				else{
					if (count > 0){
						//--- �r���ʒu�������ꍇ ---
						if (cmterm.st > nscope.st){
							if (rev_del_mid && count >= 2){
								int msec_st = pdata->getMsecScp(cmterm.st);
								int msec_ed = pdata->getMsecScp(cmterm.ed);
								CalcDifInfo calc2;
								calcDifSelect(calc2, msec_st, msec_ed);
								if (calc2.sec % 30 == 0){		// 30�b�P�ʂ̎��ɔF��
									setInterLogoUpdate(nsc_fin, cmterm, shortcm);
									det = true;
								}
							}
						}
						nsc_fin = cmterm.ed;
						count = 0;
					}
				}
				//--- �ŏI�ʒu�̏��� ---
				if (nsc_cur == nscope.ed){
					cmterm = {nsc_cur, nsc_cur};		// ���ԂȂ��ōŏI�ʒu���Z�b�g
					bool det_tmp = setInterLogoUpdate(nsc_fin, cmterm, shortcm);
					if (det_tmp) det = true;
				}
			}
		}
	}
	return det;
}

//---------------------------------------------------------------------
// �m��Q�_�Ԃ����S�ݒ�̍X�V
// ���́F
//   nsc_fin:   �O�񊮗��ʒu
//   cmterm:    CM���J�n�I���ʒu
//   shortcm:   true=CM�O�̒Z���ԍ\����CM��
// �o�́F
//   �Ԃ�l:    �\���ǉ��ifalse=�Ȃ�  true=����j
//---------------------------------------------------------------------
bool JlsAutoReform::setInterLogoUpdate(Nsc nsc_fin, RangeNsc cmterm, bool shortcm){
	bool det = false;
	//--- CM���J�n�ʒu���O�̃��S���蕔�������⊮ ---
	if (nsc_fin >= 0 && nsc_fin < cmterm.st){
		//--- �����̊m��ӏ��͌���Ԃɖ߂� ---
		Nsc nsc_cur = nsc_fin;
		nsc_cur = pdata->getNscNextScpChap(nsc_cur, SCP_CHAP_DECIDE);
		while(nsc_cur > 0 && nsc_cur < cmterm.st){
			pdata->setScpChap(nsc_cur, SCP_CHAP_CDET);
			nsc_cur = pdata->getNscNextScpChap(nsc_cur, SCP_CHAP_DECIDE);
		}
		//--- �Z���ԃ��S�����̏��� ---
		if (shortcm){
			Msec msec_fin = pdata->getMsecScp(nsc_fin);
			Msec msec_st  = pdata->getMsecScp(cmterm.st);
			if (abs(msec_st - msec_fin) < 15*1000 - pdata->pdata->msecValLap2){
				pdata->setScpArstat(cmterm.st, SCP_AR_N_OTHER);
			}
		}
		//--- �⊮���� ---
		bool logomode = true;
		RangeNsc rnsc_target = {nsc_fin, cmterm.st};
		det = setInterpolar(rnsc_target, logomode);			// �����\������
	}
	//--- CM������ ---
	if (cmterm.st >= 0 && cmterm.st < cmterm.ed){
		det = true;
		Nsc nsc_cur = cmterm.st;
		nsc_cur = pdata->getNscNextScpChap(nsc_cur, SCP_CHAP_DECIDE);
		while(nsc_cur > 0 && nsc_cur <= cmterm.ed){
			pdata->setScpArstat(nsc_cur, SCP_AR_N_UNIT);
			nsc_cur = pdata->getNscNextScpChap(nsc_cur, SCP_CHAP_DECIDE);
		}
	}
	return det;
}


//---------------------------------------------------------------------
// �J�b�g���ԓ��̊m��Q�_�Ԃɕb���P�ʂƂȂ�\��������Βǉ��i�{�����\���̋���CM�������j
// ���́F
//   nscbounds:  �J�n�I���\���ԍ�
//   interfill:  1=CM��CM�̊Ԃ�CM�Ŗ��߂� 2=CM��CM�̊Ԃ�CM�Ŗ��߂�i�����ݒ�j
// �o�́F
//   �Ԃ�l:     �X�V�L���ifalse=�Ȃ�  true=����j
//---------------------------------------------------------------------
bool JlsAutoReform::setInterMultiCM(RangeNsc nscbounds, int interfill){
	//--- ���S�Ȃ���Ԃ�CM�O�\���ɂ��Đݒ�擾 ---
	Msec msec_rev_logo = pdata->getConfig(CONFIG_VAR_msecWLogoRevMin);
	int rev_logo = pdata->getConfigAction(CONFIG_ACT_LogoUCRemain);
	bool rev_add = (rev_logo > 0)? true : false;

	int upcnt = 0;
	Nsc nsc_cur = nscbounds.st;
	while(nsc_cur >= nscbounds.st && nsc_cur <= nscbounds.ed && nsc_cur >= 0){
		Nsc nsc_last = nsc_cur;
		nsc_cur = pdata->getNscNextScpChap(nsc_cur, SCP_CHAP_DECIDE);
		if (nsc_cur >= nscbounds.st && nsc_cur <= nscbounds.ed){
			//--- ���S�\���͋����I�Ƀ��S�Ȃ��ɕύX���鏈�� ---
			bool logomode = false;						// ���S�O�Ƃ���ݒ�
			bool check_inter = (interfill > 0)? true : false;	// CM�Ŗ��߂�ꍇ�͖������œ����\���`�F�b�N
			ScpArType arstat_cur = pdata->getScpArstat(nsc_cur);
			if ( jlsd::isScpArTypeLogo(arstat_cur) ){
				logomode = true;
				if (interfill > 0){				// CM���Ŗ��߂�ꍇ
					Msec msec_last = pdata->getMsecScp(nsc_last);
					Msec msec_cur  = pdata->getMsecScp(nsc_cur);
					Msec msec_dif  = msec_cur - msec_last;
					if (rev_add == false || msec_dif <= msec_rev_logo || interfill == 2){
						pdata->setScpArstat(nsc_cur, SCP_AR_N_OTHER);
						logomode = false;
					}
				}
			}
			else{						// CM�̎��͖������œ����\���`�F�b�N
				check_inter = true;
			}
			if (check_inter){
				RangeNsc rnsc_target = {nsc_last, nsc_cur};
				bool update = setInterpolar(rnsc_target, logomode);
				if (update) upcnt ++;
			}
		}
	}
	bool det = (upcnt > 0)? true : false;
	return det;
}

//---------------------------------------------------------------------
// �m��Q�_�Ԃ�ݒ肵�b���P�ʂƂȂ�\��������Βǉ�
// ���́F
//   rnsc_target: �J�n�I���\���ԍ�
//   logomode:    false:���S�O  true:���S��
// �o�́F
//   �Ԃ�l:  �X�V�L���ifalse=�Ȃ�  true=����j
//---------------------------------------------------------------------
bool JlsAutoReform::setInterpolar(RangeNsc rnsc_target, bool logomode){
	RangeNscMsec target;
	target.nsc = rnsc_target;
	//--- �擪�ƍŏI�̎w��Ȃ��͍\���n�_���珜�� ---
	int nside = 3;							// 1:�J�n������m�F  2:�I��������m�F  3:��������m�F
	if (target.nsc.st < 0){
		target.nsc.st = 0;
		nside &= 0x2;
	}
	if (target.nsc.ed < 0){
		target.nsc.ed = pdata->sizeDataScp()-1;
		nside &= 0x1;
	}
	if (nside == 0){						// �m�F�Ȃ��Ȃ�I��
		return false;
	}
	//--- ������ ---
	for(Nsc i = target.nsc.st + 1; i < target.nsc.ed; i++){
		setInterpolarClearChap(i);					// �����̍\��������
	}
	target.msec = getRangeMsec(target.nsc);

	int upcnt = 0;
	Nsc nsc_fix = target.nsc.st;
	//--- �J�n�ʒu����I���ʒu�܂Ō��� ---
	{
		TraceInterpolar trace;
		trace.keepType = 0;
		trace.keepGap = 0;
		trace.nscKeep = -1;
		trace.nscBase = target.nsc.st;
		trace.keep15s = false;
		for(Nsc i = target.nsc.st + 1; i <= target.nsc.ed; i++){
			if ((pdata->getScpStill(i) == false) ||		// �摜�ω����邱�ƕK�{
				(i == target.nsc.ed)){					// �ŏI�ʒu
				bool update = setInterpolarDetect(trace, i, target, logomode, nside);
				if (update) upcnt ++;
			}
		}
		nsc_fix = trace.nscBase;		// �m��ʒu��ݒ�
	}
	//--- �I���ʒu����J�n�ʒu�i�m��ӏ�����Ίm��ʒu�j�܂Ō��� ---
	{
		TraceInterpolar trace;
		trace.keepType = 0;
		trace.keepGap = 0;
		trace.nscKeep = -1;
		trace.nscBase = target.nsc.ed;
		trace.keep15s = false;
		for(Nsc i = target.nsc.ed - 1; i >= nsc_fix; i--){
			if ((pdata->getScpStill(i) == false) ||			// �摜�ω����邱�ƕK�{
				(i == nsc_fix)){						// �ŏI�ʒu
				bool update = setInterpolarDetect(trace, i, target, logomode, nside);
				if (update) upcnt ++;
			}
		}
	}
	//--- ����\���̒ǉ����� ---
	{
		bool update = setInterpolarExtra(target, logomode);
		if (update) upcnt ++;
	}
	bool det = (upcnt > 0)? true : false;
	return det;
}

//---------------------------------------------------------------------
// ��؂肪�m�肵�Ă������͏���
//---------------------------------------------------------------------
void JlsAutoReform::setInterpolarClearChap(Nsc nsc_n){
	int num_scpos = pdata->sizeDataScp();
	ScpChapType chap_n, chap_set;

	if (nsc_n <= 0 || nsc_n >= num_scpos - 1){
		return;
	}
	chap_n = pdata->getScpChap(nsc_n);
	if (chap_n >= SCP_CHAP_DECIDE){
		if (chap_n >= SCP_CHAP_DFIX){
			chap_set = SCP_CHAP_CDET;
		}
		else{
			chap_set = SCP_CHAP_NONE;
		}
		pdata->setScpChap(nsc_n, chap_set);
	}
}

//---------------------------------------------------------------------
// �m��Q�_�Ԃɕb���P�ʂƂȂ�\�������o
// ���́F
//   trace      
//     keepType : �ێ��\���̎�ށi0=�ێ��Ȃ� 1=��₠�� 2=����̑傫����₠��j
//     keepGap :  �ێ��\���ꏊ�̌덷�~���b
//     nscKeep :  �ێ��\���ԍ�
//     nscBase :  �m��\���ԍ�
//   nsc_cur:     ���ݍ\���ԍ�
//   target:      �m��Q�_�V�[���`�F���W�ʒu
//   logomode:    0:���S�O  1:���S��
//   nside:       1:�J�n������m�F  2:�I��������m�F  3:��������m�F
// �o�́F
//   �Ԃ�l:     �X�V�L���ifalse=�Ȃ�  true=����j
//---------------------------------------------------------------------
bool JlsAutoReform::setInterpolarDetect(TraceInterpolar &trace, Nsc nsc_cur, RangeNscMsec target, bool logomode, int nside){
	int num_scpos = pdata->sizeDataScp();
	bool update = false;
	Msec msec_val_lap2  = pdata->msecValLap2;
	Msec msec_val_near2 = pdata->msecValNear2;
	Msec msec_cur  = pdata->getMsecScp(nsc_cur);
	Msec msec_base = pdata->getMsecScp(trace.nscBase);
	Msec msec_mid  = (target.msec.st + target.msec.ed) / 2;
	bool flag_last = ((nsc_cur == target.nsc.st) || (nsc_cur == target.nsc.ed))? true : false;

	//--- �m��ꏊ�߂��ł͌��o���Ȃ� ---
	if (nsc_cur != trace.nscBase && nsc_cur != target.nsc.st && nsc_cur != target.nsc.ed){
		if (abs(msec_cur - msec_base) < pdata->msecValLap2 ||
			abs(msec_cur - target.msec.st) < pdata->msecValLap2 ||
			abs(msec_cur - target.msec.ed) < pdata->msecValLap2){
			return false;
		}
	}
	//--- ���e�덷�ݒ� ---
	Msec mgn_cm_detect = (Msec) pdata->getConfig(CONFIG_VAR_msecMgnCmDetect);
	Msec mgn_cm_divide  = (Msec) pdata->getConfig(CONFIG_VAR_msecMgnCmDivide);
	Msec msec_mgn = mgn_cm_detect;
	if (mgn_cm_divide < mgn_cm_detect){
		if (abs(target.msec.ed - target.msec.st) <= 15*1000 + pdata->msecValNear2){
			msec_mgn = mgn_cm_divide;		// 15�b�ȓ��\���̕����͌������p�����[�^�ݒ�g�p
		}
	}
	//--- ���e�덷���L������ʒu�̊m�F ---
	bool flag_curmgn = false;
	{
		CalcDifInfo calc_st;
		CalcDifInfo calc_ed;
		calcDifSelect(calc_st, target.msec.st, msec_cur);
		calcDifSelect(calc_ed, msec_cur, target.msec.ed);
		if ((calc_st.sec % 5) == 0 && (calc_st.gap <= pdata->msecValNear2) &&
			(calc_ed.sec % 5) == 0 && (calc_ed.gap <= pdata->msecValNear2)){
			flag_curmgn = true;		// ���[����Ƃ���5�b�P�ʂȂ狖�e�덷���L������
		}
	}
	Msec msec_curmgn = msec_mgn;
	if (flag_curmgn){
		if (msec_curmgn < pdata->msecValNear2){	// ���e�͈͂��������͍L��
			msec_curmgn = pdata->msecValNear2;
		}
	}

	CalcDifInfo calc1;
	int type1 = calcDifSelect(calc1, msec_base, msec_cur);
	if (calc1.gap > msec_curmgn) type1 = 0;
	//--- �O�ʒu�m��i�ԑg�\���ǉ��j ---
	if (trace.nscKeep > 0){
		Msec msec_keep = pdata->getMsecScp(trace.nscKeep);
		Msec msec_dif_keepcur  = abs(msec_cur - msec_keep);
		Msec msec_dif_keepbase = abs(msec_base - msec_keep);
		if (msec_dif_keepcur > msec_val_lap2 || flag_last){
			CalcDifInfo calc2;
			int type2 = 0;
			if (trace.keepType == 2 || flag_last){
				type2 = calcDifSelect(calc2, msec_keep, msec_cur);
			}
			//--- ������ɂ����锻�� ---
			bool flag_keepsteady = false;
			{
				CalcDifInfo calc_st;
				CalcDifInfo calc_ed;
				calcDifSelect(calc_st, target.msec.st, msec_keep);
				calcDifSelect(calc_ed, msec_keep, target.msec.ed);
				if ((calc_st.sec % 5) == 0 && (calc_st.gap <= pdata->msecValNear2) &&
					(calc_ed.sec % 5) == 0 && (calc_ed.gap <= pdata->msecValNear2)){
					flag_keepsteady = true;		// ���[����Ƃ���5�b�P�ʂȂ狖�e���L������
				}
			}
			//--- �]���Ȗ������������͊O������ ---
			bool flag_mute = false;
			if (abs(trace.nscBase - trace.nscKeep) > 1){
				RangeNsc rnsc;
				if (trace.nscBase > trace.nscKeep){
					rnsc = {trace.nscKeep, trace.nscBase};
				}
				else{
					rnsc = {trace.nscBase, trace.nscKeep};
				}
				int type_mute = checkMuteMany(rnsc);
				if (type_mute >= 2){		// �������ُ�ɑ����ꍇ�͖�����
					flag_mute = true;
				}
				else if (type_mute == 1){	// ������������x���݂���ꍇ�͔���
					if (msec_dif_keepbase >= 29500){	// ����������Ă����疳����
						flag_mute = true;
					}
					else if (trace.keep15s == false){		// ����15�b�P�ʍ\���łȂ���Ζ�����
						if (flag_keepsteady == false){
							flag_mute = true;
						}
					}
				}
				//--- �����̔��f�𖳌��ɂ���ݒ�̏ꍇ ---
				bool calcel_cntsc = pdata->getConfig(CONFIG_VAR_cancelCntSc);
				if (calcel_cntsc){
					flag_mute = false;
				}
			}
			if (flag_mute){								// �Ԃɖ����������ꍇ�͊O��
			}
			else if (trace.keepType == 2 &&				// ���̂��ꂪ����ꍇ�͍���̂���m�F
				(type1 == 0 || type2 == 0 || calc1.sec <= 30)){
			}
			else if (flag_last && type2 == 0 &&
				((msec_dif_keepcur + msec_val_near2 < msec_dif_keepbase) || msec_dif_keepcur < 14500)){
			}
			else{
				pdata->setScpChap(trace.nscKeep, SCP_CHAP_DINT);
				ScpArType arstat_new = (logomode)? SCP_AR_L_OTHER : SCP_AR_N_OTHER;
				pdata->setScpArstat(trace.nscKeep, arstat_new);
				update = true;
				trace.nscBase = trace.nscKeep;
				msec_base = pdata->getMsecScp(trace.nscBase);
				trace.nscKeep = -1;
				// base���ω�����̂Ŏ撼��
				type1 = calcDifSelect(calc1, msec_base, msec_cur);
				if (calc1.gap > msec_curmgn) type1 = 0;
			}
		}
	}

	//--- ���ǉ��ɕK�v�ȏ���ݒ� ---
	bool flag_decr = (nsc_cur < trace.nscBase)? true : false;
	Msec msec_st, msec_ed;
	bool enable_nside, enable_mid;
	if (flag_decr == false){				// �O������㑤
		msec_st      = target.msec.st;
		msec_ed      = target.msec.ed;
		enable_nside = ((nside & 0x01)!=0)? true : false;
		enable_mid   = (msec_cur < msec_mid + msec_val_near2)? true : false;
	}
	else{								// �㑤����O��
		msec_st      = target.msec.ed;
		msec_ed      = target.msec.st;
		enable_nside = ((nside & 0x02)!=0)? true : false;
		enable_mid   = (msec_cur > msec_mid - msec_val_near2)? true : false;
	}
	bool sec30_cur_st  = (abs(msec_cur - msec_st) <= 30*1000+msec_val_near2)? true : false;
	bool sec30_cur_ed  = (abs(msec_cur - msec_ed) <= 30*1000+msec_val_near2)? true : false;
	bool enable_from   = (abs(msec_cur - target.msec.st) > msec_val_lap2)? true : false;
	bool enable_to     = (abs(msec_cur - target.msec.ed) > msec_val_lap2)? true : false;
	bool enable_base   = (abs(msec_cur - msec_base     ) > msec_val_lap2)? true : false;
	if (target.nsc.st == 0){							// �擪�ԍ����m�F
		nside &= 0x2;
		enable_from = 1;
		enable_base = 1;
	}
	else if (target.nsc.ed == num_scpos-1){	// �ŏI�ԍ����m�F
		nside &= 0x1;
		enable_to = 1;
		enable_base = 1;
	}
	//--- ���ǉ� ---
	//--- �����Ɗm��ʒu���痣��Ă���ꍇ������ ---
	if (enable_from && enable_to && enable_base){
		//--- �ŏ��̈ʒu�������b���̏ꍇ�Ɋm��ʒu����̒l��ύX ---
		{
			CalcDifInfo calcsub;
			int typesub = calcDifSelect(calcsub, msec_cur, msec_st);
			if (typesub > 0){
				if (type1 == 0 || (type1 <= typesub && calc1.gap > calcsub.gap)){
					type1 = typesub;
					calc1 = calcsub;
				}
			}
		}
		//--- ���ɂ��邩���� ---
		if ((enable_mid || trace.nscKeep >= 0) &&		// �����I���n�_�܂�
			(sec30_cur_st || type1 > 1)){				// �J�n���̒[�ɋ߂�������b���̎�
			int type_upkeep = 0;
			CalcDifInfo calc2;
			int type2 = calcDifSelect(calc2, msec_cur, msec_ed);
			if (calc2.gap > msec_curmgn) type2 = 0;
			// �I�����ɂ��߂�������������ꍇ�͗�������m�F
			if (sec30_cur_ed && nside == 3 && flag_decr == false){
				if ((trace.keepGap > calc1.gap || trace.nscKeep < 0) && type2 > 0 && type1 > 0){
					type_upkeep = 1;
				}
			}
			// �J�n���Ɍ�₠��ꍇ
			else if (enable_nside && type1 > 0){
				if (trace.keepGap > calc1.gap || trace.nscKeep < 0){
					type_upkeep = 1;
				}
			}
			// �J�n���Ɍ�₠��ꍇ�i���ꂪ��������ꍇ�j
			else if (enable_nside && trace.nscKeep < 0 &&
					 calc1.sec <= 15 && calc1.gap <= msec_val_near2){
				type_upkeep = 2;
			}
			//--- ���ʒu��������15�b�P�ʍ\���ɂȂ邩�`�F�b�N ---
			bool flag_det15s = false;
			if (type_upkeep > 0){
				int step = (flag_decr)? -1 : +1;
				int cnt_15s = 0;
				for(int i=-4; i<=4; i++){		// �߂���CM�P�ʂ��������݂��邩�Ŕ��f
					Msec msec_tmp = msec_cur + (step * i * 15000);
					if (i != 0 &&
						msec_tmp >= target.msec.st - msec_val_near2 &&
						msec_tmp <= target.msec.ed + msec_val_near2){
						ScpChapType chap_cmp = (i < 0)? SCP_CHAP_DECIDE : SCP_CHAP_NONE;
						Nsc nsc_tmp = pdata->getNscFromMsecChap(msec_tmp, msec_val_near2, chap_cmp);
						if (nsc_tmp > 0){
							cnt_15s ++;
						}
					}
				}
				if (cnt_15s >= 2){
					flag_det15s = true;
				}
			}
			//--- �����X�V ---
			if (type_upkeep > 0){
				trace.nscKeep = nsc_cur;
				trace.keepGap  = calc1.gap;
				trace.keepType = type_upkeep;
				trace.keep15s  = flag_det15s;
			}
		}
	}
	//--- �o�͐ݒ� ---
	return update;
}


//---------------------------------------------------------------------
// �m��Q�_�Ԃ̒ǉ��ݒ�iCM�����A���S90�b�F���j
// ���́F
//   target:     �m��Q�_�V�[���`�F���W�ʒu
//   logomode:   false:���S�O  true:���S��
// �o�́F
//   �Ԃ�l:  �X�V�L���ifalse=�Ȃ�  true=����j
//---------------------------------------------------------------------
bool JlsAutoReform::setInterpolarExtra(RangeNscMsec target, bool logomode){
	int num_scpos = pdata->sizeDataScp();
	//--- �ǉ����o���邩�m�F�i1=���S90�b���o�A2=CM�����A3=CM�������Ȃ�CM�j
	int type = (logomode)? 1 : 0;
	if (logomode == false && target.nsc.st > 0 && target.nsc.ed < num_scpos-1){
		if (pdata->getScpArstat(target.nsc.ed) == SCP_AR_N_UNIT){
			Msec msec_dif = target.msec.ed - target.msec.st;
			int secdif    = pdata->cnv.getSecFromMsec(msec_dif);
			if (secdif == 15 || secdif == 30){
				type = 2;
			}
			else{
				type = 3;
			}
		}
	}
	bool update = false;
	//--- ���S90�b�F�� ---
	if (type == 1){
		for(Nsc i=target.nsc.st+1; i<=target.nsc.ed; i++){
			bool flag_search = true;
			int msec_i = pdata->getMsecScp(i);
			//--- �[�߂��͔F�����Ȃ� ---
			if ((abs(msec_i - target.msec.st) <= 89 * 1000) ||
				(abs(msec_i - target.msec.ed) <= pdata->msecValLap2 && i != target.nsc.ed)){
				flag_search = false;
			}
			//--- �߂��Ɋm��ӏ�������ꍇ�͏������� ---
			if (flag_search){
				Nsc nsc_tmp = pdata->getNscFromMsecChap(msec_i, pdata->msecValLap2, SCP_CHAP_DECIDE);
				if (nsc_tmp >= 0 && nsc_tmp != i){
					flag_search = false;
				}
			}
			//--- 90�b�\�����擾���鏈�� ---
			if (flag_search){
				//--- 90�b�O�n�_�擾 ---
				Nsc nsc_pts = pdata->getNscFromMsecChap(
								msec_i - 90000, pdata->msecValLap2, SCP_CHAP_DECIDE);
				if (nsc_pts < 0){		// �m��n�_���Ȃ���΂���ȊO�̒n�_
					nsc_pts = pdata->getNscFromMsecChap(
								msec_i - 90000, pdata->msecValLap2, SCP_CHAP_NONE);
				}
				//--- �Ԃɖ����V�[���`�F���W���Q�ӏ�����ꍇ�͌��ɂ��Ȃ� ---
				if (nsc_pts > 0 && (i - nsc_pts) > 2){
					nsc_pts = -1;
				}
				//--- �Ԃɖ����V�[���`�F���W���P�ӏ�����ꍇ�͌��Ƃ��邩���f ---
				else if (nsc_pts > 0 && (i - nsc_pts) == 2){
					Nsc nsc_tmp = i - 1;
					bool flag_dist = false;
					{
						static const int MSEC_CM90S_SAME = 4500;		// ����Ƃ݂Ȃ�����
						Msec msec_pts = pdata->getMsecScp(nsc_pts);
						Msec msec_tmp = pdata->getMsecScp(nsc_tmp);
						if (abs(msec_tmp - msec_pts) < MSEC_CM90S_SAME ||
							abs(msec_tmp - msec_i  ) < MSEC_CM90S_SAME){
							flag_dist = true;
						}
					}
					//--- �Ԃ��m��ʒu�������ꍇ�͏������Ȃ� ---
					if (jlsd::isScpChapTypeDecide( pdata->getScpChap(nsc_tmp) )){
						nsc_pts = -1;
					}
					//--- �Ԃ��O��ǂ���̖�����Ԃł��Ȃ��ꍇ�������Ȃ� ---
					else if ((pdata->isSmuteSameArea(nsc_tmp, nsc_pts) == false) &&
							 (pdata->isSmuteSameArea(nsc_tmp, i) == false) &&
							 (flag_dist == false)){
						nsc_pts = -1;
					}
				}
				if (nsc_pts > 0){
					Nsc  nsc_pte = i;
					Msec msec_pts = pdata->getMsecScp(nsc_pts);
					Msec msec_pte = msec_i;
					Sec  sec_dif = pdata->cnv.getSecFromMsec(abs(msec_pte - msec_pts));
					if (sec_dif == 90){
						//--- 90�b�\���̈ʒu�m�菈�� ---
						ScpChapType chap_pts = pdata->getScpChap(nsc_pts);		// �J�n�n�_
						ScpChapType chap_pte = pdata->getScpChap(nsc_pte);		// �I���n�_
						if (chap_pts < SCP_CHAP_DECIDE){
							pdata->setScpChap(nsc_pts, SCP_CHAP_DINT);
							pdata->setScpArstat(nsc_pts, SCP_AR_L_OTHER);
							update = true;
						}
						if (chap_pte < SCP_CHAP_DECIDE){
							pdata->setScpChap(nsc_pte, SCP_CHAP_DINT);
							pdata->setScpArstat(nsc_pte, SCP_AR_L_OTHER);
							update = true;
						}
						//--- 90�b����5�b�܂���10�b���ꂽ�\�����ǉ� ---
						Nsc nsc_a05 = pdata->getNscFromMsecChap(
										msec_pte+5000,  pdata->msecValNear1, SCP_CHAP_NONE);
						Nsc nsc_a10 = pdata->getNscFromMsecChap(
										msec_pte+10000, pdata->msecValNear1, SCP_CHAP_NONE);
						Nsc nsc_d05 = pdata->getNscFromMsecChap(
										msec_pte+5000, 4500, SCP_CHAP_DECIDE);
						Nsc nsc_d10 = pdata->getNscFromMsecChap(
										msec_pte+10000, 4500, SCP_CHAP_DECIDE);
						if (nsc_d05 <= 0 && nsc_d10 <= 0){		// �߂��Ɋ����\�����Ȃ��ꍇ�̂ݎ��s
							if (nsc_a05 > 0 && msec_pte+5000 < target.msec.ed - pdata->msecValLap2){
								pdata->setScpChap(nsc_a05, SCP_CHAP_DINT);
								pdata->setScpArstat(nsc_a05, SCP_AR_L_OTHER);
								update = true;
							}
							else if (nsc_a10 > 0 && msec_pte+10000 < target.msec.ed - pdata->msecValLap2){
								pdata->setScpChap(nsc_a10, SCP_CHAP_DINT);
								pdata->setScpArstat(nsc_a10, SCP_AR_L_OTHER);
								update = true;
							}
						}
					}
				}
			}
		}
	}
	//--- CM���� ---
	if (type == 2 || type == 3){
		//--- �����ŏ��l��ݒ� ---
		bool flag_1st = true;
		for(Nsc i=target.nsc.st+1; i<=target.nsc.ed; i++){
			Msec msec_i = pdata->getMsecScp(i);
			ScpChapType chap_i = pdata->getScpChap(i);
			if (chap_i >= SCP_CHAP_DECIDE){
				//--- ����������͈� ---
				Msec msec_dif_target = abs(target.msec.ed - target.msec.st);
				Msec msec_minst = (type == 3)? 60*1000 : msec_dif_target / 4;
				Msec msec_mined = (type == 3)? 60*1000 : msec_dif_target / 8;
				if (msec_minst < pdata->msecValLap2){
					msec_minst = pdata->msecValLap2;
				}
				if (msec_mined < pdata->msecValLap2){
					msec_mined = pdata->msecValLap2;
				}
				//--- �������� ---
				bool flag_div = false;
				if ((abs(msec_i - target.msec.st) > msec_minst &&
					 abs(msec_i - target.msec.ed) > msec_mined)){
					flag_div = true;
				}
				if (flag_div){
					//--- �b�P�ʂ���̌덷���ݒ�l�ȏゾ�����ꍇ��CM�������Ȃ� ---
					CalcDifInfo calc_st;
					CalcDifInfo calc_ed;
					calcDifSelect(calc_st, msec_i, target.msec.st);
					calcDifSelect(calc_ed, msec_i, target.msec.ed);
					Msec msec_prm_gap = pdata->getConfig(CONFIG_VAR_msecMgnCmDivide);
					if (calc_st.gap > msec_prm_gap || calc_ed.gap > msec_prm_gap){
						//--- 5�b�P�ʂ������ꍇ�͏��������ɂ����� ---
						if ((calc_st.sec % 5)!=0 || (calc_ed.sec % 5)!=0 ||
							calc_st.gap >= pdata->msecValNear1 || calc_ed.gap >= pdata->msecValNear1){
							flag_div = false;
						}
					}
				}
				if (flag_div || i == target.nsc.ed){
					if (flag_1st && i < target.nsc.ed){
						pdata->setScpArstat(i, SCP_AR_N_BUNIT);
						flag_1st = false;
					}
					else if (flag_1st == false){
						pdata->setScpArstat(i, SCP_AR_N_AUNIT);
					}
				}
				else{
					pdata->setScpChap(i, SCP_CHAP_CPOSQ);
				}
			}
		}
	}
	return update;
}



//=====================================================================
// CM�\�����쐬
//=====================================================================

//---------------------------------------------------------------------
// CM�\�����o
// ���́F
//   cmscope:   CM��������͈�
//   logo1st:   �擪�����̃��S��ԁifalse=���S�Ȃ� true=���S����j
//   logointer: ���ԕ����̃��S��ԁifalse=���S�Ȃ� true=���S����j
// �o�́F
//   �Ԃ�l: �\���ǉ�����i0=�Ȃ�  1=����j
//   bounds: CM�ʒu�Ɣ��肵���͈�
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
bool JlsAutoReform::setCMForm(RangeMsec &bounds, RangeWideMsec cmscope, bool logo1st, bool logointer){
	int num_scpos = pdata->sizeDataScp();
	bool logoon_st = logo1st;
	if (num_scpos-1 <= 1){				// �����Ō�܂Ŋ������͏I��
		return false;
	}
	//--- ���S�\����ԓ���15�b�\����CM�����鏈�� ---
	int rev_del_edge = pdata->getConfigAction(CONFIG_ACT_LogoDelEdge);

	//--- 15�b�͈͐ݒ� ---
	RangeWideMsec  findscope = cmscope;
	findscope.st.early = cmscope.st.just - pdata->msecValLap2;
	findscope.st.late  = cmscope.st.just + pdata->msecValLap2;
	findscope.ed.early = cmscope.ed.just - pdata->msecValLap2;
	findscope.ed.late  = cmscope.ed.just + pdata->msecValLap2;
	if (rev_del_edge == 0){
		if (findscope.st.late < cmscope.st.late){
			findscope.st.late = cmscope.st.late;
		}
		if (findscope.ed.early > cmscope.ed.early){
			findscope.ed.early = cmscope.ed.early;
		}
	}
	if (rev_del_edge == 4){
		if (findscope.st.early > cmscope.st.early){
			findscope.st.early = cmscope.st.early;
		}
		if (findscope.ed.late < cmscope.ed.late){
			findscope.ed.late = cmscope.ed.late;
		}
	}
	if (findscope.st.just < 0){
		findscope.st.early = -1;
		findscope.st.late  = -1;
	}
	if (findscope.ed.just < 0){
		findscope.ed.early = -1;
		findscope.ed.late  = -1;
	}
	//--- �Ώی�₪�s���m���S�����Ō��݂̑I��͈͊O�ł���Ε␳ ---
	if (cmscope.st.just > 0){
		WideMsec wmsec_logo = cmscope.st;
		wmsec_logo.early -= pdata->msecValSpc;
		wmsec_logo.late  += pdata->msecValSpc;
		Nsc nsc_st = pdata->getNscFromWideMsecByChap(wmsec_logo, SCP_CHAP_CDET);
		if (nsc_st < 0){
			nsc_st = pdata->getNscFromWideMsecByChap(wmsec_logo, SCP_CHAP_NONE);
		}
		if (nsc_st > 0){
			Msec msec_st = pdata->getMsecScp(nsc_st);
			if (findscope.st.early > msec_st - pdata->msecValSpc){
				findscope.st.early = msec_st - pdata->msecValSpc;
			}
		}
	}
	if (cmscope.ed.just > 0){
		WideMsec wmsec_logo = cmscope.ed;
		wmsec_logo.early -= pdata->msecValSpc;
		wmsec_logo.late  += pdata->msecValSpc;
		Nsc nsc_ed = pdata->getNscFromWideMsecByChap(wmsec_logo, SCP_CHAP_CDET);
		if (nsc_ed < 0){
			nsc_ed = pdata->getNscFromWideMsecByChap(wmsec_logo, SCP_CHAP_NONE);
		}
		if (nsc_ed > 0){
			Msec msec_ed = pdata->getMsecScp(nsc_ed);
			if (findscope.ed.late < msec_ed + pdata->msecValSpc){
				findscope.ed.late = msec_ed + pdata->msecValSpc;
			}
		}
	}
	//--- �͈͓��̌��\�������� ---
	bounds = {-1, -1};			// 15�b�P�ʂ̊J�n�I���ʒu
	bool det = false;
	{
		Nsc nsc_cur = 1;
		bool over = false;
		while( over == false ){
			int msec_cur = pdata->getMsecScp(nsc_cur);
			ScpChapType chap_cur = pdata->getScpChap(nsc_cur);
			//--- �͈͓��̌��ʒu�ł���Ώ��� ---
			if ((msec_cur >= findscope.st.early || findscope.st.early < 0) &&
				(msec_cur <= findscope.ed.late  || findscope.ed.late  < 0) &&
				chap_cur >= SCP_CHAP_CDET){
				//--- 15�b�P�ʂ���O�����ɒT�� ---
				int msec_stpoint;
				bool det_tmp = setCMFormDetect(msec_stpoint, nsc_cur, findscope, logoon_st);
				if (msec_stpoint > 0 && (bounds.st > msec_stpoint || bounds.st < 0)){
					bounds.st = msec_stpoint;
				}
				if (det_tmp || chap_cur >= SCP_CHAP_DFIX){
					bounds.ed = msec_cur;
					if (bounds.st < 0){
						bounds.st = bounds.ed;
					}
				}
				if (det_tmp){
					det = true;
					logoon_st = (logointer)? true : false;
				}
			}
			//--- �V�[���`�F���W�}�����͔ԍ����V�t�g ---
			int nsc_tmp = pdata->getNscFromMsecAll(msec_cur);
			if (nsc_tmp > nsc_cur && nsc_tmp > 0 && nsc_cur > 0){
				nsc_cur = nsc_tmp;
			}
			nsc_cur ++;
			if (nsc_cur >= num_scpos-1 ||
				(msec_cur > findscope.ed.late && findscope.ed.late >= 0)){
				over = true;
			}
		}
	}
	//--- CM�\���Ō덷���傫�����̕␳���� ---
	bool fix_mode = (cmscope.fixSt || cmscope.fixEd)? true : false;
	if (cmscope.logomode == false && det){
		setCMFormDetRevise(bounds, fix_mode);
	}
	//--- �[���� ---
	if (cmscope.logomode == false && fix_mode == false){
		int lvlogo = pdata->getLevelUseLogo();
		//--- ���S�ɂ��CM�[�̕␳���� ---
		if (det && lvlogo >= CONFIG_LOGO_LEVEL_USE_LOW){
			setCMFormByLogo(bounds, cmscope);
		}
		//--- �[��15�b�������� ---
		if (det || lvlogo > CONFIG_LOGO_LEVEL_USE_LOW){
			bool det_tmp = setCMFormEdge(bounds, cmscope, logo1st);
			if (det_tmp){
				det = true;
			}
		}
	}
	return det;
}

//---------------------------------------------------------------------
// CM�ʒu����O�����Ɍ��o���Ă���
// ���́F
//   nsc_base:  �����J�n����ʒu�ԍ�
//   findscope: CM��������͈�
//   logoon_st: �擪�����̃��S��ԁifalse=���S�Ȃ� true=���S����j
// �o�́F
//   �Ԃ�l:    CM����ifalse=�Ȃ�  true=����j
//   msec_stpoint: �����Ώۂ̊J�n�ʒu
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
bool JlsAutoReform::setCMFormDetect(Msec &msec_stpoint, Nsc nsc_base, RangeWideMsec findscope, bool logoon_st){
	msec_stpoint = -1;				// ��ԍŏ���15�b�P�ʈʒu�i-1�̎��͑��݂Ȃ��j
	//--- �ݒ�l�ǂݍ��� ---
	int type_nosc = pdata->getConfigAction(CONFIG_ACT_MuteNoSc);	// �V�[���`�F���W�Ȃ������̎g�p�L�����Z��
	//--- 15�b�P�ʈʒu���㑤����m�F ---
	bool det = false;				// 15�b�P�ʈʒu�̌��o�L��
	bool flag_force = false;		// �����ʒu�ݒ�ӏ��̗L��
	Nsc nsc_save = nsc_base;		// ���ɐݒ肷��ӏ�
	{
		Msec msec_base = pdata->getMsecScp(nsc_base);
		Sec  sec_difend = 0;
		int  i = nsc_base;
		bool fin = false;
		while(fin == false){
			//--- ���̈ʒu�擾 ---
			i = pdata->getNscPrevScpChap(i, SCP_CHAP_CDET);		// CDET�ȏ�̎��ʒu
			//--- �����͈͂𒴂�����I�� ---
			if (i <= 0) break;
			Msec msec_i = pdata->getMsecScp(i);
			if ((msec_base <= findscope.st.late && findscope.st.late >= 0) ||
				(msec_i < findscope.st.early && findscope.st.early >= 0)){
				break;
			}
			//--- �����ʒu�ݒ�̊m�F ---
			ScpChapType chap_i = pdata->getScpChap(i);
			if (chap_i >= SCP_CHAP_DFORCE){
				flag_force = true;
				if (msec_stpoint > msec_i || msec_stpoint < 0){
					msec_stpoint = msec_i;
				}
			}
			//--- ���Ԓn�_���͍폜 ---
			if (chap_i == SCP_CHAP_DINT){
				pdata->setScpChap(i, SCP_CHAP_NONE);
				chap_i = SCP_CHAP_NONE;
			}
			//--- 15�b�P�ʂŌ��� ---
			Sec sec_dif = pdata->cnv.getSecFromMsec(msec_base - msec_i);
			Sec sec_dif15 = ((sec_dif + 7) / 15) * 15;
			Msec msec_align = msec_base - (sec_dif15 * 1000);
			//--- ��ԋ߂��\���ʒu�Ɗm��ʒu������ ---
			Nsc nsc_near = pdata->getNscFromMsecChap(msec_align, pdata->msecValLap2, SCP_CHAP_CDET);
			Nsc nsc_chk  = pdata->getNscFromMsecChap(msec_align, pdata->msecValLap2, SCP_CHAP_DFORCE);
			//--- �I���������� ---
			if (sec_dif15 > 120 || (sec_dif15 > sec_difend && sec_difend > 0)){
				fin = true;
			}
			else{
				bool flag_dif_near = (abs(msec_i - msec_align) < pdata->msecValLap2)? true : false;
				//--- 15�b�\����؂�̊����ʒu������΂��̕b���ŏI�������邽�ߕێ� ---
				if (chap_i >= SCP_CHAP_CDET && sec_difend <= 0){
					int score_tmp = pdata->getScpScore(i);
					int score_base = pdata->getScpScore(nsc_base);
					//--- 15�b�P�ʂɋ߂��܂��̓X�R�A���傫�����̂͏I���b���Ƃ��Ċm�� ---
					if (flag_dif_near || score_tmp + 30 >= score_base){
						sec_difend = sec_dif15;
					}
				}
				//--- �\���O�̐����m��ӏ�������ꍇ�͏I�� ---
				if (chap_i >= SCP_CHAP_DFIX && nsc_near != i){
					if (flag_dif_near == false){
						fin = true;
					}
				}
			}
			//--- 15�b�P�ʂ̍\�� ---
			if (fin == false){
				if (nsc_near == i && nsc_chk < 0 && sec_dif15 > 0 && sec_dif15 <= 120){
					//--- 15�b�P�ʂ̐ݒ� ---
					det = true;
					fin = true;
					pdata->setScpChap(nsc_save, SCP_CHAP_DFIX);
					if (findscope.logomode == false){
						pdata->setScpArstat(nsc_save, SCP_AR_N_UNIT);
					}
					else{
						pdata->setScpArstat(nsc_save, SCP_AR_L_UNIT);
					}
					//--- �����J�n�ʒu�̍X�V ---
					int msec_update = pdata->getMsecScp(nsc_save);
					if (msec_stpoint > msec_update || msec_stpoint < 0){
						msec_stpoint = msec_update;
					}
					//--- �����ݒ肪�Ȃ���Ύ��̈ʒu�ݒ� ---
					if (flag_force == 0){
						nsc_save = i;
						//--- 15�b�P�ʉӏ��������őΏۈʒu��������������Ă���ꍇ ---
						int nsc_tmp = pdata->getNscFromMsecChap(msec_align, pdata->msecValNear2, SCP_CHAP_CDET);
						if ( pdata->isSmuteFromMsec(msec_align) == true && nsc_tmp < 0 && type_nosc != 1){
							//--- 15�b�P�ʉӏ����Ώۈʒu���X�R�A�������ꍇ�����ւ� ---
							int score_align = getScore(msec_align, findscope);
							//--- 60�b�ȏ��30�b�P�ʂł͂Ȃ��ʒu�̓V�[���`�F���W�Ȃ���Β��~ ---
							if (sec_dif15 > 60 && sec_dif15 % 30 > 0){
								score_align = 0;
							}
							if (score_align > pdata->getScpScore(i)){
								//--- �t���[���P�ʂ̏ꏊ�ɕϊ� ---
								int msec_newrev = pdata->cnv.getMsecAlignFromMsec(msec_align);
								int msec_newbk  = pdata->cnv.getMsecAdjustFrmFromMsec(msec_align, -1);
								//--- �X�V�O�̈ʒu���\���P�ʂ���O�� ---
								pdata->setScpChap(i, SCP_CHAP_NONE);
								pdata->setScpStatpos(i, SCP_PRIOR_NONE);
								//--- �f�[�^�X�V ---
								int nsc_tmp = pdata->insertScpos(msec_newrev, msec_newbk, i, SCP_PRIOR_DECIDE);
								pdata->setScpChap(nsc_tmp, SCP_CHAP_CDET);
								pdata->setScpScore(nsc_tmp, score_align);
								nsc_save = nsc_tmp;
								msec_update = pdata->getMsecScp(nsc_tmp);
								fin = false;					// ���̈ʒu������
								sec_difend = 0;
								if (i > nsc_tmp){				// �}���ꏊ����O�Ȃ猻�݈ʒu���ړ�
									i = nsc_tmp;
								}
							}
						}
					}
				}
			}
		}
	}
	//--- �X�V�����ꍇ ---
	if (det){
		if (nsc_save > 0 && flag_force == false){
			//--- �Ō�̈ʒu����m��ł���΍X�V ---
			if (pdata->getScpChap(nsc_save) < SCP_CHAP_DECIDE){
				pdata->setScpChap(nsc_save, SCP_CHAP_DFIX);
				ScpArType arstat_save = (logoon_st)? SCP_AR_L_OTHER : SCP_AR_N_OTHER;
				pdata->setScpArstat(nsc_save, arstat_save);
			}
			Msec msec_update = pdata->getMsecScp(nsc_save);
			if (msec_stpoint > msec_update || msec_stpoint < 0){
				msec_stpoint = msec_update;
			}
		}
	}
	return det;
}

//---------------------------------------------------------------------
// CM�F�������\���Ō덷���傫�����̏C��
// ���́F
//   fix_mode: �ʒu�Œ胂�[�h(0:�Œ�ł͂Ȃ� 1:�Œ胂�[�h)
// ���o�́F
//   bounds:   15�b�P��CM�̈ʒu�ԍ�
// �o�́F
//   pdata(chap,arstat)
//---------------------------------------------------------------------
void JlsAutoReform::setCMFormDetRevise(RangeMsec &bounds, bool fix_mode){
	//--- �ݒ�l�ǂݍ��� ---
	int mgn_cm_detect = pdata->getConfig(CONFIG_VAR_msecMgnCmDetect);
	//--- �����ʒu�ԍ����擾 ---
	RangeNsc nscbounds;
	if (pdata->getRangeNscFromRangeMsec(nscbounds, bounds) == false){
		return;
	}
	//--- �͈͓������ԂɌ��o ---
	{
		Nsc nsc_head = pdata->getNscNextScpChap(0, SCP_CHAP_DECIDE);
		Nsc nsc_loc0 = -1;
		Nsc nsc_loc1 = nscbounds.st;
		Msec msec_loc0 = 0;
		Msec msec_loc1 = pdata->getMsecScp(nsc_loc1);
		bool flag_cm0  = false;
		bool flag_cm1  = false;
		CalcModInfo calc1 = {};
		bool cont1 = true;
		while(cont1){
			//--- ���̈ʒu�擾 ---
			Nsc  nsc_loc2  = pdata->getNscNextScpChap(nsc_loc1, SCP_CHAP_DECIDE);
			Msec msec_loc2 = pdata->getMsecScp(nsc_loc2);
			bool cont2     = (nsc_loc2 <= nscbounds.ed && nsc_loc2 > 0)? true : false;
			ScpArType arstat_loc2 = pdata->getScpArstat(nsc_loc2);
			bool flag_cm2 = (arstat_loc2 == SCP_AR_N_UNIT)? true : false;
			CalcModInfo calc2;
			calcDifMod0515(calc2, msec_loc2 - msec_loc1);
			if (calc2.mod15 == -1) flag_cm2 = false;		// 15�b�P�ʔ͈͊O
			//--- 1�O�̈ʒu��CM��15�b�P�ʂ���덷���傫���ꍇ�̏��� ---
			if (flag_cm1 && calc1.mod15 > mgn_cm_detect && nsc_loc0 >= 0){
				bool ignore1 = false;
				if (cont2 && flag_cm2){		// ���ʒu��CM�L���ȏꍇ
					Msec msec_dif20 = msec_loc2-msec_loc0;
					if (msec_dif20 <= 120*1000+pdata->msecValLap2){
						//--- �폜�Ń}�[�W���������ȉ��ɂȂ�Ȃ�폜���Ė������� ---
						CalcModInfo calcsum;
						calcDifMod0515(calcsum, msec_loc2 - msec_loc0);
						if (calcsum.mod15 < mgn_cm_detect &&
							(calcsum.mod15 <= mgn_cm_detect/2 ||
							 calcsum.mod15 < (calc1.mod15 + calc2.mod15)/6)){
							//--- �X�V�i��؂���O���Ė�������j ---
							pdata->setScpChap(nsc_loc1, SCP_CHAP_NONE);
							ignore1 = true;
							nsc_loc1  = nsc_loc0;
							msec_loc1 = msec_loc0;
							calc2 = calcsum;
						}
					}
				}
				if (ignore1 == false && fix_mode == false){
					//--- CM������5/10�b�\���������ꍇ�͋�؂����� ---
					Nsc nsc_subs = -1;
					Nsc nsc_sube = -1;
					SearchDirType dr = SEARCH_DIR_NEXT;
					if (flag_cm0 == false && flag_cm2 == true){		// �㑤��CM
						nsc_subs = nsc_loc0;
						nsc_sube = nsc_loc1;
						dr = SEARCH_DIR_NEXT;
					}
					if (flag_cm0 == true && flag_cm2 == false){		// �O����CM
						nsc_subs = nsc_loc1;
						nsc_sube = nsc_loc0;
						dr = SEARCH_DIR_PREV;
					}
					if (nsc_sube > 0){
						bool cont_tmp = true;
						Nsc  nsc_tmp  = nsc_subs;
						Msec msec_subs = pdata->getMsecScp(nsc_subs);
						Msec msec_sube = pdata->getMsecScp(nsc_sube);
						while(cont_tmp){
							nsc_tmp = pdata->getNscDirScpChap(nsc_tmp, dr, SCP_CHAP_NONE);
							if (nsc_tmp <= 0 || nsc_tmp <= nsc_loc0 || nsc_tmp >= nsc_loc1){
								cont_tmp = false;
							}
							else{
								FormLogoLevelExt extype = {};
								Msec msec_tmp = pdata->getMsecScp(nsc_tmp);
								if (abs(msec_tmp - msec_subs) >= pdata->msecValLap2){
									CalcModInfo calc_mod;
									Msec msec_diftmp = abs(msec_sube - msec_tmp);
									bool flag_len = isLengthLogoLevel(msec_diftmp, extype);
									int  flag_s05 = calcDifMod0515(calc_mod, msec_diftmp);
									if (flag_len && flag_s05 > 0){
										cont_tmp = false;
										pdata->setScpChap(nsc_tmp, SCP_CHAP_DINT);
										pdata->setScpArstat(nsc_tmp, SCP_AR_N_OTHER);
										pdata->setScpArstat(nsc_loc1, SCP_AR_N_OTHER);
										Sec sec_logos = pdata->getSecLogoComponentFromLogo(msec_loc0, msec_tmp);
										if (sec_logos > (abs(msec_tmp - msec_loc0) + 500)/2000){
											pdata->setScpArstat(nsc_tmp, SCP_AR_L_OTHER);
											if (bounds.st == msec_loc0){
												bounds.st = msec_tmp;
											}
										}
										Sec sec_logoe = pdata->getSecLogoComponentFromLogo(msec_tmp, msec_loc1);
										if (sec_logoe * 1000 + 500 >= abs(msec_loc1 - msec_tmp)/2){
											pdata->setScpArstat(nsc_loc1, SCP_AR_L_OTHER);
											if (bounds.ed == msec_loc1){
												bounds.ed = msec_tmp;
											}
										}
									}
								}
							}
						}
					}
				}
				if (ignore1 == false && fix_mode == false){
					Nsc nsc_delete = -1;
					//--- �X�V�iCM�P�ʂ���덷�傫�����c���Ȃ烍�S����Ƃ���CM����O���j ---
					if (pdata->getConfigAction(CONFIG_ACT_LogoUCGapCm) > 0){
						//--- �擪���Ō�̋�؂肾�����ꍇ�͊O�� ---
						if (nsc_loc0 == nsc_head){
							ScpArType arstat_loc0 = pdata->getScpArstat(nsc_loc0);
							if (jlsd::isScpArTypeLogo(arstat_loc0)){
								pdata->setScpArstat(nsc_loc1, SCP_AR_L_OTHER);
							}
							else{
								pdata->setScpArstat(nsc_loc1, SCP_AR_N_OTHER);
							}
							pdata->setScpChap(nsc_loc0, SCP_CHAP_NONE);
							nsc_delete = nsc_loc0;
						}
						else if (nsc_loc2 < 0){
							pdata->setScpChap(nsc_loc1, SCP_CHAP_NONE);
							nsc_delete = nsc_loc1;
						}
						else{
							pdata->setScpArstat(nsc_loc1, SCP_AR_L_UNIT);
						}
					}
					//--- ���S����ł͐擪�Ōオ�ɒ[�ɒ[�������ꍇ�̂݊O�� ---
					else{
						if (nsc_loc0 == nsc_head && msec_loc0 <= pdata->msecValLap2){
							pdata->setScpChap(nsc_loc0, SCP_CHAP_NONE);
							nsc_delete = nsc_loc0;
						}
						else if (nsc_loc2 < 0 && msec_loc1 >= pdata->getMsecTotalMax() - pdata->msecValLap2){
							pdata->setScpChap(nsc_loc1, SCP_CHAP_NONE);
							nsc_delete = nsc_loc1;
						}
					}
					//--- CM���E���폜�����ꍇ�̏��� ---
					if (nsc_delete == nscbounds.st){
						nscbounds.st = pdata->getNscNextScpChap(nsc_delete, SCP_CHAP_DECIDE);
						bounds.st = pdata->getMsecScp(nscbounds.st);
					}
					else if (nsc_delete == nscbounds.ed){
						nscbounds.ed = pdata->getNscPrevScpChap(nsc_delete, SCP_CHAP_DECIDE);
						bounds.ed = pdata->getMsecScp(nscbounds.ed);
					}
				}
			}
			//--- �ʒu�̃V�t�g ---
			nsc_loc0  = nsc_loc1;
			nsc_loc1  = nsc_loc2;
			msec_loc0 = msec_loc1;
			msec_loc1 = msec_loc2;
			flag_cm0  = flag_cm1;
			flag_cm1  = flag_cm2;
			calc1     = calc2;
			cont1     = cont2;
		}
	}
}

//---------------------------------------------------------------------
// CM�ʒu�[�����̃��S���ɂ��15�b�P�ʕ␳
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
bool JlsAutoReform::setCMFormByLogo(RangeMsec &bounds, RangeWideMsec cmscope){
	bool det = false;
	//--- ���S�\����ԓ���15�b�\����CM�����鏈�� ---
	int rev_del_edge = pdata->getConfigAction(CONFIG_ACT_LogoDelEdge);
	//--- �J�n�ʒu�̕␳ ---
	if (cmscope.fixSt == false && bounds.st > 0 && cmscope.st.just > 0){
		int msec_limit = setCMFormByLogoLimit(bounds.st, SEARCH_DIR_PREV);
		if (bounds.st > msec_limit || msec_limit < 0){
			if (bounds.st > cmscope.st.just){
				//--- CM���̒ǉ� ---
				FormCMByLogo form;
				form.msecTarget   = cmscope.st.just;
				form.msecLimit    = msec_limit;
				form.msecCmSide   = cmscope.st.late;
				form.msecLogoSide = cmscope.st.early;
				form.revDelEdge   = rev_del_edge;
				form.dr           = SEARCH_DIR_PREV;
				int det_tmp = setCMFormByLogoAdd(bounds.st, form);
				if (det_tmp) det = true;
			}
		}
	}
	//--- �I���ʒu�̕␳ ---
	if (cmscope.fixEd == 0 && bounds.ed > 0 && cmscope.ed.just > 0){
		Msec msec_limit = setCMFormByLogoLimit(bounds.ed, SEARCH_DIR_NEXT);
		if (bounds.ed < msec_limit || msec_limit < 0){
			if (bounds.ed < cmscope.ed.just){
				//--- CM���̒ǉ� ---
				FormCMByLogo form;
				form.msecTarget   = cmscope.ed.just;
				form.msecLimit    = msec_limit;
				form.msecCmSide   = cmscope.ed.early;
				form.msecLogoSide = cmscope.ed.late;
				form.revDelEdge   = rev_del_edge;
				form.dr           = SEARCH_DIR_NEXT;
				int det_tmp = setCMFormByLogoAdd(bounds.ed, form);
				if (det_tmp) det = true;
			}
		}
	}
	return det;
}

//---------------------------------------------------------------------
// CM�ʒu�[�����̃��S���ɂ��15�b�P�ʕ␳�Ŋg�����E�n�_���擾
// �o�́F
//   �Ԃ�l: �g�����E�n�_�i-1�̎��͐ݒ�Ȃ��j
//---------------------------------------------------------------------
int JlsAutoReform::setCMFormByLogoLimit(Msec msec_point, SearchDirType dr){
	if (msec_point < 0){
		return -1;
	}
	//--- ���� ---
	LogoEdgeType edge_logo;
	int nrf_target;
	if (dr == SEARCH_DIR_PREV){			// CM�O���Ɋg�����͑O�̃��S�����オ������E�n�_
		edge_logo = LOGO_EDGE_RISE;
		nrf_target = -1;
	}
	else{								// CM�㑤�Ɋg�����͌�̃��S��������������E�n�_
		edge_logo = LOGO_EDGE_FALL;
		nrf_target = pdata->sizeDataLogo();
	}
	//--- �Ή����S�ʒu�擾 ---
	Msec msec_result = -1;
	Msec msec_last_r = -1;
	bool det = false;
	do{
		int msec_lg_c, msec_lg_l, msec_lg_r;
		nrf_target = pdata->getNrfDirLogo(nrf_target, dr, edge_logo, LOGO_SELECT_VALID);
		if (nrf_target >= 0){
			pdata->getMsecLogoNrfWide(msec_lg_c, msec_lg_l, msec_lg_r, nrf_target);
			if (dr == SEARCH_DIR_PREV){
				if (msec_lg_r > msec_point){
					det = true;
					msec_result = msec_last_r + pdata->msecValLap2;
				}
			}
			else{
				if (msec_lg_l > msec_point){
					det = true;
					msec_result = msec_lg_l - pdata->msecValLap2;
				}
			}
			msec_last_r = msec_lg_r;
		}
	}while(nrf_target >= 0 && det == false);
	//--- �\���ʒu�ɂ��ݒ� ---
	Nsc nsc_point = pdata->getNscFromMsecChap(msec_point, pdata->msecValNear2, SCP_CHAP_DFIX);
	if (nsc_point <= 0){
		msec_result = msec_point;		// �\�����Ȃ������炻�̏�ŏI��
	}
	else{		// �m��ʒu�܂łɐ���
		nsc_point = pdata->getNscDirScpChap(nsc_point, dr, SCP_CHAP_DECIDE);
		if (nsc_point >= 0){
			int msec_point = pdata->getMsecScp(nsc_point);
			if (dr == SEARCH_DIR_PREV){
				if (msec_result < msec_point || msec_result < 0){
					msec_result = msec_point;
				}
			}
			else{
				if (msec_result > msec_point || msec_result < 0){
					msec_result = msec_point;
				}
			}
		}
	}
	return msec_result;
}

//---------------------------------------------------------------------
// CM�ʒu�[�����̃��S���ɂ��15�b�P�ʕ␳�Œǉ�����
// ���o�́F
//   msec_result : �ǉ����15�b�P�ʂ̈ʒu
// �o�́F
//   �Ԃ�l: CM�ǉ�����ifalse=�Ȃ�  true=����j
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
bool JlsAutoReform::setCMFormByLogoAdd(Msec &msec_result, FormCMByLogo form){
	Nsc nsc_point = pdata->getNscFromMsecChap(
						msec_result, pdata->msecValNear2, SCP_CHAP_DFIX);
	if (nsc_point < 0 || form.revDelEdge <= 1){	// �ǉ����Ȃ�����
		return 0;
	}
	//--- �������� ---
	ScpArType arstat_new = SCP_AR_N_OTHER;
	int step;
	Msec msec_cmdetect;
	Msec msec_limit_rev;
	if (form.dr == SEARCH_DIR_PREV){
		step = -1;
		msec_cmdetect = msec_result - form.msecTarget;
		msec_limit_rev = form.msecLimit + pdata->msecValLap2;
		ScpArType arstat_point = pdata->getScpArstat(nsc_point);
		if (jlsd::isScpArTypeLogo(arstat_point)){
			arstat_new = SCP_AR_L_OTHER;
		}
	}
	else{
		step = +1;
		msec_cmdetect = form.msecTarget - msec_result;
		msec_limit_rev = form.msecLimit - pdata->msecValLap2;
	}
	if (msec_cmdetect < 0){
		return 0;
	}
	//--- �ݒ�l�ǂݍ��� ---
	int type_nosc = pdata->getConfigAction(CONFIG_ACT_MuteNoSc);	// �V�[���`�F���W�Ȃ������̎g�p�L�����Z��
	//--- 15/30/45/60�b�n�_�̌��� ---
	bool det = false;
	Msec msec_set_point = msec_result;					// �m��ݒ�ʒu
	for(int i=0; i<4; i++){
		int msec_difpt = 15*1000*(i+1);
		int msec_revpt = msec_result + (step * msec_difpt);
		//--- ������؂�ł���Ό�� ---
		if (pdata->isSmuteFromMsec(msec_revpt)){
			bool flag_match = true;
			//--- limit�ɂ����� ---
			if (form.msecLimit >= 0 &&
				((msec_revpt <= msec_limit_rev && step < 0) ||
				 (msec_revpt >= msec_limit_rev && step > 0))){
					flag_match = false;
			}
			//--- limit�ȊO�̏��� ---
			if (flag_match == true){
				bool match_none = false;		// CM���ԂȂ�����
				bool match_full = false;		// CM���Ԃ��S�̔���
				bool match_half = false;		// CM���Ԃ������ȏ㔻��
				bool match_minlogo = false;		// ���S���Ԃ����S�L���m��ȏ�̔���
				bool match_later = false;		// CM���Ԃ���ō폜�̑ΏۂƂȂ���Ԃ�����
				{
					Msec msec_difbase = abs(msec_result - msec_set_point);
					Msec msec_diftarget = msec_cmdetect - msec_difbase;
					Msec msec_difrev  = abs(msec_revpt - msec_set_point);
					if (msec_diftarget <= pdata->msecValLap2){
						match_none = true;
					}
					else if (msec_diftarget >= msec_difrev - pdata->msecValLap2){
						match_full = true;
					}
					else if (msec_diftarget >= msec_difrev / 2){
						match_half = true;
					}
					Msec msec_recmin = pdata->getConfig(CONFIG_VAR_msecWLogoLgMin);
					if (msec_difrev - msec_diftarget >= msec_recmin){
						Msec msec_difcm_max = abs(msec_result - form.msecLogoSide);
						if (msec_difrev - msec_difcm_max >= msec_recmin){
							match_minlogo = true;
						}
					}
					if (msec_diftarget < 15000 - pdata->msecValLap2 && msec_diftarget > 0){
						FormLogoLevelExt extype = {};
						extype.mid5s = true;			// ���S���x��MIDL��5�b�P�ʂɂ���
						if (isLengthLogoLevel(msec_diftarget, extype)){
							match_later = true;
						}
					}
				}
				bool match_logo = false;
				if ((step > 0 && form.msecLogoSide < msec_revpt) ||
					(step < 0 && form.msecLogoSide > msec_revpt)){
					match_logo = true;			// ���S���Ԃ��m���ɑ���
				}
				bool match_cm = false;
				if ((step > 0 && form.msecCmSide > msec_set_point) ||
					(step < 0 && form.msecCmSide < msec_set_point)){
					match_cm = true;			// CM���Ԃ��m���ɑ���
				}
				//--- �ݒ�ʂ̏��� ---
				if (form.revDelEdge == 4){
					flag_match = true;
				}
				else if (match_none){
					flag_match = false;
				}
				else if (match_minlogo || match_later){
					flag_match = false;
				}
				else if (match_full){
					flag_match = true;
				}
				else if (form.revDelEdge <= 1){
					if (!match_half || match_logo){
						flag_match = false;
					}
				}
				else if (form.revDelEdge == 2){
					if (!match_half){
						flag_match = false;
					}
				}
				else if (form.revDelEdge == 3){
					if (!match_half && !match_cm){
						flag_match = false;
					}
				}
			}
			//--- CM�Ƃ��Ēǉ� ---
			if (flag_match == true){
				int nsc_new = pdata->getNscFromMsecChap(
								msec_revpt, pdata->msecValNear2, SCP_CHAP_DFIX);
				if (nsc_new < 0){
					nsc_new = pdata->getNscFromMsecChap(
								msec_revpt, pdata->msecValNear2, SCP_CHAP_DUPE);
					if (nsc_new < 0 && type_nosc != 1){		// �����V�[���`�F���W�}��
						nsc_new = pdata->getNscForceMsec(msec_revpt, LOGO_EDGE_RISE);
					}
				}
				if (nsc_new > 0){
					det = true;
					//--- �}�������邽��nsc��蒼�� ---
					int nsc_set_point = pdata->getNscFromMsecChap(
										msec_set_point, pdata->msecValNear2, SCP_CHAP_DFIX);
					pdata->setScpChap(nsc_new, SCP_CHAP_DFIX);
					if (step < 0){
						pdata->setScpArstat(nsc_new, arstat_new);
						pdata->setScpArstat(nsc_set_point, SCP_AR_N_UNIT);
					}
					else{
						pdata->setScpArstat(nsc_new, SCP_AR_N_UNIT);
					}
					msec_set_point = msec_revpt;
				}
			}
		}
	}
	//--- ���ʍX�V ---
	msec_result = msec_set_point;
	return det;
}



//---------------------------------------------------------------------
// CM�ʒu�̒[����15�b��������
// ���́F
//   cmscope:     �����͈�
//   logo1st:     �擪�ʒu�̃��S�L���i0=�Ȃ�  1=����j
// ���o�́F
//   bounds: CM�m��ʒu
// �o�́F
//   �Ԃ�l: �\���ǉ�����ifalse=�Ȃ�  true=����j
//---------------------------------------------------------------------
bool JlsAutoReform::setCMFormEdge(RangeMsec &bounds, RangeWideMsec cmscope, bool logo1st){
	//--- ���S���̎g�p���f ---
	int lvlogo = pdata->getLevelUseLogo();
	ScpChapType chap_cmp;
	if (lvlogo < CONFIG_LOGO_LEVEL_USE_HIGH){
		chap_cmp = SCP_CHAP_CPOSIT;			// 5�b�P�ʍ\��
	}
	else{
		chap_cmp = SCP_CHAP_NONE;			// �d���ȊO���ׂ�
	}
	//--- ���S����̏C�����擾 ---
	FormCMEdgeSide sidest;
	FormCMEdgeSide sideed;
	bool valid_detect_st = setCMFormEdgeSideInfo(sidest, bounds.st, cmscope.st, chap_cmp, false);
	bool valid_detect_ed = setCMFormEdgeSideInfo(sideed, bounds.ed, cmscope.ed, chap_cmp, true);
	//--- �\������ ---
	int level;
	if (lvlogo <= CONFIG_LOGO_LEVEL_USE_LOW){
		level = 0;			// 15�b�P��
	}
	else if (lvlogo < CONFIG_LOGO_LEVEL_USE_HIGH){
		level = 1;			// 5�b�P��
	}
	else if (lvlogo == CONFIG_LOGO_LEVEL_USE_HIGH){
		level = 2;			// �b�P��
	}
	else{
		level = 3;			// �����Ȃ�
	}
	//--- �[�����̒ǉ��܂��̓J�b�g�̎��s ---
	bool change_st = false;
	bool change_ed = false;
	if (valid_detect_st && sidest.nscFixed >= 0){
		sidest.logoModePrev = logo1st;						// �擪���S��Ԃ�ݒ�
		change_st = setCMFormEdgeSetSide(sidest, level);
	}
	if (valid_detect_ed && sideed.nscFixed >= 0){
		change_ed = setCMFormEdgeSetSide(sideed, level);
	}
	//--- 15�b�\�����Ȃ��ꍇ�̃��S�O��ʒu�ō\���쐬 ---
	if (valid_detect_st && valid_detect_ed && sidest.nscFixed < 0 && sideed.nscFixed < 0){
		if (lvlogo == CONFIG_LOGO_LEVEL_USE_MIDL){
			level = 0;			// 15�b�P��
		}
		RangeNsc rnsc_detect = {sidest.nscDetect, sideed.nscDetect};
		RangeNsc rnsc_scope;
		rnsc_scope.st = pdata->getNscPrevScpChap(sideed.nscDetect, SCP_CHAP_DECIDE);
		rnsc_scope.ed = pdata->getNscNextScpChap(sidest.nscDetect, SCP_CHAP_DECIDE);
		change_st = setCMFormEdgeSetBoth(rnsc_detect, rnsc_scope, level, logo1st);
		change_ed = change_st;
	}
	//--- �͈͍X�V ---
	if (change_st){
		bounds.st = pdata->getMsecScp(sidest.nscDetect);
	}
	if (change_ed){
		bounds.ed = pdata->getMsecScp(sideed.nscDetect);
	}

	bool det = change_st | change_ed;
	return det;
}

//---------------------------------------------------------------------
// CM�ʒu�̒[�����i�Б��j�̃��S����̏C�����擾
// �o�́F
//   �Ԃ�l:  ���L������ifalse=�Ȃ�  true=����j
//   sidesel: ���S����̏C�����
//---------------------------------------------------------------------
bool JlsAutoReform::setCMFormEdgeSideInfo(FormCMEdgeSide &sidesel, Msec msec_bounds, WideMsec wmsec_scope, ScpChapType chap_cmp, bool endside){
	//--- ���S�\����ԓ��̒[�\����CM�����鏈�� ---
	int rev_del_edge = pdata->getConfigAction(CONFIG_ACT_LogoDelEdge);
	//--- �����͈͂�ݒ� ---
	bool valid_detect = true;
	WideMsec usescope = wmsec_scope;
	if (wmsec_scope.just < 0){
		valid_detect = false;
	}
	else{
		if (rev_del_edge <= 3){			// ���S���S�t�߂ɐ���
			if (endside == false){
				if (wmsec_scope.early < wmsec_scope.just - pdata->msecValSpc){
					usescope.early = wmsec_scope.just - pdata->msecValSpc;
				}
			}
			else{
				if (wmsec_scope.late > wmsec_scope.just + pdata->msecValSpc){
					usescope.late = wmsec_scope.just + pdata->msecValSpc;
				}
			}
		}
		if (rev_del_edge == 0){			// CM�ŏ���
			if (endside == false){
				if (msec_bounds <= wmsec_scope.late && msec_bounds >= 0){
					valid_detect = false;
				}
			}
			else{
				if (msec_bounds >= wmsec_scope.early && msec_bounds >= 0){
					valid_detect = false;
				}
			}
		}
	}
	if (valid_detect == false){
		sidesel.nscDetect = -1;
		return false;
	}

	//--- �ǂ���̒[��񂩋L�� ---
	sidesel.endSide = endside;
	//--- ���S�ω��͈͓��̖����V�[���`�F���W�����o ---
	sidesel.nscDetect = pdata->getNscFromWideMsecByChap(usescope, chap_cmp);
	//--- CM�\���m�肵�Ă���ʒu���擾 ---
	sidesel.nscFixed = pdata->getNscFromMsecChap(msec_bounds, pdata->msecValSpc, SCP_CHAP_DECIDE);
	if ((pdata->getNscFromWideMsecByChap(usescope, SCP_CHAP_DFORCE) >= 0) ||
		(pdata->getNscFromMsecChap(msec_bounds, pdata->msecValSpc, SCP_CHAP_DFORCE) >= 0)){
		sidesel.nscDetect = -1;			// �Œ�ӏ�������ΐݒ肵�Ȃ�
	}
	//--- CM�m��ʒu�Ɣ��Α��̊m��ʒu���擾 ---
	sidesel.nscOther = -1;							// �J�n�ʒu
	if (sidesel.nscDetect >= 0 && sidesel.nscFixed >= 0){
		if (sidesel.nscDetect < sidesel.nscFixed){
			sidesel.nscOther = pdata->getNscPrevScpChap(sidesel.nscFixed, SCP_CHAP_DECIDE);
			if (sidesel.nscOther >= sidesel.nscDetect && sidesel.nscOther >= 0){	// ��Ɋm��ʒu���������璆�~
				sidesel.nscDetect = -1;
			}
		}
		else if (sidesel.nscDetect > sidesel.nscFixed){
			sidesel.nscOther = pdata->getNscNextScpChap(sidesel.nscFixed, SCP_CHAP_DECIDE);
			if (sidesel.nscOther <= sidesel.nscDetect && sidesel.nscOther >= 0){	// ��Ɋm��ʒu���������璆�~
				sidesel.nscDetect = -1;
			}
		}
		else{
			sidesel.nscDetect = -1;
		}
	}
	//--- �O��̃��S����{�ݒ� ---
	if (endside == false){
		sidesel.logoModePrev = true;
		sidesel.logoModeNext = false;
	}
	else{
		sidesel.logoModePrev = false;
		sidesel.logoModeNext = true;
	}
	if (sidesel.nscDetect < 0) valid_detect = false;
	return valid_detect;
}

//---------------------------------------------------------------------
// CM�ʒu�̒[����15�b�������� - �Б��̎��s
// �o�́F
//   �Ԃ�l: �\���ǉ�����ifalse=�Ȃ�  true=����j
//---------------------------------------------------------------------
bool JlsAutoReform::setCMFormEdgeSetSide(FormCMEdgeSide &sidesel, int level){
	Msec msec_detect = pdata->getMsecScp(sidesel.nscDetect);
	Msec msec_fixed  = pdata->getMsecScp(sidesel.nscFixed);
	Msec msec_other  = pdata->getMsecScp(sidesel.nscOther);
	Msec msec_dif_fixed = abs(msec_detect - msec_fixed);
	Msec msec_dif_other = abs(msec_detect - msec_other);
	SearchDirType dr = (sidesel.nscDetect < sidesel.nscFixed)? SEARCH_DIR_PREV : SEARCH_DIR_NEXT;
	//--- �ΏۂQ�_���݂Ȃ��A�m��ʒu�Ɨאڎ��͎��s�Ȃ� ---
	if (sidesel.nscDetect < 0 ||
		sidesel.nscFixed  < 0 ||
		(msec_dif_fixed <= pdata->msecValLap2) ||
		(msec_dif_other <= pdata->msecValLap2 && sidesel.nscOther >= 0)){
		return false;
	}
	//--- �ΏۂQ�_�̋���������s�L�������o ---
	bool det = false;
	if (level <= 1){
		//--- 15�b�P��(level=0)�܂���5�b�P��(levle=1)�����o ---
		CalcModInfo calcmod;
		int tmpdet = calcDifMod0515(calcmod, msec_dif_fixed);
		if (tmpdet >= 2 || (level == 1 && tmpdet >= 1)){
			det = true;
		}
	}
	else{
		CalcDifInfo calc1;
		calcDifSelect(calc1, 0, msec_dif_fixed);
		//--- �b�P�ʂ̏ꍇ(level=2)���A�P�ʊ֌W�Ȃ���������ꍇ(level=3) ---
		if (calc1.gap <= pdata->msecValExact || level == 3){
			det = true;
		}
	}
	//--- ���s ---
	if (det){
		bool flag_other_s15  = (isCmLengthMsec(msec_dif_other) && sidesel.nscOther >= 0)? true : false;
		bool flag_fixed_s15  = isCmLengthMsec(msec_dif_fixed);
		pdata->setScpChap(sidesel.nscDetect, SCP_CHAP_DFIX);			// �Œ�ݒ�
		if (dr == SEARCH_DIR_PREV){			// �ݒ�n�_���O����
			ScpArType arstat_fixed;
			if (sidesel.logoModeNext){
				arstat_fixed = (flag_fixed_s15)? SCP_AR_L_UNIT : SCP_AR_L_OTHER;
			}
			else{
				arstat_fixed = (flag_fixed_s15)? SCP_AR_N_UNIT : SCP_AR_N_OTHER;
			}
			ScpArType arstat_detect;
			if (sidesel.logoModePrev){
				arstat_detect = (flag_other_s15)? SCP_AR_L_UNIT : SCP_AR_L_OTHER;
			}
			else{
				arstat_detect = (flag_other_s15)? SCP_AR_N_UNIT : SCP_AR_N_OTHER;
			}
			pdata->setScpArstat(sidesel.nscFixed,  arstat_fixed);
			pdata->setScpArstat(sidesel.nscDetect, arstat_detect);
		}
		else{							// �ݒ�n�_�������
			ScpArType arstat_detect;
			if (sidesel.logoModePrev){
				arstat_detect = (flag_fixed_s15)? SCP_AR_L_UNIT : SCP_AR_L_OTHER;
			}
			else{
				arstat_detect = (flag_fixed_s15)? SCP_AR_N_UNIT : SCP_AR_N_OTHER;
			}
			ScpArType arstat_other;
			if (sidesel.logoModeNext){
				arstat_other = (flag_other_s15)? SCP_AR_L_UNIT : SCP_AR_L_OTHER;
			}
			else{
				arstat_other = (flag_other_s15)? SCP_AR_N_UNIT : SCP_AR_N_OTHER;
			}
			pdata->setScpArstat(sidesel.nscDetect, arstat_detect);
			pdata->setScpArstat(sidesel.nscOther,  arstat_other);
		}
	}
	return det;
}

//---------------------------------------------------------------------
// CM�ʒu�̒[����15�b�������� - 15�b�P�ʍ\���m��ʒu���Ȃ��ꍇ�̂Q�_�Ԓǉ�
// �o�́F
//   �Ԃ�l: �\���ǉ�����ifalse=�Ȃ�  true=����j
//---------------------------------------------------------------------
bool JlsAutoReform::setCMFormEdgeSetBoth(RangeNsc rnsc_detect, RangeNsc rnsc_scope, int level, bool logo1st){
	RangeMsec rmsec_detect = getRangeMsec(rnsc_detect);
	RangeMsec rmsec_scope  = getRangeMsec(rnsc_scope);
	Msec msec_dif_st = abs(rmsec_detect.st - rmsec_scope.st);
	Msec msec_dif_ed = abs(rmsec_detect.ed - rmsec_scope.ed);
	Msec msec_dif_detect = abs(rmsec_detect.st - rmsec_detect.ed);

	//--- �ΏۂQ�_���݂Ȃ��A���o�O��֌W�t�A�m��ʒu�Ɨאڎ��͎��s�Ȃ� ---
	if (rnsc_detect.st < 0 ||
		rnsc_detect.ed < 0 ||
		(rnsc_detect.st >= rnsc_detect.ed) ||
		(rnsc_detect.st <= rnsc_scope.st && rnsc_scope.st >= 0) ||
		(rnsc_detect.ed >= rnsc_scope.ed && rnsc_scope.ed >= 0) ||
		(msec_dif_st <= pdata->msecValLap2 && rnsc_scope.st >= 0) ||
		(msec_dif_ed <= pdata->msecValLap2 && rnsc_scope.ed >= 0)){
		return false;
	}
	//--- �ΏۂQ�_�̋���������s�L�������o ---
	bool det = false;
	if (level <= 2){
		//--- 15�b�P��(level=0)�܂���5�b�P��(levle=1)�����o ---
		CalcModInfo calcmod;
		int tmpdet = calcDifMod0515(calcmod, msec_dif_detect);
		if (tmpdet >= 2 || (level == 1 && tmpdet >= 1)){
			det = true;
		}
	}
	else{
		CalcDifInfo calc1;
		calcDifSelect(calc1, 0, msec_dif_detect);
		//--- �b�P�ʂ̏ꍇ(level=2)���A�P�ʊ֌W�Ȃ���������ꍇ(level=3) ---
		if (calc1.gap <= pdata->msecValExact || level == 3){
			det = true;
		}
	}
	//--- ���s ---
	if (det){
		Nsc  nsc_end = rnsc_scope.ed;
		bool flag_st_s15 = isCmLengthMsec(msec_dif_st);
		bool flag_ed_s15 = isCmLengthMsec(msec_dif_ed);
		bool flag_detect_s15 = isCmLengthMsec(msec_dif_detect);
		bool logo_next = true;
		if (nsc_end >= 0){
			ScpArType arstat_end = pdata->getScpArstat(nsc_end);
			logo_next = jlsd::isScpArTypeLogo(arstat_end);
		}
		ScpArType arstat_c1;
		ScpArType arstat_c2;
		ScpArType arstat_c3;
		if (logo1st){
			arstat_c1  = (flag_st_s15)? SCP_AR_L_UNIT : SCP_AR_L_OTHER;
		}
		else{
			arstat_c1  = (flag_st_s15)? SCP_AR_N_UNIT : SCP_AR_N_OTHER;
		}
		arstat_c2 = (flag_detect_s15)? SCP_AR_N_UNIT : SCP_AR_N_OTHER;
		if (logo_next){
			arstat_c3  = (flag_ed_s15)? SCP_AR_L_UNIT : SCP_AR_L_OTHER;
		}
		else{
			arstat_c3  = (flag_ed_s15)? SCP_AR_N_UNIT : SCP_AR_N_OTHER;
		}
		pdata->setScpChap(rnsc_detect.st, SCP_CHAP_DFIX);
		pdata->setScpChap(rnsc_detect.ed, SCP_CHAP_DFIX);
		pdata->setScpArstat(rnsc_detect.st, arstat_c1);
		pdata->setScpArstat(rnsc_detect.ed, arstat_c2);
		if (nsc_end >= 0){
			pdata->setScpArstat(nsc_end,        arstat_c3 );
		}
	}
	return det;
}



//=====================================================================
// �Ώ۔͈͂̃X�R�A���쐬
//=====================================================================

//---------------------------------------------------------------------
// �Ώ۔͈͓��̃X�R�A�ݒ�
// ���́F
//   scope
//     st: �X�R�A�v�Z������J�n�ʒu
//     ed: �X�R�A�v�Z������I���ʒu
// �o�́F
//   pdata(score) : �Ώۈʒu�̃X�R�A
//   pdata(chap)  : �Ώۈʒu���\�����O�ɊԈ�������ꍇ-1��ݒ�
//---------------------------------------------------------------------
void JlsAutoReform::setScore(RangeMsec scope){
	int num_scpos = pdata->sizeDataScp();
	Nsc nsc_cur = 1;
	Nsc nsc_last = -1;
	bool over = false;
	if (nsc_cur >= num_scpos-1){				// �����Ō�܂Ŋ������͏I��
		over = true;
	}
	while( over == 0 ){
		ScpChapType chap_cur = pdata->getScpChap(nsc_cur);
		int msec_cur = pdata->getMsecScp(nsc_cur);
		if ((msec_cur >= scope.st || scope.st < 0) &&
			(msec_cur <= scope.ed || scope.ed < 0)){
			//--- �X�R�A�擾 ---
			int score = getScore(msec_cur, scope);
			int score_org = pdata->getScpScore(nsc_cur);
			if (chap_cur < SCP_CHAP_DECIDE || score >= score_org){
				pdata->setScpScore(nsc_cur, score);
			}
			//--- chap�ݒ� ---
			if (chap_cur < SCP_CHAP_DECIDE){
				if (score >= 30){
					chap_cur = SCP_CHAP_CPOSQ;
				}
				else if (score >= 10){
					chap_cur = SCP_CHAP_CPOSIT;
				}
				else{
					chap_cur = SCP_CHAP_NONE;
				}
				pdata->setScpChap(nsc_cur, chap_cur);
			}
			//--- �אڌ�₠��ΊԈ��� ---
			if (nsc_last >= 0 && chap_cur > SCP_CHAP_NONE){
				ScpChapType chap_last = pdata->getScpChap(nsc_last);
				int msec_last = pdata->getMsecScp(nsc_last);
				int msec_diff = msec_cur - msec_last;
				if (msec_diff <= pdata->msecValLap1){
					if (pdata->getScpScore(nsc_last) < score &&
						chap_last < SCP_CHAP_DECIDE){
						pdata->setScpChap(nsc_last, SCP_CHAP_DUPE);		// �O�����Ԉ���
					}
					else if (chap_cur <= 2){
						pdata->setScpChap(nsc_cur, SCP_CHAP_DUPE);		// ���ʒu���Ԉ���
					}
				}
			}
		}
		//--- ���ʒu�ݒ� ---
		if (chap_cur > SCP_CHAP_NONE){
			nsc_last = nsc_cur;
		}
		nsc_cur ++;
		if (nsc_cur >= num_scpos-1){
			over = true;
		}
	}
}

void JlsAutoReform::setScore(RangeFixMsec fixscope){
	RangeMsec scope = {fixscope.st, fixscope.ed};
	setScore(scope);
}


//=====================================================================
// �Ώ۔͈͂̍\����؂���쐬
//=====================================================================

//---------------------------------------------------------------------
// �Ώ۔͈͓��̐����\�������쐬
// ���́F
//   fixscope
//     st:     �\�������J�n�ʒu
//     ed:     �\�������I���ʒu
//     fixSt:  �J�n�����Œ�󋵁i0:��� 1:�Œ�j
//     fixEd:  �I�������Œ�󋵁i0:��� 1:�Œ�j
// �o�́F
//   pdata(chap)  : �Ώۈʒu���\�����Ƃ��Đݒ�
//---------------------------------------------------------------------
void JlsAutoReform::setChap(RangeFixMsec fixscope){
	TraceChap trace;
	trace.numDa = 0;
	trace.numDb = 0;

	//--- ��؂茟�o�J�n ---
	int num_scpos = pdata->sizeDataScp();
	Nsc nsc_cur = 1;
	bool over = false;
	if (nsc_cur >= num_scpos-1){					// �����Ō�܂Ŋ������͏I��
		over = true;
	}
	//--- �͈͓��̌��\�������� ---
	while( over == false ){
		ScpChapType chap_cur = pdata->getScpChap(nsc_cur);
		Msec msec_cur = pdata->getMsecScp(nsc_cur);
		//--- �͈͓��̌��ʒu�ł���Ώ��� ---
		if ((msec_cur >= fixscope.st || fixscope.st < 0) &&
			(msec_cur <= fixscope.ed || fixscope.ed < 0) &&
			chap_cur > SCP_CHAP_NONE){
			setChapUpdate(trace, nsc_cur);
			setChapGuess( trace, nsc_cur);
		}
		//--- ���̈ʒu�ݒ� ---
		nsc_cur ++;
		if ((nsc_cur >= num_scpos-1) ||
			(msec_cur > fixscope.ed && fixscope.ed > 0)){
			over = true;
		}
		//--- �ŏI�m�菈�� ---
		if (over){
			setChapUpdate(trace, -1);
		}
	}
	//--- �d���폜 ---
	setChapThinOverlap(fixscope);
}

//---------------------------------------------------------------------
// �Ώۈʒu�̐����\�������X�V
// ���́F
//   trace
//     num_d:   ���o�́F�ێ��f�[�^���i�T�C�Y=2�A�p���{�V���j
//     nsc_da:  ���o�́F�ێ��f�[�^�̌p�����ʒu�i�T�C�Y=SIZE_SETFORM_KEEP�j
//     nsc_db:  ���o�́F�ێ��f�[�^�̐V���ʒu�i�T�C�Y=SIZE_SETFORM_KEEP�j
//   nsc_cur:   �ǉ�����f�[�^�̈ʒu�i-1�̎��͊��������j
// �o�́F
//   pdata(chap)  : �Ώۈʒu���\�����Ƃ��Đݒ�
//---------------------------------------------------------------------
void JlsAutoReform::setChapUpdate(TraceChap &trace, Nsc nsc_cur){
	Msec msec_cur = 0;
	if (nsc_cur >= 0){
		msec_cur = pdata->getMsecScp(nsc_cur);
	}
	//--- �V���ʒu�̊m�菈�� ---
	if (trace.numDb > 0){
		//--- �V���̐擪�ʒu���擾 ---
		Msec msec_chk = pdata->getMsecScp(trace.nscDb[0]);
		Msec msec_dif;
		if (nsc_cur >= 0){
			msec_dif = msec_cur - msec_chk;
		}
		else{						// ���������̏ꍇ
			msec_dif = 180 * 1000 + pdata->msecValLap2;
		}
		//--- �ŏ��̐V��₩��3���ȏ�o�߂����ꍇ ---
		if (msec_dif >= 180 * 1000 + pdata->msecValLap2){
			int msec_tmp;
			if (trace.numDa > 0){
				int nsc_tmp = trace.nscDa[trace.numDa-1];
				msec_tmp  = pdata->getMsecScp(nsc_tmp);
			}
			else{
				msec_tmp = msec_chk - 1;
			}
			//--- �V���̌�Ɍp����₪�Ȃ���ΐV�����m�肷�� ---
			if (msec_chk > msec_tmp && trace.numDb > 1){		// 2�����ȏ�̌��
				for(int j=0; j<trace.numDb; j++){
					int nsc_tmp = trace.nscDb[j];
					trace.nscDa[j] = nsc_tmp;
				}
				trace.numDa = trace.numDb;
				trace.numDb = 0;
				//--- �X�V ---
				setChapFixed(trace);
			}
			else{											// ����ȊO�̎��͍폜
				trace.numDb = 0;
			}
		}
	}
	//--- �p�����̊m�菈�� ---
	if (trace.numDa > 0){
		bool flag_loop = true;
		while(flag_loop){
			flag_loop = false;
			//--- �p�����̐擪�ʒu���擾 ---
			Nsc nsc_chk = trace.nscDa[0];
			Msec msec_chk = pdata->getMsecScp(nsc_chk);
			Msec msec_dif;
			if (nsc_cur >= 0){
				msec_dif = msec_cur - msec_chk;
			}
			else{						// ���������̏ꍇ
				msec_dif = 180 * 1000 + pdata->msecValLap2;
			}
			//--- �擪�̌p����₩��3���ȏ�o�߂����ꍇ ---
			if (msec_dif >= 180 * 1000 + pdata->msecValLap2){
				setChapFixed(trace);
				//--- �m�肵�Č�₩��폜 ---
				for(int j=1; j<trace.numDa; j++){
					int nsc_tmp = trace.nscDa[j];
					trace.nscDa[j-1] = nsc_tmp;
				}
				trace.numDa -= 1;
				//--- �p����₪�c���Ă���ꍇ�͎��ʒu������ ---
				if (trace.numDa > 0){
					flag_loop = true;
				}
			}
		};
	}
}

//---------------------------------------------------------------------
// �Ώۈʒu�̐����\�������쐬
// ���o�́F
//   trace
//     num_d:   ���o�́F�ێ��f�[�^���i�T�C�Y=2�A�p���{�V���j
//     nsc_da:  ���o�́F�ێ��f�[�^�̌p�����ʒu�i�T�C�Y=SIZE_SETFORM_KEEP�j
//     nsc_db:  ���o�́F�ێ��f�[�^�̐V���ʒu�i�T�C�Y=SIZE_SETFORM_KEEP�j
//   nsc_cur:   �ǉ�����f�[�^�̈ʒu�i-1�̎��͊��������j
//---------------------------------------------------------------------
void JlsAutoReform::setChapGuess(TraceChap &trace, Nsc nsc_cur){
	//--- �L���m�F ---
	if (nsc_cur < 0) return;

	//--- �ǉ��f�[�^�����擾 ---
	int score_cur = pdata->getScpScore(nsc_cur);
	int msec_cur  = pdata->getMsecScp(nsc_cur);
	//--- �X�R�A���̐����i�p�����j ---
	int scoremax_prev = 0;
	int scoremax_mid  = 0;
	int scoremax_post = 0;
	if (trace.numDa > 0){
		Nsc nsc_chks;
		Nsc nsc_chke;
		if (trace.numDb > 0){			// �V��₪���݂�����
			nsc_chks = trace.nscDb[0];	// �V���ŏ��̈ʒu
			nsc_chke = trace.nscDb[trace.numDb-1];
		}
		else{							// �V���Ȃ���Όp����₷�ׂ�
			nsc_chks = trace.nscDa[trace.numDa-1]+1;
			nsc_chke = nsc_chks;
		}
		//--- �V���ŏ��̈ʒu�܂ł̍ő�X�R�A�擾 ---
		for(int j=0; j<trace.numDa; j++){
			Nsc  nsc_tmp   = trace.nscDa[j];
			Msec msec_tmp  = pdata->getMsecScp(nsc_tmp);
			Msec msec_dif  = abs(msec_cur - msec_tmp);
			int  score_tmp = setChapGetDistanceScore(nsc_tmp, msec_dif);
			if (nsc_tmp < nsc_chks){
				if (scoremax_prev < score_tmp){
					scoremax_prev = score_tmp;
				}
			}
			else if (nsc_tmp > nsc_chke){
				if (scoremax_mid < score_tmp){
					scoremax_mid = score_tmp;
				}
			}
		}
		scoremax_post = setChapMaxscore(trace.nscDa[trace.numDa-1]);
	}
	//--- �X�R�A���̐����i�V���j ---
	int scoremax_db = 0;
	if (trace.numDb > 0){
		for(int j=0; j<trace.numDb; j++){
			int  nsc_tmp   = trace.nscDb[j];
			Msec msec_tmp  = pdata->getMsecScp(nsc_tmp);
			Msec msec_dif  = abs(msec_cur - msec_tmp);
			int  score_tmp = setChapGetDistanceScore(nsc_tmp, msec_dif);
			if (scoremax_db < score_tmp){
				scoremax_db = score_tmp;
			}
		}
	}

	//--- �ێ���񂩂�p�����(k=0)�ƐV���(k=1)��5/15�b�P�ʋ������擾 ---
	int chkmod_d[2];
	int difmod_d[2];
	for(int k=0; k<2; k++){
		chkmod_d[k] = 0;
		int jmax = (k==0)? trace.numDa : trace.numDb;
		for(int j=0; j<jmax; j++){
			int nsc_chk;
			if (k == 0){
				nsc_chk = trace.nscDa[j];
			}
			else{
				nsc_chk = trace.nscDb[j];
			}
			int msec_dif = msec_cur - pdata->getMsecScp(nsc_chk);
			CalcModInfo calcmod;
			int modtype = calcDifMod0515(calcmod, msec_dif);
			//--- �O��15�b�P�ʁA���O��15�b�P�ʂł͂Ȃ����̏��� ---
			if (chkmod_d[k] == 2 && modtype == 0){
				int score_tmp = pdata->getScpScore(nsc_chk);
				if (score_cur <= score_tmp){
					chkmod_d[k] = 0;
				}
				else if (msec_dif <= 31 * 1000){
					// ��₩��폜
					if (pdata->getScpChap(nsc_chk) >= SCP_CHAP_CDET){
						pdata->setScpChap(nsc_chk, SCP_CHAP_NONE);
					}
					// ��₩��폜���Č�̃f�[�^���l�߂�
					for(int m=j; m<jmax-1; m++){
						if (k==0) trace.nscDa[m] = trace.nscDa[m+1];
						else	  trace.nscDb[m] = trace.nscDb[m+1];
					}
					if (k==0) trace.numDa--;
					else	  trace.numDb--;
					j--;
					jmax--;
				}
			}
			//--- 15/5�b�P�ʍœK�ӏ��擾 ---
			if (modtype >= 2){					// 15�b�P��
				if (chkmod_d[k] == 2){
					if (difmod_d[k] > calcmod.mod15){
						difmod_d[k] = calcmod.mod15;
					}
				}
				else if (chkmod_d[k] < 2){
					chkmod_d[k] = 2;			// 15�b�P�ʂ�ݒ�
					difmod_d[k] = calcmod.mod15;
				}
			}
			else if (modtype > 0){				// 5�b�P��
				if  (chkmod_d[k] == 1){
					if (difmod_d[k] > calcmod.mod05){
						difmod_d[k] = calcmod.mod05;
					}
				}
				else if (chkmod_d[k] < 1){
					chkmod_d[k] = 1;			// 5�b�P�ʂ�ݒ�
					difmod_d[k] = calcmod.mod05;
				}
			}
		}
	}
	//--- 5/15�b�P�ʏ�񂩂���Ƃ��Ēǉ����Ă��� ---
	//--- �p�����ʒu����5/15�b�P�ʂ̏ꍇ ---
	if (chkmod_d[0] > 0){
		//--- �V�������݂���ꍇ
		if (trace.numDb > 0){
			//--- �p�����̗����X�R�A��������ΐV�������� ---
			if (scoremax_prev >= scoremax_db && score_cur >= scoremax_db){
				trace.numDb = 0;
			}
			//--- �V������̌p�����̃X�R�A�������ꍇ�V�������� ---
			else if (scoremax_db < scoremax_mid){
				trace.numDb = 0;
			}
		}
		//--- �V��₪���݂��ċ������߂���ΐV��⑤�ɃZ�b�g ---
		if (chkmod_d[1] > 0 && trace.numDb > 0 && difmod_d[0] > difmod_d[1]){
			chkmod_d[0] = 0;
		}
		//--- �p�����Ƃ��ď��� ---
		else{
			chkmod_d[1] = 0;
			//--- ��t�̏ꍇ�͂P���� ---
			if (trace.numDa >= SIZE_SETFORM_KEEP){
				setChapFixed(trace);
				for(int j=1; j<SIZE_SETFORM_KEEP; j++){
					trace.nscDa[j-1] = trace.nscDa[j];
				}
				trace.numDa = SIZE_SETFORM_KEEP - 1;
			}
			//--- �p�����ɒǉ� ---
			trace.nscDa[trace.numDa] = nsc_cur;
			trace.numDa += 1;
			//--- 15�b�P�ʂł���ΐV���Ȃ���ԂȂ琄�����\���Ƃ��Ēǉ� ---
			if (chkmod_d[0] > 1 && trace.numDa > 1 && trace.numDb == 0){
				//--- �\�������Ȃ��ꍇ�͐��� ---
				setChapFixed(trace);
			}
		}
	}
	//--- �V���ʒu����5/15�b�P�ʂ̏ꍇ ---
	if (chkmod_d[1] > 0){
		//--- �O�̂��߈�t�ł͂Ȃ����Ɗm�F���ĐV���ɒǉ� ---
		if (trace.numDb < SIZE_SETFORM_KEEP){
			trace.nscDb[trace.numDb] = nsc_cur;
			trace.numDb += 1;
		}
		//--- 15�b�P�ʂł���Όp�����Ɉڂ��������\���Ƃ��Ēǉ� ---
		if (chkmod_d[1] > 1){
			bool flag_change = true;
			//--- �V���S�̂̃X�R�A���p�����Ō�̃X�R�A��菬�������͍X�V���f ---
			if (trace.numDa > 0){
				int nsc_k0 = trace.nscDa[trace.numDa-1];
				int score_k0 = pdata->getScpScore(nsc_k0);
				int score_k1 = 0;
				for(int j=0; j<trace.numDb; j++){
					score_k1 += pdata->getScpScore(trace.nscDb[j]);
				}
				if (score_k0 >= score_k1){
					int msec_dif = msec_cur - pdata->getMsecScp(nsc_k0);
					if (msec_dif < 60*1000){	// �p�����Ōォ��܂��P������ĂȂ�
						flag_change = false;
					}
				}
			}
			if (flag_change == true){
				for(int j=0; j<trace.numDb; j++){
					int nsc_chk = trace.nscDb[j];
					trace.nscDa[j] = nsc_chk;
				}
				trace.numDa = trace.numDb;
				trace.numDb = 0;
				setChapFixed(trace);
			}
		}
	}
	//--- �p�����ƐV���ʒu�ǂ��炩���5/15�b�P�ʂłȂ��ꍇ ---
	if (chkmod_d[0] == 0 && chkmod_d[1] == 0){
		//--- �p����₪�Ȃ���Ζ������Œǉ� ---
		if (trace.numDa == 0){
			trace.nscDa[0] = nsc_cur;
			trace.numDa += 1;
		}
		//--- �p�����̗����ǂ��炩���ǉ��f�[�^���X�R�A����������� ---
		else if (scoremax_prev < score_cur || scoremax_post < score_cur){
			//--- �V��₪�Ȃ���Ζ������Œǉ� ---
			if (trace.numDb == 0){
				trace.nscDb[0] = nsc_cur;
				trace.numDb = 1;
			}
			//--- �V�����X�R�A���傫����Βǉ� ---
			else if (scoremax_db < score_cur){
				trace.nscDb[0] = nsc_cur;
				trace.numDb = 1;
			}
		}
	}
//printf("(d:%d,A:",nsc_cur);
//for(int t1=0; t1<trace.numDa; t1++){
//	printf("%d,", trace.nscDa[t1]);
//}
//printf("B:");
//for(int t1=0; t1<trace.numDb; t1++){
//	printf("%d,", trace.nscDb[t1]);
//}
}

//---------------------------------------------------------------------
// �Ώۈʒu�ȍ~�̂̍ő�X�R�A�擾
// ���́F
//   nsc_target: �Ώۈʒu�ԍ�
// �o�́F
//   �Ԃ�l: �Ώۈʒu�ȍ~�̍ő�X�R�A
//---------------------------------------------------------------------
int JlsAutoReform::setChapMaxscore(Nsc nsc_target){
	int num_scpos = pdata->sizeDataScp();
	int max_score = 0;
	int i = nsc_target + 1;					// �Ώۈʒu�̎����猟���J�n
	bool over = false;
	if (i >= num_scpos-1 || i < 0){
		over = true;
	}
	while( over == false ){
		//--- �����ʒu�ƑΏۈʒu�̋����擾 ---
		Msec msec_target = pdata->getMsecScp(nsc_target);
		Msec msec_i = pdata->getMsecScp(i);
		Msec msec_dif = msec_i - msec_target;
		//--- 5/15�b�P�ʂ̈ʒu�ł���΃X�R�A�ǂݍ��� ---
		CalcModInfo calcmod;
		int modtype = calcDifMod0515(calcmod, msec_dif);
		if (modtype > 0){
			int score = setChapGetDistanceScore(i, msec_dif);
			if (max_score < score){
				max_score = score;
			}
		}
		//--- 180�b�ȏ㗣��Ă��鎞�͏I�� ---
		if (calcmod.mod15 < 0 || i >= num_scpos-1){
			over = true;
		}
		i ++;
	}
	return max_score;
}

//---------------------------------------------------------------------
// �����ɂ��X�R�A�C��
// ���́F
//   nsc:      �Ώۈʒu�ԍ�
//   msec_dif: �Ώۈʒu�܂ł̋����~���b
// �o�́F
//   �Ԃ�l: �Ώۈʒu�̋����␳�ς݃X�R�A
//---------------------------------------------------------------------
int JlsAutoReform::setChapGetDistanceScore(Nsc nsc, Msec msec_dif){
	int score = pdata->getScpScore(nsc);;
	if (msec_dif > 122 * 1000){			// 2�������ė���Ă���X�R�A�͔�������
		score = score / 2;
	}
	else if (msec_dif > 62 * 1000){		// 1�������ė���Ă���X�R�A�͌�������
		score = score * 3 / 4;
	}
	return score;
}

//---------------------------------------------------------------------
// �m��\�Ȑ����\�����m��
// ���́F
//   trace
//     num_da:  �ێ��f�[�^���i�p�����j
//     num_db:  �ێ��f�[�^���i�V���j
//     nsc_da:  �ێ��f�[�^�̌p�����ʒu�i�T�C�Y=SIZE_SETFORM_KEEP�j
//     nsc_db:  �ێ��f�[�^�̐V���ʒu�i�T�C�Y=SIZE_SETFORM_KEEP�j
// �o�́F
//   pdata(chap)  :  �Ώۈʒu���\�����Ƃ��Đݒ�
//---------------------------------------------------------------------
bool JlsAutoReform::setChapFixed(TraceChap &trace){
	bool flag_strict = setChapFixedLimit(trace);
	if (flag_strict == false){
		//--- ���m��ʒu ---
		Nsc nsc_wait = -1;
		if (trace.numDb > 0){
			nsc_wait = trace.nscDb[0];
		}
		for(int j=0; j<trace.numDa; j++){
			Nsc nsc_chap = trace.nscDa[j];
			if (nsc_chap < nsc_wait || nsc_wait < 0){
				ScpChapType chap_dst = pdata->getScpChap(nsc_chap);
				if (chap_dst < SCP_CHAP_CDET){
					pdata->setScpChap(nsc_chap, SCP_CHAP_CDET);
				}
			}
		}
		return true;
	}
	return false;
}

//---------------------------------------------------------------------
// �������Ȃ��\���ł͍\���𐧌�����
// ���́F
//   trace
//     num_da: �ێ��f�[�^���i�p�����j
//     nsc_da: �ێ��f�[�^�̌p�����ʒu�i�T�C�Y=SIZE_SETFORM_KEEP�j
// �o�́F
//   �Ԃ�l:  false=�����Ȃ�  true=�\���͖���
//---------------------------------------------------------------------
bool JlsAutoReform::setChapFixedLimit(TraceChap &trace){
	if (trace.numDa <= 1){
		return true;
	}

	bool flag_remove = false;
	int sum_s15 = 0;
	int sum_s05 = 0;
	for(int i=0; i<trace.numDa-1; i++){
		Nsc nsc_from  = trace.nscDa[i];
		Nsc nsc_to    = trace.nscDa[i+1];
		Msec msec_from = pdata->getMsecScp(nsc_from);
		Msec msec_to   = pdata->getMsecScp(nsc_to);
		//--- �Ԋu ---
		Sec sec_dif   = (msec_to - msec_from + 500) / 1000;
		Sec sec_dif05 = ((sec_dif + 2) / 5) * 5;
		Msec msec_new  = msec_from + (sec_dif05 * 1000);
		Msec msecdif   = abs(msec_to - msec_new);
		//--- ��� ---
		bool flag_still = false;
		if ((pdata->getScpStill(nsc_from)) ||
			(pdata->getScpStill(nsc_to))){
			flag_still = true;
		}
		bool flag_both_still = false;
		if ((pdata->getScpStill(nsc_from)) &&
			(pdata->getScpStill(nsc_to))){
			flag_both_still = true;
		}
		bool flag_nomute      = false;
		bool flag_long_nomute = false;
		if (nsc_to - nsc_from == 1){
			flag_nomute      = true;
			flag_long_nomute = true;
		}
		else{
			bool flag_sub = true;
			for(int k=nsc_from+1; k<nsc_to; k++){
				if (pdata->isSmuteSameArea(nsc_from, k) == false &&
					pdata->isSmuteSameArea(nsc_to  , k) == false){
					flag_sub = false;
				}
				else{
					int msec_k = pdata->getMsecScp(k);
					if (abs(msec_k - msec_from) > pdata->msecValLap2 ||
						abs(msec_to - msec_k  ) > pdata->msecValLap2){
						flag_sub = false;
					}
				}
			}
			if (flag_sub == true){
				flag_long_nomute = true;
			}
		}
		//--- �ݒ�Ŗ����Ȃ������ɂ���ꍇ ---
		bool calcel_cntsc = pdata->getConfig(CONFIG_VAR_cancelCntSc);
		if (calcel_cntsc){
			flag_nomute = true;
		}
		//--- 30�b�ȉ��\���̐��� ---
		if (sec_dif05 <= 30 && sec_dif05 > 0){
			//--- ��������---
			if (flag_still == false){
				if (msecdif <= pdata->msecValExact){	// ���m�ȕb���\��
					sum_s05 ++;
					if (flag_nomute){					// �Ԃɖ������Ȃ��\��
						sum_s05 ++;
					}
				}
			}
		}
		//--- 15�b�{���\���̐��� ---
		if ((sec_dif05 % 15 == 0) && sec_dif05 > 0){
			//--- 60�b�ȏ��15�b�{���\�� ---
			if ((sec_dif05 % 30 == 0) && (sec_dif05 >= 60)){
				if (flag_long_nomute){						// �Ԃɖ������Ȃ��\��
					sum_s15 += 6;
				}
				else if (msecdif <= pdata->msecValExact && flag_still == false){
					sum_s15 += 6;
				}
			}
			//--- �������� ---
			if (flag_still == false){
				sum_s15 ++;
			}
			else if (flag_nomute){		// �����Ȃ��Ă��אڂȂ�ǉ�
				sum_s15 ++;
			}
			//--- �ʒu���� ---
			if (msecdif <= pdata->msecValNear3){
				sum_s15 ++;
			}
			else if (flag_nomute){		// �ʒu���ꂠ���Ă��אڂȂ�ǉ�
				sum_s15 ++;
			}
			if (flag_both_still == false){
				if (msecdif <= pdata->msecValNear1){
					sum_s15 ++;
				}
			}
		}
	}
	if (sum_s05 < 2 && sum_s15 < 6){
		flag_remove = true;
	}
	return flag_remove;
}


//---------------------------------------------------------------------
// �אڂ�����₪����ΕБ�������
//---------------------------------------------------------------------
void JlsAutoReform::setChapThinOverlap(RangeFixMsec fixscope){
	//--- �Œ�ӏ��擾 ---
	Nsc nsc_fix_st = -1;
	Nsc nsc_fix_ed = -1;
	if (fixscope.fixSt){
		nsc_fix_st = pdata->getNscFromMsecAll(fixscope.st);
	}
	if (fixscope.fixEd){
		nsc_fix_ed = pdata->getNscFromMsecAll(fixscope.ed);
	}
	//--- �͈͓��̌��\�������� ---
	Nsc nsc_cur  = 0;
	Msec msec_cur = 0;
	bool over = false;
	while( over == false ){
		Nsc nsc_last  = nsc_cur;
		Msec msec_last = msec_cur;
		nsc_cur = pdata->getNscNextScpChap(nsc_last, SCP_CHAP_CDET);
		msec_cur = pdata->getMsecScp(nsc_cur);
		if (nsc_cur < 0){
			over = true;
		}
		else if (msec_cur > fixscope.ed && fixscope.ed >= 0){
			over = true;
		}
		else if (nsc_last <= 0){
		}
		//--- �͈͓��̌��ʒu�ł���Ώ��� ---
		else if ((msec_cur  >= fixscope.st || fixscope.st < 0) &&
				 (msec_last <= fixscope.ed || fixscope.ed < 0)){
			int msecdif_cur = abs(msec_cur - msec_last);
			//--- �אڂ��Ă���ꍇ�͕Б����O�� ---
			if (msecdif_cur < pdata->msecValLap2){
				int score_last = pdata->getScpScore(nsc_last);
				int score_cur  = pdata->getScpScore(nsc_cur);
				bool flag_fix_last = (nsc_last == nsc_fix_st || nsc_last == nsc_fix_ed)? true : false;
				bool flag_fix_cur  = (nsc_cur  == nsc_fix_st || nsc_cur  == nsc_fix_ed)? true : false;
				bool flag_score_last = (score_last >= score_cur)? true : false;
				if ((flag_fix_last || flag_score_last) && !flag_fix_cur){
					pdata->setScpChap(nsc_cur, SCP_CHAP_CPOSQ);
				}
				if ((flag_fix_cur || !flag_score_last) && !flag_fix_last){
					pdata->setScpChap(nsc_last, SCP_CHAP_CPOSQ);
				}
			}
		}
	}
}



//=====================================================================
// �Ώۈʒu�̃X�R�A���擾
//=====================================================================

//---------------------------------------------------------------------
// �Ώۈʒu�̎w��͈͓��ł̃X�R�A�擾
// ���́F
//   msec_target: �Ώۈʒu�̃t�@�C���擪����̃~���b��
//   scope
//     st:        �X�R�A�v�Z������J�n�ʒu
//     ed:        �X�R�A�v�Z������I���ʒu
// �o�́F
//   �Ԃ�l: �Ώۈʒu�̎Z�o���ʃX�R�A
//---------------------------------------------------------------------
int JlsAutoReform::getScore(Msec msec_target, RangeMsec scope){
	int score_all = 0;
	int score_m3 = 0;
	int score_m2 = 0;
	int score_m1 = 0;
	int score_mt = 0;
	int score_sec5 = 0;
	Nsc nsc_cur = 0;
	//--- target�ʒu����O��R�������� ---
	for(int i=-16; i<=16; i++){
		//--- �����ʒu�ݒ� ---
		Sec sec_width;
		if (i < -6){					// -180�b <= sec_width < -30�b
			sec_width = i * 15 + 60;
		}
		else if (i <= 6){				// -30�b <= sec_width <= 30�b
			sec_width = i * 5;
		}
		else{							// 30�b < sec_width <= 180�b
			sec_width = i * 15 - 60;
		}
		Msec msec_cmp = msec_target + sec_width * 1000;		// ���ɔ�r����ʒu

		// --- �X�R�A�ǉ� ---
		if ((scope.st <= msec_cmp + pdata->msecValLap2 || scope.st < 0) &&
			(scope.ed >= msec_cmp - pdata->msecValLap2 || scope.ed < 0) &&
			sec_width != 0){

			//--- ��ԋ߂��̍\���ʒu���� ---
			int msec_dist_min = getScoreDist(nsc_cur, msec_cmp);
			//--- �����m�F ---
			bool smute_det;
			bool still_det;
			if ((scope.st < 0 && sec_width < 0) ||
				(scope.ed < 0 && sec_width > 0)){	// �͈͂Ȃ��ł͖����`�F�b�N�Ȃ�
				smute_det = 0;
				still_det = 0;
			}
			else{
				smute_det = pdata->isSmuteFromMsec(msec_cmp);
				still_det = pdata->isStillFromMsec(msec_cmp);
			}
			//--- �X�R�A�ǉ� ---
			int score_tmp = getScoreTarget(msec_dist_min, sec_width, smute_det, still_det);
			if (abs(sec_width) > 120){			// 120�b����
				score_m3 += score_tmp;
			}
			else if (abs(sec_width) > 60){		// 60�b���� - 120�b
				score_m2 += score_tmp;
			}
			else if (abs(sec_width) % 15 == 0){	// 60�b�܂�15�b�P��
				score_m1 += score_tmp;
				if (abs(sec_width) <= 30){		// 30�b�܂�15�b�P��
					score_mt += score_tmp;
				}
			}
			else{								// 5�b�P��
				score_sec5 += score_tmp;
			}
		}
	}
	//--- 30�b�ȓ��̃X�R�A���傫��1���ȓ��̑啔�����߂�ꍇ�͂P�������̏���𐧌� ---
	if (score_mt >= 50 && score_m1 <= score_mt + 50/2){
		if (score_m2 > (score_m1 - score_mt)){
			score_m2 = (score_m1 - score_mt);
		}
	}
	//--- 2�������́A1���������1���O�̃X�R�A���傫����Ώ���𐧌� ---
	if (score_m3 > score_m2 / 2 && score_m2 < score_m1){
		score_m3 = score_m2 / 2;
	}
	int score_ma = score_m1 + score_m2 + score_m3;
	//--- �X�R�A���傫������15�b�P�ʂ݂̂ɂ���5�b�P�ʂ͊܂߂Ȃ� ---
	if (score_ma + score_sec5 >= 50){
		if (score_ma >= 50){
			score_all = score_ma;
		}
		else{
			score_all = 50;
		}
	}
	else{
		score_all = score_ma + score_sec5;
	}
	return score_all;
}
int JlsAutoReform::getScore(Msec msec_target, RangeWideMsec range_wide_scope){
	RangeMsec scope = {range_wide_scope.st.early, range_wide_scope.ed.late};
	return getScore(msec_target, scope);
};

//---------------------------------------------------------------------
// �Ώۈʒu�Ɉ�ԋ߂�����SC�̋����擾
// ���́F
//   msec_cmp:  ��r�Ώۈʒu�̃t�@�C���擪����̃~���b��
// ���o�́F
//   nsc_cur: �������J�n����ʒu�ԍ��i���̌����Z�k�̂��ߒl�X�V�j
// �o�́F
//   �Ԃ�l: -1=�ΏۂƂȂ�\���Ȃ� 0�ȏ�=����msec
//---------------------------------------------------------------------
int JlsAutoReform::getScoreDist(Nsc &nsc_cur, Msec msec_cmp){
	int num_scpos = pdata->sizeDataScp();

	Nsc  nsc_cand = -1;
	Msec msec_dist_min = -1;
	Msec msec_val_valid = pdata->msecValNear3;
	bool over = 0;
	if (nsc_cur >= num_scpos-1){				// �����Ō�܂Ŋ������͏I��
		over = true;
	}
	while( over == false ){
		Msec msec_cur = pdata->getMsecScp(nsc_cur);
		Msec msec_dist_cur = abs(msec_cur - msec_cmp);	// ��r�ʒu�ƌ��݈ʒu�̍���
		if (msec_dist_cur <= msec_val_valid){			// �������L���͈͓�
			//--- ��ԋ߂��ʒu�̂݋L�� ---
			if (nsc_cand < 0 || msec_dist_cur < msec_dist_min){
				nsc_cand = nsc_cur;
				msec_dist_min = msec_dist_cur;
			}
		}
		//--- ���̈ʒu���� ---
		if (msec_cur > msec_cmp + msec_val_valid){	// ���݂̗L���͈͊O�܂ŏI��
			over = true;
		}
		else{
			nsc_cur ++;								// ���̈ʒu
			if (nsc_cur >= num_scpos-1){				// �ŏI�ʒu�ȍ~
				over = true;
			}
		}
	}
	return msec_dist_min;
}


//---------------------------------------------------------------------
// �����ʂ̒P�̃X�R�A�擾
// ���́F
//	msec_dist				// �Ώۂ܂ł̋������i�~���b�j
//	sec_width				// ���o���Ă��鋗���i�b�j
//	smute_det				// �������i1=�����j
//	still_det				// �����Ȃ����i1=�����Ȃ��j
// �o�́F
//   �Ԃ�l: �Ώۈʒu�̎Z�o���ʃX�R�A
//---------------------------------------------------------------------
int JlsAutoReform::getScoreTarget(Msec msec_dist, int sec_width, bool smute_det, bool still_det){
	int score = 0;
	int flag_d15 = 0;
	//--- ����ӏ� ---
	int abs_sec = abs(sec_width);
	if (abs_sec % 15 == 0){
		flag_d15 = 1;
	}
	//--- �����ʂɕ��� ---
	int ndist = 0;
	if (msec_dist < 0){
		ndist = -1;
	}
	else if (msec_dist <= pdata->msecValExact){
		ndist = 3;
	}
	else if (msec_dist <= pdata->msecValNear1){
		ndist = 2;
	}
	else if (msec_dist <= pdata->msecValNear2){
		ndist = 1;
	}
	//--- �X�R�A�t�� ---
	if (abs_sec == 0){							// ��_
		score = 0;
	}
	else if (abs_sec <= 60){					// �P���ȓ�
		if (flag_d15 > 0){					// �P���ȓ�15�b�P��
			if (ndist >= 0 || smute_det){
				score = 21;
			}
			if (ndist > 0){
				score += ndist * 3;
				if (ndist >= 3) score += ndist;	// �ʒu���m�Ȏ��͂͒ǉ�
			}
		}
		else{									// �P���ȓ�5�b�P��
			if (ndist > 0){
				if (still_det){					// �����Ȃ��Ȃ�ŏ�
					ndist = 1;
				}
				score = 7;
				score += ndist * 3;
			}
		}
	}
	else{
		if (ndist >= 0 || smute_det){
			if (abs_sec <= 120){					// �Q���ȓ�
				score = 15;
			}
			else{									// �Q������
				score = 3;
			}
			score += ndist * 3;
		}
	}
	return score;
}



//=====================================================================
// ���ʏ����̍\���ύX
//=====================================================================

//---------------------------------------------------------------------
// �����������ꍇ�⋫�E�ɃV�[���`�F���W���Ȃ��ꍇ��߂���
// �o��:
//   �Ԃ�l: 0=�ʏ�  1=����������  2=�������������E�ɃV�[���`�F���W���Ȃ� +4:�V�[���`�F���W�Ȃ�������������
//---------------------------------------------------------------------
int JlsAutoReform::checkMuteMany(RangeNsc rnsc){
	int type_mute = 0;
	if (rnsc.st < rnsc.ed && rnsc.st >= 0 && rnsc.ed >= 0){
		{
			int count_smute = 0;
			if (abs(rnsc.ed - rnsc.st) > 1){
				int lpcnt = abs(rnsc.ed - rnsc.st) - 1;
				Nsc nsc_tmp = rnsc.st;
				for(int i=0; i<lpcnt; i++){
					nsc_tmp ++;
					if ((pdata->isSmuteSameArea(nsc_tmp, rnsc.st) == false) &&
						(pdata->isSmuteSameArea(nsc_tmp, rnsc.ed) == false) &&
						(pdata->isSmuteSameArea(nsc_tmp, nsc_tmp-1) == false)){
						count_smute ++;
					}
				}
				//--- �����̔��f�𖳌��ɂ���ݒ�̏ꍇ ---
				bool calcel_cntsc = pdata->getConfig(CONFIG_VAR_cancelCntSc);
				if (calcel_cntsc){
					count_smute = 0;
				}
			}
			RangeMsec rmsec = getRangeMsec(rnsc);
			Msec msec_dif = rmsec.ed - rmsec.st;
			if ((count_smute > 3 && msec_dif < 14500) ||	// �Ԃɖ��������ɑ����ꍇ�͖������ŊO��
				(count_smute > 4 && msec_dif < 29500) ||
				(count_smute > 6 && msec_dif < 59500) ||
				(count_smute > 8)){
				type_mute = 2;
			}
			else if ((count_smute > 1 && msec_dif < 14500) ||	// �Ԃɖ����������ꍇ�͊O��
				(count_smute > 2 && msec_dif < 29500) ||
				(count_smute > 4 && msec_dif < 59500) ||
				(count_smute > 6)){
				type_mute = 1;
			}
		}
		int type_nosc = pdata->getConfigAction(CONFIG_ACT_MuteNoSc);	// �V�[���`�F���W�Ȃ������̎g�p�L�����Z��
		if (type_mute > 0 && type_nosc == 1){
			if (pdata->getScpStill(rnsc.st) || pdata->getScpStill(rnsc.ed)){
				type_mute += 4;
			}
		}
	}
	return type_mute;
}

//---------------------------------------------------------------------
// �P�\�������ǉ�����
//---------------------------------------------------------------------
void JlsAutoReform::addCommonComponentOne(Nsc nsc_target, Nsc nsc_side, bool enable_short){
	SearchDirType dr = (nsc_target < nsc_side)? SEARCH_DIR_NEXT : SEARCH_DIR_PREV;
	//--- �אڍ\���Ȃ���s�����I�� ---
	if (abs(nsc_target - nsc_side) <= 1) return;
	//--- �ݒ�l�擾 ---
	Msec msec_mgn = pdata->getConfig(CONFIG_VAR_msecMgnCmDivide);
	Msec msec_target = pdata->getMsecScp(nsc_target);
	Msec msec_side   = pdata->getMsecScp(nsc_side);
	bool logomode = true;
	{
		Nsc nsc_tmp = (dr == SEARCH_DIR_NEXT)? nsc_side : nsc_target;
		ScpArType arstat_tmp = pdata->getScpArstat(nsc_tmp);
		if (jlsd::isScpArTypeLogo(arstat_tmp) == false){
			logomode = false;
		}
	}
	{
		Nsc nsc_cur = nsc_target;
		bool cont = true;
		while(cont){
			nsc_cur = pdata->getNscDirScpChap(nsc_cur, dr, SCP_CHAP_NONE);
			if (nsc_cur < 0 || nsc_cur == nsc_side){
				cont = false;
			}
			else{
				Msec msec_cur = pdata->getMsecScp(nsc_cur);
				CalcDifInfo calc_target;
				CalcDifInfo calc_side;
				int type_target = calcDifSelect(calc_target, msec_target, msec_cur);
				int type_side   = calcDifSelect(calc_side  , msec_side  , msec_cur);
				//--- �����ɂ��␳ ---
				if (calc_target.sec < 30 && enable_short == false){
					type_target = 0;
				}
				else if (calc_target.sec >= 122){		// �����͈͂𒴂������߂�
					cont = false;
				}
				//--- �b�P�ʋ�؂�Ō덷����������΍\���ǉ� ---
				if (type_target > 0 && (type_side > 0 || calc_side.sec > 30)){
					if (calc_target.gap <= msec_mgn){
						//--- �\����؂�ǉ����� ---
						ScpArType arstat_cur = (logomode)? SCP_AR_L_OTHER : SCP_AR_N_OTHER;
						pdata->setScpChap(nsc_cur, SCP_CHAP_DINT);
						pdata->setScpArstat(nsc_cur, arstat_cur);
						cont = false;		// �I��
					}
				}
			}
		}
	}
}



//=====================================================================
// �Q�_�Ԃ̎��ԂƔ��ʏ����擾
//=====================================================================

//---------------------------------------------------------------------
// �Q�t���[���Ԃ̕b�����擾
// ���́F
//   msec_src  : ��r���t���[���ԍ�
//   msec_dst  : ��r��t���[���ԍ�
// �o�́F
//   calcdif
//     sgn : ��r�t���[���召�isrc�����傫��=-1,����=0,dst�����傫��=1�j
//     sec : �t���[���ԕb��
//     gap : �t���[���ԕb���̌덷�i�~���b�j
// �Ԃ�l�F
//   2 : 10,15,30,45,60,90,120�b
//   1 : �R�b�ȏ�Q�T�b�ȉ����b�P�ʂŌ덷�����Ȃ��ꍇ
//   0 : ����ȊO
//---------------------------------------------------------------------
int JlsAutoReform::calcDifSelect(CalcDifInfo &calcdif, Msec msec_src, Msec msec_dst){
	Msec msec_val_near1 = pdata->msecValNear1;
	Msec msec_val_near2 = pdata->msecValNear2;

	Msec msec_dif = abs(msec_dst - msec_src);
	Sec  sec_dif = pdata->cnv.getSecFromMsec(msec_dif);
	Msec gap_dif = abs(sec_dif * 1000 - msec_dif);
	int type = 0;
	if (gap_dif <= msec_val_near2 &&
		(sec_dif == 10 || sec_dif == 15 || sec_dif == 30 || sec_dif == 45 ||
		 sec_dif == 60 || sec_dif == 90 || sec_dif == 120)){
		type = 2;
	}
	else if (gap_dif <= msec_val_near1 &&
			 (sec_dif >= 3 && sec_dif <= 25)){
		type = 1;
	}
	// ���ʊi�[
	if (msec_src > msec_dst){
		calcdif.sgn = -1;
	}
	else if (msec_src < msec_dst){
		calcdif.sgn = 1;
	}
	else{
		calcdif.sgn = 0;
	}
	calcdif.sec = sec_dif;
	calcdif.gap = gap_dif;

	return type;
}

//---------------------------------------------------------------------
// �Ώۋ�����5/15�b�P�ʂ��擾
// ���́F
//   msec_dif:  �Ώۋ���
// �o�́F
//   �Ԃ�l: 0=5/15�b�P�ʂł͂Ȃ� 1=5�b�P��  2=15�b�P��  3=5/15�b�P��
//   calcmod.mod15: 15�b�P�ʂ���̌덷msec�o�́i-1�̎��͔͈͊O�j
//   calcmod.mod05: 05�b�P�ʂ���̌덷msec�o�́i-1�̎��͔͈͊O�j
//---------------------------------------------------------------------
int JlsAutoReform::calcDifMod0515(CalcModInfo &calcmod, Msec msec_dif){
	int ret = 0;
	Msec calc_mod15 = -1;
	Msec calc_mod05 = -1;
	//--- 180�b�ȓ���15�b�P�ʂ��m�F ---
	if (msec_dif > pdata->msecValLap2 && 
		msec_dif <= 180 * 1000 + pdata->msecValLap2){
		calc_mod15 = msec_dif % (15 * 1000);
		if (calc_mod15 > (15 * 1000)/2){
			calc_mod15 = (15 * 1000) - calc_mod15;
		}
		if (calc_mod15 <= pdata->msecValNear3){
			ret += 2;
		}
	}
	//--- 30�b�ȓ���5�b�P�ʂ��m�F ---
	if (msec_dif > pdata->msecValLap2 && 
		msec_dif <= 30 * 1000 + pdata->msecValLap2){
		calc_mod05 = msec_dif % (5 * 1000);
		if (calc_mod05 > (5 * 1000)/2){
			calc_mod05 = (5 * 1000) - calc_mod05;
		}
		if (calc_mod05 <= pdata->msecValNear1){
			ret += 1;
		}
	}
	//--- ���ʊi�[ ---
	calcmod.mod15 = calc_mod15;
	calcmod.mod05 = calc_mod05;
	return ret;
}

//---------------------------------------------------------------------
// �͈͈ʒu���擾
//---------------------------------------------------------------------
RangeMsec JlsAutoReform::getRangeMsec(RangeNsc range_nsc){
	return pdata->getRangeMsecFromRangeNsc(range_nsc);
}

//---------------------------------------------------------------------
// �~���b���͂�15�b�P�ʂ����f
//---------------------------------------------------------------------
bool JlsAutoReform::isCmLengthMsec(Msec msec_target){
	Sec sec_target = pdata->cnv.getSecFromMsec(msec_target);
	return (sec_target % 15 == 0)? true : false;
}

//---------------------------------------------------------------------
// ���Ԃ����S�F�����ԂɂȂ��Ă��邩�m�F
//---------------------------------------------------------------------
bool JlsAutoReform::isLengthLogoLevel(Msec msec_target, FormLogoLevelExt extype){
	//--- �ɒ[�ɒZ�����͖��� ---
	if (msec_target < pdata->msecValLap2) return false;
	//--- ���S���x���ɂ�蔻�� ---
	bool match_len = false;
	int lvmid = (extype.mid5s)? CONFIG_LOGO_LEVEL_USE_MIDL : CONFIG_LOGO_LEVEL_USE_MIDH;
	int lvlogo = pdata->getLevelUseLogo();
	if (lvlogo == CONFIG_LOGO_LEVEL_USE_MAX){
		match_len = true;
	}
	else{
		CalcModInfo calcmod;
		int typemod = calcDifMod0515(calcmod, msec_target);
		CalcDifInfo calcdif;
		calcDifSelect(calcdif, 0, msec_target);
		if (lvlogo >= CONFIG_LOGO_LEVEL_USE_HIGH){		// �b���P�ʊm�F
			if (calcdif.gap <= pdata->msecValNear1) match_len = true;
		}
		if (lvlogo >= lvmid){							// 5�b�P�ʊm�F
			if (typemod >= 1) match_len = true;
		}
		if (typemod >= 2) match_len = true;				// 15�b�P�ʊm�F
		//--- �����Ԃ̍\����������ꍇ ---
		if (extype.en_long){
			if (calcdif.sec % 30 == 0 && calcdif.sec > 180 && calcdif.sec <= 600){
				match_len = true;						// �����Ԃ�30�b�P�ʍ\��
			}
		}
		//--- �����Ԃ̒P�ʍ\��������ꍇ ---
		if (extype.longmod){
			if (lvlogo >= lvmid){						// 5�b�P�ʊm�F
				if (calcdif.sec % 5 == 0 && calcdif.gap <= pdata->msecValNear1) match_len = true;
			}
			if (calcdif.sec % 15 == 0 && calcdif.gap <= pdata->msecValNear1) match_len = true;
		}
	}
	
	return match_len;
}

